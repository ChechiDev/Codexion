#include "main.h"

#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef CODEXION_TIME_SLACK_MS
# define CODEXION_TIME_SLACK_MS 0
#endif

void *run_monitor(void *arg);

typedef struct s_capture
{
	int saved;
	int read_fd;
	pthread_t reader;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int ready;
	char data[256];
	size_t len;
	long received_ms;
} t_capture;

typedef struct s_run
{
	t_sim *sim;
	pthread_t thread;
	pthread_mutex_t mutex;
	int done;
	void *result;
	long ended_ms;
} t_run;

typedef struct s_waiter
{
	t_sim *sim;
	t_dongle *dongle;
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int ready;
} t_waiter;

static t_config get_config(int coders, int burnout, int target, t_mode mode)
{
	t_config config;

	config.number_of_coders = coders;
	config.time_to_burnout = burnout;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = target;
	config.dongle_cooldown = 50;
	config.mode = mode;
	return (config);
}

static void init_case(t_sim *sim, t_config config, long start, int count)
{
	int i;

	assert(init_simulation(sim, &config) == 0);
	for (i = 0; i < config.number_of_coders; ++i)
	{
		sim->coders[i].last_compile_start = start;
		sim->coders[i].compile_count = count;
	}
}

static void assert_unlocked(t_sim *sim)
{
	int i;

	assert(pthread_mutex_trylock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_trylock(&sim->log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->log_mutex) == 0);
	for (i = 0; i < sim->config.number_of_coders; ++i)
	{
		assert(pthread_mutex_trylock(&sim->coders[i].mutex) == 0);
		assert(pthread_mutex_unlock(&sim->coders[i].mutex) == 0);
	}
}

static void *read_output(void *arg)
{
	t_capture *cap = arg;
	char ch;
	ssize_t n;

	assert(pthread_mutex_lock(&cap->mutex) == 0);
	cap->ready = 1;
	assert(pthread_cond_signal(&cap->cond) == 0);
	assert(pthread_mutex_unlock(&cap->mutex) == 0);
	while (1)
	{
		n = read(cap->read_fd, &ch, 1);
		assert(n >= 0);
		if (n == 0)
			break;
		assert(cap->len + 1 < sizeof(cap->data));
		cap->data[cap->len++] = ch;
		if (ch == '\n')
			cap->received_ms = get_time_ms();
	}
	cap->data[cap->len] = '\0';
	return (NULL);
}

static void start_capture(t_capture *cap)
{
	int fds[2];

	memset(cap, 0, sizeof(*cap));
	assert(pthread_mutex_init(&cap->mutex, NULL) == 0);
	assert(pthread_cond_init(&cap->cond, NULL) == 0);
	assert(pipe(fds) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->read_fd = fds[0];
	assert(dup2(fds[1], STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, read_output, cap) == 0);
	assert(pthread_mutex_lock(&cap->mutex) == 0);
	while (!cap->ready)
		assert(pthread_cond_wait(&cap->cond, &cap->mutex) == 0);
	assert(pthread_mutex_unlock(&cap->mutex) == 0);
}

static void end_capture(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(cap->saved) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->read_fd) == 0);
	assert(pthread_cond_destroy(&cap->cond) == 0);
	assert(pthread_mutex_destroy(&cap->mutex) == 0);
}

static void *run_and_record(void *arg)
{
	t_run *run = arg;
	void *result;
	long ended;

	result = run_monitor(run->sim);
	ended = get_time_ms();
	assert(ended >= 0);
	assert(pthread_mutex_lock(&run->mutex) == 0);
	run->result = result;
	run->ended_ms = ended;
	run->done = 1;
	assert(pthread_mutex_unlock(&run->mutex) == 0);
	return (NULL);
}

static void start_run(t_run *run, t_sim *sim)
{
	memset(run, 0, sizeof(*run));
	run->sim = sim;
	assert(pthread_mutex_init(&run->mutex, NULL) == 0);
	assert(pthread_create(&run->thread, NULL, run_and_record, run) == 0);
}

static void assert_running(t_run *run)
{
	assert(pthread_mutex_lock(&run->mutex) == 0);
	assert(run->done == 0);
	assert(pthread_mutex_unlock(&run->mutex) == 0);
}

static void join_run(t_run *run)
{
	assert(pthread_join(run->thread, NULL) == 0);
	assert(run->done == 1);
	assert(run->result == NULL);
	assert(pthread_mutex_destroy(&run->mutex) == 0);
}

