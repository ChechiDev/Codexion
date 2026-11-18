#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUTPUT_CAP 8192
#define README_CAP 32768

typedef struct s_output
{
	char out[OUTPUT_CAP];
	char err[OUTPUT_CAP];
	int status;
} t_output;

static const char *names[] = {"number_of_coders", "time_to_burnout",
	"time_to_compile", "time_to_debug", "time_to_refactor",
	"number_of_compiles_required", "dongle_cooldown"};

static const char *usage = "\nUsage: ./codexion coders burnout compile debug "
	"refactor compiles cooldown fifo|edf\n\n"
	"  e.g. ./codexion 4 800 200 200 200 3 50 edf\n";

static void read_stream(FILE *stream, char *buffer, size_t capacity)
{
	size_t used;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, capacity - 1, stream);
	assert(!ferror(stream));
	assert(used < capacity - 1 || fgetc(stream) == EOF);
	buffer[used] = '\0';
}

static t_output capture_arg_error(int position, int kind)
{
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;
	t_output result;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(err), STDERR_FILENO) >= 0);
	print_arg_error(position, kind);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(saved_err, STDERR_FILENO) >= 0);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	read_stream(out, result.out, sizeof(result.out));
	read_stream(err, result.err, sizeof(result.err));
	assert(fclose(out) == 0 && fclose(err) == 0);
	return (result);
}

static t_output run_binary(char *args[])
{
	const char *binary;
	FILE *out;
	FILE *err;
	pid_t child;
	int status;
	t_output result;

	binary = getenv("CODEXION_BIN");
	if (binary == NULL || binary[0] == '\0')
		binary = "coders/codexion";
	args[0] = (char *)binary;
	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		if (dup2(fileno(out), STDOUT_FILENO) < 0
			|| dup2(fileno(err), STDERR_FILENO) < 0)
			_exit(126);
		execv(binary, args);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status));
	result.status = WEXITSTATUS(status);
	read_stream(out, result.out, sizeof(result.out));
	read_stream(err, result.err, sizeof(result.err));
	assert(fclose(out) == 0 && fclose(err) == 0);
	return (result);
}

static void assert_line(int position, int kind, const char *expected)
{
	t_output result;

	result = capture_arg_error(position, kind);
	assert(strcmp(result.out, "") == 0);
	assert(strcmp(result.err, expected) == 0);
}

/* AC1: every numeric position prints its exact malformed-sign line. */
static void test_ac1_sign_line(void)
{
	char expected[256];
	int position;
	int count;

	position = 1;
	while (position <= 7)
	{
		count = snprintf(expected, sizeof(expected),
			"Error: %s: invalid sign, use at most one '+' right before the digits\n",
			names[position - 1]);
		assert(count > 0 && (size_t)count < sizeof(expected));
		assert_line(position, ARG_BAD_SIGN, expected);
		position++;
	}
}

/* AC2: range and scheduler errors retain their exact lines. */
static void test_ac2_range_line(void)
{
	char expected[256];
	int position;
	int count;

	position = 1;
	while (position <= 7)
	{
		count = snprintf(expected, sizeof(expected),
			"Error: %s: expected an integer from %d to 2147483647\n",
			names[position - 1], position <= 2 ? 1 : 0);
		assert(count > 0 && (size_t)count < sizeof(expected));
		assert_line(position, ARG_BAD_RANGE, expected);
		position++;
	}
	assert_line(8, ARG_BAD_RANGE,
		"Error: scheduler: expected fifo or edf\n");
}

/* AC3: invalid positions and kinds, including scheduler sign, are silent. */
static void test_ac3_invalid_inputs_silent(void)
{
	static const int positions[] = {0, 9, -1};
	static const int kinds[] = {ARG_BAD_RANGE, ARG_BAD_SIGN};
	static const int bad_kinds[] = {0, 3, -1};
	size_t i;
	size_t j;
	int position;

	i = 0;
	while (i < sizeof(positions) / sizeof(positions[0]))
	{
		j = 0;
		while (j < sizeof(kinds) / sizeof(kinds[0]))
			assert_line(positions[i], kinds[j++], "");
		i++;
	}
	position = 1;
	while (position <= 8)
	{
		j = 0;
		while (j < sizeof(bad_kinds) / sizeof(bad_kinds[0]))
			assert_line(position, bad_kinds[j++], "");
		position++;
	}
	assert_line(8, ARG_BAD_SIGN, "");
}

