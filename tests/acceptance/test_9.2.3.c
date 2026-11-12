#include "main.h"

#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct s_capture
{
	int saved;
	int read_fd;
	pthread_t reader;
	char data[256];
	size_t len;
} t_capture;

typedef struct s_waiter
{
	t_sim *sim;
	t_dongle *dongle;
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t ready_cond;
	int ready;
} t_waiter;

static void guard_start(void)
{
	if (getenv("CODEXION_UNDER_VALGRIND") != NULL
		|| getenv("TSAN_OPTIONS") != NULL)
		alarm(120);
	else
		alarm(2);
}

static void init_case(t_sim *sim, int burnout, int required,
		long last, int count)
{
	t_config cfg;
	int i;

	cfg.number_of_coders = 4;
	cfg.time_to_burnout = burnout;
	cfg.time_to_compile = 200;
	cfg.time_to_debug = 200;
	cfg.time_to_refactor = 200;
	cfg.number_of_compiles_required = required;
	cfg.dongle_cooldown = 50;
	cfg.mode = MODE_FIFO;
	assert(init_simulation(sim, &cfg) == 0);
	for (i = 0; i < 4; ++i)
	{
		sim->coders[i].last_compile_start = last;
		sim->coders[i].compile_count = count;
	}
	sim->start_ms = get_time_ms();
	assert(sim->start_ms >= 0);
}

static void *drain_output(void *arg)
{
	t_capture *cap = arg;
	char buf[64];
	ssize_t n;

	while (1)
	{
		n = read(cap->read_fd, buf, sizeof(buf));
		assert(n >= 0);
		if (n == 0)
			break;
		assert(cap->len + (size_t)n < sizeof(cap->data));
		memcpy(cap->data + cap->len, buf, (size_t)n);
		cap->len += (size_t)n;
	}
	cap->data[cap->len] = '\0';
	return (NULL);
}

static void capture_start(t_capture *cap)
{
	int fds[2];

	memset(cap, 0, sizeof(*cap));
	assert(pipe(fds) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->read_fd = fds[0];
	assert(dup2(fds[1], STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, drain_output, cap) == 0);
}

static void capture_end(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(cap->saved) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->read_fd) == 0);
}

static void monitor_join(t_sim *sim)
{
	pthread_t thread;
	void *result = (void *)1;

	assert(pthread_create(&thread, NULL, run_monitor, sim) == 0);
	assert(pthread_join(thread, &result) == 0);
	assert(result == NULL);
}

static void assert_burnout_line(t_capture *cap)
{
	long ts;
	int id;
	int pos = -1;

	assert(sscanf(cap->data, "%ld %d burned out%n", &ts, &id, &pos) == 2);
	assert(pos >= 0 && cap->data[pos] == '\n');
	assert((size_t)pos + 1 == cap->len);
	assert(ts >= 0);
	assert(id == 3);
}

static void assert_unlocked(t_sim *sim)
{
	int i;

	assert(pthread_mutex_trylock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_trylock(&sim->log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->log_mutex) == 0);
	for (i = 0; i < 4; ++i)
	{
		assert(pthread_mutex_trylock(&sim->coders[i].mutex) == 0);
		assert(pthread_mutex_unlock(&sim->coders[i].mutex) == 0);
	}
}

/* AC1: target met stops silently without marking failure. */
static void test_ac1_target_met_clean(void)
{
	t_sim sim;
	t_capture cap;

	init_case(&sim, 800, 3, LONG_MAX, 3);
	capture_start(&cap);
	guard_start();
	monitor_join(&sim);
	capture_end(&cap);
	alarm(0);
	assert(cap.len == 0);
	assert(sim.stopped == 1 && sim.failed == 0);
	assert(free_sim(&sim) == 0);
}

/* AC2: already passed burnout deadlines win over zero required. */
static void test_ac2_zero_required_reports_burnout(void)
{
	t_sim sim;
	t_capture cap;

	init_case(&sim, 800, 0, 0, 0);
	sim.start_ms -= 1000;
	capture_start(&cap);
	guard_start();
	monitor_join(&sim);
	capture_end(&cap);
	alarm(0);
	{
		long ts;
		int id;
		int end = -1;

		assert(sscanf(cap.data, "%ld %d burned out%n", &ts, &id, &end) == 2);
		assert(ts >= 0 && id >= 1 && id <= 4);
		assert(end >= 0 && cap.data[end] == '\n');
		assert((size_t)end + 1 == cap.len);
	}
	assert(sim.stopped == 1 && sim.failed == 0);
	assert(free_sim(&sim) == 0);
}

/* AC3: one successfully logged burnout stops without marking failure. */
static void test_ac3_logged_burnout_clean(void)
{
	t_sim sim;
	t_capture cap;

	init_case(&sim, 300, 3, LONG_MAX, 0);
	sim.coders[2].last_compile_start = 0;
	capture_start(&cap);
	guard_start();
	monitor_join(&sim);
	capture_end(&cap);
	alarm(0);
	assert_burnout_line(&cap);
	assert(sim.stopped == 1 && sim.failed == 0);
	assert(free_sim(&sim) == 0);
}

