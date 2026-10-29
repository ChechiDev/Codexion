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
	int saved;
	int saved_err;
	int read_fd;
	FILE *err;
	pthread_t reader;
	char *data;
	size_t len;
	size_t cap;
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
	char *grown;

	cap = arg;
	cap->cap = 4096;
	cap->data = malloc(cap->cap);
	assert(cap->data != NULL);
	while (1)
	{
		if (cap->cap - cap->len < 1024)
		{
			cap->cap *= 2;
			grown = realloc(cap->data, cap->cap);
			assert(grown != NULL);
			cap->data = grown;
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

static void assert_line(const char *line, size_t len, int coder,
	const char *text, long *timestamp)
{
	char expected[LOG_LINE_MAX + 1];
	long ts;
	int got_coder;
	int offset;
	int expected_len;

	offset = 0;
	assert(sscanf(line, "%ld %d %n", &ts, &got_coder, &offset) == 2);
	assert(ts >= 0 && got_coder == coder);
	expected_len = snprintf(expected, sizeof(expected), "%ld %d %s\n",
		ts, coder, text);
	assert(expected_len > 0 && (size_t)expected_len == len);
	assert(memcmp(line, expected, len) == 0);
	if (timestamp != NULL)
		*timestamp = ts;
}

static void assert_only_burnout(t_capture *cap, int coder, long *ts)
{
	assert(cap->len > 0);
	assert(cap->data[cap->len - 1] == '\n');
	assert(memchr(cap->data, '\n', cap->len - 1) == NULL);
	assert_line(cap->data, cap->len, coder, "burned out", ts);
}

/* AC1: one exact stdout line, no stderr, first stop, and released locks. */
static void test_ac1_line_stop_and_unlocks(void)
{
	t_sim sim;
	t_capture cap;

	init_sim(&sim, 0);
	start_capture(&cap);
	assert(print_burnout(&sim, 42) == 0);
	stop_capture(&cap);
	assert_only_burnout(&cap, 42, NULL);
	assert(sim.stopped == 1);
	destroy_sim(&sim);
	free(cap.data);
}

/* AC2: the entire line is immediately readable from a nonblocking pipe. */
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
	assert(print_burnout(&sim, 1) == 0);
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
	assert_line(line, len, 1, "burned out", NULL);
	destroy_sim(&sim);
}

/* AC3: the timestamp falls between the before and after wall clocks. */
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
	assert(print_burnout(&sim, 1) == 0);
	after = get_time_ms();
	stop_capture(&cap);
	assert_only_burnout(&cap, 1, &ts);
	assert(ts >= 100 && ts <= after - sim.start_ms);
	destroy_sim(&sim);
	free(cap.data);
}

/* AC4: both ways of reaching an already stopped run reject the call. */
static void test_ac4_already_stopped(void)
{
	t_sim sim;
	t_capture cap;
	int i;

	i = 0;
	while (i < 2)
	{
		init_sim(&sim, 0);
		if (i == 0)
			assert(set_stopped(&sim) == 0);
		else
		{
			start_capture(&cap);
			assert(print_burnout(&sim, 1) == 0);
			stop_capture(&cap);
			assert_only_burnout(&cap, 1, NULL);
			free(cap.data);
		}
		start_capture(&cap);
		assert(print_burnout(&sim, 2) == -1);
		stop_capture(&cap);
		assert(cap.len == 0 && sim.stopped == 1);
		destroy_sim(&sim);
		free(cap.data);
		i++;
	}
}

/* AC5: set_stopped is silent and idempotent after the burnout wins. */
static void test_ac5_reverse_stop_protocol(void)
{
	t_sim sim;
	t_capture cap;

	init_sim(&sim, 0);
	start_capture(&cap);
	assert(print_burnout(&sim, 1) == 0);
	stop_capture(&cap);
	assert_only_burnout(&cap, 1, NULL);
	free(cap.data);
	start_capture(&cap);
	assert(set_stopped(&sim) == 0);
	assert(print_burnout(&sim, 2) == -1);
	stop_capture(&cap);
	assert(cap.len == 0 && sim.stopped == 1);
	destroy_sim(&sim);
	free(cap.data);
}

/* AC6: every state logger rejects a call after the burnout. */
static void test_ac6_no_states_after_burnout(void)
{
	t_sim sim;
	t_capture cap;
	int state;

	init_sim(&sim, 0);
	start_capture(&cap);
	assert(print_burnout(&sim, 1) == 0);
	stop_capture(&cap);
	free(cap.data);
	start_capture(&cap);
	state = 0;
	while (state < 4)
		assert(print_state(&sim, 2, (t_state)state++) == -1);
	stop_capture(&cap);
	assert(cap.len == 0);
	destroy_sim(&sim);
	free(cap.data);
}

