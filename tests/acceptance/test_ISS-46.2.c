#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct s_capture
{
	int read_fd;
	int saved_out;
	pthread_t reader;
	char data[256];
	size_t length;
} t_capture;

static t_config get_config(void)
{
	t_config config;

	config.number_of_coders = 4;
	config.time_to_burnout = 300;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
	return (config);
}

static void init_case(t_sim *sim, int overdue, int target_met)
{
	t_config config;
	int i;

	config = get_config();
	assert(init_simulation(sim, &config) == 0);
	for (i = 0; i < 4; ++i)
	{
		sim->coders[i].last_compile_start = LONG_MAX;
		sim->coders[i].compile_count = 3;
	}
	if (overdue)
		sim->coders[0].last_compile_start = 0;
	if (!target_met)
		sim->coders[3].compile_count = 2;
	sim->start_ms = get_time_ms() - 1000;
	assert(sim->start_ms > 0);
}

static void *read_output(void *arg)
{
	t_capture *capture;
	ssize_t count;

	capture = arg;
	while (capture->length < sizeof(capture->data) - 1)
	{
		count = read(capture->read_fd, capture->data + capture->length,
			sizeof(capture->data) - 1 - capture->length);
		assert(count >= 0);
		if (count == 0)
			break ;
		capture->length += (size_t)count;
	}
	capture->data[capture->length] = '\0';
	return (NULL);
}

static void start_capture(t_capture *capture)
{
	int fds[2];

	memset(capture, 0, sizeof(*capture));
	assert(pipe(fds) == 0);
	capture->read_fd = fds[0];
	capture->saved_out = dup(STDOUT_FILENO);
	assert(capture->saved_out >= 0);
	assert(dup2(fds[1], STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&capture->reader, NULL, read_output, capture) == 0);
}

static void stop_capture(t_capture *capture)
{
	assert(dup2(capture->saved_out, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(capture->saved_out) == 0);
	assert(pthread_join(capture->reader, NULL) == 0);
	assert(close(capture->read_fd) == 0);
}

static void run_case(t_sim *sim, t_capture *capture)
{
	pthread_t thread;
	void *result;

	start_capture(capture);
	alarm(getenv("CODEXION_UNDER_VALGRIND") || getenv("TSAN_OPTIONS") ? 120 : 1);
	assert(pthread_create(&thread, NULL, run_monitor, sim) == 0);
	result = (void *)1;
	assert(pthread_join(thread, &result) == 0);
	alarm(0);
	stop_capture(capture);
	assert(result == NULL);
}

static void assert_burnout_line(t_capture *capture)
{
	long timestamp;
	int id;
	int end;

	end = -1;
	assert(sscanf(capture->data, "%ld %d burned out%n",
		&timestamp, &id, &end) == 2);
	assert(timestamp >= 0 && id == 1);
	assert(end >= 0 && capture->data[end] == '\n');
	assert((size_t)end + 1 == capture->length);
}

/* AC1: burnout wins when the target and an overdue deadline coexist. */
static void test_ac1_burnout_wins_joint_observation(void)
{
	t_sim sim;
	t_capture capture;

	init_case(&sim, 1, 1);
	run_case(&sim, &capture);
	assert_burnout_line(&capture);
	assert(sim.stopped == 1 && sim.failed == 0);
	assert(free_sim(&sim) == 0);
}

/* AC2: a met target with no overdue coder ends without output. */
static void test_ac2_target_alone(void)
{
	t_sim sim;
	t_capture capture;

	init_case(&sim, 0, 1);
	run_case(&sim, &capture);
	assert(capture.length == 0);
	assert(sim.stopped == 1 && sim.failed == 0);
	assert(free_sim(&sim) == 0);
}

/* AC3: an overdue coder is reported when the target is not met. */
static void test_ac3_burnout_alone(void)
{
	t_sim sim;
	t_capture capture;

	init_case(&sim, 1, 0);
	run_case(&sim, &capture);
	assert_burnout_line(&capture);
	assert(sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

/* AC4: an external stop makes the monitor leave silently. */
static void test_ac4_external_stop(void)
{
	t_sim sim;
	t_capture capture;

	init_case(&sim, 1, 1);
	assert(set_stopped(&sim) == 0);
	run_case(&sim, &capture);
	assert(capture.length == 0 && sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

/* AC5: zero required exits at the binary boundary without output. */
static void test_ac5_zero_required_binary(void)
{
	const char *path;
	FILE *out;
	FILE *err;
	pid_t child;
	int status;

	path = getenv("CODEXION_BIN");
	if (path == NULL || *path == '\0')
		path = "coders/codexion";
	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		alarm(1);
		if (dup2(fileno(out), STDOUT_FILENO) < 0
			|| dup2(fileno(err), STDERR_FILENO) < 0)
			_exit(126);
		execl(path, path, "4", "800", "200", "200", "200", "0",
			"50", "fifo", (char *)NULL);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(fseek(out, 0, SEEK_END) == 0 && ftell(out) == 0);
	assert(fseek(err, 0, SEEK_END) == 0 && ftell(err) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC6: the deliverable builds, exports run_monitor, and passes Norm. */
static void test_ac6_build_symbol_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]run_monitor$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_burnout_wins_joint_observation();
	test_ac2_target_alone();
	test_ac3_burnout_alone();
	test_ac4_external_stop();
	test_ac5_zero_required_binary();
	test_ac6_build_symbol_norm();
	return (0);
}
