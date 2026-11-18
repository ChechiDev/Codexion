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

#define OUTPUT_CAP 8192

typedef struct s_run
{
	int exit_code;
	char out[OUTPUT_CAP];
	char err[OUTPUT_CAP];
} t_run;

static const char *usage =
	"\nUsage: ./codexion coders burnout compile debug refactor "
	"compiles cooldown fifo|edf\n\n"
	"  e.g. ./codexion 4 800 200 200 200 3 50 edf\n";

static void expect_number(const char *input, int expected)
{
	int value;

	value = 12345;
	assert(parse_number(input, &value) == 0);
	assert(value == expected);
}

static void expect_number_error(const char *input, int expected)
{
	int value;

	value = 12345;
	assert(parse_number(input, &value) == expected);
	assert(value == 12345);
}

/* AC1: one leading plus and surrounding whitespace are accepted. */
static void test_ac1_accepted_plus_sign(void)
{
	expect_number("+600", 600);
	expect_number("+0", 0);
	expect_number("+007", 7);
	expect_number("+2147483647", INT_MAX);
	expect_number(" +42 ", 42);
	expect_number("\t+1\n", 1);
}

/* AC2: a plus without an immediately following digit is a sign error. */
static void test_ac2_malformed_sign(void)
{
	static const char *inputs[] = {
		"+", "++600", "+-600", "+ 600", "+\t600", "  +  ",
		"+a", "++"
	};
	size_t i;

	for (i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
		expect_number_error(inputs[i], -2);
}

/* AC3: other invalid values and null pointers retain range errors. */
static void test_ac3_range_errors(void)
{
	static const char *inputs[] = {
		"-600", "-0", "- 0", "--1", "-+600", "6+00", "600+",
		"+60a", "+1.5", "+2147483648", "+99999999999", "",
		"   ", "abc"
	};
	size_t i;
	int value;

	for (i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
		expect_number_error(inputs[i], -1);
	value = 12345;
	assert(parse_number(NULL, &value) == -1 && value == 12345);
	assert(parse_number("+600", NULL) == -1);
}

static void set_valid_args(char **args)
{
	args[0] = "codexion";
	args[1] = "+4";
	args[2] = "+800";
	args[3] = "+200";
	args[4] = "+200";
	args[5] = "+200";
	args[6] = "+3";
	args[7] = "+0";
	args[8] = "edf";
	args[9] = NULL;
}

/* AC4: config accepts signed positives and exposes fixed error-kind values. */
static void test_ac4_config_accepts_plus(void)
{
	char *args[10];
	t_config config;
	int invalid[9];
	int i;

	assert(ARG_BAD_RANGE == 1);
	assert(ARG_BAD_SIGN == 2);
	set_valid_args(args);
	for (i = 0; i < 9; i++)
		invalid[i] = 42;
	assert(parse_config(args, &config, invalid) == 0);
	for (i = 1; i <= 8; i++)
		assert(invalid[i] == 0);
	assert(config.number_of_coders == 4);
	assert(config.time_to_burnout == 800);
	assert(config.time_to_compile == 200);
	assert(config.time_to_debug == 200);
	assert(config.time_to_refactor == 200);
	assert(config.number_of_compiles_required == 3);
	assert(config.dongle_cooldown == 0);
	assert(config.mode == MODE_EDF);
}

/* AC5: config stores each kind but returns invalid position count. */
static void test_ac5_config_error_kinds(void)
{
	char *args[10];
	t_config config;
	int invalid[9];
	int i;

	set_valid_args(args);
	args[2] = "++800";
	args[3] = "-200";
	args[5] = "+ 200";
	args[8] = "rr";
	for (i = 0; i < 9; i++)
		invalid[i] = 42;
	assert(parse_config(args, &config, invalid) == 4);
	assert(invalid[0] == 42);
	for (i = 1; i <= 8; i++)
	{
		if (i == 2 || i == 5)
			assert(invalid[i] == ARG_BAD_SIGN);
		else if (i == 3 || i == 8)
			assert(invalid[i] == ARG_BAD_RANGE);
		else
			assert(invalid[i] == 0);
	}
}

/* AC6: plus zero remains below the two positive-only minimums. */
static void test_ac6_minimums(void)
{
	char *args[10];
	t_config config;
	int invalid[9];
	int position;

	for (position = 1; position <= 2; position++)
	{
		set_valid_args(args);
		args[position] = "+0";
		assert(parse_config(args, &config, invalid) == 1);
		assert(invalid[position] == ARG_BAD_RANGE);
	}
}

static const char *binary_path(void)
{
	const char *path;

	path = getenv("CODEXION_BIN");
	if (path == NULL || path[0] == '\0')
		return ("coders/codexion");
	return (path);
}

static void read_stream(FILE *stream, char *buffer, size_t capacity)
{
	size_t used;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, capacity - 1, stream);
	assert(!ferror(stream));
	assert(used < capacity - 1 || fgetc(stream) == EOF);
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
		if (dup2(fileno(out), STDOUT_FILENO) < 0
			|| dup2(fileno(err), STDERR_FILENO) < 0)
			_exit(126);
		execvp(argv[0], argv);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status));
	result.exit_code = WEXITSTATUS(status);
	read_stream(out, result.out, sizeof(result.out));
	read_stream(err, result.err, sizeof(result.err));
	assert(fclose(out) == 0 && fclose(err) == 0);
	return (result);
}

/* AC7: the binary accepts plus and retains one error prefix and usage. */
static void test_ac7_full_binary(void)
{
	char *valid[] = {(char *)binary_path(), "4", "+800", "+200", "+200",
		"+200", "1", "+0", "fifo", NULL};
	char *invalid[] = {(char *)binary_path(), "4", "++800", "200", "200",
		"200", "3", "50", "edf", NULL};
	const char *prefix = "Error: time_to_burnout: ";
	const char *line_end;
	t_run run;

	run = run_process(valid);
	assert(run.exit_code == 0 && run.err[0] == '\0');
	run = run_process(invalid);
	assert(run.exit_code == 1 && run.out[0] == '\0');
	assert(strncmp(run.err, prefix, strlen(prefix)) == 0);
	line_end = strchr(run.err, '\n');
	assert(line_end != NULL && line_end > run.err + strlen(prefix));
	assert(strcmp(line_end + 1, usage) == 0);
}

/* AC8: the deliverable builds and norminette reports no error. */
static void test_ac8_build_and_norm(void)
{
	FILE *pipe;
	struct stat info;
	char output[OUTPUT_CAP];
	size_t used;
	int status;

	pipe = popen("make -C coders 2>&1", "r");
	assert(pipe != NULL);
	while (fread(output, 1, sizeof(output), pipe) != 0)
		;
	assert(!ferror(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	pipe = popen("norminette coders/ 2>&1", "r");
	assert(pipe != NULL);
	used = fread(output, 1, sizeof(output) - 1, pipe);
	assert(!ferror(pipe));
	assert(used < sizeof(output) - 1 || fgetc(pipe) == EOF);
	output[used] = '\0';
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(strstr(output, "Error") == NULL);
}

int main(void)
{
	test_ac1_accepted_plus_sign();
	test_ac2_malformed_sign();
	test_ac3_range_errors();
	test_ac4_config_accepts_plus();
	test_ac5_config_error_kinds();
	test_ac6_minimums();
	test_ac7_full_binary();
	test_ac8_build_and_norm();
	return (0);
}
