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
	int read_fd;
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

static void destroy_sim(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
}

static void start_capture(t_capture *cap, int nonblock)
{
	int fds[2];

	assert(pipe(fds) == 0);
	if (nonblock)
		assert(fcntl(fds[0], F_SETFL, O_NONBLOCK) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->read_fd = fds[0];
	assert(dup2(fds[1], STDOUT_FILENO) >= 0);
	assert(close(fds[1]) == 0);
}

static void stop_capture(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) >= 0);
	assert(close(cap->saved) == 0);
}

static size_t read_all(t_capture *cap, char *line, size_t capacity)
{
	size_t len;
	ssize_t n;

	len = 0;
	while (1)
	{
		assert(len < capacity);
		n = read(cap->read_fd, line + len, capacity - len);
		if (n < 0 && errno == EINTR)
			continue;
		assert(n >= 0);
		if (n == 0)
			break;
		len += (size_t)n;
	}
	assert(close(cap->read_fd) == 0);
	return (len);
}

static long assert_line(const char *line, size_t len, int coder,
	const char *text)
{
	char expected[LOG_LINE_MAX + 1];
	long ts;
	int parsed_coder;
	int offset;
	int expected_len;

	assert(len > 0 && line[len - 1] == '\n');
	assert(memchr(line, '\n', len - 1) == NULL);
	offset = 0;
	assert(sscanf(line, "%ld %d %n", &ts, &parsed_coder, &offset) == 2);
	assert(ts >= 0 && parsed_coder == coder);
	expected_len = snprintf(expected, sizeof(expected), "%ld %d %s\n",
		ts, coder, text);
	assert(expected_len > 0 && (size_t)expected_len == len);
	assert(memcmp(line, expected, len) == 0);
	return (ts);
}

/* AC1: one exact stdout line; stop is unchanged and caller keeps log_mutex. */
static void test_ac1_line_and_lock_ownership(void)
{
	t_sim sim;
	t_capture cap;
	char line[LOG_LINE_MAX + 1];
	size_t len;
	int lock_result;

	init_sim(&sim, 1);
	start_capture(&cap, 0);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(print_log_line(&sim, 7, "is compiling") == 0);
	assert(sim.stopped == 0);
	lock_result = pthread_mutex_trylock(&sim.log_mutex);
	assert(lock_result == EDEADLK || lock_result == EBUSY);
	assert(pthread_mutex_trylock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	stop_capture(&cap);
	len = read_all(&cap, line, sizeof(line));
	assert_line(line, len, 7, "is compiling");
	destroy_sim(&sim);
}

/* AC2: timestamp is within the before/after wall-clock interval. */
static void test_ac2_timestamp_bounds(void)
{
	t_sim sim;
	t_capture cap;
	char line[LOG_LINE_MAX + 1];
	long before;
	long after;
	long ts;
	size_t len;

	init_sim(&sim, 0);
	before = get_time_ms();
	assert(before >= 100);
	sim.start_ms = before - 100;
	start_capture(&cap, 0);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(print_log_line(&sim, 7, "is compiling") == 0);
	after = get_time_ms();
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	stop_capture(&cap);
	len = read_all(&cap, line, sizeof(line));
	ts = assert_line(line, len, 7, "is compiling");
	assert(ts >= 100 && ts <= after - sim.start_ms);
	destroy_sim(&sim);
}

/* AC3: the complete line is immediately readable without a flush. */
static void test_ac3_unbuffered_pipe_visibility(void)
{
	t_sim sim;
	t_capture cap;
	char line[LOG_LINE_MAX + 1];
	size_t len;
	ssize_t n;

	init_sim(&sim, 0);
	start_capture(&cap, 1);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(print_log_line(&sim, 7, "is compiling") == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	len = 0;
	while (len < sizeof(line))
	{
		n = read(cap.read_fd, line + len, sizeof(line) - len);
		if (n < 0 && errno == EINTR)
			continue;
		assert(n > 0);
		len += (size_t)n;
		if (line[len - 1] == '\n')
			break;
	}
	stop_capture(&cap);
	assert(close(cap.read_fd) == 0);
	assert_line(line, len, 7, "is compiling");
	destroy_sim(&sim);
}

/* AC4: it takes no lock and writes despite stopped being set. */
static void test_ac4_no_lock_or_stop_check(void)
{
	t_sim sim;
	t_capture cap;
	char line[LOG_LINE_MAX + 1];
	size_t len;

	init_sim(&sim, 0);
	sim.stopped = 1;
	start_capture(&cap, 0);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(print_log_line(&sim, 7, "is compiling") == 0);
	assert(sim.stopped == 1);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	stop_capture(&cap);
	len = read_all(&cap, line, sizeof(line));
	assert_line(line, len, 7, "is compiling");
	destroy_sim(&sim);
}

/* AC5: invalid coder or text returns -1 without output. */
static void test_ac5_invalid_line_rejected(void)
{
	t_sim sim;
	t_capture cap;
	char line[1];

	init_sim(&sim, 0);
	start_capture(&cap, 0);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(print_log_line(&sim, 0, "is compiling") == -1);
	assert(print_log_line(&sim, -3, "is compiling") == -1);
	assert(print_log_line(&sim, 7, "") == -1);
	assert(print_log_line(&sim, 7, "is\ncompiling") == -1);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	stop_capture(&cap);
	assert(read_all(&cap, line, sizeof(line)) == 0);
	destroy_sim(&sim);
}

/* AC6: /dev/full makes the write fail and returns -1. */
static void test_ac6_write_error(void)
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
		assert(full >= 0 && dup2(full, STDOUT_FILENO) >= 0);
		assert(close(full) == 0);
		init_sim(&sim, 0);
		assert(pthread_mutex_lock(&sim.log_mutex) == 0);
		assert(print_log_line(&sim, 7, "is compiling") == -1);
		assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
		destroy_sim(&sim);
		_exit(0);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC7: print_state retains a state line and suppresses post-stop output. */
static void test_ac7_print_state_regression(void)
{
	t_sim sim;
	t_capture cap;
	char line[LOG_LINE_MAX + 1];
	size_t len;

	init_sim(&sim, 0);
	start_capture(&cap, 0);
	assert(print_state(&sim, 7, STATE_COMPILING) == 0);
	stop_capture(&cap);
	len = read_all(&cap, line, sizeof(line));
	assert_line(line, len, 7, "is compiling");
	assert(set_stopped(&sim) == 0);
	start_capture(&cap, 0);
	assert(print_state(&sim, 7, STATE_COMPILING) == -1);
	stop_capture(&cap);
	assert(read_all(&cap, line, sizeof(line)) == 0);
	destroy_sim(&sim);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC8: deliverable builds, exports both functions, and passes Norm. */
static void test_ac8_build_symbols_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]print_log_line$'");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]print_state$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	alarm(30);
	test_ac1_line_and_lock_ownership();
	test_ac2_timestamp_bounds();
	test_ac3_unbuffered_pipe_visibility();
	test_ac4_no_lock_or_stop_check();
	test_ac5_invalid_line_rejected();
	test_ac6_write_error();
	test_ac7_print_state_regression();
	test_ac8_build_symbols_and_norm();
	alarm(0);
	return (0);
}