typedef struct s_burn_worker
{
	t_sim *sim;
	pthread_barrier_t *ready;
	int coder;
	int result;
} t_burn_worker;

static void *race_burnout(void *arg)
{
	t_burn_worker *worker;
	int result;

	worker = arg;
	result = pthread_barrier_wait(worker->ready);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	worker->result = print_burnout(worker->sim, worker->coder);
	return (NULL);
}

/* AC7: eight simultaneous callers produce exactly one winning line. */
static void test_ac7_single_burnout_race(void)
{
	t_sim sim;
	t_capture cap;
	t_burn_worker workers[8];
	pthread_t threads[8];
	pthread_barrier_t ready;
	int i;
	int winner;

	alarm(30);
	init_sim(&sim, 0);
	assert(pthread_barrier_init(&ready, NULL, 9) == 0);
	start_capture(&cap);
	i = 0;
	while (i < 8)
	{
		workers[i].sim = &sim;
		workers[i].ready = &ready;
		workers[i].coder = i + 1;
		assert(pthread_create(&threads[i], NULL, race_burnout, &workers[i]) == 0);
		i++;
	}
	i = pthread_barrier_wait(&ready);
	assert(i == 0 || i == PTHREAD_BARRIER_SERIAL_THREAD);
	i = 0;
	while (i < 8)
		assert(pthread_join(threads[i++], NULL) == 0);
	stop_capture(&cap);
	winner = 0;
	i = 0;
	while (i < 8)
	{
		assert(workers[i].result == 0 || workers[i].result == -1);
		if (workers[i].result == 0)
		{
			assert(winner == 0);
			winner = workers[i].coder;
		}
		i++;
	}
	assert(winner != 0);
	assert_only_burnout(&cap, winner, NULL);
	assert(sim.stopped == 1);
	assert(pthread_barrier_destroy(&ready) == 0);
	destroy_sim(&sim);
	free(cap.data);
	alarm(0);
}

typedef struct s_state_run
{
	t_sim sim;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_barrier_t begin;
	pthread_barrier_t after_burn;
	int ready[4];
	int successes[4];
} t_state_run;

typedef struct s_state_worker
{
	t_state_run *run;
	int index;
} t_state_worker;

static void *write_until_burnout(void *arg)
{
	t_state_worker *worker;
	t_state_run *run;
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
	result = pthread_barrier_wait(&run->begin);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	while (1)
	{
		result = print_state(&run->sim, index + 1, STATE_DEBUGGING);
		if (result == -1)
			break;
		assert(result == 0);
		assert(pthread_mutex_lock(&run->mutex) == 0);
		run->successes[index]++;
		assert(pthread_cond_broadcast(&run->cond) == 0);
		assert(pthread_mutex_unlock(&run->mutex) == 0);
	}
	result = pthread_barrier_wait(&run->after_burn);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	assert(print_state(&run->sim, index + 1, STATE_REFACTORING) == -1);
	return (NULL);
}

static int count_lines_and_check_terminal(t_capture *cap)
{
	const char *cursor;
	const char *end;
	char expected[LOG_LINE_MAX + 1];
	long ts;
	int coder;
	int offset;
	int len;
	int count;
	int state;
	const char *texts[4] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring"};

	cursor = cap->data;
	count = 0;
	while (*cursor != '\0')
	{
		end = strchr(cursor, '\n');
		assert(end != NULL);
		offset = 0;
		assert(sscanf(cursor, "%ld %d %n", &ts, &coder, &offset) == 2);
		assert(ts >= 0 && coder >= 1);
		if (end[1] == '\0')
			assert_line(cursor, (size_t)(end - cursor + 1), 5,
				"burned out", NULL);
		else
		{
			assert(coder <= 4);
			state = 0;
			while (state < 4)
			{
				len = snprintf(expected, sizeof(expected), "%ld %d %s\n",
					ts, coder, texts[state]);
				if (len == end - cursor + 1 &&
					memcmp(cursor, expected, (size_t)len) == 0)
					break;
				state++;
			}
			assert(state < 4);
		}
		count++;
		cursor = end + 1;
	}
	assert(count > 0);
	return (count);
}