static void *wait_on_stop(void *arg)
{
	t_waiter *waiter = arg;

	assert(pthread_mutex_lock(&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_lock(&waiter->mutex) == 0);
	waiter->ready = 1;
	assert(pthread_cond_signal(&waiter->ready_cond) == 0);
	assert(pthread_mutex_unlock(&waiter->mutex) == 0);
	while (!is_stopped(waiter->sim))
		assert(pthread_cond_wait(&waiter->dongle->cond,
				&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->dongle->mutex) == 0);
	return (NULL);
}

static void start_waiter(t_waiter *waiter, t_sim *sim, int index)
{
	memset(waiter, 0, sizeof(*waiter));
	waiter->sim = sim;
	waiter->dongle = &sim->dongles[index];
	assert(pthread_mutex_init(&waiter->mutex, NULL) == 0);
	assert(pthread_cond_init(&waiter->ready_cond, NULL) == 0);
	assert(pthread_create(&waiter->thread, NULL, wait_on_stop, waiter) == 0);
	assert(pthread_mutex_lock(&waiter->mutex) == 0);
	while (!waiter->ready)
		assert(pthread_cond_wait(&waiter->ready_cond, &waiter->mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->mutex) == 0);
	assert(pthread_mutex_lock(&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->dongle->mutex) == 0);
}

static void join_waiter(t_waiter *waiter)
{
	assert(pthread_join(waiter->thread, NULL) == 0);
	assert(pthread_cond_destroy(&waiter->ready_cond) == 0);
	assert(pthread_mutex_destroy(&waiter->mutex) == 0);
}

static void child_write_failure(int with_waiters)
{
	t_sim sim;
	t_waiter waiters[4];
	int full;
	int i;

	guard_start();
	init_case(&sim, 300, 3, LONG_MAX, 0);
	sim.coders[2].last_compile_start = 0;
	full = open("/dev/full", O_WRONLY);
	assert(full >= 0);
	assert(dup2(full, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(full) == 0);
	if (with_waiters)
		for (i = 0; i < 4; ++i)
			start_waiter(&waiters[i], &sim, i);
	monitor_join(&sim);
	if (with_waiters)
		for (i = 0; i < 4; ++i)
			join_waiter(&waiters[i]);
	assert(sim.stopped == 1 && sim.failed == 1);
	assert(free_sim(&sim) == 0);
	alarm(0);
	_exit(0);
}

static void assert_child_write_failure(int with_waiters)
{
	pid_t pid;
	int status;

	guard_start();
	pid = fork();
	assert(pid >= 0);
	if (pid == 0)
		child_write_failure(with_waiters);
	assert(waitpid(pid, &status, 0) == pid);
	alarm(0);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC4: a failed burnout-line write marks the simulation failed. */
static void test_ac4_burnout_write_failure(void)
{
	assert_child_write_failure(0);
}

/* AC5: an external stop leaves the monitor silent and preserves failed. */
static void test_ac5_external_stop_untouched(void)
{
	t_sim sim;
	t_capture cap;
	int failed;

	for (failed = 0; failed <= 1; ++failed)
	{
		init_case(&sim, 300, 3, 0, 0);
		sim.start_ms -= 1000;
		if (failed)
			assert(set_failed(&sim) == 0);
		assert(set_stopped(&sim) == 0);
		capture_start(&cap);
		guard_start();
		monitor_join(&sim);
		capture_end(&cap);
		alarm(0);
		assert(cap.len == 0);
		assert(sim.stopped == 1 && sim.failed == failed);
		assert(free_sim(&sim) == 0);
	}
}

/* AC6: target, burnout and external-stop exits leave all locks free. */
static void test_ac6_lock_state_after_exit(void)
{
	t_sim sim;
	t_capture cap;
	int mode;

	for (mode = 0; mode < 3; ++mode)
	{
		init_case(&sim, mode == 1 ? 300 : 800, 3,
				mode == 2 ? 0 : LONG_MAX, mode == 0 ? 3 : 0);
		if (mode == 1)
			sim.coders[2].last_compile_start = 0;
		if (mode == 2)
		{
			sim.start_ms -= 1000;
			assert(set_stopped(&sim) == 0);
		}
		capture_start(&cap);
		guard_start();
		monitor_join(&sim);
		capture_end(&cap);
		alarm(0);
		assert_unlocked(&sim);
		assert(free_sim(&sim) == 0);
	}
}

/* AC7: failed-write exit wakes every condition waiter before returning. */
static void test_ac7_error_wakes_waiters(void)
{
	assert_child_write_failure(1);
}

int main(void)
{
	test_ac1_target_met_clean();
	test_ac2_zero_required_reports_burnout();
	test_ac3_logged_burnout_clean();
	test_ac4_burnout_write_failure();
	test_ac5_external_stop_untouched();
	test_ac6_lock_state_after_exit();
	test_ac7_error_wakes_waiters();
	return (0);
}