/* AC4: three malformed signs produce ordered lines and one usage. */
static void test_ac4_binary_sign_errors(void)
{
	char *args[] = {NULL, "4", "++800", "+-200", "+", "200", "3",
		"50", "edf", NULL};
	const char *lines = "Error: time_to_burnout: invalid sign, use at most "
		"one '+' right before the digits\n"
		"Error: time_to_compile: invalid sign, use at most "
		"one '+' right before the digits\n"
		"Error: time_to_debug: invalid sign, use at most "
		"one '+' right before the digits\n";
	char expected[OUTPUT_CAP];
	t_output result;
	int count;

	count = snprintf(expected, sizeof(expected), "%s%s", lines, usage);
	assert(count > 0 && (size_t)count < sizeof(expected));
	result = run_binary(args);
	assert(result.status == 1 && result.out[0] == '\0');
	assert(strcmp(result.err, expected) == 0);
}

/* AC5: mixed sign, range, scheduler errors precede one usage. */
static void test_ac5_binary_mixed_errors(void)
{
	char *args[] = {NULL, "4", "++800", "-200", "200", "200", "3",
		"50", "rr", NULL};
	const char *lines = "Error: time_to_burnout: invalid sign, use at most "
		"one '+' right before the digits\n"
		"Error: time_to_compile: expected an integer from 0 to 2147483647\n"
		"Error: scheduler: expected fifo or edf\n";
	char expected[OUTPUT_CAP];
	t_output result;
	int count;

	count = snprintf(expected, sizeof(expected), "%s%s", lines, usage);
	assert(count > 0 && (size_t)count < sizeof(expected));
	result = run_binary(args);
	assert(result.status == 1 && result.out[0] == '\0');
	assert(strcmp(result.err, expected) == 0);
}

/* AC6: README describes the sign rule and shows the binary's first error. */
static void test_ac6_readme_sign_example(void)
{
	char readme[README_CAP];
	char *args[] = {NULL, "4", "++800", "200", "200", "200", "3",
		"50", "edf", NULL};
	const char *command = "$ ./coders/codexion 4 ++800 200 200 200 3 50 edf\n";
	const char *paragraph;
	const char *end;
	const char *rule;
	const char *example;
	const char *line_end;
	FILE *file;
	t_output result;

	file = fopen("README.md", "rb");
	assert(file != NULL);
	read_stream(file, readme, sizeof(readme));
	assert(fclose(file) == 0);
	assert(strstr(readme, "decimal digits only") == NULL);
	paragraph = strstr(readme, "Argument values are validated");
	assert(paragraph != NULL);
	end = strstr(paragraph, "```");
	assert(end != NULL);
	rule = strstr(paragraph, "single");
	assert(rule != NULL && rule < end);
	rule = strchr(paragraph, '+');
	assert(rule != NULL && rule < end);
	rule = strstr(paragraph, "malformed sign");
	assert(rule != NULL && rule < end);
	rule = strstr(paragraph, "error line");
	assert(rule != NULL && rule < end);
	example = strstr(readme, command);
	assert(example != NULL && example >= end + 3);
	assert(example >= readme + 4 && strncmp(example - 4, "```\n", 4) == 0);
	example += strlen(command);
	line_end = strchr(example, '\n');
	assert(line_end != NULL);
	result = run_binary(args);
	assert(result.status == 1 && result.out[0] == '\0');
	assert((size_t)(line_end + 1 - example)
		== strcspn(result.err, "\n") + 1);
	assert(memcmp(example, result.err, (size_t)(line_end + 1 - example)) == 0);
}

/* AC7: the deliverable builds and passes Norm. */
static void test_ac7_build_and_norm(void)
{
	struct stat info;
	int status;

	status = system("make -C coders >/dev/null 2>&1");
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	status = system("norminette coders/ >/dev/null 2>&1");
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

int main(void)
{
	test_ac1_sign_line();
	test_ac2_range_line();
	test_ac3_invalid_inputs_silent();
	test_ac4_binary_sign_errors();
	test_ac5_binary_mixed_errors();
	test_ac6_readme_sign_example();
	test_ac7_build_and_norm();
	return (0);
}