/* AC8: state lines precede the sole terminal line and match success counts. */
static void test_ac8_burnout_serializes_against_states(void)
{
	t_state_run run;
	t_state_worker workers[4];
	pthread_t threads[4];
	t_capture cap;
	int i;
	int total;
	int result;

	alarm(30);
	memset(&run, 0, sizeof(run));
	init_sim(&run.sim, 0);
	assert(pthread_mutex_init(&run.mutex, NULL) == 0);
	assert(pthread_cond_init(&run.cond, NULL) == 0);
	assert(pthread_barrier_init(&run.begin, NULL, 5) == 0);
	assert(pthread_barrier_init(&run.after_burn, NULL, 5) == 0);
	start_capture(&cap);
	i = 0;
	while (i < 4)
	{
		workers[i].run = &run;
		workers[i].index = i;
		assert(pthread_create(&threads[i], NULL, write_until_burnout,
			&workers[i]) == 0);
		i++;
	}
	assert(pthread_mutex_lock(&run.mutex) == 0);
	while (!run.ready[0] || !run.ready[1] || !run.ready[2] || !run.ready[3])
		assert(pthread_cond_wait(&run.cond, &run.mutex) == 0);
	assert(pthread_mutex_unlock(&run.mutex) == 0);
	result = pthread_barrier_wait(&run.begin);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	assert(pthread_mutex_lock(&run.mutex) == 0);
	while (run.successes[0] < 2 || run.successes[1] < 2 ||
		run.successes[2] < 2 || run.successes[3] < 2)
		assert(pthread_cond_wait(&run.cond, &run.mutex) == 0);
	assert(pthread_mutex_unlock(&run.mutex) == 0);
	assert(print_burnout(&run.sim, 5) == 0);
	result = pthread_barrier_wait(&run.after_burn);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	i = 0;
	while (i < 4)
		assert(pthread_join(threads[i++], NULL) == 0);
	stop_capture(&cap);
	total = 1;
	i = 0;
	while (i < 4)
		total += run.successes[i++];
	assert(count_lines_and_check_terminal(&cap) == total);
	assert(run.sim.stopped == 1);
	assert(pthread_barrier_destroy(&run.begin) == 0);
	assert(pthread_barrier_destroy(&run.after_burn) == 0);
	assert(pthread_cond_destroy(&run.cond) == 0);
	assert(pthread_mutex_destroy(&run.mutex) == 0);
	destroy_sim(&run.sim);
	free(cap.data);
	alarm(0);
}

/* AC9: invalid coder IDs commit the stop but produce no output. */
static void test_ac9_rejected_coder_commits_stop(void)
{
	t_sim sim;
	t_capture cap;
	int coders[2] = {0, -3};
	int i;

	i = 0;
	while (i < 2)
	{
		init_sim(&sim, 0);
		start_capture(&cap);
		assert(print_burnout(&sim, coders[i]) == -1);
		stop_capture(&cap);
		assert(cap.len == 0 && sim.stopped == 1);
		destroy_sim(&sim);
		free(cap.data);
		i++;
	}
}

/* AC10: failed recursive locks leave stop clear and caller ownership intact. */
static void test_ac10_lock_failure_postconditions(void)
{
	t_sim sim;
	t_capture cap;

	init_sim(&sim, 1);
	start_capture(&cap);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(print_burnout(&sim, 1) == -1);
	assert(sim.stopped == 0);
	assert(pthread_mutex_trylock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(print_burnout(&sim, 1) == -1);
	assert(sim.stopped == 0);
	assert(pthread_mutex_trylock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	stop_capture(&cap);
	assert(cap.len == 0);
	destroy_sim(&sim);
	free(cap.data);
}

/* AC11: a failed write keeps the committed stop and releases both locks. */
static void test_ac11_write_error(void)
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
		assert(print_burnout(&sim, 1) == -1);
		assert(sim.stopped == 1);
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

/* AC12: the deliverable builds, exports print_burnout, and passes Norm. */
static void test_ac12_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]print_burnout$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_line_stop_and_unlocks();
	test_ac2_unbuffered_pipe_visibility();
	test_ac3_timestamp_bounds();
	test_ac4_already_stopped();
	test_ac5_reverse_stop_protocol();
	test_ac6_no_states_after_burnout();
	test_ac7_single_burnout_race();
	test_ac8_burnout_serializes_against_states();
	test_ac9_rejected_coder_commits_stop();
	test_ac10_lock_failure_postconditions();
	test_ac11_write_error();
	test_ac12_build_symbol_and_norm();
	return (0);
}
