#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* AC1: trim_slice preserves the six-space, inner-space, empty, NULL and input contracts. */
static void test_ac1_trim_slice_regression(void)
{
	char input[] = " \t\n\v\f\r fi fo \r\f\v\n\t ";
	char original[sizeof(input)];
	t_slice slice;

	memcpy(original, input, sizeof(input));
	slice = trim_slice(input);
	assert(slice.start == input + 7 && slice.len == 5);
	assert(strncmp(slice.start, "fi fo", (size_t)slice.len) == 0);
	assert(memcmp(input, original, sizeof(input)) == 0);
	slice = trim_slice("");
	assert(slice.len == 0);
	slice = trim_slice("   ");
	assert(slice.len == 0);
	slice = trim_slice(NULL);
	assert(slice.start == NULL && slice.len == 0);
}

/* AC2: parse_number keeps trimmed numbers and leaves output untouched on error. */
static void test_ac2_parse_number_regression(void)
{
	int n;

	n = -1;
	assert(parse_number("  42  ", &n) == 0 && n == 42);
	n = 123;
	assert(parse_number("4 2", &n) == -1 && n == 123);
	assert(parse_number("   ", &n) == -1 && n == 123);
}

/* AC3: bytes outside the whitespace set are never trimmed. */
static void test_ac3_non_whitespace_boundaries(void)
{
	const char *controls = "\x01" "ab" "\x7f";
	const char *underscores = "_x_";
	t_slice slice;

	slice = trim_slice(controls);
	assert(slice.start == controls && slice.len == 4);
	assert((unsigned char)slice.start[0] == 1);
	slice = trim_slice(underscores);
	assert(slice.start == underscores && slice.len == 3);
}

static void run_command(const char *command, char *output)
{
	FILE *pipe;
	size_t used;
	int status;

	pipe = popen(command, "r");
	assert(pipe != NULL);
	used = fread(output, 1, 8191, pipe);
	assert(!ferror(pipe));
	assert(used < 8191 || fgetc(pipe) == EOF);
	output[used] = '\0';
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC4: the deliverable builds and Norm reports no error. */
static void test_ac4_build_and_norm(void)
{
	struct stat info;
	char output[8192];

	run_command("make -C coders 2>&1", output);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	run_command("norminette coders/ 2>&1", output);
	assert(strstr(output, "Error") == NULL);
}

int main(void)
{
	test_ac1_trim_slice_regression();
	test_ac2_parse_number_regression();
	test_ac3_non_whitespace_boundaries();
	test_ac4_build_and_norm();
	return (0);
}
