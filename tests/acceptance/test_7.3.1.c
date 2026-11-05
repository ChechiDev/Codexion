#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef CODEXION_TIME_SLACK_MS
# define CODEXION_TIME_SLACK_MS 0
#endif

void *run_coder(void *arg);

typedef struct s_capture
{
	int saved;
	int fd;
	pthread_t reader;
	pthread_mutex_t mutex;
	char *data;
	size_t len;
	size_t capacity;
} t_capture;

typedef struct s_line
{
	long ts;
	int coder;
	int state;
} t_line;

typedef struct s_observed
{
	t_line *lines;
	size_t count;
	int states[4][4];
} t_observed;

static t_config get_config(int coders, int compile, int debug, int refactor)
{
	t_config config;

	config.number_of_coders = coders;
	config.time_to_burnout = 10000;
	config.time_to_compile = compile;
	config.time_to_debug = debug;
	config.time_to_refactor = refactor;
	config.number_of_compiles_required = 0;
	config.dongle_cooldown = 0;
	config.mode = MODE_FIFO;
	return (config);
}

static void init_run(t_sim *sim, t_config config)
{
	assert(init_simulation(sim, &config) == 0);
	sim->start_ms = get_time_ms();
	assert(sim->start_ms >= 0);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

static void assert_all_unlocked(t_sim *sim)
{
	int i;

	assert_unlocked(&sim->stop_mutex);
	assert_unlocked(&sim->log_mutex);
	assert_unlocked(&sim->ticket_mutex);
	for (i = 0; i < sim->config.number_of_coders; ++i)
	{
		assert_unlocked(&sim->coders[i].mutex);
		assert_unlocked(&sim->dongles[i].mutex);
	}
}

static void assert_available(t_sim *sim)
{
	int i;

	for (i = 0; i < sim->config.number_of_coders; ++i)
		assert(sim->dongles[i].available == 1);
	assert_all_unlocked(sim);
}

static void *read_pipe(void *arg)
{
	t_capture *cap;
	char chunk[4096];
	ssize_t got;
	char *grown;
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
		assert(pthread_mutex_lock(&cap->mutex) == 0);
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
		assert(pthread_mutex_unlock(&cap->mutex) == 0);
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
	assert(pthread_mutex_init(&cap->mutex, NULL) == 0);
	assert(pipe(pipefd) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->fd = pipefd[0];
	assert(dup2(pipefd[1], STDOUT_FILENO) >= 0);
	assert(close(pipefd[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, read_pipe, cap) == 0);
}

static void end_capture(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) >= 0);
	assert(close(cap->saved) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->fd) == 0);
}

static void free_capture(t_capture *cap)
{
	assert(pthread_mutex_destroy(&cap->mutex) == 0);
	free(cap->data);
}

static int count_text(t_capture *cap, const char *text)
{
	const char *cursor;
	int count;

	assert(pthread_mutex_lock(&cap->mutex) == 0);
	cursor = cap->data;
	count = 0;
	while ((cursor = strstr(cursor, text)) != NULL)
	{
		++count;
		cursor += strlen(text);
	}
	assert(pthread_mutex_unlock(&cap->mutex) == 0);
	return (count);
}

static int count_newlines(t_capture *cap)
{
	size_t i;
	int count;

	assert(pthread_mutex_lock(&cap->mutex) == 0);
	count = 0;
	for (i = 0; i < cap->len; ++i)
		count += cap->data[i] == '\n';
	assert(pthread_mutex_unlock(&cap->mutex) == 0);
	return (count);
}

static void wait_for_text(t_capture *cap, const char *text, int minimum)
{
	while (count_text(cap, text) < minimum)
		usleep(1000);
}

static void wait_for_lines(t_capture *cap, int minimum)
{
	while (count_newlines(cap) < minimum)
		usleep(1000);
}

static void stop_run(t_sim *sim)
{
	assert(set_stopped(sim) == 0);
	assert(wake_dongles(sim) == 0);
}

static void start_coders(t_sim *sim, pthread_t *threads)
{
	int i;

	for (i = 0; i < sim->config.number_of_coders; ++i)
		assert(pthread_create(&threads[i], NULL, run_coder,
			&sim->coders[i]) == 0);
}

static void join_coders(t_sim *sim, pthread_t *threads)
{
	int i;

	for (i = 0; i < sim->config.number_of_coders; ++i)
		assert(pthread_join(threads[i], NULL) == 0);
}

