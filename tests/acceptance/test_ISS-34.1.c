#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct s_capture
{
	int saved_out;
	int saved_err;
	int read_fd;
	FILE *err;
	pthread_t reader;
	char *data;
	size_t len;
	size_t cap;
} t_capture;

static void *read_output(void *arg)
{
	t_capture *cap = arg;
	ssize_t n;

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
	cap->saved_out = dup(STDOUT_FILENO);
	cap->saved_err = dup(STDERR_FILENO);
	assert(cap->saved_out >= 0 && cap->saved_err >= 0);
	cap->err = tmpfile();
	assert(cap->err != NULL);
	cap->read_fd = fds[0];
	assert(dup2(fds[1], STDOUT_FILENO) >= 0);
	assert(dup2(fileno(cap->err), STDERR_FILENO) >= 0);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, read_output, cap) == 0);
}

static void stop_capture(t_capture *cap)
{
	assert(dup2(cap->saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(cap->saved_err, STDERR_FILENO) >= 0);
	assert(close(cap->saved_out) == 0 && close(cap->saved_err) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->read_fd) == 0);
	assert(fseek(cap->err, 0, SEEK_END) == 0);
	assert(ftell(cap->err) == 0);
	assert(fclose(cap->err) == 0);
}

static void free_capture(t_capture *cap)
{
	free(cap->data);
}

static t_config config(int debug, int refactor, int required)
{
	t_config cfg;

	cfg.number_of_coders = 4;
	cfg.time_to_burnout = 800;
	cfg.time_to_compile = 200;
	cfg.time_to_debug = debug;
	cfg.time_to_refactor = refactor;
	cfg.number_of_compiles_required = required;
	cfg.dongle_cooldown = 50;
	cfg.mode = MODE_FIFO;
	return (cfg);
}

static void assert_unlocked(t_sim *sim)
{
	assert(pthread_mutex_trylock(&sim->log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->log_mutex) == 0);
	assert(pthread_mutex_trylock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->stop_mutex) == 0);
}

