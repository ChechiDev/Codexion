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

typedef struct s_output
{
	char out[OUTPUT_CAP];
	char err[OUTPUT_CAP];
} t_output;

static void read_stream(FILE *stream, char *buffer)
{
	size_t used;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, OUTPUT_CAP - 1, stream);
	assert(!ferror(stream));
	assert(used < OUTPUT_CAP - 1 || fgetc(stream) == EOF);
	buffer[used] = '\0';
}

static t_output capture_arg_error(int position)
{
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;
	t_output output;

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
	read_stream(out, output.out);
	read_stream(err, output.err);
	assert(fclose(out) == 0 && fclose(err) == 0);
	return (output);
}

/* AC1: each numeric position prints its subject name and exact range. */
static void test_ac1_numeric_argument_lines(void)
{
	static const char *names[] = {"number_of_coders", "time_to_burnout",
		"time_to_compile", "time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown"};
	char expected[256];
	t_output output;
	int position;

	position = 1;
	while (position <= 7)
	{
		assert(snprintf(expected, sizeof(expected),
			"Error: %s: expected an integer from %d to 2147483647\n",
			names[position - 1], position <= 2 ? 1 : 0)
			< (int)sizeof(expected));
		output = capture_arg_error(position);
		assert(output.out[0] == '\0');
		assert(strcmp(output.err, expected) == 0);
		position++;
	}
}

/* AC2: scheduler prints the exact new line to stderr only. */
static void test_ac2_scheduler_line(void)
{
	t_output output;

	output = capture_arg_error(8);
	assert(output.out[0] == '\0');
	assert(strcmp(output.err, "Error: scheduler: expected fifo or edf\n") == 0);
}

/* AC3: out-of-range positions print no bytes to either stream. */
static void test_ac3_out_of_range_silent(void)
{
	static const int positions[] = {0, 9, -1};
	t_output output;
	size_t i;

	i = 0;
	while (i < sizeof(positions) / sizeof(positions[0]))
	{
		output = capture_arg_error(positions[i]);
		assert(output.out[0] == '\0' && output.err[0] == '\0');
		i++;
	}
}

/* AC4: the binary orders all three new errors before its usage. */
static void test_ac4_binary_error_order(void)
{
	char *args[] = {"codexion", "4", "-5", "abc", "200", "200",
		"3", "50", "rr", NULL};
	const char *path;
	const char *prefix;
	FILE *out;
	FILE *err;
	pid_t child;
	int status;
	t_output output;

	path = getenv("CODEXION_BIN");
	if (path == NULL || path[0] == '\0')
		path = "coders/codexion";
	prefix = "Error: time_to_burnout: expected an integer from 1 to 2147483647\n"
		"Error: time_to_compile: expected an integer from 0 to 2147483647\n"
		"Error: scheduler: expected fifo or edf\n";
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
		execv(path, args);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
	read_stream(out, output.out);
	read_stream(err, output.err);
	assert(output.out[0] == '\0');
	assert(strncmp(output.err, prefix, strlen(prefix)) == 0);
	assert(strstr(output.err + strlen(prefix), "Usage: ./codexion") != NULL);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

/* AC5: the deliverable builds and passes Norm. */
static void test_ac5_build_and_norm(void)
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
	test_ac1_numeric_argument_lines();
	test_ac2_scheduler_line();
	test_ac3_out_of_range_silent();
	test_ac4_binary_error_order();
	test_ac5_build_and_norm();
	return (0);
}
