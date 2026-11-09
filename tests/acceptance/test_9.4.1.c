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

typedef struct s_reader_arg
{
	struct s_capture *cap;
	int channel;
} t_reader_arg;

typedef struct s_capture
{
	int saved[2];
	int read_fd[2];
	pthread_t reader[2];
	char data[2][32768];
	size_t len[2];
	t_reader_arg reader_arg[2];
} t_capture;

static void set_watchdog(void)
{
	if (getenv("CODEXION_UNDER_VALGRIND") != NULL
		|| getenv("TSAN_OPTIONS") != NULL)
		alarm(120);
	else
		alarm(20);
}

static int get_slack(void)
{
	const char *value;
	char *end;
	long slack;

	value = getenv("CODEXION_TIME_SLACK_MS");
	if (value == NULL)
		return (0);
	errno = 0;
	slack = strtol(value, &end, 10);
	assert(errno == 0 && end != value && *end == '\0');
	assert(slack >= 0 && slack <= INT_MAX - 310);
	return ((int)slack);
}

static t_config get_config(int coders, int burnout, int compile,
		int required, t_mode mode)
{
	t_config cfg;

	cfg.number_of_coders = coders;
	cfg.time_to_burnout = burnout;
	cfg.time_to_compile = compile;
	cfg.time_to_debug = 200;
	cfg.time_to_refactor = 200;
	cfg.number_of_compiles_required = required;
	cfg.dongle_cooldown = 50;
	cfg.mode = mode;
	return (cfg);
}

static void *drain_output(void *arg)
{
	t_reader_arg *reader;
	t_capture *cap;
	int channel;
	char buf[256];
	ssize_t n;

	reader = arg;
	cap = reader->cap;
	channel = reader->channel;
	while (1)
	{
		n = read(cap->read_fd[channel], buf, sizeof(buf));
		if (n < 0 && errno == EINTR)
			continue;
		assert(n >= 0);
		if (n == 0)
			break;
		assert(cap->len[channel] + (size_t)n < sizeof(cap->data[channel]));
		memcpy(cap->data[channel] + cap->len[channel], buf, (size_t)n);
		cap->len[channel] += (size_t)n;
	}
	cap->data[channel][cap->len[channel]] = '\0';
	return (NULL);
}

static void capture_start(t_capture *cap)
{
	int fds[2];
	int i;

	memset(cap, 0, sizeof(*cap));
	i = 0;
	while (i < 2)
	{
		assert(pipe(fds) == 0);
		cap->saved[i] = dup(i + STDOUT_FILENO);
		assert(cap->saved[i] >= 0);
		cap->read_fd[i] = fds[0];
		assert(dup2(fds[1], i + STDOUT_FILENO) == i + STDOUT_FILENO);
		assert(close(fds[1]) == 0);
		cap->reader_arg[i].cap = cap;
		cap->reader_arg[i].channel = i;
		assert(pthread_create(&cap->reader[i], NULL, drain_output,
				&cap->reader_arg[i]) == 0);
		i++;
	}
}

static void capture_end(t_capture *cap)
{
	int i;

	i = 0;
	while (i < 2)
	{
		assert(dup2(cap->saved[i], i + STDOUT_FILENO) == i + STDOUT_FILENO);
		assert(close(cap->saved[i]) == 0);
		assert(pthread_join(cap->reader[i], NULL) == 0);
		assert(close(cap->read_fd[i]) == 0);
		i++;
	}
}

static int get_line(const char *line, long *ts, int *id, const char **text)
{
	char prefix[64];
	int used;
	int prefix_len;

	used = -1;
	if (sscanf(line, "%ld %d %n", ts, id, &used) != 2 || used <= 0)
		return (0);
	prefix_len = snprintf(prefix, sizeof(prefix), "%ld %d ", *ts, *id);
	if (prefix_len != used || strncmp(line, prefix, (size_t)used) != 0)
		return (0);
	*text = line + used;
	return (1);
}

