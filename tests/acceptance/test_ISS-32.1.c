#define _DEFAULT_SOURCE

#include "main.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct s_sleep_worker
{
	t_sim *sim;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int ready;
	int result;
	long returned_ms;
} t_sleep_worker;

typedef struct s_capture
{
	int saved;
	int fd;
	pthread_t reader;
	char *data;
	size_t len;
	size_t capacity;
} t_capture;

static long get_timing_slack_ms(void)
{
	const char *value;
	char *end;
	long slack;

	value = getenv("CODEXION_TIME_SLACK_MS");
	if (value == NULL)
		return (0);
	slack = strtol(value, &end, 10);
	assert(end != value && *end == '\0' && slack >= 0);
	return (slack);
}

static void init_sleep_sim(t_sim *sim)
{
	sim->start_ms = 0;
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
}

static void destroy_sleep_sim(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
}

static void assert_immediate(t_sim *sim, long duration, int result)
{
	long before;
	long after;

	before = get_time_ms();
	assert(before >= 0);
	assert(sleep_ms(sim, duration) == result);
	after = get_time_ms();
	assert(after >= before && after - before < 2 + get_timing_slack_ms());
}

/* AC1: nonpositive durations, including LONG_MIN, return zero immediately. */
static void test_ac1_nonpositive_duration(void)
{
	t_sim sim;

	init_sleep_sim(&sim);
	assert_immediate(&sim, 0, 0);
	assert_immediate(&sim, -1, 0);
	assert_immediate(&sim, LONG_MIN, 0);
	assert(set_stopped(&sim) == 0);
	assert_immediate(&sim, 0, 0);
	assert_immediate(&sim, -1, 0);
	assert_immediate(&sim, LONG_MIN, 0);
	destroy_sleep_sim(&sim);
}

/* AC2: a running simulation sleeps at least 100 ms within the upper bound. */
static void test_ac2_running_duration(void)
{
	t_sim sim;
	long before;
	long after;

	init_sleep_sim(&sim);
	before = get_time_ms();
	assert(before >= 0);
	assert(sleep_ms(&sim, 100) == 0);
	after = get_time_ms();
	assert(after - before >= 100);
	assert(after - before <= 110 + get_timing_slack_ms());
	destroy_sleep_sim(&sim);
}

/* AC3: an already stopped simulation returns one for long durations. */
static void test_ac3_already_stopped(void)
{
	t_sim sim;

	init_sleep_sim(&sim);
	assert(set_stopped(&sim) == 0);
	assert_immediate(&sim, 1000, 1);
	assert_immediate(&sim, LONG_MAX, 1);
	destroy_sleep_sim(&sim);
}

static void *run_sleep_worker(void *arg)
{
	t_sleep_worker *worker;

	worker = arg;
	assert(pthread_mutex_lock(&worker->mutex) == 0);
	worker->ready = 1;
	assert(pthread_cond_signal(&worker->cond) == 0);
	assert(pthread_mutex_unlock(&worker->mutex) == 0);
	worker->result = sleep_ms(worker->sim, 10000);
	worker->returned_ms = get_time_ms();
	return (NULL);
}

/* AC4: stop during sleep returns one within 10 + S ms. */
static void test_ac4_concurrent_stop(void)
{
	t_sim sim;
	t_sleep_worker worker;
	pthread_t thread;
	long stop_ms;

	init_sleep_sim(&sim);
	worker.sim = &sim;
	worker.ready = 0;
	assert(pthread_mutex_init(&worker.mutex, NULL) == 0);
	assert(pthread_cond_init(&worker.cond, NULL) == 0);
	assert(alarm(5) == 0);
	assert(pthread_create(&thread, NULL, run_sleep_worker, &worker) == 0);
	assert(pthread_mutex_lock(&worker.mutex) == 0);
	while (!worker.ready)
		assert(pthread_cond_wait(&worker.cond, &worker.mutex) == 0);
	assert(pthread_mutex_unlock(&worker.mutex) == 0);
	assert(usleep(50000) == 0);
	stop_ms = get_time_ms();
	assert(stop_ms >= 0);
	assert(set_stopped(&sim) == 0);
	assert(pthread_join(thread, NULL) == 0);
	assert(alarm(0) != 0);
	assert(worker.result == 1);
	assert(worker.returned_ms >= stop_ms);
	assert(worker.returned_ms <= stop_ms + 10 + get_timing_slack_ms());
	assert(pthread_cond_destroy(&worker.cond) == 0);
	assert(pthread_mutex_destroy(&worker.mutex) == 0);
	destroy_sleep_sim(&sim);
}

