#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* AC1: parse_number retains the complete 2.1.2 and 2.2.5 contract. */
static void test_ac1_parse_number_regression(void)
{
	static const struct {
		const char *input;
		int expected;
	} valid[] = {
		{"0", 0}, {"42", 42}, {"007", 7},
		{"2147483647", INT_MAX},
		{"00000000000000000000000000000042", 42},
		{" \t\n\v\f\r 42 \r\f\v\n\t ", 42}
	};
	static const char *invalid[] = {
		"2147483648", "4294967338", "18446744073709551658",
		"-1", "+1", "4 2", "1a", "a1", "1.5", "", "   ",
		"1\xff", "\xd9\xa1"
	};
	char input[] = "  0007  ";
	char original[sizeof(input)];
	int value;
	size_t i;

	for (i = 0; i < sizeof(valid) / sizeof(valid[0]); i++)
	{
		value = -77;
		assert(parse_number(valid[i].input, &value) == 0);
		assert(value == valid[i].expected);
	}
	for (i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++)
	{
		value = 12345;
		assert(parse_number(invalid[i], &value) == -1);
		assert(value == 12345);
	}
	memcpy(original, input, sizeof(input));
	assert(parse_number(input, &value) == 0 && value == 7);
	assert(memcmp(input, original, sizeof(input)) == 0);
	value = 12345;
	assert(parse_number(NULL, &value) == -1 && value == 12345);
	assert(parse_number(input, NULL) == -1);
}

/* AC2: digit endpoints are accepted; adjacent bytes are rejected. */
static void test_ac2_digit_boundaries(void)
{
	static const char *invalid[] = {"/", ":", "0/", "9:"};
	int value;
	size_t i;

	value = -1;
	assert(parse_number("0", &value) == 0 && value == 0);
	value = -1;
	assert(parse_number("9", &value) == 0 && value == 9);
	for (i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++)
	{
		value = 12345;
		assert(parse_number(invalid[i], &value) == -1);
		assert(value == 12345);
	}
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
	test_ac2_digit_boundaries();
	test_ac3_build_and_norm();
	return (0);
}
