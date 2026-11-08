#define _GNU_SOURCE

#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct s_capture
{
	int saved;
	int read_fd;
	pthread_t reader;
	pthread_mutex_t mutex;
	pthread_cond_t changed;
	char data[65536];
	size_t len;
} t_capture;

static void init_sim(t_sim *sim, int n, int burnout, int compile,
		int debug, int refactor, int required, int cooldown)
{
	t_config cfg;

	cfg.number_of_coders = n;
	cfg.time_to_burnout = burnout;
	cfg.time_to_compile = compile;
	cfg.time_to_debug = debug;
	cfg.time_to_refactor = refactor;
	cfg.number_of_compiles_required = required;
	cfg.dongle_cooldown = cooldown;
	cfg.mode = MODE_FIFO;
	assert(init_simulation(sim, &cfg) == 0);
	sim->start_ms = get_time_ms();
	assert(sim->start_ms >= 0);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

static void assert_clean(t_sim *sim)
{
	int i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		assert(sim->dongles[i].available == 1);
		assert_unlocked(&sim->dongles[i].mutex);
		assert_unlocked(&sim->coders[i].mutex);
		i++;
	}
	assert_unlocked(&sim->stop_mutex);
	assert_unlocked(&sim->log_mutex);
	assert_unlocked(&sim->ticket_mutex);
}

static void *drain_output(void *arg)
{
	t_capture *cap;
	char buf[1024];
	ssize_t n;

	cap = arg;
	while (1)
	{
		n = read(cap->read_fd, buf, sizeof(buf));
		if (n < 0 && errno == EINTR)
			continue;
		assert(n >= 0);
		if (n == 0)
			break;
		assert(pthread_mutex_lock(&cap->mutex) == 0);
		assert(cap->len + (size_t)n < sizeof(cap->data));
		memcpy(cap->data + cap->len, buf, (size_t)n);
		cap->len += (size_t)n;
		cap->data[cap->len] = '\0';
		assert(pthread_cond_broadcast(&cap->changed) == 0);
		assert(pthread_mutex_unlock(&cap->mutex) == 0);
	}
	return (NULL);
}

static void start_capture(t_capture *cap)
{
	int fds[2];

	memset(cap, 0, sizeof(*cap));
	assert(pthread_mutex_init(&cap->mutex, NULL) == 0);
	assert(pthread_cond_init(&cap->changed, NULL) == 0);
	assert(pipe(fds) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->read_fd = fds[0];
	assert(dup2(fds[1], STDOUT_FILENO) >= 0);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, drain_output, cap) == 0);
}

static void stop_capture(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) >= 0);
	assert(close(cap->saved) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->read_fd) == 0);
	assert(pthread_cond_destroy(&cap->changed) == 0);
	assert(pthread_mutex_destroy(&cap->mutex) == 0);
}

static void wait_for_line(t_capture *cap, const char *line)
{
	struct timespec until;

	assert(clock_gettime(CLOCK_REALTIME, &until) == 0);
	until.tv_sec += 5;
	assert(pthread_mutex_lock(&cap->mutex) == 0);
	while (strstr(cap->data, line) == NULL)
		assert(pthread_cond_timedwait(&cap->changed, &cap->mutex,
				&until) == 0);
	assert(pthread_mutex_unlock(&cap->mutex) == 0);
}

static void start_coders(t_sim *sim, pthread_t *threads)
{
	int i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		assert(pthread_create(&threads[i], NULL, run_coder,
				&sim->coders[i]) == 0);
		i++;
	}
}

static void join_coders(t_sim *sim, pthread_t *threads)
{
	void *result;
	int i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		assert(pthread_join(threads[i], &result) == 0);
		assert(result == NULL);
		i++;
	}
}

static void stop_coders(t_sim *sim)
{
	assert(set_stopped(sim) == 0);
	assert(wake_dongles(sim) == 0);
}

static void sleep_test_ms(long ms)
{
	struct timespec delay;

	delay.tv_sec = ms / 1000;
	delay.tv_nsec = (ms % 1000) * 1000000L;
	while (nanosleep(&delay, &delay) == -1)
		assert(errno == EINTR);
}

static void start_devnull(int *saved)
{
	int fd;

	*saved = dup(STDOUT_FILENO);
	assert(*saved >= 0);
	fd = open("/dev/null", O_WRONLY);
	assert(fd >= 0);
	assert(dup2(fd, STDOUT_FILENO) >= 0);
	assert(close(fd) == 0);
}

static void stop_devnull(int saved)
{
	assert(dup2(saved, STDOUT_FILENO) >= 0);
	assert(close(saved) == 0);
}

/* AC1: an external stop after real work is clean and leaves resources free. */
static void test_ac1_normal_stop(void)
{
	t_sim sim;
	t_capture cap;
	pthread_t threads[4];
	int i;
	int total;

	init_sim(&sim, 4, 800, 200, 100, 100, 50, 50);
	start_capture(&cap);
	start_coders(&sim, threads);
	sleep_test_ms(700);
	stop_coders(&sim);
	join_coders(&sim, threads);
	stop_capture(&cap);
	assert(sim.failed == 0 && sim.stopped == 1);
	assert_clean(&sim);
	i = 0;
	total = 0;
	while (i < 4)
		total += sim.coders[i++].compile_count;
	assert(total >= 2);
	assert(free_sim(&sim) == 0);
}

