#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct s_capture
{
	int		saved;
	int		saved_err;
	int		read_fd;
	FILE		*err;
	pthread_t	reader;
	char		*data;
	size_t		len;
	size_t		cap;
} t_capture;

static void init_sim(t_sim *sim, int errorcheck)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_t *use;

	use = NULL;
	if (errorcheck)
	{
		assert(pthread_mutexattr_init(&attr) == 0);
		assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
		use = &attr;
	}
	sim->start_ms = get_time_ms() - 100;
	assert(sim->start_ms >= 0);
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->log_mutex, use) == 0);
	assert(pthread_mutex_init(&sim->stop_mutex, use) == 0);
	if (errorcheck)
		assert(pthread_mutexattr_destroy(&attr) == 0);
}

static void assert_unlocked(t_sim *sim)
{
	assert(pthread_mutex_trylock(&sim->log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->log_mutex) == 0);
	assert(pthread_mutex_trylock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->stop_mutex) == 0);
}

static void destroy_sim(t_sim *sim)
{
	assert_unlocked(sim);
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
}

static void *read_capture(void *arg)
{
	t_capture *cap;
	ssize_t n;

	cap = arg;
	cap->cap = 4096;
	cap->data = malloc(cap->cap);
	assert(cap->data != NULL);
	while (1)
	{
		if (cap->cap - cap->len < 1024)
		{
			cap->cap *= 2;
			cap->data = realloc(cap->data, cap->cap);
			assert(cap->data != NULL);
		}
		n = read(cap->read_fd, cap->data + cap->len, cap->cap - cap->len - 1);
		if (n < 0 && errno == EINTR)
			continue;
		assert(n >= 0);
		if (n == 0)
			break;
		cap->len += (size_t)n;
	}
	cap->data[cap->len] = '\0';
	return (NULL);
}

static void start_capture(t_capture *cap)
{
	int fds[2];

	memset(cap, 0, sizeof(*cap));
	assert(pipe(fds) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->saved_err = dup(STDERR_FILENO);
	assert(cap->saved_err >= 0);
	cap->err = tmpfile();
	assert(cap->err != NULL);
	cap->read_fd = fds[0];
	assert(dup2(fds[1], STDOUT_FILENO) >= 0);
	assert(dup2(fileno(cap->err), STDERR_FILENO) >= 0);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, read_capture, cap) == 0);
}

static void stop_capture(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) >= 0);
	assert(dup2(cap->saved_err, STDERR_FILENO) >= 0);
	assert(close(cap->saved) == 0);
	assert(close(cap->saved_err) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->read_fd) == 0);
	assert(fseek(cap->err, 0, SEEK_END) == 0);
	assert(ftell(cap->err) == 0);
	assert(fclose(cap->err) == 0);
}