static void *read_pipe(void *arg)
{
	t_capture *cap;
	char chunk[4096];
	char *grown;
	ssize_t got;
	size_t next;

	cap = arg;
	while (1)
	{
		got = read(cap->fd, chunk, sizeof(chunk));
		if (got < 0 && errno == EINTR)
			continue;
		assert(got >= 0);
		if (got == 0)
			break;
		if (cap->len + (size_t)got + 1 > cap->capacity)
		{
			next = cap->capacity * 2;
			while (next < cap->len + (size_t)got + 1)
				next *= 2;
			grown = realloc(cap->data, next);
			assert(grown != NULL);
			cap->data = grown;
			cap->capacity = next;
		}
		memcpy(cap->data + cap->len, chunk, (size_t)got);
		cap->len += (size_t)got;
		cap->data[cap->len] = '\0';
	}
	return (NULL);
}

static void start_capture(t_capture *cap)
{
	int pipefd[2];

	memset(cap, 0, sizeof(*cap));
	cap->capacity = 4096;
	cap->data = malloc(cap->capacity);
	assert(cap->data != NULL);
	cap->data[0] = '\0';
	assert(pipe(pipefd) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->fd = pipefd[0];
	assert(dup2(pipefd[1], STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(pipefd[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, read_pipe, cap) == 0);
}

static void end_capture(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(cap->saved) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->fd) == 0);
}

static void assert_log_lines(t_capture *cap)
{
	static const char *states[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring"};
	char *cursor;
	char *end;
	char expected[128];
	long ts;
	long previous;
	int id;
	int offset;
	int state;
	int length;

	cursor = cap->data;
	previous = -1;
	assert(*cursor != '\0');
	while (*cursor)
	{
		end = strchr(cursor, '\n');
		assert(end != NULL);
		*end = '\0';
		offset = -1;
		assert(sscanf(cursor, "%ld %d %n", &ts, &id, &offset) == 2);
		assert(offset > 0 && id >= 1 && id <= 4 && ts >= previous);
		for (state = 0; state < 4; ++state)
			if (strcmp(cursor + offset, states[state]) == 0)
				break;
		assert(state < 4);
		length = snprintf(expected, sizeof(expected), "%ld %d %s", ts,
			id, states[state]);
		assert(length > 0 && (size_t)length < sizeof(expected));
		assert(strcmp(cursor, expected) == 0);
		previous = ts;
		cursor = end + 1;
	}
}

static t_config get_coder_config(void)
{
	t_config config;

	config.number_of_coders = 4;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 100;
	config.time_to_refactor = 100;
	config.number_of_compiles_required = 50;
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
	return (config);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

/* AC5: coder threads make progress, preserve logs, and stop cleanly. */
static void test_ac5_coder_behavior(void)
{
	t_sim sim;
	t_config config;
	t_capture cap;
	pthread_t threads[4];
	void *result;
	long stop_ms;
	long joined_ms;
	int i;
	int total;

	assert(alarm(5) == 0);
	config = get_coder_config();
	assert(init_simulation(&sim, &config) == 0);
	start_capture(&cap);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	for (i = 0; i < 4; ++i)
		assert(pthread_create(&threads[i], NULL, run_coder, &sim.coders[i]) == 0);
	assert(usleep(700000) == 0);
	stop_ms = get_time_ms();
	assert(stop_ms >= 0);
	assert(set_stopped(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	for (i = 0; i < 4; ++i)
	{
		result = (void *)1;
		assert(pthread_join(threads[i], &result) == 0);
		assert(result == NULL);
	}
	joined_ms = get_time_ms();
	assert(joined_ms >= stop_ms);
	assert(joined_ms <= stop_ms + 250 + get_timing_slack_ms());
	end_capture(&cap);
	assert_log_lines(&cap);
	total = 0;
	for (i = 0; i < 4; ++i)
	{
		total += sim.coders[i].compile_count;
		assert_unlocked(&sim.coders[i].mutex);
	}
	assert(total >= 2);
	assert_unlocked(&sim.stop_mutex);
	assert_unlocked(&sim.log_mutex);
	assert(free_sim(&sim) == 0);
	free(cap.data);
	assert(alarm(0) != 0);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC6: the deliverable builds, defines sleep_ms, and passes Norm. */
static void test_ac6_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]sleep_ms$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_nonpositive_duration();
	test_ac2_running_duration();
	test_ac3_already_stopped();
	test_ac4_concurrent_stop();
	test_ac5_coder_behavior();
	test_ac6_build_symbol_and_norm();
	return (0);
}
