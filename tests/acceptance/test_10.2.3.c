#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define CAPACITY 512

static const char *get_binary(void)
{
	const char *path;

	path = getenv("CODEXION_BIN");
	if (path == NULL || path[0] == '\0')
		return ("coders/codexion");
	return (path);
}

static size_t read_bytes(FILE *stream, char *buffer, size_t capacity)
{
	size_t used;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, capacity, stream);
	assert(!ferror(stream));
	assert(used < capacity || fgetc(stream) == EOF);
	return (used);
}

static void assert_output(void (*print)(void), const char *expected)
{
	char out_text[CAPACITY];
	char err_text[CAPACITY];
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;
	size_t out_len;
	size_t err_len;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(err), STDERR_FILENO) >= 0);
	print();
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(saved_err, STDERR_FILENO) >= 0);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	out_len = read_bytes(out, out_text, sizeof(out_text));
	err_len = read_bytes(err, err_text, sizeof(err_text));
	assert(out_len == 0);
	assert(err_len == strlen(expected));
	assert(memcmp(err_text, expected, err_len) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

static int has_forbidden_symbol(const char *symbol)
{
	static const char *forbidden[] = {"fwrite", "fputs", "fputc", "puts"};
	size_t i;

	i = 0;
	while (i < sizeof(forbidden) / sizeof(forbidden[0]))
	{
		if (strncmp(symbol, forbidden[i], strlen(forbidden[i])) == 0
			&& (symbol[strlen(forbidden[i])] == '\0'
				|| symbol[strlen(forbidden[i])] == '@'))
			return (1);
		i++;
	}
	return (0);
}

/* AC1: nm -u of the deliverable lists no fwrite, fputs, fputc, or puts. */
static void test_ac1_no_forbidden_symbols(void)
{
	char line[CAPACITY];
	char *symbol;
	FILE *output;
	int status;

	output = popen("nm -u coders/codexion", "r");
	assert(output != NULL);
	while (fgets(line, sizeof(line), output) != NULL)
	{
		assert(strchr(line, '\n') != NULL || feof(output));
		line[strcspn(line, "\n")] = '\0';
		symbol = strrchr(line, ' ');
		if (symbol == NULL)
			symbol = line;
		else
			symbol++;
		assert(!has_forbidden_symbol(symbol));
	}
	assert(!ferror(output));
	status = pclose(output);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC2: print_usage writes the exact current bytes to stderr only. */
static void test_ac2_usage_exact_stderr(void)
{
	assert_output(print_usage, "\nUsage: ./codexion coders burnout compile debug refactor "
		"compiles cooldown fifo|edf\n\n"
		"  e.g. ./codexion 4 800 200 200 200 3 50 edf\n");
}

/* AC3: print_run_error writes exactly one simulation error to stderr. */
static void test_ac3_run_error_exact_stderr(void)
{
	assert_output(print_run_error, "Error: simulation failed\n");
}

static void assert_arg_error(int position, const char *expected)
{
	char out_text[CAPACITY];
	char err_text[CAPACITY];
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;
	size_t out_len;
	size_t err_len;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(err), STDERR_FILENO) >= 0);
	print_arg_error(position, ARG_BAD_RANGE);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(saved_err, STDERR_FILENO) >= 0);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	out_len = read_bytes(out, out_text, sizeof(out_text));
	err_len = read_bytes(err, err_text, sizeof(err_text));
	assert(out_len == 0);
	assert(err_len == strlen(expected));
	assert(memcmp(err_text, expected, err_len) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

/* AC4: scheduler and numeric messages retain their bytes; invalid slots stay silent. */
static void test_ac4_argument_errors(void)
{
	static const char *names[] = {"number_of_coders", "time_to_burnout",
		"time_to_compile", "time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown"};
	char expected[CAPACITY];
	int position;
	int count;

	position = 1;
	while (position <= 7)
	{
		count = snprintf(expected, sizeof(expected),
			"Error: %s: expected an integer from %d to %d\n",
			names[position - 1], get_arg_minimum(position), INT_MAX);
		assert(count > 0 && (size_t)count < sizeof(expected));
		assert_arg_error(position, expected);
		position++;
	}
	assert_arg_error(8, "Error: scheduler: expected fifo or edf\n");
	assert_arg_error(0, "");
	assert_arg_error(9, "");
	assert_arg_error(-1, "");
}

/* AC5: closing stderr leaves the invalid invocation exiting 1 normally. */
static void test_ac5_closed_stderr_exit(void)
{
	pid_t child;
	int status;
	const char *binary;
	char *const args[] = {(char *)"codexion", (char *)"1", (char *)"2", NULL};

	binary = get_binary();
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		if (close(STDERR_FILENO) != 0)
			_exit(126);
		execv(binary, args);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
}

int main(void)
{
	test_ac1_no_forbidden_symbols();
	test_ac2_usage_exact_stderr();
	test_ac3_run_error_exact_stderr();
	test_ac4_argument_errors();
	test_ac5_closed_stderr_exit();
	return (0);
}
