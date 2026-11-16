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

typedef struct s_run
{
	int status;
	char out[CAP];
	char err[CAP];
} t_run;

static const char *usage =
	"Usage: ./codexion number_of_coders time_to_burnout "
	"time_to_compile time_to_debug time_to_refactor "
	"number_of_compiles_required dongle_cooldown scheduler\n"
	"  scheduler is either fifo or edf\n";

static void valid_args(char **argv)
{
	const char *values[] = {"codexion", "4", "800", "200", "200",
		"200", "3", "50", "edf"};
	int i;

	i = 0;
	while (i <= 8)
	{
		argv[i] = (char *)values[i];
		i++;
	}
	argv[9] = NULL;
}

static void assert_flags(const int *invalid, int a, int b, int c)
{
	int i;

	assert(invalid[0] == 77);
	i = 1;
	while (i <= 8)
	{
		assert(invalid[i] == (i == a || i == b || i == c));
		i++;
	}
}

/* AC1: valid EDF input fills the config and clears every argument flag. */
static void test_ac1_valid_edf_config(void)
{
	char *argv[10];
	t_config config;
	int invalid[9];
	int i;

	valid_args(argv);
	invalid[0] = 77;
	i = 1;
	while (i <= 8)
		invalid[i++] = 42;
	assert(parse_config(argv, &config, invalid) == 0);
	assert_flags(invalid, 0, 0, 0);
	assert(config.number_of_coders == 4);
	assert(config.time_to_burnout == 800);
	assert(config.time_to_compile == 200);
	assert(config.time_to_debug == 200);
	assert(config.time_to_refactor == 200);
	assert(config.number_of_compiles_required == 3);
	assert(config.dongle_cooldown == 50);
	assert(config.mode == MODE_EDF);
}

/* AC2: only positions 2, 3, and 8 remain flagged after a garbage fill. */
static void test_ac2_mixed_invalid_flags(void)
{
	char *argv[10];
	t_config config;
	int invalid[9];
	int i;

	valid_args(argv);
	argv[2] = "-5";
	argv[3] = "abc";
	argv[8] = "rr";
	invalid[0] = 77;
	i = 1;
	while (i <= 8)
		invalid[i++] = 42;
	assert(parse_config(argv, &config, invalid) == 3);
	assert_flags(invalid, 2, 3, 8);
}

/* AC3: eight invalid arguments report eight flags and preserve index zero. */
static void test_ac3_all_invalid_flags(void)
{
	char *argv[10];
	t_config config;
	int invalid[9];
	int i;

	valid_args(argv);
	i = 1;
	while (i <= 7)
		argv[i++] = "abc";
	argv[8] = "rr";
	invalid[0] = 77;
	assert(parse_config(argv, &config, invalid) == 8);
	assert(invalid[0] == 77);
	i = 1;
	while (i <= 8)
		assert(invalid[i++] == 1);
}

/* AC4: each invalid value is isolated and no input string is changed. */
static void test_ac4_single_invalid_position_and_argv(void)
{
	const char *numbers[] = {"-1", "abc", "", "2147483648"};
	const char *modes[] = {"FIFO", " edf", "rr"};
	const char *value;
	char *argv[10];
	char input[9][32];
	char original[9][32];
	t_config config;
	int invalid[9];
	int position;
	int k;
	size_t j;

	position = 1;
	while (position <= 8)
	{
		j = 0;
		while (j < (position == 8 ? 3u : 4u))
		{
			valid_args(argv);
			value = position == 8 ? modes[j] : numbers[j];
			k = 0;
			while (k <= 8)
			{
				assert(strlen(argv[k]) < sizeof(input[k]));
				strcpy(input[k], argv[k]);
				argv[k] = input[k];
				k++;
			}
			assert(strlen(value) < sizeof(input[position]));
			strcpy(input[position], value);
			k = 0;
			while (k <= 8)
			{
				strcpy(original[k], input[k]);
				k++;
			}
			invalid[0] = 77;
			assert(parse_config(argv, &config, invalid) == 1);
			assert_flags(invalid, position, 0, 0);
			k = 0;
			while (k <= 8)
			{
				assert(memcmp(argv[k], original[k],
					strlen(original[k]) + 1) == 0);
				k++;
			}
			j++;
		}
		position++;
	}
}