static int get_state_text(const char *text)
{
	static const char *states[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring"};
	int i;

	for (i = 0; i < 4; ++i)
		if (strcmp(text, states[i]) == 0)
			return (i);
	return (-1);
}

static t_observed parse_lines(t_capture *cap, int coders)
{
	t_observed seen;
	char *cursor;
	char *end;
	char *message;
	char expected[128];
	int offset;
	int length;
	long previous;

	memset(&seen, 0, sizeof(seen));
	seen.lines = calloc(cap->len + 1, sizeof(*seen.lines));
	assert(seen.lines != NULL);
	cursor = cap->data;
	previous = -1;
	while (*cursor)
	{
		end = strchr(cursor, '\n');
		assert(end != NULL);
		*end = '\0';
		offset = -1;
		assert(sscanf(cursor, "%ld %d %n", &seen.lines[seen.count].ts,
			&seen.lines[seen.count].coder, &offset) == 2);
		assert(offset > 0);
		message = cursor + offset;
		seen.lines[seen.count].state = get_state_text(message);
		assert(seen.lines[seen.count].state >= 0);
		assert(seen.lines[seen.count].coder >= 1);
		assert(seen.lines[seen.count].coder <= coders);
		assert(seen.lines[seen.count].ts >= previous);
		length = snprintf(expected, sizeof(expected), "%ld %d %s",
			seen.lines[seen.count].ts, seen.lines[seen.count].coder,
			message);
		assert(length > 0 && (size_t)length < sizeof(expected));
		assert(strcmp(cursor, expected) == 0);
		previous = seen.lines[seen.count].ts;
		++seen.states[seen.lines[seen.count].coder - 1]
			[seen.lines[seen.count].state];
		++seen.count;
		cursor = end + 1;
	}
	return (seen);
}

static void assert_coder_cycle(t_observed *seen, int coder)
{
	static const int order[] = {0, 0, 1, 2, 3};
	long previous[5];
	int step;
	int completed;
	size_t i;

	memset(previous, 0, sizeof(previous));
	step = 0;
	completed = 0;
	for (i = 0; i < seen->count; ++i)
	{
		if (seen->lines[i].coder != coder)
			continue;
		assert(seen->lines[i].state == order[step]);
		if (step == 0 && completed)
			assert(seen->lines[i].ts >= previous[4] + 40);
		if (step == 3)
			assert(seen->lines[i].ts >= previous[2] + 50);
		if (step == 4)
			assert(seen->lines[i].ts >= previous[3] + 30);
		previous[step] = seen->lines[i].ts;
		if (step == 4)
			completed = 1;
		step = (step + 1) % 5;
	}
	assert(seen->states[coder - 1][3] >= 2);
}

/* AC1: two coders log intact, ordered cycles with distinct phase durations. */
static void test_ac1_cycle_and_order(void)
{
	t_sim sim;
	t_capture cap;
	t_observed seen;
	pthread_t threads[2];

	alarm(30);
	init_run(&sim, get_config(2, 50, 30, 40));
	start_capture(&cap);
	start_coders(&sim, threads);
	wait_for_text(&cap, " 1 is refactoring\n", 2);
	wait_for_text(&cap, " 2 is refactoring\n", 2);
	stop_run(&sim);
	join_coders(&sim, threads);
	end_capture(&cap);
	seen = parse_lines(&cap, 2);
	assert_coder_cycle(&seen, 1);
	assert_coder_cycle(&seen, 2);
	assert(free_sim(&sim) == 0);
	free(seen.lines);
	free_capture(&cap);
	alarm(0);
}

/* AC2: a shared pair excludes concurrent compiles and records completed work. */
static void test_ac2_shared_pair_and_state(void)
{
	t_sim sim;
	t_capture cap;
	t_observed seen;
	pthread_t threads[2];
	long last_compile;
	long now;
	size_t i;

	alarm(30);
	init_run(&sim, get_config(2, 50, 30, 40));
	start_capture(&cap);
	start_coders(&sim, threads);
	wait_for_text(&cap, " 1 is refactoring\n", 2);
	wait_for_text(&cap, " 2 is refactoring\n", 2);
	stop_run(&sim);
	join_coders(&sim, threads);
	end_capture(&cap);
	seen = parse_lines(&cap, 2);
	last_compile = -1;
	for (i = 0; i < seen.count; ++i)
	{
		if (seen.lines[i].state != STATE_COMPILING)
			continue;
		if (last_compile >= 0)
			assert(seen.lines[i].ts >= last_compile + 50);
		last_compile = seen.lines[i].ts;
	}
	now = get_elapsed_ms(sim.start_ms);
	assert(now >= 0);
	for (i = 0; i < 2; ++i)
	{
		assert(sim.coders[i].compile_count >= seen.states[i][2]);
		assert(sim.coders[i].compile_count <= seen.states[i][1]);
		assert(sim.coders[i].last_compile_start > 0);
		assert(sim.coders[i].last_compile_start <= now);
	}
	assert_available(&sim);
	assert(free_sim(&sim) == 0);
	free(seen.lines);
	free_capture(&cap);
	alarm(0);
}

typedef struct s_single_call
{
	t_coder *coder;
	pthread_mutex_t mutex;
	int done;
} t_single_call;

static void *run_and_mark(void *arg)
{
	t_single_call *call;

	call = arg;
	assert(run_coder(call->coder) == NULL);
	assert(pthread_mutex_lock(&call->mutex) == 0);
	call->done = 1;
	assert(pthread_mutex_unlock(&call->mutex) == 0);
	return (NULL);
}

/* AC3: one coder holds its sole dongle until stop and never compiles. */
static void test_ac3_one_coder(void)
{
	t_sim sim;
	t_capture cap;
	t_observed seen;
	t_single_call call;
	pthread_t thread;
	long first_seen;
	int done;

	alarm(30);
	init_run(&sim, get_config(1, 50, 30, 40));
	call.coder = &sim.coders[0];
	call.done = 0;
	assert(pthread_mutex_init(&call.mutex, NULL) == 0);
	start_capture(&cap);
	assert(pthread_create(&thread, NULL, run_and_mark, &call) == 0);
	wait_for_text(&cap, " 1 has taken a dongle\n", 1);
	first_seen = get_time_ms();
	assert(first_seen >= 0);
	while (get_time_ms() - first_seen < 200)
		usleep(1000);
	assert(pthread_mutex_lock(&sim.dongles[0].mutex) == 0);
	assert(sim.dongles[0].available == 0);
	assert(pthread_mutex_unlock(&sim.dongles[0].mutex) == 0);
	assert(pthread_mutex_lock(&call.mutex) == 0);
	done = call.done;
	assert(pthread_mutex_unlock(&call.mutex) == 0);
	assert(done == 0);
	assert(count_newlines(&cap) == 1);
	stop_run(&sim);
	assert(pthread_join(thread, NULL) == 0);
	end_capture(&cap);
	seen = parse_lines(&cap, 1);
	assert(seen.count == 1);
	assert(seen.lines[0].coder == 1);
	assert(seen.lines[0].state == STATE_TAKEN_DONGLE);
	assert(sim.dongles[0].available == 1);
	assert(sim.coders[0].compile_count == 0);
	assert_all_unlocked(&sim);
	assert(pthread_mutex_destroy(&call.mutex) == 0);
	assert(free_sim(&sim) == 0);
	free(seen.lines);
	free_capture(&cap);
	alarm(0);
}

static void run_prompt_stop_case(int duration)
{
	t_sim sim;
	t_capture cap;
	pthread_t threads[4];
	long stopped_at;
	long joined_at;

	init_run(&sim, get_config(4, duration, duration, duration));
	start_capture(&cap);
	start_coders(&sim, threads);
	if (duration == 0)
		wait_for_lines(&cap, 20);
	else
		wait_for_text(&cap, " is compiling\n", 1);
	stopped_at = get_time_ms();
	assert(stopped_at >= 0);
	stop_run(&sim);
	join_coders(&sim, threads);
	joined_at = get_time_ms();
	assert(joined_at >= stopped_at);
	assert(joined_at - stopped_at <= 100 + CODEXION_TIME_SLACK_MS);
	end_capture(&cap);
	assert_available(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
}

/* AC4: long and zero-length phases both stop every thread promptly. */
static void test_ac4_prompt_stop(void)
{
	alarm(30);
	run_prompt_stop_case(1000);
	run_prompt_stop_case(0);
	alarm(0);
}

/* AC5: stopping during a long compile releases the pair without a count. */
static void test_ac5_interrupted_compile(void)
{
	t_sim sim;
	t_capture cap;
	pthread_t threads[2];
	long stopped_at;

	alarm(30);
	init_run(&sim, get_config(2, 60000, 10, 10));
	sim.config.time_to_burnout = 100000;
	start_capture(&cap);
	start_coders(&sim, threads);
	wait_for_text(&cap, " is compiling\n", 1);
	stopped_at = get_time_ms();
	stop_run(&sim);
	join_coders(&sim, threads);
	assert(get_time_ms() - stopped_at <= 100 + CODEXION_TIME_SLACK_MS);
	end_capture(&cap);
	assert(count_text(&cap, " is debugging\n") == 0);
	assert(sim.coders[0].compile_count == 0);
	assert(sim.coders[1].compile_count == 0);
	assert_available(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
	alarm(0);
}

/* AC6: an already stopped coder returns without output or acquisition. */
static void test_ac6_already_stopped(void)
{
	t_sim sim;
	t_capture cap;

	alarm(30);
	init_run(&sim, get_config(2, 50, 30, 40));
	assert(set_stopped(&sim) == 0);
	start_capture(&cap);
	assert(run_coder(&sim.coders[0]) == NULL);
	end_capture(&cap);
	assert(cap.len == 0);
	assert(sim.coders[0].compile_count == 0);
	assert_available(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
	alarm(0);
}

static void check_full_device_child(void)
{
	t_sim sim;
	int fd;

	init_run(&sim, get_config(2, 50, 30, 40));
	fd = open("/dev/full", O_WRONLY);
	assert(fd >= 0);
	assert(dup2(fd, STDOUT_FILENO) >= 0);
	assert(close(fd) == 0);
	assert(run_coder(&sim.coders[0]) == NULL);
	assert(is_stopped(&sim) == 1);
	assert_available(&sim);
	assert(free_sim(&sim) == 0);
	_exit(0);
}

/* AC7: count overflow, ticket exhaustion, and log failure stop the run. */
static void test_ac7_failure_exit_policy(void)
{
	t_sim sim;
	t_capture cap;
	t_observed seen;
	pid_t child;
	int status;

	alarm(30);
	init_run(&sim, get_config(2, 50, 30, 40));
	sim.coders[0].compile_count = INT_MAX;
	start_capture(&cap);
	assert(run_coder(&sim.coders[0]) == NULL);
	end_capture(&cap);
	assert(is_stopped(&sim) == 1);
	assert(count_text(&cap, " 1 has taken a dongle\n") == 2);
	assert(count_text(&cap, " 1 is compiling\n") == 1);
	assert(count_newlines(&cap) == 3);
	seen = parse_lines(&cap, 2);
	assert(seen.count == 3);
	assert(seen.lines[0].coder == 1 && seen.lines[0].state == STATE_TAKEN_DONGLE);
	assert(seen.lines[1].coder == 1 && seen.lines[1].state == STATE_TAKEN_DONGLE);
	assert(seen.lines[2].coder == 1 && seen.lines[2].state == STATE_COMPILING);
	assert(sim.coders[0].compile_count == INT_MAX);
	assert_available(&sim);
	assert(free_sim(&sim) == 0);
	free(seen.lines);
	free_capture(&cap);
	init_run(&sim, get_config(2, 50, 30, 40));
	sim.next_ticket = LONG_MAX;
	start_capture(&cap);
	assert(run_coder(&sim.coders[0]) == NULL);
	end_capture(&cap);
	assert(cap.len == 0);
	assert(is_stopped(&sim) == 1);
	assert_available(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
	child = fork();
	assert(child >= 0);
	if (child == 0)
		check_full_device_child();
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	alarm(0);
}

/* AC8: invalid pointers return NULL without output or resource changes. */
static void test_ac8_invalid_input(void)
{
	t_sim sim;
	t_capture cap;
	t_coder invalid;

	alarm(30);
	init_run(&sim, get_config(2, 50, 30, 40));
	memset(&invalid, 0, sizeof(invalid));
	start_capture(&cap);
	assert(run_coder(NULL) == NULL);
	assert(run_coder(&invalid) == NULL);
	end_capture(&cap);
	assert(cap.len == 0);
	assert(sim.next_ticket == 0);
	assert_available(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
	alarm(0);
}

int main(void)
{
	test_ac1_cycle_and_order();
	test_ac2_shared_pair_and_state();
	test_ac3_one_coder();
	test_ac4_prompt_stop();
	test_ac5_interrupted_compile();
	test_ac6_already_stopped();
	test_ac7_failure_exit_policy();
	test_ac8_invalid_input();
	return (0);
}
