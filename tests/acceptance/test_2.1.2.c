#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

static void expect_value(const char *input, int expected)
{
	int value;

	value = -77;
	assert(parse_number(input, &value) == 0);
	assert(value == expected);
}

static void expect_rejection(const char *input)
{
	int value;

	value = 12345;
	assert(parse_number(input, &value) == -1);
	assert(value == 12345);
}

/* AC1: valid ASCII digits, leading zeros, and both INT_MAX boundaries. */
static void test_ac1_valid_numbers(void)
{
	expect_value("0", 0);
	expect_value("42", 42);
	expect_value("007", 7);
	expect_value("2147483646", 2147483646);
	expect_value("2147483647", INT_MAX);
	expect_value("00000000000000000000000000000042", 42);
}

/* AC2: surrounding whitespace is ignored, including tabs and newlines. */
static void test_ac2_surrounding_whitespace(void)
{
	expect_value("     0007     ", 7);
	expect_value(" 42", 42);
	expect_value("42 ", 42);
	expect_value("\t1", 1);
	expect_value("1\n", 1);
}

/* AC3: empty, signed, mixed, and non-ASCII input leaves output untouched. */
static void test_ac3_invalid_digits(void)
{
	static const char *inputs[] = {
		"", "   ", "4 2", "-1", " -1", "+1", "+ 1", "-0",
		"- 0", "+0", "--1", "1-", "1a", "a1", "1.5", "0x10",
		"1\xff", "\xd9\xa1"
	};
	size_t i;

	for (i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
		expect_rejection(inputs[i]);
}

/* AC4: values beyond INT_MAX are rejected before arithmetic can wrap. */
static void test_ac4_overflow(void)
{
	static const char *inputs[] = {
		"2147483648", "4294967296", "4294967338",
		"18446744073709551658", "123456789012345678901234567890"
	};
	size_t i;

	for (i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
		expect_rejection(inputs[i]);
}

/* AC5: null pointers fail safely, and parsing never edits input bytes. */
static void test_ac5_null_and_input_unchanged(void)
{
	char valid[] = " \t0007 \n";
	char invalid[] = " 4 2 ";
	char original_valid[sizeof(valid)];
	char original_invalid[sizeof(invalid)];
	int value;

	value = 12345;
	assert(parse_number(NULL, &value) == -1);
	assert(value == 12345);
	assert(parse_number("42", NULL) == -1);
	memcpy(original_valid, valid, sizeof(valid));
	memcpy(original_invalid, invalid, sizeof(invalid));
	assert(parse_number(valid, &value) == 0 && value == 7);
	assert(memcmp(valid, original_valid, sizeof(valid)) == 0);
	value = 12345;
	assert(parse_number(invalid, &value) == -1 && value == 12345);
	assert(memcmp(invalid, original_invalid, sizeof(invalid)) == 0);
	assert(parse_number(valid, NULL) == -1);
	assert(memcmp(valid, original_valid, sizeof(valid)) == 0);
}

/* AC6: the deliverable includes the source, builds, and passes Norm. */
static void test_ac6_build_and_norm(void)
{
	FILE *file;
	FILE *pipe;
	char makefile[8192];
	char output[8192];
	struct stat info;
	size_t used;
	size_t count;
	int status;

	file = fopen("coders/Makefile", "r");
	assert(file != NULL);
	used = fread(makefile, 1, sizeof(makefile) - 1, file);
	assert(!ferror(file));
	assert(used < sizeof(makefile) - 1 || fgetc(file) == EOF);
	makefile[used] = '\0';
	assert(fclose(file) == 0);
	assert(strstr(makefile, "src/parsing/parse_number.c") != NULL);
	pipe = popen("make -C coders 2>&1", "r");
	assert(pipe != NULL);
	while (fread(output, 1, sizeof(output), pipe) != 0)
		;
	assert(!ferror(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	assert((info.st_mode & 0111) != 0);
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
	test_ac1_valid_numbers();
	test_ac2_surrounding_whitespace();
	test_ac3_invalid_digits();
	test_ac4_overflow();
	test_ac5_null_and_input_unchanged();
	test_ac6_build_and_norm();
	return (0);
}
