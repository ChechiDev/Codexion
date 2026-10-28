#define _DEFAULT_SOURCE

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

static long get_reference_ms(void)
{
	struct timeval	value;

	assert(gettimeofday(&value, NULL) == 0);
	return ((long)value.tv_sec * 1000 + value.tv_usec / 1000);
}

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

/* AC1: the public clock result is bracketed by independent wall-clock reads. */
static void test_ac1_wall_clock_bracket(void)
{
	long	before;
	long	actual;
	long	after;

	before = get_reference_ms();
	actual = get_time_ms();
	after = get_reference_ms();
	assert(before <= actual && actual <= after);
}

/* AC2: 1000 consecutive public clock reads never decrease. */
static void test_ac2_consecutive_reads_do_not_decrease(void)
{
	long	previous;
	long	current;
	int		index;

	previous = get_time_ms();
	index = 0;
	while (index < 1000)
	{
		current = get_time_ms();
		assert(current >= previous);
		previous = current;
		index++;
	}
}

/* AC3: a 50 ms sleep advances the clock by 50 to 80 + S ms. */
static void test_ac3_sleep_interval(void)
{
	long	before;
	long	after;

	before = get_time_ms();
	assert(usleep(50000) == 0);
	after = get_time_ms();
	assert(after - before >= 50);
	assert(after - before <= 80 + get_timing_slack_ms());
}

static void run_command(const char *command)
{
	int	status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC4: the new clock source builds into a Norm-clean deliverable. */
static void test_ac4_build_and_norm(void)
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
	assert(strstr(text, "src/time/get_time_ms.c") != NULL);
	run_command("make -C coders >/dev/null 2>&1");
	assert(stat("coders/codexion", &info) == 0);
	assert(S_ISREG(info.st_mode) && (info.st_mode & 0111) != 0);
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]get_time_ms$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int	main(void)
{
	test_ac1_wall_clock_bracket();
	test_ac2_consecutive_reads_do_not_decrease();
	test_ac3_sleep_interval();
	test_ac4_build_and_norm();
	return (0);
}
