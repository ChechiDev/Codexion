#define _GNU_SOURCE

#include "main.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

static int clock_fail;

int gettimeofday(struct timeval *tv, void *tz)
{
	if (clock_fail)
	{
		errno = EIO;
		return (-1);
	}
	return ((int)syscall(SYS_gettimeofday, tv, tz));
}

static void init_case(t_sim *sim)
{
	t_config cfg;

	memset(&cfg, 0, sizeof(cfg));
	cfg.number_of_coders = 2;
	cfg.time_to_burnout = 1000;
	cfg.time_to_compile = 10;
	cfg.mode = MODE_EDF;
	clock_fail = 0;
	assert(init_simulation(sim, &cfg) == 0);
	sim->start_ms = get_time_ms();
	assert(sim->start_ms >= 0);
}

static void capture_silence(t_coder *coder)
{
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) == STDOUT_FILENO);
	assert(dup2(fileno(err), STDERR_FILENO) == STDERR_FILENO);
	assert(run_coder(coder) == NULL);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) == STDOUT_FILENO);
	assert(dup2(saved_err, STDERR_FILENO) == STDERR_FILENO);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	assert(fseek(out, 0, SEEK_END) == 0 && ftell(out) == 0);
	assert(fseek(err, 0, SEEK_END) == 0 && ftell(err) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

/* AC1: a stopped even coder exits before requesting a ticket, silently. */
static void test_ac1_even_stops_before_request(void)
{
	t_sim sim;

	init_case(&sim);
	assert(set_stopped(&sim) == 0);
	capture_silence(&sim.coders[1]);
	assert(sim.failed == 0 && sim.next_ticket == 0);
	assert(free_sim(&sim) == 0);
}

/* AC2: a stopped odd coder reaches its first request without an offset. */
static void test_ac2_odd_reaches_request(void)
{
	t_sim sim;

	init_case(&sim);
	assert(set_stopped(&sim) == 0);
	capture_silence(&sim.coders[0]);
	assert(sim.failed == 0 && sim.next_ticket == 1);
	assert(free_sim(&sim) == 0);
}

/* AC3: an even coder's offset clock error marks failure before any ticket. */
static void test_ac3_offset_error_routes_failure(void)
{
	t_sim sim;

	init_case(&sim);
	clock_fail = 1;
	capture_silence(&sim.coders[1]);
	clock_fail = 0;
	assert(sim.failed == 1 && sim.stopped == 1);
	assert(sim.next_ticket == 0);
	assert(free_sim(&sim) == 0);
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

/* AC4: the one-coder binary takes one dongle and burns out on deadline. */
static void test_ac4_one_coder_burnout(void)
{
	const char *path;
	char output[4096];
	char line[128];
	char *save;
	char *cursor;
	char *text;
	long ts;
	int id;
	int taken;
	int burned;
	int status;
	int fds[2];
	pid_t child;
	ssize_t n;
	size_t len;

	path = getenv("CODEXION_BIN");
	if (path == NULL)
		path = "coders/codexion";
	assert(pipe(fds) == 0);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		assert(dup2(fds[1], STDOUT_FILENO) == STDOUT_FILENO);
		assert(close(fds[0]) == 0 && close(fds[1]) == 0);
		execl(path, path, "1", "300", "200", "200", "200", "3", "50",
			"edf", (char *)NULL);
		_exit(127);
	}
	assert(close(fds[1]) == 0);
	len = 0;
	while ((n = read(fds[0], output + len, sizeof(output) - len - 1)) > 0)
	{
		len += (size_t)n;
		assert(len < sizeof(output) - 1);
	}
	assert(n == 0 && close(fds[0]) == 0);
	output[len] = '\0';
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	taken = 0;
	burned = 0;
	cursor = strtok_r(output, "\n", &save);
	while (cursor != NULL)
	{
		assert(sscanf(cursor, "%ld %d %127[^\n]", &ts, &id, line) == 3);
		assert(id == 1);
		text = line;
		if (strcmp(text, "has taken a dongle") == 0)
		{
			assert(taken == 0 && burned == 0);
			taken++;
		}
		else
		{
			assert(strcmp(text, "burned out") == 0);
			assert(taken == 1 && burned == 0);
			assert(ts >= 300 && ts <= 310 + get_slack());
			burned++;
		}
		cursor = strtok_r(NULL, "\n", &save);
	}
	assert(taken == 1 && burned == 1);
}

int main(void)
{
	if (getenv("CODEXION_UNDER_VALGRIND") != NULL
		|| getenv("TSAN_OPTIONS") != NULL)
		alarm(120);
	else
		alarm(20);
	test_ac1_even_stops_before_request();
	test_ac2_odd_reaches_request();
	test_ac3_offset_error_routes_failure();
	test_ac4_one_coder_burnout();
	alarm(0);
	return (0);
}