/* AC2: stop during a long compile is a clean return and releases dongles. */
static void test_ac2_stop_during_compile(void)
{
	t_sim sim;
	t_capture cap;
	pthread_t threads[2];

	init_sim(&sim, 2, 100000, 60000, 10, 10, 0, 0);
	start_capture(&cap);
	start_coders(&sim, threads);
	wait_for_line(&cap, "is compiling\n");
	stop_coders(&sim);
	join_coders(&sim, threads);
	stop_capture(&cap);
	assert(sim.failed == 0);
	assert_clean(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC3: stop during a long rest is a clean return. */
static void test_ac3_stop_during_rest(void)
{
	t_sim sim;
	t_capture cap;
	pthread_t threads[2];

	init_sim(&sim, 2, 100000, 10, 60000, 60000, 0, 0);
	start_capture(&cap);
	start_coders(&sim, threads);
	wait_for_line(&cap, "is debugging\n");
	stop_coders(&sim);
	join_coders(&sim, threads);
	stop_capture(&cap);
	assert(sim.failed == 0);
	assert_clean(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC4: zero-length phases still permit a clean external stop. */
static void test_ac4_zero_length_phases(void)
{
	t_sim sim;
	pthread_t threads[2];
	int saved;

	init_sim(&sim, 2, 10000, 0, 0, 0, 0, 0);
	start_devnull(&saved);
	start_coders(&sim, threads);
	sleep_test_ms(100);
	stop_coders(&sim);
	join_coders(&sim, threads);
	stop_devnull(saved);
	assert(sim.failed == 0);
	assert_clean(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC5: one coder holds its sole dongle until the normal stop. */
static void test_ac5_single_coder(void)
{
	t_sim sim;
	t_capture cap;
	pthread_t thread;
	void *result;

	init_sim(&sim, 1, 10000, 50, 30, 40, 3, 0);
	start_capture(&cap);
	assert(pthread_create(&thread, NULL, run_coder, &sim.coders[0]) == 0);
	wait_for_line(&cap, "has taken a dongle\n");
	sleep_test_ms(200);
	stop_coders(&sim);
	assert(pthread_join(thread, &result) == 0 && result == NULL);
	stop_capture(&cap);
	assert(sim.failed == 0);
	assert_clean(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC6: an already-stopped coder is silent and does not report failure. */
static void test_ac6_already_stopped(void)
{
	t_sim sim;
	t_capture cap;

	init_sim(&sim, 2, 10000, 50, 30, 40, 0, 0);
	assert(set_stopped(&sim) == 0);
	start_capture(&cap);
	assert(run_coder(&sim.coders[0]) == NULL);
	stop_capture(&cap);
	assert(cap.len == 0);
	assert(sim.failed == 0);
	assert_clean(&sim);
	assert(free_sim(&sim) == 0);
}

static void assert_failed(t_sim *sim)
{
	assert(sim->failed == 1 && sim->stopped == 1);
	assert_clean(sim);
	assert(free_sim(sim) == 0);
}

static void test_count_failure(void)
{
	t_sim sim;
	t_capture cap;

	init_sim(&sim, 2, 10000, 50, 30, 40, 0, 0);
	sim.coders[0].compile_count = INT_MAX;
	start_capture(&cap);
	assert(run_coder(&sim.coders[0]) == NULL);
	stop_capture(&cap);
	assert(strstr(cap.data, "has taken a dongle\n") != NULL);
	assert(strstr(cap.data, "is compiling\n") != NULL);
	assert_failed(&sim);
}

static void test_ticket_failure(void)
{
	t_sim sim;
	t_capture cap;

	init_sim(&sim, 2, 10000, 50, 30, 40, 0, 0);
	sim.next_ticket = LONG_MAX;
	start_capture(&cap);
	assert(run_coder(&sim.coders[0]) == NULL);
	stop_capture(&cap);
	assert(cap.len == 0);
	assert_failed(&sim);
}

static void test_log_failure(void)
{
	t_sim sim;
	int fd;

	fd = open("/dev/full", O_WRONLY);
	assert(fd >= 0);
	assert(dup2(fd, STDOUT_FILENO) >= 0);
	assert(close(fd) == 0);
	init_sim(&sim, 2, 10000, 50, 30, 40, 0, 0);
	assert(run_coder(&sim.coders[0]) == NULL);
	assert_failed(&sim);
}

/* AC7: real count, acquisition and log errors mark failed and stop the run. */
static void test_ac7_real_failures(void)
{
	pid_t child;
	int status;

	test_count_failure();
	test_ticket_failure();
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		if (getenv("CODEXION_UNDER_VALGRIND") != NULL
			|| getenv("TSAN_OPTIONS") != NULL)
			alarm(120);
		else
			alarm(20);
		test_log_failure();
		_exit(0);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC8: null arguments return safely without writing output. */
static void test_ac8_invalid_input(void)
{
	t_coder coder;
	t_capture cap;

	memset(&coder, 0, sizeof(coder));
	start_capture(&cap);
	assert(run_coder(NULL) == NULL);
	assert(run_coder(&coder) == NULL);
	stop_capture(&cap);
	assert(cap.len == 0);
}

int main(void)
{
	if (getenv("CODEXION_UNDER_VALGRIND") != NULL
		|| getenv("TSAN_OPTIONS") != NULL)
		alarm(120);
	else
		alarm(20);
	test_ac1_normal_stop();
	test_ac2_stop_during_compile();
	test_ac3_stop_during_rest();
	test_ac4_zero_length_phases();
	test_ac5_single_coder();
	test_ac6_already_stopped();
	test_ac7_real_failures();
	test_ac8_invalid_input();
	alarm(0);
	return (0);
}