static const char *state_text(int state)
{
	static const char *texts[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring"};

	return (texts[state]);
}

static void assert_line(const char *line, size_t len, int wanted_id,
	int wanted_state, long *previous)
{
	char expected[LOG_LINE_MAX + 1];
	long ts;
	int id;
	int state;
	int n;

	assert(sscanf(line, "%ld %d", &ts, &id) == 2);
	assert(ts >= 0 && id >= 1 && id <= 4);
	if (wanted_id)
		assert(id == wanted_id);
	if (previous)
	{
		assert(ts >= *previous);
		*previous = ts;
	}
	state = 0;
	while (state < 4)
	{
		n = snprintf(expected, sizeof(expected), "%ld %d %s\n", ts, id,
			state_text(state));
		if (n > 0 && (size_t)n == len && memcmp(line, expected, len) == 0)
			break;
		state++;
	}
	assert(state < 4);
	if (wanted_state >= 0)
		assert(state == wanted_state);
}

static int assert_lines(t_capture *cap, int wanted_id, long *previous)
{
	const char *line = cap->data;
	const char *end;
	int count = 0;

	while (*line)
	{
		end = strchr(line, '\n');
		assert(end != NULL);
		assert_line(line, (size_t)(end - line + 1), wanted_id, -1, previous);
		line = end + 1;
		count++;
	}
	return (count);
}

/* AC1: running simulation prints each state once and preserves unlocked state. */
static void test_ac1_running_states(void)
{
	t_sim sim;
	t_capture cap;
	t_config cfg = config(200, 200, 3);
	const char *line;
	const char *end;
	int state;

	assert(init_simulation(&sim, &cfg) == 0);
	sim.start_ms = get_time_ms();
	start_capture(&cap);
	state = 0;
	while (state < 4)
		assert(print_state(&sim, 1, (t_state)state++) == 0);
	stop_capture(&cap);
	line = cap.data;
	state = 0;
	while (state < 4)
	{
		end = strchr(line, '\n');
		assert(end != NULL);
		assert_line(line, (size_t)(end - line + 1), 1, state++, NULL);
		line = end + 1;
	}
	assert(*line == '\0' && sim.stopped == 0);
	assert_unlocked(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
}

/* AC2: every state returns 1 silently after stop and releases both locks. */
static void test_ac2_stopped_states(void)
{
	t_sim sim;
	t_capture cap;
	t_config cfg = config(200, 200, 3);
	int state;

	assert(init_simulation(&sim, &cfg) == 0);
	assert(set_stopped(&sim) == 0);
	start_capture(&cap);
	state = 0;
	while (state < 4)
		assert(print_state(&sim, 1, (t_state)state++) == 1);
	stop_capture(&cap);
	assert(cap.len == 0);
	assert_unlocked(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
}

typedef struct s_stop_worker
{
	t_sim *sim;
	pthread_barrier_t *barrier;
	int id;
	int successes;
} t_stop_worker;

static void *write_until_stop(void *arg)
{
	t_stop_worker *worker = arg;
	int result;

	assert(print_state(worker->sim, worker->id, STATE_COMPILING) == 0);
	worker->successes = 1;
	result = pthread_barrier_wait(worker->barrier);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	while (1)
	{
		result = print_state(worker->sim, worker->id, STATE_DEBUGGING);
		if (result != 0)
		{
			assert(result == 1);
			break;
		}
		worker->successes++;
	}
	return (NULL);
}

/* AC3: concurrent stop returns 1 and line count equals successful calls. */
static void test_ac3_concurrent_stop(void)
{
	t_sim sim;
	t_config cfg = config(200, 200, 3);
	t_capture cap;
	t_stop_worker workers[4];
	pthread_t threads[4];
	pthread_barrier_t barrier;
	int i;
	int result;
	int total = 0;

	assert(init_simulation(&sim, &cfg) == 0);
	sim.start_ms = get_time_ms();
	assert(pthread_barrier_init(&barrier, NULL, 5) == 0);
	start_capture(&cap);
	for (i = 0; i < 4; i++)
	{
		workers[i] = (t_stop_worker){&sim, &barrier, i + 1, 0};
		assert(pthread_create(&threads[i], NULL, write_until_stop,
			&workers[i]) == 0);
	}
	result = pthread_barrier_wait(&barrier);
	assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
	assert(set_stopped(&sim) == 0);
	for (i = 0; i < 4; i++)
	{
		assert(pthread_join(threads[i], NULL) == 0);
		total += workers[i].successes;
	}
	stop_capture(&cap);
	assert(assert_lines(&cap, 0, NULL) == total);
	assert_unlocked(&sim);
	assert(pthread_barrier_destroy(&barrier) == 0);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
}

static void init_lock_sim(t_sim *sim)
{
	pthread_mutexattr_t attr;

	assert(pthread_mutexattr_init(&attr) == 0);
	assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	sim->start_ms = get_time_ms();
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->log_mutex, &attr) == 0);
	assert(pthread_mutex_init(&sim->stop_mutex, &attr) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
}

static void free_lock_sim(t_sim *sim)
{
	assert_unlocked(sim);
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
}

/* AC4: rejected input, recursive locks and write failure remain -1. */
static void test_ac4_failures_remain_errors(void)
{
	t_sim sim;
	t_config cfg = config(200, 200, 3);
	t_capture cap;
	pid_t child;
	int status;
	int full;

	assert(init_simulation(&sim, &cfg) == 0);
	start_capture(&cap);
	assert(print_state(&sim, 1, (t_state)-1) == -1);
	assert(print_state(&sim, 1, (t_state)4) == -1);
	assert(print_state(&sim, 0, STATE_COMPILING) == -1);
	assert(print_state(&sim, -3, STATE_COMPILING) == -1);
	stop_capture(&cap);
	assert(cap.len == 0);
	assert_unlocked(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
	init_lock_sim(&sim);
	start_capture(&cap);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == -1);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == -1);
	assert(pthread_mutex_trylock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	stop_capture(&cap);
	assert(cap.len == 0);
	free_lock_sim(&sim);
	free_capture(&cap);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		full = open("/dev/full", O_WRONLY);
		assert(full >= 0 && dup2(full, STDOUT_FILENO) >= 0);
		assert(close(full) == 0);
		assert(init_simulation(&sim, &cfg) == 0);
		sim.start_ms = get_time_ms();
		assert(print_state(&sim, 1, STATE_COMPILING) == -1);
		assert(free_sim(&sim) == 0);
		_exit(0);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC5: bad state and coder are -1 even when simulation is stopped. */
static void test_ac5_rejected_input_wins(void)
{
	t_sim sim;
	t_config cfg = config(200, 200, 3);
	t_capture cap;

	assert(init_simulation(&sim, &cfg) == 0);
	assert(set_stopped(&sim) == 0);
	start_capture(&cap);
	assert(print_state(&sim, 1, (t_state)-1) == -1);
	assert(print_state(&sim, 1, (t_state)4) == -1);
	assert(print_state(&sim, 0, STATE_COMPILING) == -1);
	assert(print_state(&sim, -3, STATE_COMPILING) == -1);
	stop_capture(&cap);
	assert(cap.len == 0);
	assert_unlocked(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
}

static long time_slack(void)
{
	const char *value = getenv("CODEXION_TIME_SLACK_MS");

	if (value == NULL)
		return (0);
	return (atol(value));
}

/* AC6: public callers exit on stop, preserve resources and valid output. */
static void test_ac6_callers_unchanged(void)
{
	t_sim sim;
	t_config cfg = config(100, 100, 50);
	t_capture cap;
	pthread_t threads[4];
	long stop_at;
	long previous = -1;
	int total = 0;
	int taken = -1;
	int i;
	int first;
	int second;
	struct timespec wait_time = {0, 700000000};

	assert(init_simulation(&sim, &cfg) == 0);
	start_capture(&cap);
	sim.start_ms = get_time_ms();
	for (i = 0; i < 4; i++)
		assert(pthread_create(&threads[i], NULL, run_coder,
			&sim.coders[i]) == 0);
	assert(nanosleep(&wait_time, NULL) == 0);
	stop_at = get_time_ms();
	assert(set_stopped(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	for (i = 0; i < 4; i++)
	{
		assert(pthread_join(threads[i], NULL) == 0);
		assert(pthread_mutex_lock(&sim.coders[i].mutex) == 0);
		total += sim.coders[i].compile_count;
		assert(pthread_mutex_unlock(&sim.coders[i].mutex) == 0);
	}
	assert(get_time_ms() <= stop_at + 250 + time_slack());
	stop_capture(&cap);
	assert(total >= 2);
	assert(assert_lines(&cap, 0, &previous) > 0);
	for (i = 0; i < 4; i++)
	{
		assert(pthread_mutex_trylock(&sim.coders[i].mutex) == 0);
		assert(pthread_mutex_unlock(&sim.coders[i].mutex) == 0);
	}
	assert_unlocked(&sim);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
	assert(init_simulation(&sim, &cfg) == 0);
	assert(set_stopped(&sim) == 0);
	start_capture(&cap);
	assert(take_coder_dongles(&sim.coders[0], &taken) == 0);
	stop_capture(&cap);
	assert(taken == 0 && cap.len == 0);
	first = sim.coders[0].first_dongle;
	second = sim.coders[0].second_dongle;
	assert(sim.dongles[first].available == 1);
	assert(sim.dongles[second].available == 1);
	assert(free_sim(&sim) == 0);
	free_capture(&cap);
}

static void run_command(const char *command)
{
	int status = system(command);

	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC7: deliverable builds, exports print_state and passes Norm. */
static void test_ac7_build_symbol_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]print_state$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	alarm(30);
	test_ac1_running_states();
	test_ac2_stopped_states();
	test_ac3_concurrent_stop();
	test_ac4_failures_remain_errors();
	test_ac5_rejected_input_wins();
	test_ac6_callers_unchanged();
	test_ac7_build_symbol_norm();
	alarm(0);
	return (0);
}
