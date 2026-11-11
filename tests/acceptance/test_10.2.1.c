#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUTPUT_CAP 65536

typedef struct s_run
{
	int status;
	char out[OUTPUT_CAP];
	char err[OUTPUT_CAP];
	size_t out_len;
	size_t err_len;
} t_run;

typedef struct s_case
{
	const char *count;
	const char *burnout;
	const char *compile;
	const char *debug;
	const char *refactor;
	const char *required;
	const char *cooldown;
	int coders;
	int target;
} t_case;

static const t_case cases[] = {
	{"6", "870", "100", "50", "50", "3", "10", 6, 3},
	{"5", "1610", "200", "200", "200", "3", "0", 5, 3},
	{"4", "1610", "200", "200", "200", "3", "50", 4, 3},
	{"2", "1010", "200", "200", "200", "3", "0", 2, 3},
	{"6", "1200", "100", "50", "50", "5", "10", 6, 5}
};
static t_run runs[sizeof(cases) / sizeof(cases[0])];

static const char *get_binary_path(void)
{
	const char *path;

	path = getenv("CODEXION_BIN");
	if (path == NULL || *path == '\0')
		return ("coders/codexion");
	return (path);
}

static size_t read_stream(FILE *stream, char *text)
{
	size_t length;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	length = fread(text, 1, OUTPUT_CAP - 1, stream);
	assert(!ferror(stream));
	assert(fgetc(stream) == EOF && !ferror(stream));
	text[length] = '\0';
	return (length);
}

static void run_case(size_t index)
{
	const t_case *scenario;
	const char *path;
	FILE *out;
	FILE *err;
	pid_t child;
	pid_t waited;
	int status;

	scenario = &cases[index];
	path = get_binary_path();
	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		alarm(getenv("CODEXION_UNDER_VALGRIND") || getenv("TSAN_OPTIONS")
			? 120 : 20);
		if (dup2(fileno(out), STDOUT_FILENO) < 0
			|| dup2(fileno(err), STDERR_FILENO) < 0)
			_exit(126);
		execl(path, path, scenario->count, scenario->burnout,
			scenario->compile, scenario->debug, scenario->refactor,
			scenario->required, scenario->cooldown, "edf", (char *)NULL);
		_exit(127);
	}
	do
	{
		status = 0;
		waited = waitpid(child, &status, 0);
	} while (waited < 0 && errno == EINTR);
	assert(waited == child);
	assert(WIFEXITED(status));
	runs[index].status = WEXITSTATUS(status);
	runs[index].out_len = read_stream(out, runs[index].out);
	runs[index].err_len = read_stream(err, runs[index].err);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

static int get_compile_count(const char *output, int coder)
{
	char pattern[32];
	const char *cursor;
	int count;

	assert(snprintf(pattern, sizeof(pattern), " %d is compiling\n", coder)
		> 0);
	count = 0;
	cursor = output;
	while ((cursor = strstr(cursor, pattern)) != NULL)
	{
		count++;
		cursor += strlen(pattern);
	}
	return (count);
}

static void assert_target_met(size_t index)
{
	int coder;

	run_case(index);
	assert(runs[index].status == 0);
	assert(strstr(runs[index].out, "burned out") == NULL);
	for (coder = 1; coder <= cases[index].coders; coder++)
		assert(get_compile_count(runs[index].out, coder)
			>= cases[index].target);
}

/* AC1: six coders at the 870 ms viability boundary reach three compiles. */
static void test_ac1_six_coders_at_bound(void)
{
	assert_target_met(0);
}

/* AC2: five coders at the 1610 ms viability boundary reach three compiles. */
static void test_ac2_five_coders_at_bound(void)
{
	assert_target_met(1);
}

/* AC3: four coders with cooldown at the bound reach three compiles. */
static void test_ac3_cooldown_at_bound(void)
{
	assert_target_met(2);
}

/* AC4: the two-coder table at the bound reaches three compiles each. */
static void test_ac4_smallest_table_at_bound(void)
{
	assert_target_met(3);
}

/* AC5: six coders above the bound reach five compiles each. */
static void test_ac5_above_bound(void)
{
	assert_target_met(4);
}

static int is_state_text(const char *text, size_t length)
{
	static const char *states[] = {
		"has taken a dongle", "is compiling", "is debugging",
		"is refactoring", "burned out"
	};
	size_t index;

	for (index = 0; index < sizeof(states) / sizeof(states[0]); index++)
		if (strlen(states[index]) == length
			&& strncmp(text, states[index], length) == 0)
			return (1);
	return (0);
}

static void assert_log_format(size_t index)
{
	const char *cursor;
	const char *end;
	char *next;
	long timestamp;
	long previous;
	long coder;
	int lines;

	assert(runs[index].err_len == 0);
	assert(memchr(runs[index].out, '\0', runs[index].out_len) == NULL);
	cursor = runs[index].out;
	previous = -1;
	lines = 0;
	while (*cursor != '\0')
	{
		assert(*cursor >= '0' && *cursor <= '9');
		errno = 0;
		timestamp = strtol(cursor, &next, 10);
		assert(errno == 0 && timestamp >= previous && *next == ' ');
		cursor = next + 1;
		assert(*cursor >= '1' && *cursor <= '9');
		errno = 0;
		coder = strtol(cursor, &next, 10);
		assert(errno == 0 && coder >= 1 && coder <= cases[index].coders);
		assert(*next == ' ');
		cursor = next + 1;
		end = strchr(cursor, '\n');
		assert(end != NULL && is_state_text(cursor, (size_t)(end - cursor)));
		previous = timestamp;
		cursor = end + 1;
		lines++;
	}
	assert(lines > 0);
}

/* AC6: every captured line is complete, ordered, and stderr is empty. */
static void test_ac6_complete_ordered_logs(void)
{
	size_t index;

	for (index = 0; index < sizeof(cases) / sizeof(cases[0]); index++)
		assert_log_format(index);
}

int main(void)
{
	alarm(getenv("CODEXION_UNDER_VALGRIND") || getenv("TSAN_OPTIONS")
		? 300 : 60);
	test_ac1_six_coders_at_bound();
	test_ac2_five_coders_at_bound();
	test_ac3_cooldown_at_bound();
	test_ac4_smallest_table_at_bound();
	test_ac5_above_bound();
	test_ac6_complete_ordered_logs();
	alarm(0);
	return (0);
}
