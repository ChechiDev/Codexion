#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	"Usage: ./codexion number_of_coders time_to_burnout "
	"time_to_compile time_to_debug time_to_refactor "
	"number_of_compiles_required dongle_cooldown scheduler\n"
	"  scheduler is either fifo or edf\n";

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

static void set_valid_args(char **args)
{
	args[0] = (char *)binary_path();
	args[1] = "4";
	args[2] = "800";
	args[3] = "200";
	args[4] = "200";
	args[5] = "200";
	args[6] = "3";
	args[7] = "50";
	args[8] = "edf";
	args[9] = NULL;
}

static void assert_rejected(char **args, const char *errors)
{
	char expected[OUTPUT_CAP];
	t_run run;

	assert(snprintf(expected, sizeof(expected), "%s%s", errors, usage)
		< (int)sizeof(expected));
	run = run_process(args);
	assert(run.exit_code == 1);
	assert(run.out[0] == '\0');
	assert(strcmp(run.err, expected) == 0);
}

/* AC1: a negative burnout time prints its error, then usage, on stderr. */
static void test_ac1_negative_burnout(void)
{
	char *args[10];

	set_valid_args(args);
	args[2] = "-5";
	assert_rejected(args, "Error: time_to_burnout must be an integer "
		"from 1 to 2147483647\n");
}

/* AC2: each invalid field produces only its own error and usage. */
static void test_ac2_invalid_fields(void)
{
	static const struct { int position; const char *value; const char *error; }
		cases[] = {
		{1, "0", "Error: number_of_coders must be an integer from 1 to 2147483647\n"},
		{3, "abc", "Error: time_to_compile must be an integer from 0 to 2147483647\n"},
		{4, "+3", "Error: time_to_debug must be an integer from 0 to 2147483647\n"},
		{5, "", "Error: time_to_refactor must be an integer from 0 to 2147483647\n"},
		{6, "2147483648", "Error: number_of_compiles_required must be an integer from 0 to 2147483647\n"},
		{7, "1.5", "Error: dongle_cooldown must be an integer from 0 to 2147483647\n"},
		{8, "FIFO", "Error: scheduler must be fifo or edf\n"},
		{8, " edf", "Error: scheduler must be fifo or edf\n"}
	};
	char *args[10];
	size_t i;

	i = 0;
	while (i < sizeof(cases) / sizeof(cases[0]))
	{
		set_valid_args(args);
		args[cases[i].position] = (char *)cases[i].value;
		assert_rejected(args, cases[i].error);
		i++;
	}
}

/* AC3: all three invalid positions are reported in argument order once. */
static void test_ac3_multiple_errors_ordered(void)
{
	char *args[10];

	set_valid_args(args);
	args[2] = "-5";
	args[3] = "abc";
	args[8] = "rr";
	assert_rejected(args, "Error: time_to_burnout must be an integer "
		"from 1 to 2147483647\n"
		"Error: time_to_compile must be an integer from 0 to 2147483647\n"
		"Error: scheduler must be fifo or edf\n");
}

/* AC4: standard, minimum, and trimmed numeric arguments are accepted. */
static void test_ac4_valid_arguments(void)
{
	char *args[10];
	t_run run;

	set_valid_args(args);
	run = run_process(args);
	assert(run.exit_code == 0 && run.err[0] == '\0');
	set_valid_args(args);
	args[1] = "1";
	args[2] = "1";
	args[3] = "0";
	args[4] = "0";
	args[5] = "0";
	args[6] = "0";
	args[7] = "0";
	args[8] = "fifo";
	run = run_process(args);
	assert(run.exit_code == 0 && run.err[0] == '\0');
	set_valid_args(args);
	args[1] = "  4  ";
	args[2] = " 0800";
	run = run_process(args);
	assert(run.exit_code == 0 && run.err[0] == '\0');
}

/* AC5: wrong argument counts print usage alone and exit with status 1. */
static void test_ac5_wrong_count_usage_only(void)
{
	char *args[11];
	t_run run;

	set_valid_args(args);
	args[8] = NULL;
	run = run_process(args);
	assert(run.exit_code == 1 && run.out[0] == '\0');
	assert(strcmp(run.err, usage) == 0);
	set_valid_args(args);
	args[9] = "extra";
	args[10] = NULL;
	run = run_process(args);
	assert(run.exit_code == 1 && run.out[0] == '\0');
	assert(strcmp(run.err, usage) == 0);
}

/* AC6: an invalid run has no Valgrind memory errors or leaks. */
static void test_ac6_invalid_run_valgrind_clean(void)
{
	char *args[16];
	t_run run;

	args[0] = "valgrind";
	args[1] = "--leak-check=full";
	args[2] = "--show-leak-kinds=all";
	args[3] = "--errors-for-leak-kinds=all";
	args[4] = "--error-exitcode=42";
	set_valid_args(args + 5);
	args[7] = "-5";
	run = run_process(args);
	assert(run.exit_code == 1);
	assert(run.out[0] == '\0');
	assert(strstr(run.err, "ERROR SUMMARY: 0 errors") != NULL);
	assert(strstr(run.err, "All heap blocks were freed") != NULL
		|| strstr(run.err, "definitely lost: 0 bytes") != NULL);
}

/* AC7: norminette reports no error for the deliverable. */
static void test_ac7_norminette_clean(void)
{
	int status;

	status = system("norminette coders/ >/dev/null 2>&1");
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

int main(void)
{
	test_ac1_negative_burnout();
	test_ac2_invalid_fields();
	test_ac3_multiple_errors_ordered();
	test_ac4_valid_arguments();
	test_ac5_wrong_count_usage_only();
	test_ac6_invalid_run_valgrind_clean();
	test_ac7_norminette_clean();
	return (0);
}