static int get_state(const char *text)
{
	if (strcmp(text, "has taken a dongle") == 0)
		return (1);
	if (strcmp(text, "is compiling") == 0)
		return (2);
	if (strcmp(text, "is debugging") == 0)
		return (3);
	if (strcmp(text, "is refactoring") == 0)
		return (4);
	if (strcmp(text, "burned out") == 0)
		return (5);
	return (0);
}

static int inspect_log(const t_capture *cap, int max_id, int *counts,
		int *burned, long *burned_ts)
{
	char copy[sizeof(cap->data[0])];
	char *line;
	char *save;
	const char *state_text;
	long ts;
	long previous;
	int id;
	int state;
	int lines;
	int last_state;
	size_t i;
	int newlines;

	assert(cap->len[0] > 0 && cap->data[0][cap->len[0] - 1] == '\n');
	memcpy(copy, cap->data[0], cap->len[0] + 1);
	newlines = 0;
	for (i = 0; i < cap->len[0]; ++i)
		if (cap->data[0][i] == '\n')
			newlines++;
	previous = -1;
	lines = 0;
	last_state = 0;
	line = strtok_r(copy, "\n", &save);
	while (line != NULL)
	{
		assert(get_line(line, &ts, &id, &state_text));
		assert(ts >= 0 && ts >= previous && id >= 1 && id <= max_id);
		state = get_state(state_text);
		assert(state != 0);
		if (state == 2)
			counts[id]++;
		if (state == 5)
		{
			(*burned)++;
			*burned_ts = ts;
		}
		previous = ts;
		last_state = state;
		lines++;
		line = strtok_r(NULL, "\n", &save);
	}
	assert(lines == newlines);
	return (last_state);
}

/* AC1: zero required compiles returns at once without changing caller storage. */
static void test_ac1_zero_compiles_required(void)
{
	t_config cfg;
	t_sim sim;
	unsigned char before[sizeof(sim)];
	t_capture cap;
	long start;
	long end;

	cfg = get_config(4, 800, 200, 0, MODE_FIFO);
	memset(&sim, 0x5a, sizeof(sim));
	memcpy(before, &sim, sizeof(sim));
	capture_start(&cap);
	start = get_time_ms();
	assert(start >= 0);
	assert(run_simulation(&sim, &cfg) == 0);
	end = get_time_ms();
	capture_end(&cap);
	assert(end >= start && end - start < 50);
	assert(cap.len[0] == 0 && cap.len[1] == 0);
	assert(memcmp(before, &sim, sizeof(sim)) == 0);
}

/* AC2: the FIFO run ends normally after every coder compiles three times. */
static void test_ac2_target_met(void)
{
	t_config cfg;
	t_sim sim;
	t_capture cap;
	int counts[5] = {0};
	int burned;
	long burned_ts;
	int i;

	cfg = get_config(4, 5000, 200, 3, MODE_FIFO);
	capture_start(&cap);
	set_watchdog();
	assert(run_simulation(&sim, &cfg) == 0);
	capture_end(&cap);
	alarm(0);
	assert(cap.len[1] == 0);
	burned = 0;
	inspect_log(&cap, 4, counts, &burned, &burned_ts);
	assert(burned == 0);
	for (i = 1; i <= 4; ++i)
		assert(counts[i] >= 3);
}

/* AC3: an unavoidable burnout is reported once, on time, and on the last line. */
static void test_ac3_burnout(void)
{
	t_config cfg;
	t_sim sim;
	t_capture cap;
	int counts[3] = {0};
	int burned;
	long burned_ts;
	int last_state;

	cfg = get_config(2, 300, 400, 3, MODE_FIFO);
	cfg.time_to_debug = 100;
	cfg.time_to_refactor = 100;
	cfg.dongle_cooldown = 0;
	capture_start(&cap);
	set_watchdog();
	assert(run_simulation(&sim, &cfg) == 0);
	capture_end(&cap);
	alarm(0);
	assert(cap.len[1] == 0);
	burned = 0;
	last_state = inspect_log(&cap, 2, counts, &burned, &burned_ts);
	assert(burned == 1 && last_state == 5);
	assert(burned_ts >= 300 && burned_ts <= 310 + get_slack());
}

