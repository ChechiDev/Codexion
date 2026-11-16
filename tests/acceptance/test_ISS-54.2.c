#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define CAP 8192
#define README_CAP 131072

typedef struct s_run
{
	int status;
	char out[CAP];
	char err[CAP];
} t_run;

static const char *usage =
	"\nUsage: ./codexion coders burnout compile debug refactor "
	"compiles cooldown fifo|edf\n\n"
	"  e.g. ./codexion 4 800 200 200 200 3 50 edf\n";

static const char *errors =
	"Error: time_to_burnout: expected an integer from 1 to 2147483647\n"
	"Error: time_to_compile: expected an integer from 0 to 2147483647\n"
	"Error: scheduler: expected fifo or edf\n";

static void read_stream(FILE *stream, char *buffer, size_t capacity)
{
	size_t used;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, capacity - 1, stream);
	assert(!ferror(stream));
	assert(used < capacity - 1 || fgetc(stream) == EOF);
	buffer[used] = '\0';
}

static t_run run_binary(char *arguments[])
{
	const char *binary;
	FILE *out;
	FILE *err;
	pid_t child;
	int status;
	t_run run;

	binary = getenv("CODEXION_BIN");
	if (binary == NULL || *binary == '\0')
		binary = "coders/codexion";
	arguments[0] = (char *)binary;
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
		execv(binary, arguments);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status));
	run.status = WEXITSTATUS(status);
	read_stream(out, run.out, sizeof(run.out));
	read_stream(err, run.err, sizeof(run.err));
	assert(fclose(out) == 0 && fclose(err) == 0);
	return (run);
}

/* AC1: print_usage writes exactly four specified lines to stderr only. */
static void test_ac1_exact_usage(void)
{
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;
	char out_text[CAP];
	char err_text[CAP];

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(err), STDERR_FILENO) >= 0);
	print_usage();
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(saved_err, STDERR_FILENO) >= 0);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	read_stream(out, out_text, sizeof(out_text));
	read_stream(err, err_text, sizeof(err_text));
	assert(out_text[0] == '\0');
	assert(strcmp(err_text, usage) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

/* AC2: zero, seven and nine arguments yield only the exact usage. */
static void test_ac2_wrong_argument_counts(void)
{
	char *arguments[11] = {NULL};
	t_run run;
	int counts[] = {0, 7, 9};
	int i;
	int j;

	i = 0;
	while (i < 3)
	{
		j = 1;
		while (j <= counts[i])
			arguments[j++] = "1";
		arguments[j] = NULL;
		run = run_binary(arguments);
		assert(run.status == 1 && run.out[0] == '\0');
		assert(strcmp(run.err, usage) == 0);
		i++;
	}
}

/* AC3: three invalid values yield three ordered errors then exact usage. */
static void test_ac3_invalid_values(void)
{
	char *arguments[] = {NULL, "4", "-5", "abc", "200", "200",
		"3", "50", "rr", NULL};
	char expected[CAP];
	t_run run;

	assert(snprintf(expected, sizeof(expected), "%s%s", errors, usage)
		< (int)sizeof(expected));
	run = run_binary(arguments);
	assert(run.status == 1 && run.out[0] == '\0');
	assert(strcmp(run.err, expected) == 0);
}

static void assert_readme_block(const char *readme, const char *command,
	const char *expected)
{
	const char *start;
	const char *end;

	start = strstr(readme, command);
	assert(start != NULL);
	start += strlen(command);
	end = strstr(start, "```");
	assert(end != NULL);
	assert((size_t)(end - start) == strlen(expected));
	assert(strncmp(start, expected, strlen(expected)) == 0);
}

/* AC4: both fenced README examples match binary stderr and omit old usage. */
static void test_ac4_readme_examples(void)
{
	char readme[README_CAP];
	char combined[CAP];
	char *no_args[] = {NULL, NULL};
	char *invalid[] = {NULL, "4", "-5", "abc", "200", "200",
		"3", "50", "rr", NULL};
	FILE *file;
	t_run run;

	file = fopen("README.md", "rb");
	assert(file != NULL);
	read_stream(file, readme, sizeof(readme));
	assert(fclose(file) == 0);
	run = run_binary(no_args);
	assert(run.status == 1 && strcmp(run.err, usage) == 0);
	assert_readme_block(readme, "```\n$ ./coders/codexion\n", run.err);
	run = run_binary(invalid);
	assert(run.status == 1);
	assert(snprintf(combined, sizeof(combined), "%s%s", errors, usage)
		< (int)sizeof(combined));
	assert(strcmp(run.err, combined) == 0);
	assert_readme_block(readme,
		"```\n$ ./coders/codexion 4 -5 abc 200 200 3 50 rr\n", run.err);
	assert(strstr(readme, "Usage: ./codexion number_of_coders") == NULL);
	assert(strstr(readme, "scheduler is either fifo or edf") == NULL);
}

/* AC5: the deliverable builds and passes norminette. */
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
	test_ac1_exact_usage();
	test_ac2_wrong_argument_counts();
	test_ac3_invalid_values();
	test_ac4_readme_examples();
	test_ac5_build_and_norm();
	return (0);
}
