#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define OUTPUT_CAP 65536

typedef struct s_run
{
	int status;
	size_t out_len;
	char out[OUTPUT_CAP];
	char err[OUTPUT_CAP];
} t_run;

typedef struct s_log
{
	int lines;
	int compiles[5];
	int burnouts;
	int first_state;
	int last_state;
	long burnout_ts;
} t_log;

static const char *usage =
	"Usage: ./codexion number_of_coders time_to_burnout "
	"time_to_compile time_to_debug time_to_refactor "
	"number_of_compiles_required dongle_cooldown scheduler\n"
	"  scheduler is either fifo or edf\n";

static const char *binary_path(void)
{
	const char *path = getenv("CODEXION_BIN");

	if (path == NULL || path[0] == '\0')
		return ("coders/codexion");
	return (path);
}

static void read_stream(FILE *stream, char *text, size_t capacity,
		size_t *length)
{
	int extra;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	*length = fread(text, 1, capacity - 1, stream);
	assert(!ferror(stream));
	extra = fgetc(stream);
	assert(extra == EOF && !ferror(stream));
	text[*length] = '\0';
}

static t_run run_process(char *const args[], int use_valgrind,
		int full_stdout, unsigned timeout)
{
	FILE *out;
	FILE *err;
	pid_t child;
	int status;
	t_run run;
	size_t err_len;
	char *vg_args[16];
	int i;
	int full;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		alarm(timeout);
		full = -1;
		if (full_stdout)
			full = open("/dev/full", O_WRONLY);
		if (full_stdout && full < 0)
			_exit(126);
		if (dup2(full_stdout ? full : fileno(out), STDOUT_FILENO) < 0
			|| dup2(fileno(err), STDERR_FILENO) < 0)
			_exit(126);
		if (full >= 0)
			close(full);
		if (use_valgrind)
		{
			vg_args[0] = "valgrind";
			vg_args[1] = "--leak-check=full";
			vg_args[2] = "--show-leak-kinds=all";
			vg_args[3] = "--errors-for-leak-kinds=all";
			vg_args[4] = "--error-exitcode=42";
			i = -1;
			while (args[++i] != NULL)
				vg_args[i + 5] = args[i];
			vg_args[i + 5] = NULL;
			execvp(vg_args[0], vg_args);
		}
		else
			execv(args[0], args);
		_exit(127);
	}
	do
	{
		status = 0;
		i = waitpid(child, &status, 0);
	} while (i < 0 && errno == EINTR);
	assert(i == child && WIFEXITED(status));
	run.status = WEXITSTATUS(status);
	read_stream(out, run.out, sizeof(run.out), &run.out_len);
	read_stream(err, run.err, sizeof(run.err), &err_len);
	assert(fclose(out) == 0 && fclose(err) == 0);
	return (run);
}

static int get_slack(void)
{
	const char *value = getenv("CODEXION_TIME_SLACK_MS");
	char *end;
	long slack;

	if (value == NULL)
		return (0);
	errno = 0;
	slack = strtol(value, &end, 10);
	assert(errno == 0 && end != value && *end == '\0');
	assert(slack >= 0 && slack <= INT_MAX - 310);
	return ((int)slack);
}

static int get_state(const char *text)
{
	if (strcmp(text, "has taken a dongle") == 0)
		return (1);
	if (strcmp(text, "is compiling") == 0)
		return (2);
	if (strcmp(text, "is debugging") == 0)
		return (3);
	if (strcmp(text, "is refactoring") == 0)
		return (4);
	if (strcmp(text, "burned out") == 0)
		return (5);
	return (0);
}

static t_log inspect_log(const t_run *run, int max_id)
{
	t_log log = {0};
	const char *begin = run->out;
	const char *end;
	char line[128];
	char prefix[64];
	size_t len;
	long ts;
	long previous = -1;
	int id;
	int used;
	int state;

	while (*begin != '\0')
	{
		end = strchr(begin, '\n');
		assert(end != NULL);
		len = (size_t)(end - begin);
		assert(len > 0 && len < sizeof(line));
		memcpy(line, begin, len);
		line[len] = '\0';
		used = -1;
		assert(sscanf(line, "%ld %d %n", &ts, &id, &used) == 2);
		assert(used > 0 && ts >= 0 && ts >= previous);
		assert(id >= 1 && id <= max_id);
		assert(snprintf(prefix, sizeof(prefix), "%ld %d ", ts, id) == used);
		assert(strncmp(line, prefix, (size_t)used) == 0);
		state = get_state(line + used);
		assert(state != 0);
		if (log.lines == 0)
			log.first_state = state;
		if (state == 2)
			log.compiles[id]++;
		if (state == 5)
		{
			log.burnouts++;
			log.burnout_ts = ts;
		}
		log.last_state = state;
		log.lines++;
		previous = ts;
		begin = end + 1;
	}
	return (log);
}

static long elapsed_ms(const struct timespec *start, const struct timespec *end)
{
	return ((end->tv_sec - start->tv_sec) * 1000L
		+ (end->tv_nsec - start->tv_nsec) / 1000000L);
}