/* AC4: one coder takes its single dongle and burns out on time. */
static void test_ac4_single_coder(void)
{
	t_config cfg;
	t_sim sim;
	t_capture cap;
	char copy[sizeof(cap.data[0])];
	char *first;
	char *second;
	char *save;
	const char *state_text;
	long taken_ts;
	long burned_ts;
	int id;

	cfg = get_config(1, 300, 200, 3, MODE_EDF);
	capture_start(&cap);
	set_watchdog();
	assert(run_simulation(&sim, &cfg) == 0);
	capture_end(&cap);
	alarm(0);
	assert(cap.len[1] == 0 && cap.len[0] > 0);
	assert(cap.data[0][cap.len[0] - 1] == '\n');
	memcpy(copy, cap.data[0], cap.len[0] + 1);
	first = strtok_r(copy, "\n", &save);
	assert(first != NULL && get_line(first, &taken_ts, &id, &state_text));
	assert(id == 1 && taken_ts >= 0);
	assert(strcmp(state_text, "has taken a dongle") == 0);
	second = strtok_r(NULL, "\n", &save);
	assert(second != NULL && get_line(second, &burned_ts, &id, &state_text));
	assert(id == 1 && burned_ts >= taken_ts);
	assert(strcmp(state_text, "burned out") == 0);
	assert(strtok_r(NULL, "\n", &save) == NULL);
	assert(cap.len[0] > 0);
	{
		size_t i;
		int newlines;

		newlines = 0;
		for (i = 0; i < cap.len[0]; ++i)
			if (cap.data[0][i] == '\n')
				newlines++;
		assert(newlines == 2);
	}
	assert(burned_ts >= 300 && burned_ts <= 310 + get_slack());
}

/* AC5: a log write failure returns -1 without reporting to stderr. */
static void test_ac5_runtime_failure(void)
{
	int fds[2];
	pid_t child;
	int status;
	char buf[256];
	ssize_t n;
	int full;
	t_config cfg;
	t_sim sim;

	assert(pipe(fds) == 0);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		close(fds[0]);
		if (dup2(fds[1], STDERR_FILENO) != STDERR_FILENO)
			exit(77);
		close(fds[1]);
		full = open("/dev/full", O_WRONLY);
		if (full < 0 || dup2(full, STDOUT_FILENO) != STDOUT_FILENO)
			exit(77);
		close(full);
		cfg = get_config(4, 5000, 200, 3, MODE_FIFO);
		set_watchdog();
		if (run_simulation(&sim, &cfg) != -1)
			exit(1);
		exit(0);
	}
	assert(close(fds[1]) == 0);
	set_watchdog();
	do
	{
		n = read(fds[0], buf, sizeof(buf));
	}
	while (n < 0 && errno == EINTR);
	assert(n == 0);
	assert(close(fds[0]) == 0);
	assert(waitpid(child, &status, 0) == child);
	alarm(0);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) != 77);
	assert(WEXITSTATUS(status) == 0);
}

/* AC6: null arguments fail without output or starting a simulation. */
static void test_ac6_invalid_input(void)
{
	t_config cfg;
	t_sim sim;
	t_capture cap;

	cfg = get_config(4, 800, 200, 3, MODE_FIFO);
	capture_start(&cap);
	assert(run_simulation(NULL, &cfg) == -1);
	assert(run_simulation(&sim, NULL) == -1);
	capture_end(&cap);
	assert(cap.len[0] == 0 && cap.len[1] == 0);
}

int main(void)
{
	test_ac1_zero_compiles_required();
	test_ac2_target_met();
	test_ac3_burnout();
	test_ac4_single_coder();
	test_ac5_runtime_failure();
	test_ac6_invalid_input();
	return (0);
}
