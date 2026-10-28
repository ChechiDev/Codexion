#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static void read_output(FILE *stream, char *buffer, size_t capacity)
{
	size_t used;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, capacity - 1, stream);
	assert(!ferror(stream));
	assert(used < capacity - 1 || fgetc(stream) == EOF);
	buffer[used] = '\0';
}

static void assert_arg_error(int position, const char *expected)
{
	char out_text[512];
	char err_text[512];
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(err), STDERR_FILENO) >= 0);
	print_arg_error(position);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(saved_err, STDERR_FILENO) >= 0);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	read_output(out, out_text, sizeof(out_text));
	read_output(err, err_text, sizeof(err_text));
	assert(strcmp(out_text, "") == 0);
	assert(strcmp(err_text, expected) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

/* AC1: positions 1 and 2 emit exact positive-integer errors to stderr. */
static void test_ac1_positive_integer_errors(void)
{
	assert_arg_error(1, "Error: number_of_coders must be an integer "
		"from 1 to 2147483647\n");
	assert_arg_error(2, "Error: time_to_burnout must be an integer "
		"from 1 to 2147483647\n");
}

/* AC2: positions 3 through 7 emit exact nonnegative-integer errors. */
static void test_ac2_nonnegative_integer_errors(void)
{
	static const char *names[] = {"time_to_compile", "time_to_debug",
		"time_to_refactor", "number_of_compiles_required",
		"dongle_cooldown"};
	char expected[512];
	int position;

	position = 3;
	while (position <= 7)
	{
		assert(snprintf(expected, sizeof(expected), "Error: %s must be an "
			"integer from 0 to 2147483647\n", names[position - 3]) > 0);
		assert_arg_error(position, expected);
		position++;
	}
}

/* AC3: position 8 emits the exact scheduler error. */
static void test_ac3_scheduler_error(void)
{
	assert_arg_error(8, "Error: scheduler must be fifo or edf\n");
}

/* AC4: out-of-range positions produce no output. */
static void test_ac4_out_of_range_silent(void)
{
	assert_arg_error(0, "");
	assert_arg_error(9, "");
	assert_arg_error(-1, "");
}

/* AC5: each numeric error uses the public minimum for its position. */
static void test_ac5_minimum_matches_parser(void)
{
	static const char *names[] = {"number_of_coders", "time_to_burnout",
		"time_to_compile", "time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown"};
	char expected[512];
	int position;

	position = 1;
	while (position <= 7)
	{
		assert(snprintf(expected, sizeof(expected), "Error: %s must be an "
			"integer from %d to %d\n", names[position - 1],
			get_arg_minimum(position), INT_MAX) > 0);
		assert_arg_error(position, expected);
		position++;
	}
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC6: the deliverable includes the source, builds, and passes Norm. */
static void test_ac6_build_and_norm(void)
{
	char text[4096];
	FILE *file;
	size_t used;
	struct stat info;

	file = fopen("coders/Makefile", "r");
	assert(file != NULL);
	used = fread(text, 1, sizeof(text) - 1, file);
	assert(!ferror(file));
	assert(used < sizeof(text) - 1 || fgetc(file) == EOF);
	assert(fclose(file) == 0);
	text[used] = '\0';
	assert(strstr(text, "src/cli/print_arg_error.c") != NULL);
	run_command("make -C coders >/dev/null 2>&1");
	assert(stat("coders/codexion", &info) == 0);
	assert(S_ISREG(info.st_mode));
	assert((info.st_mode & 0111) != 0);
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_positive_integer_errors();
	test_ac2_nonnegative_integer_errors();
	test_ac3_scheduler_error();
	test_ac4_out_of_range_silent();
	test_ac5_minimum_matches_parser();
	test_ac6_build_and_norm();
	return (0);
}