static const char *state_text(int state)
{
	static const char *texts[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring"};

	assert(state >= 0 && state < 4);
	return (texts[state]);
}

static void assert_line(const char *line, size_t len, int coder, int state,
	long *timestamp)
{
	char expected[LOG_LINE_MAX + 1];
	int offset;
	int parsed_coder;
	int expected_len;
	long parsed_ts;

	offset = 0;
	assert(sscanf(line, "%ld %d %n", &parsed_ts, &parsed_coder, &offset) == 2);
	assert(parsed_ts >= 0 && parsed_coder == coder);
	expected_len = snprintf(expected, sizeof(expected), "%ld %d %s\n",
		parsed_ts, coder, state_text(state));
	assert(expected_len > 0 && (size_t)expected_len == len);
	assert(memcmp(line, expected, len) == 0);
	if (timestamp != NULL)
		*timestamp = parsed_ts;
}

static void assert_one_line(t_capture *cap, int coder, int state)
{
	assert(cap->len > 0);
	assert(cap->data[cap->len - 1] == '\n');
	assert(memchr(cap->data, '\n', cap->len - 1) == NULL);
	assert_line(cap->data, cap->len, coder, state, NULL);
}

/* AC1: four states and coder IDs emit exact stdout lines and release locks. */
static void test_ac1_state_lines_and_unlocks(void)
{
	t_sim sim;
	t_capture cap;
	int state;
	int coder;

	state = 0;
	while (state < 4)
	{
		coder = (state % 2) ? 42 : 1;
		init_sim(&sim, 0);
		start_capture(&cap);
		assert(print_state(&sim, coder, (t_state)state) == 0);
		stop_capture(&cap);
		assert_one_line(&cap, coder, state);
		assert(sim.stopped == 0);
		destroy_sim(&sim);
		free(cap.data);
		state++;
	}
}

/* AC2: the complete line is readable immediately, without flushing. */
static void test_ac2_unbuffered_pipe_visibility(void)
{
	t_sim sim;
	int fds[2];
	int saved;
	char line[LOG_LINE_MAX + 1];
	size_t len;
	ssize_t n;

	init_sim(&sim, 0);
	assert(pipe(fds) == 0);
	assert(fcntl(fds[0], F_SETFL, O_NONBLOCK) == 0);
	saved = dup(STDOUT_FILENO);
	assert(saved >= 0 && dup2(fds[1], STDOUT_FILENO) >= 0);
	assert(close(fds[1]) == 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == 0);
	len = 0;
	while (len < sizeof(line) - 1)
	{
		n = read(fds[0], line + len, sizeof(line) - 1 - len);
		if (n < 0 && errno == EINTR)
			continue;
		assert(n > 0);
		len += (size_t)n;
		if (line[len - 1] == '\n')
			break;
	}
	assert(dup2(saved, STDOUT_FILENO) >= 0);
	assert(close(saved) == 0 && close(fds[0]) == 0);
	assert_line(line, len, 1, STATE_COMPILING, NULL);
	destroy_sim(&sim);
}

/* AC3: the printed timestamp lies between the before and after clocks. */
static void test_ac3_timestamp_bounds(void)
{
	t_sim sim;
	t_capture cap;
	long before;
	long after;
	long ts;

	init_sim(&sim, 0);
	before = get_time_ms();
	assert(before >= 100);
	sim.start_ms = before - 100;
	start_capture(&cap);
	assert(print_state(&sim, 1, STATE_COMPILING) == 0);
	after = get_time_ms();
	stop_capture(&cap);
	assert_one_line(&cap, 1, STATE_COMPILING);
	assert_line(cap.data, cap.len, 1, STATE_COMPILING, &ts);
	assert(ts >= 100 && ts <= after - sim.start_ms);
	destroy_sim(&sim);
	free(cap.data);
}

/* AC4: a stopped simulation rejects every state without output. */
static void test_ac4_stopped_rejects_all_states(void)
{
	t_sim sim;
	t_capture cap;
	int state;

	init_sim(&sim, 0);
	assert(set_stopped(&sim) == 0);
	start_capture(&cap);
	state = 0;
	while (state < 4)
		assert(print_state(&sim, 1, (t_state)state++) == -1);
	stop_capture(&cap);
	assert(cap.len == 0);
	destroy_sim(&sim);
	free(cap.data);
}

/* AC5: invalid states and coder IDs are rejected without changing stop. */
static void test_ac5_invalid_inputs(void)
{
	t_sim sim;
	t_capture cap;

	init_sim(&sim, 0);
	start_capture(&cap);
	assert(print_state(&sim, 1, (t_state)-1) == -1);
	assert(print_state(&sim, 1, (t_state)4) == -1);
	assert(print_state(&sim, 0, STATE_COMPILING) == -1);
	assert(print_state(&sim, -3, STATE_COMPILING) == -1);
	stop_capture(&cap);
	assert(cap.len == 0 && sim.stopped == 0);
	destroy_sim(&sim);
	free(cap.data);
}

typedef struct s_worker
{
	t_sim *sim;
	int coder;
	int successes;
	int state_counts[4];
} t_worker;

static void *write_500(void *arg)
{
	t_worker *worker;
	int i;
	int state;

	worker = arg;
	i = 0;
	while (i < 500)
	{
		state = (i + worker->coder) % 4;
		assert(print_state(worker->sim, worker->coder, (t_state)state) == 0);
		worker->state_counts[state]++;
		i++;
	}
	return (NULL);
}

static void assert_concurrent_lines(t_capture *cap, int counts[8][4],
	int expected_lines)
{
	const char *cursor;
	const char *end;
	long previous;
	long ts;
	int coder;
	int state;
	int lines;
	int offset;

	cursor = cap->data;
	previous = -1;
	lines = 0;
	while (*cursor != '\0')
	{
		end = strchr(cursor, '\n');
		assert(end != NULL);
		offset = 0;
		assert(sscanf(cursor, "%ld %d %n", &ts, &coder, &offset) == 2);
		assert(coder >= 1 && coder <= 8);
		state = 0;
		while (state < 4)
		{
			char expected[LOG_LINE_MAX + 1];
			int length;

			length = snprintf(expected, sizeof(expected), "%ld %d %s\n",
				ts, coder, state_text(state));
			if (length == end - cursor + 1 &&
				memcmp(cursor, expected, (size_t)length) == 0)
				break;
			state++;
		}
		assert(state < 4);
		assert_line(cursor, (size_t)(end - cursor + 1), coder, state, &ts);
		assert(ts >= previous);
		previous = ts;
		counts[coder - 1][state]++;
		lines++;
		cursor = end + 1;
	}
	assert(lines == expected_lines);
}

/* AC6: eight writers produce 4000 intact, ordered, countable lines. */
static void test_ac6_serialized_parallel_writers(void)
{
	t_sim sim;
	t_capture cap;
	t_worker workers[8];
	pthread_t threads[8];
	int counts[8][4];
	int i;
	int state;

	alarm(30);
	memset(workers, 0, sizeof(workers));
	memset(counts, 0, sizeof(counts));
	init_sim(&sim, 0);
	start_capture(&cap);
	i = 0;
	while (i < 8)
	{
		workers[i].sim = &sim;
		workers[i].coder = i + 1;
		assert(pthread_create(&threads[i], NULL, write_500, &workers[i]) == 0);
		i++;
	}
	i = 0;
	while (i < 8)
		assert(pthread_join(threads[i++], NULL) == 0);
	stop_capture(&cap);
	assert_concurrent_lines(&cap, counts, 4000);
	i = 0;
	while (i < 8)
	{
		state = 0;
		while (state < 4)
		{
			assert(workers[i].state_counts[state] == 125);
			assert(counts[i][state] == 125);
			state++;
		}
		i++;
	}
	destroy_sim(&sim);
	free(cap.data);
	alarm(0);
}

typedef struct s_stop_run
{
	t_sim sim;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_barrier_t start;
	pthread_barrier_t after_stop;
	int ready[4];
	int successes[4];
} t_stop_run;

typedef struct s_stop_worker
{
	t_stop_run *run;
	int index;
} t_stop_worker;

static void *write_until_stopped(void *arg)
{
	t_stop_worker *worker;
	t_stop_run *run;
	int index;
	int result;

	worker = arg;
	run = worker->run;
	index = worker->index;
	assert(print_state(&run->sim, index + 1, STATE_COMPILING) == 0);
	assert(pthread_mutex_lock(&run->mutex) == 0);
	run->successes[index]++;
	run->ready[index] = 1;
	assert(pthread_cond_broadcast(&run->cond) == 0);
	assert(pthread_mutex_unlock(&run->mutex) == 0);
	result = pthread_barrier_wait(&run->start);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	while (1)
	{
		result = print_state(&run->sim, index + 1, STATE_DEBUGGING);
		if (result == -1)
			break;
		assert(pthread_mutex_lock(&run->mutex) == 0);
		run->successes[index]++;
		assert(pthread_cond_broadcast(&run->cond) == 0);
		assert(pthread_mutex_unlock(&run->mutex) == 0);
	}
	result = pthread_barrier_wait(&run->after_stop);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	assert(print_state(&run->sim, index + 1, STATE_REFACTORING) == -1);
	return (NULL);
}

/* AC7: stop under load suppresses every post-stop call and counts all lines. */
static void test_ac7_stop_under_load(void)
{
	t_stop_run run;
	t_stop_worker workers[4];
	pthread_t threads[4];
	t_capture cap;
	int counts[8][4];
	int i;
	int total;
	int barrier_result;

	alarm(30);
	memset(&run, 0, sizeof(run));
	memset(counts, 0, sizeof(counts));
	init_sim(&run.sim, 0);
	assert(pthread_mutex_init(&run.mutex, NULL) == 0);
	assert(pthread_cond_init(&run.cond, NULL) == 0);
	assert(pthread_barrier_init(&run.start, NULL, 5) == 0);
	assert(pthread_barrier_init(&run.after_stop, NULL, 5) == 0);
	start_capture(&cap);
	i = 0;
	while (i < 4)
	{
		workers[i].run = &run;
		workers[i].index = i;
		assert(pthread_create(&threads[i], NULL, write_until_stopped,
			&workers[i]) == 0);
		i++;
	}
	assert(pthread_mutex_lock(&run.mutex) == 0);
	while (!run.ready[0] || !run.ready[1] || !run.ready[2] || !run.ready[3])
		assert(pthread_cond_wait(&run.cond, &run.mutex) == 0);
	assert(pthread_mutex_unlock(&run.mutex) == 0);
	barrier_result = pthread_barrier_wait(&run.start);
	assert(barrier_result == 0 || barrier_result == PTHREAD_BARRIER_SERIAL_THREAD);
	assert(pthread_mutex_lock(&run.mutex) == 0);
	while (run.successes[0] < 2 || run.successes[1] < 2 ||
		run.successes[2] < 2 || run.successes[3] < 2)
		assert(pthread_cond_wait(&run.cond, &run.mutex) == 0);
	assert(pthread_mutex_unlock(&run.mutex) == 0);
	assert(set_stopped(&run.sim) == 0);
	barrier_result = pthread_barrier_wait(&run.after_stop);
	assert(barrier_result == 0 || barrier_result == PTHREAD_BARRIER_SERIAL_THREAD);
	i = 0;
	while (i < 4)
		assert(pthread_join(threads[i++], NULL) == 0);
	stop_capture(&cap);
	total = 0;
	i = 0;
	while (i < 4)
		total += run.successes[i++];
	assert_concurrent_lines(&cap, counts, total);
	i = 0;
	while (i < 4)
	{
		assert(counts[i][STATE_COMPILING] == 1);
		assert(counts[i][STATE_DEBUGGING] == run.successes[i] - 1);
		i++;
	}
	assert(pthread_barrier_destroy(&run.start) == 0);
	assert(pthread_barrier_destroy(&run.after_stop) == 0);
	assert(pthread_cond_destroy(&run.cond) == 0);
	assert(pthread_mutex_destroy(&run.mutex) == 0);
	destroy_sim(&run.sim);
	free(cap.data);
	alarm(0);
}

/* AC8: failed recursive locks preserve ownership and write nothing. */
static void test_ac8_lock_failure_postconditions(void)
{
	t_sim sim;
	t_capture cap;

	init_sim(&sim, 1);
	start_capture(&cap);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == -1);
	assert(sim.stopped == 0);
	assert(pthread_mutex_trylock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == -1);
	assert(sim.stopped == 0);
	assert(pthread_mutex_trylock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	stop_capture(&cap);
	assert(cap.len == 0);
	destroy_sim(&sim);
	free(cap.data);
}

/* AC9: /dev/full makes the call fail without changing stop or retaining locks. */
static void test_ac9_write_error(void)
{
	pid_t child;
	int status;
	int full;
	t_sim sim;

	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		full = open("/dev/full", O_WRONLY);
		assert(full >= 0);
		assert(dup2(full, STDOUT_FILENO) >= 0);
		assert(close(full) == 0);
		init_sim(&sim, 0);
		assert(print_state(&sim, 1, STATE_COMPILING) == -1);
		assert(sim.stopped == 0);
		destroy_sim(&sim);
		_exit(0);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC10: the deliverable builds, exports print_state, and passes Norm. */
static void test_ac10_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]print_state$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_state_lines_and_unlocks();
	test_ac2_unbuffered_pipe_visibility();
	test_ac3_timestamp_bounds();
	test_ac4_stopped_rejects_all_states();
	test_ac5_invalid_inputs();
	test_ac6_serialized_parallel_writers();
	test_ac7_stop_under_load();
	test_ac8_lock_failure_postconditions();
	test_ac9_write_error();
	test_ac10_build_symbol_and_norm();
	return (0);
}
