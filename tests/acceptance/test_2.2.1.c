#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "main.h"

#define OUTPUT_CAP 4096

static const char *get_expected_usage(void)
{
	return ("Usage: ./codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler\n"
		"  scheduler is either fifo or edf\n");
}

static void read_output(FILE *stream, char *buffer)
{
	size_t used;

	assert(fflush(stream) == 0);
	assert(fseek(stream, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, OUTPUT_CAP - 1, stream);
	assert(!ferror(stream));
	assert(used < OUTPUT_CAP - 1 || fgetc(stream) == EOF);
	buffer[used] = '\0';
}

static void run_count(int count)
{
	const char *binary;
	char *argv[11];
	char output[OUTPUT_CAP];
	char errors[OUTPUT_CAP];
	FILE *out;
	FILE *err;
	pid_t child;
	int status;
	int i;

	binary = getenv("CODEXION_BIN");
	if (binary == NULL || binary[0] == '\0')
		binary = "coders/codexion";
	argv[0] = (char *)binary;
	i = 1;
	while (i <= count)
		argv[i++] = (char *)"1";
	argv[i] = NULL;
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
		execv(binary, argv);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
	read_output(out, output);
	read_output(err, errors);
	assert(strcmp(output, "") == 0);
	assert(strcmp(errors, get_expected_usage()) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

/* AC1: every wrong count preserves the exact usage, stderr, and exit status. */
static void test_ac1_wrong_count_usage(void)
{
	int count;

	count = 0;
	while (count <= 9)
	{
		if (count != 8)
			run_count(count);
		count++;
	}
}

/* AC2: the public print_usage API works without linking main.c. */
static void test_ac2_public_print_usage(void)
{
	char errors[OUTPUT_CAP];
	FILE *err;
	int saved;

	err = tmpfile();
	assert(err != NULL);
	saved = dup(STDERR_FILENO);
	assert(saved >= 0);
	assert(dup2(fileno(err), STDERR_FILENO) >= 0);
	print_usage();
	assert(fflush(stderr) == 0);
	assert(dup2(saved, STDERR_FILENO) >= 0);
	assert(close(saved) == 0);
	read_output(err, errors);
	assert(strcmp(errors, get_expected_usage()) == 0);
	assert(fclose(err) == 0);
}

static void run_command(const char *command, char *output)
{
	FILE *pipe;
	size_t used;
	int status;

	pipe = popen(command, "r");
	assert(pipe != NULL);
	used = fread(output, 1, OUTPUT_CAP - 1, pipe);
	output[used] = '\0';
	assert(!ferror(pipe));
	assert(used < OUTPUT_CAP - 1 || fgetc(pipe) == EOF);
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC3: the deliverable builds, does not relink, and passes norminette. */
static void test_ac3_build_no_relink_and_norm(void)
{
	char output[OUTPUT_CAP];
	FILE *makefile;
	size_t used;

	makefile = fopen("coders/Makefile", "r");
	assert(makefile != NULL);
	used = fread(output, 1, OUTPUT_CAP - 1, makefile);
	assert(!ferror(makefile));
	assert(used < OUTPUT_CAP - 1 || fgetc(makefile) == EOF);
	output[used] = '\0';
	assert(strstr(output, "src/cli/print_usage.c") != NULL);
	assert(fclose(makefile) == 0);
	run_command("make -C coders 2>&1", output);
	run_command("make -C coders 2>&1", output);
	assert(strstr(output, "Nothing to be done") != NULL);
	run_command("norminette coders/ 2>&1", output);
	assert(strstr(output, "Error") == NULL);
}

int main(void)
{
	test_ac1_wrong_count_usage();
	test_ac2_public_print_usage();
	test_ac3_build_no_relink_and_norm();
	return (0);
}