/* AC1: the target-met binary run emits complete ordered state lines and exits 0. */
static void test_ac1_target_met(void)
{
	char *args[] = {(char *)binary_path(), "4", "5000", "200", "200",
		"200", "3", "50", "fifo", NULL};
	t_run run = run_process(args, 0, 0, 20);
	t_log log;
	int id;

	assert(run.status == 0 && run.out_len > 0 && run.err[0] == '\0');
	log = inspect_log(&run, 4);
	assert(log.lines > 0 && log.burnouts == 0);
	for (id = 1; id <= 4; ++id)
		assert(log.compiles[id] >= 3);
}

/* AC2: the unavoidable burnout is the only burnout, on the final line. */
static void test_ac2_burnout(void)
{
	char *args[] = {(char *)binary_path(), "2", "300", "400", "100",
		"100", "3", "0", "fifo", NULL};
	t_run run = run_process(args, 0, 0, 20);
	t_log log;

	assert(run.status == 0 && run.err[0] == '\0');
	log = inspect_log(&run, 2);
	assert(log.burnouts == 1 && log.last_state == 5);
	assert(log.burnout_ts >= 300 && log.burnout_ts <= 310 + get_slack());
}

/* AC3: one coder takes one dongle and then burns out, with no other lines. */
static void test_ac3_one_coder(void)
{
	char *args[] = {(char *)binary_path(), "1", "300", "200", "200",
		"200", "3", "50", "edf", NULL};
	t_run run = run_process(args, 0, 0, 20);
	t_log log;

	assert(run.status == 0 && run.err[0] == '\0');
	log = inspect_log(&run, 1);
	assert(log.lines == 2 && log.first_state == 1);
	assert(log.burnouts == 1 && log.last_state == 5);
	assert(log.burnout_ts >= 300 && log.burnout_ts <= 310 + get_slack());
}

/* AC4: zero required compiles exits silently before simulation time passes. */
static void test_ac4_zero_required(void)
{
	char *args[] = {(char *)binary_path(), "4", "800", "200", "200",
		"200", "0", "50", "fifo", NULL};
	struct timespec start;
	struct timespec end;
	t_run run;

	assert(clock_gettime(CLOCK_MONOTONIC, &start) == 0);
	run = run_process(args, 0, 0, 20);
	assert(clock_gettime(CLOCK_MONOTONIC, &end) == 0);
	assert(run.status == 0 && run.out_len == 0 && run.err[0] == '\0');
	assert(elapsed_ms(&start, &end) <= 500 + get_slack());
}

/* AC5: a failed log write produces exit 2 and exactly one runtime error. */
static void test_ac5_runtime_failure(void)
{
	char *args[] = {(char *)binary_path(), "4", "5000", "200", "200",
		"200", "3", "50", "fifo", NULL};
	t_run run = run_process(args, 0, 1, 20);

	assert(run.status == 2);
	assert(strcmp(run.err, "Error: simulation failed\n") == 0);
}

/* AC6: invalid fields and wrong counts retain status 1 and exact usage output. */
static void test_ac6_argument_errors(void)
{
	char *args[] = {(char *)binary_path(), "4", "-5", "abc", "200",
		"200", "3", "50", "rr", NULL};
	char expected[1024];
	t_run run;

	assert(snprintf(expected, sizeof(expected),
		"Error: time_to_burnout: expected an integer from 1 to 2147483647\n"
		"Error: time_to_compile: expected an integer from 0 to 2147483647\n"
		"Error: scheduler: expected fifo or edf\n%s", usage)
		< (int)sizeof(expected));
	run = run_process(args, 0, 0, 20);
	assert(run.status == 1 && run.out_len == 0);
	assert(strcmp(run.err, expected) == 0);
	args[8] = NULL;
	run = run_process(args, 0, 0, 20);
	assert(run.status == 1 && run.out_len == 0);
	assert(strcmp(run.err, usage) == 0);
}

/* AC7: target, burnout and zero-work runs remain leak-free under Valgrind. */
static void test_ac7_valgrind_full_runs(void)
{
	char *target[] = {(char *)binary_path(), "4", "5000", "200", "200",
		"200", "3", "50", "fifo", NULL};
	char *burnout[] = {(char *)binary_path(), "2", "300", "400", "100",
		"100", "3", "0", "fifo", NULL};
	char *zero[] = {(char *)binary_path(), "4", "800", "200", "200",
		"200", "0", "50", "fifo", NULL};
	char **cases[] = {target, burnout, zero};
	t_run run;
	int i;

	i = 0;
	while (i < 3)
	{
		run = run_process(cases[i], 1, 0, 120);
		if (run.status == 127 && strstr(run.err, "valgrind") == NULL)
		{
			fprintf(stderr, "AC7 skipped: valgrind is not installed\n");
			return ;
		}
		assert(run.status == 0);
		assert(strstr(run.err, "ERROR SUMMARY: 0 errors") != NULL);
		i++;
	}
}

int main(void)
{
	test_ac1_target_met();
	test_ac2_burnout();
	test_ac3_one_coder();
	test_ac4_zero_required();
	test_ac5_runtime_failure();
	test_ac6_argument_errors();
	test_ac7_valgrind_full_runs();
	return (0);
}