static void read_stream(FILE *file, char *buffer)
{
	size_t used;

	assert(fseek(file, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, CAP - 1, file);
	assert(!ferror(file));
	assert(used < CAP - 1 || fgetc(file) == EOF);
	buffer[used] = '\0';
}

static t_run run_process(char *const argv[])
{
	FILE *out;
	FILE *err;
	pid_t child;
	int status;
	t_run result;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		if (dup2(fileno(out), 1) < 0 || dup2(fileno(err), 2) < 0)
			_exit(126);
		execvp(argv[0], argv);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status));
	result.status = WEXITSTATUS(status);
	read_stream(out, result.out);
	read_stream(err, result.err);
	assert(fclose(out) == 0 && fclose(err) == 0);
	return (result);
}

static void binary_args(char **argv)
{
	valid_args(argv);
	argv[0] = getenv("CODEXION_BIN");
	if (argv[0] == NULL || argv[0][0] == '\0')
		argv[0] = "coders/codexion";
}

/* AC5: binary errors remain ordered with one usage and valid input succeeds. */
static void test_ac5_binary_output(void)
{
	char *argv[10];
	char expected[CAP];
	t_run run;

	binary_args(argv);
	argv[2] = "-5";
	argv[3] = "abc";
	argv[8] = "rr";
	assert(snprintf(expected, sizeof(expected),
		"Error: time_to_burnout: expected an integer from 1 to 2147483647\n"
		"Error: time_to_compile: expected an integer from 0 to 2147483647\n"
		"Error: scheduler: expected fifo or edf\n%s", usage)
		< (int)sizeof(expected));
	run = run_process(argv);
	assert(run.status == 1 && run.out[0] == '\0');
	assert(strcmp(run.err, expected) == 0);
	binary_args(argv);
	argv[2] = "-5";
	assert(snprintf(expected, sizeof(expected),
		"Error: time_to_burnout: expected an integer from 1 to 2147483647\n%s",
		usage) < (int)sizeof(expected));
	run = run_process(argv);
	assert(run.status == 1 && run.out[0] == '\0');
	assert(strcmp(run.err, expected) == 0);
	binary_args(argv);
	run = run_process(argv);
	assert(run.status == 0 && run.err[0] == '\0');
}

/* AC6: the binary builds, passes Norm, and rejects input cleanly under Valgrind. */
static void test_ac6_build_norm_and_valgrind(void)
{
	char *argv[16];
	struct stat info;
	int status;
	t_run run;

	status = system("make -C coders >/dev/null 2>&1");
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	status = system("norminette coders/ >/dev/null 2>&1");
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	argv[0] = "valgrind";
	argv[1] = "--leak-check=full";
	argv[2] = "--show-leak-kinds=all";
	argv[3] = "--errors-for-leak-kinds=all";
	argv[4] = "--error-exitcode=42";
	binary_args(argv + 5);
	argv[7] = "-5";
	run = run_process(argv);
	assert(run.status == 1 && run.out[0] == '\0');
	assert(strstr(run.err, "ERROR SUMMARY: 0 errors") != NULL);
	assert(strstr(run.err, "All heap blocks were freed") != NULL
		|| strstr(run.err, "definitely lost: 0 bytes") != NULL);
}

int main(void)
{
	test_ac1_valid_edf_config();
	test_ac2_mixed_invalid_flags();
	test_ac3_all_invalid_flags();
	test_ac4_single_invalid_position_and_argv();
	test_ac5_binary_output();
	test_ac6_build_norm_and_valgrind();
	return (0);
}