static void assert_line(t_capture *cap, int expected_id, long start_ms,
		int deadline_bound)
{
	long ts;
	int id;
	int pos;

	pos = -1;
	assert(sscanf(cap->data, "%ld %d burned out%n", &ts, &id, &pos) == 2);
	assert(pos >= 0 && cap->data[pos] == '\n');
	assert((size_t)pos + 1 == cap->len);
	assert(expected_id == 0 ? id >= 1 && id <= 4 : id == expected_id);
	if (deadline_bound)
	{
		assert(ts >= 300 && ts <= 310 + CODEXION_TIME_SLACK_MS);
		assert(cap->received_ms >= start_ms + 300);
		assert(cap->received_ms - start_ms <= 310 + CODEXION_TIME_SLACK_MS);
	}
}

/* AC1: a non-first coder burns out once, and the complete line arrives on time. */
static void test_ac1_non_first_burnout_on_time(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;

	init_case(&sim, get_config(4, 300, 3, MODE_FIFO), LONG_MAX, 0);
	sim.coders[2].last_compile_start = 0;
	start_capture(&cap);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	alarm(2);
	start_run(&run, &sim);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert_line(&cap, 3, sim.start_ms, 1);
	assert(sim.stopped == 1);
	assert_unlocked(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC2: simultaneous deadlines produce one line for any valid coder. */
static void test_ac2_simultaneous_deadlines(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;

	init_case(&sim, get_config(4, 300, 3, MODE_FIFO), 0, 0);
	start_capture(&cap);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	alarm(2);
	start_run(&run, &sim);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert_line(&cap, 0, sim.start_ms, 1);
	assert(sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

/* AC3: an already reached target stops silently. */
static void test_ac3_target_met(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;

	init_case(&sim, get_config(4, 800, 3, MODE_FIFO), LONG_MAX, 3);
	start_capture(&cap);
	sim.start_ms = get_time_ms();
	alarm(2);
	start_run(&run, &sim);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert(cap.len == 0 && sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

/* AC4: the monitor sees a target reached after it starts. */
static void test_ac4_target_met_later(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;

	init_case(&sim, get_config(4, 800, 3, MODE_FIFO), LONG_MAX, 3);
	sim.coders[1].compile_count = 2;
	start_capture(&cap);
	sim.start_ms = get_time_ms();
	alarm(2);
	start_run(&run, &sim);
	assert(usleep(100000) == 0);
	assert_running(&run);
	assert(pthread_mutex_lock(&sim.coders[1].mutex) == 0);
	sim.coders[1].compile_count = 3;
	assert(pthread_mutex_unlock(&sim.coders[1].mutex) == 0);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert(cap.len == 0 && sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

/* AC5: an overdue coder is reported even when zero compiles are required. */
static void test_ac5_zero_target_reports_burnout(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;

	init_case(&sim, get_config(4, 800, 0, MODE_FIFO), 0, 0);
	start_capture(&cap);
	sim.start_ms = get_time_ms() - 1000;
	alarm(2);
	start_run(&run, &sim);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert_line(&cap, 0, sim.start_ms, 0);
	assert(sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

/* AC6: burnout wins a joint observation with or without a met target. */
static void test_ac6_burnout_precedes_target_in_same_scan(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;
	int short_target;

	for (short_target = 0; short_target <= 1; ++short_target)
	{
		init_case(&sim, get_config(4, 300, 3, MODE_FIFO), LONG_MAX, 3);
		sim.coders[0].last_compile_start = 0;
		if (short_target)
			sim.coders[3].compile_count = 2;
		start_capture(&cap);
		sim.start_ms = get_time_ms() - 1000;
		alarm(2);
		start_run(&run, &sim);
		join_run(&run);
		end_capture(&cap);
		alarm(0);
		assert(sim.stopped == 1);
		assert_line(&cap, 1, sim.start_ms, 0);
		assert(free_sim(&sim) == 0);
	}
}

/* AC7: an existing or later external stop makes the monitor leave silently. */
static void test_ac7_stop_by_someone_else(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;
	long t0;

	init_case(&sim, get_config(4, 300, 3, MODE_FIFO), 0, 0);
	assert(set_stopped(&sim) == 0);
	start_capture(&cap);
	sim.start_ms = get_time_ms() - 1000;
	alarm(2);
	start_run(&run, &sim);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert(cap.len == 0 && sim.stopped == 1);
	assert(free_sim(&sim) == 0);
	init_case(&sim, get_config(4, 800, 3, MODE_FIFO), LONG_MAX, 0);
	start_capture(&cap);
	sim.start_ms = get_time_ms();
	alarm(2);
	start_run(&run, &sim);
	assert(usleep(100000) == 0);
	assert_running(&run);
	t0 = get_time_ms();
	assert(t0 >= 0);
	assert(set_stopped(&sim) == 0);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert(run.ended_ms - t0 <= 200 + CODEXION_TIME_SLACK_MS);
	assert(cap.len == 0 && sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

static void *wait_on_dongle(void *arg)
{
	t_waiter *waiter = arg;

	assert(pthread_mutex_lock(&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_lock(&waiter->mutex) == 0);
	waiter->ready = 1;
	assert(pthread_cond_signal(&waiter->cond) == 0);
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
	assert(pthread_cond_init(&waiter->cond, NULL) == 0);
	assert(pthread_create(&waiter->thread, NULL, wait_on_dongle, waiter) == 0);
	assert(pthread_mutex_lock(&waiter->mutex) == 0);
	while (!waiter->ready)
		assert(pthread_cond_wait(&waiter->cond, &waiter->mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->mutex) == 0);
	assert(pthread_mutex_lock(&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->dongle->mutex) == 0);
}

/* AC8: target and burnout both wake every dongle-condition waiter. */
static void test_ac8_wakes_every_waiter(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;
	t_waiter waiters[4];
	int burned;
	int i;

	for (burned = 0; burned <= 1; ++burned)
	{
		init_case(&sim, get_config(4, burned ? 300 : 800, 3,
				MODE_FIFO), LONG_MAX, burned ? 0 : 3);
		if (burned)
			sim.coders[2].last_compile_start = 0;
		start_capture(&cap);
		alarm(2);
		for (i = 0; i < 4; ++i)
			start_waiter(&waiters[i], &sim, i);
		sim.start_ms = get_time_ms();
		start_run(&run, &sim);
		join_run(&run);
		for (i = 0; i < 4; ++i)
		{
			assert(pthread_join(waiters[i].thread, NULL) == 0);
			assert(pthread_cond_destroy(&waiters[i].cond) == 0);
			assert(pthread_mutex_destroy(&waiters[i].mutex) == 0);
		}
		end_capture(&cap);
		alarm(0);
		if (burned)
			assert_line(&cap, 3, sim.start_ms, 0);
		else
			assert(cap.len == 0);
		assert(sim.stopped == 1);
		assert(free_sim(&sim) == 0);
	}
}

/* AC9: a NULL argument returns NULL and emits nothing. */
static void test_ac9_null_argument(void)
{
	t_capture cap;
	t_run run;

	start_capture(&cap);
	alarm(2);
	start_run(&run, NULL);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert(cap.len == 0);
}

/* AC10: a single coder reports its burnout within the deadline budget. */
static void test_ac10_one_coder(void)
{
	t_sim sim;
	t_capture cap;
	t_run run;

	init_case(&sim, get_config(1, 300, 3, MODE_EDF), 0, 0);
	start_capture(&cap);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	alarm(2);
	start_run(&run, &sim);
	join_run(&run);
	end_capture(&cap);
	alarm(0);
	assert_line(&cap, 1, sim.start_ms, 1);
	assert(sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

/* AC11: a failed burnout write still leaves the simulation stopped. */
static void test_ac11_output_failure(void)
{
	t_sim sim;
	t_run run;
	int saved;
	int full;

	init_case(&sim, get_config(4, 300, 3, MODE_FIFO), LONG_MAX, 0);
	sim.coders[2].last_compile_start = 0;
	full = open("/dev/full", O_WRONLY);
	assert(full >= 0);
	saved = dup(STDOUT_FILENO);
	assert(saved >= 0);
	assert(dup2(full, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(full) == 0);
	sim.start_ms = get_time_ms();
	alarm(2);
	start_run(&run, &sim);
	join_run(&run);
	assert(dup2(saved, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(saved) == 0);
	alarm(0);
	assert(sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	test_ac1_non_first_burnout_on_time();
	test_ac2_simultaneous_deadlines();
	test_ac3_target_met();
	test_ac4_target_met_later();
	test_ac5_zero_target_reports_burnout();
	test_ac6_burnout_precedes_target_in_same_scan();
	test_ac7_stop_by_someone_else();
	test_ac8_wakes_every_waiter();
	test_ac9_null_argument();
	test_ac10_one_coder();
	test_ac11_output_failure();
	return (0);
}
