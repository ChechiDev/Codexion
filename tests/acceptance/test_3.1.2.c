#define _DEFAULT_SOURCE

#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static long get_timing_slack_ms(void)
{
	const char	*value;
	char		*end;
	long		slack;

	value = getenv("CODEXION_TIME_SLACK_MS");
	if (value == NULL)
		return (0);
	slack = strtol(value, &end, 10);
	assert(end != value && *end == '\0' && slack >= 0);
	return (slack);
}

/* AC1: elapsed milliseconds from the current start fit clock brackets. */
static void test_ac1_current_start_bracket(void)
{
	long	start;
	long	before;
	long	actual;
	long	after;

	start = get_time_ms();
	assert(start >= 0);
	before = get_time_ms();
	actual = get_elapsed_ms(start);
	after = get_time_ms();
	assert(before - start <= actual && actual <= after - start);
}

/* AC2: a start 100 ms ago fits clock brackets and yields at least 100. */
static void test_ac2_past_start_bracket(void)
{
	long	start;
	long	before;
	long	actual;
	long	after;

	start = get_time_ms() - 100;
	assert(start >= 0);
	before = get_time_ms();
	actual = get_elapsed_ms(start);
	after = get_time_ms();
	assert(before - start <= actual && actual <= after - start);
	assert(actual >= 100);
}

/* AC3: sleeping 30 ms advances elapsed time by 30 to 60 + S ms. */
static void test_ac3_sleep_interval(void)
{
	long	start;
	long	actual;

	start = get_time_ms();
	assert(start >= 0);
	assert(usleep(30000) == 0);
	actual = get_elapsed_ms(start);
	assert(actual >= 30);
	assert(actual <= 60 + get_timing_slack_ms());
}

/* AC4: a far-future start clamps the result to zero. */
static void test_ac4_future_start_is_zero(void)
{
	assert(get_elapsed_ms(LONG_MAX) == 0);
}

static void run_command(const char *command)
{
	int	status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC5: the deliverable includes the new source and passes the Norm. */
static void test_ac5_build_and_norm(void)
{
	char		text[4096];
	FILE		*file;
	size_t		used;
	struct stat	info;

	file = fopen("coders/Makefile", "r");
	assert(file != NULL);
	used = fread(text, 1, sizeof(text) - 1, file);
	assert(!ferror(file));
	assert(fclose(file) == 0);
	text[used] = '\0';
	assert(strstr(text, "src/time/get_elapsed_ms.c") != NULL);
	run_command("make -C coders >/dev/null 2>&1");
	assert(stat("coders/codexion", &info) == 0);
	assert(S_ISREG(info.st_mode) && (info.st_mode & 0111) != 0);
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]get_elapsed_ms$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int	main(void)
{
	test_ac1_current_start_bracket();
	test_ac2_past_start_bracket();
	test_ac3_sleep_interval();
	test_ac4_future_start_is_zero();
	test_ac5_build_and_norm();
	return (0);
}
