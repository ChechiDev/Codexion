#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static void init_sim(t_sim *sim)
{
	sim->start_ms = get_time_ms() - 100;
	assert(sim->start_ms >= 0);
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
}

static void assert_state_line(const char *line, size_t len, int coder,
	const char *text)
{
	char expected[LOG_LINE_MAX + 1];
	long timestamp;
	int parsed_coder;
	int offset;
	int expected_len;

	offset = 0;
	assert(sscanf(line, "%ld %d %n", &timestamp, &parsed_coder, &offset) == 2);
	assert(timestamp >= 0 && parsed_coder == coder);
	expected_len = snprintf(expected, sizeof(expected), "%ld %d %s\n",
		timestamp, coder, text);
	assert(expected_len > 0 && (size_t)expected_len == len);
	assert(memcmp(line, expected, len) == 0);
}

/* AC1: print_state keeps four exact state lines and rejects a stopped sim. */
static void test_ac1_print_state_regression(void)
{
	static const char *texts[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring"};
	t_sim sim;
	FILE *capture;
	char output[4 * LOG_LINE_MAX + 1];
	char *line;
	char *end;
	int saved;
	int state;
	size_t len;

	init_sim(&sim);
	capture = tmpfile();
	assert(capture != NULL);
	saved = dup(STDOUT_FILENO);
	assert(saved >= 0 && dup2(fileno(capture), STDOUT_FILENO) >= 0);
	state = 0;
	while (state < 4)
	{
		assert(print_state(&sim, 7, (t_state)state) == 0);
		state++;
	}
	assert(set_stopped(&sim) == 0);
	state = 0;
	while (state < 4)
	{
		assert(print_state(&sim, 7, (t_state)state) == -1);
		state++;
	}
	assert(dup2(saved, STDOUT_FILENO) >= 0 && close(saved) == 0);
	assert(fseek(capture, 0, SEEK_SET) == 0);
	len = fread(output, 1, sizeof(output) - 1, capture);
	assert(!ferror(capture) && feof(capture));
	output[len] = '\0';
	line = output;
	state = 0;
	while (state < 4)
	{
		end = strchr(line, '\n');
		assert(end != NULL);
		assert_state_line(line, (size_t)(end - line + 1), 7, texts[state]);
		line = end + 1;
		state++;
	}
	assert(*line == '\0');
	assert(pthread_mutex_destroy(&sim.stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim.log_mutex) == 0);
	assert(fclose(capture) == 0);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC2: the deliverable builds without relinking and passes Norm. */
static void test_ac2_build_no_relink_and_norm(void)
{
	struct stat before;
	struct stat after;

	run_command("make -C coders >/dev/null 2>&1");
	assert(stat("coders/codexion", &before) == 0);
	assert(sleep(1) == 0);
	run_command("make -C coders >/dev/null 2>&1");
	assert(stat("coders/codexion", &after) == 0);
	assert(before.st_mtim.tv_sec == after.st_mtim.tv_sec);
	assert(before.st_mtim.tv_nsec == after.st_mtim.tv_nsec);
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_print_state_regression();
	test_ac2_build_no_relink_and_norm();
	return (0);
}
