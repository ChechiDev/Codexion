#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* AC1: parse_number retains the complete numeric parsing contract of 2.1.2. */
static void test_ac1_parse_number_regression(void)
{
	static const char *invalid[] = {
		"2147483648", "-1", "+1", "4 2", "1a", "", "   "
	};
	int value;
	size_t i;

	value = -1;
	assert(parse_number("  0007  ", &value) == 0 && value == 7);
	assert(parse_number("2147483647", &value) == 0 && value == INT_MAX);
	assert(parse_number("0", &value) == 0 && value == 0);
	i = 0;
	while (i < sizeof(invalid) / sizeof(invalid[0]))
	{
		value = 12345;
		assert(parse_number(invalid[i], &value) == -1);
		assert(value == 12345);
		i++;
	}
}

/* AC2: parse_number trims all six whitespace characters on both sides. */
static void test_ac2_all_whitespace(void)
{
	int value;

	value = -1;
	assert(parse_number(" \t\n\v\f\r 42 \r\f\v\n\t ", &value) == 0);
	assert(value == 42);
}

/* AC3: the deliverable builds and norminette reports no error. */
static void test_ac3_build_and_norm(void)
{
	FILE *pipe;
	struct stat info;
	char output[8192];
	size_t used;
	size_t count;
	int status;

	pipe = popen("make -C coders 2>&1", "r");
	assert(pipe != NULL);
	while (fread(output, 1, sizeof(output), pipe) != 0)
		;
	assert(!ferror(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	pipe = popen("norminette coders/ 2>&1", "r");
	assert(pipe != NULL);
	used = 0;
	while (used < sizeof(output) - 1 && (count = fread(output + used, 1,
		sizeof(output) - 1 - used, pipe)) != 0)
		used += count;
	output[used] = '\0';
	assert(feof(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(strstr(output, "Error") == NULL);
}

int main(void)
{
	test_ac1_parse_number_regression();
	test_ac2_all_whitespace();
	test_ac3_build_and_norm();
	return (0);
}
