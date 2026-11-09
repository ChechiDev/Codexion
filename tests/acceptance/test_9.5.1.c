#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
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

static void assert_run_error_output(int calls, const char *expected)
{
	char out_text[128];
	char err_text[128];
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;
	int i;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(err), STDERR_FILENO) >= 0);
	i = 0;
	while (i++ < calls)
		print_run_error();
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

/* AC1: one call writes the exact run error to stderr and no stdout. */
static void test_ac1_exact_stderr_message(void)
{
	assert_run_error_output(1, "Error: simulation failed\n");
}

/* AC2: two calls write exactly two consecutive lines and nothing else. */
static void test_ac2_repeated_calls(void)
{
	assert_run_error_output(2, "Error: simulation failed\n"
		"Error: simulation failed\n");
}

int main(void)
{
	test_ac1_exact_stderr_message();
	test_ac2_repeated_calls();
	return (0);
}
