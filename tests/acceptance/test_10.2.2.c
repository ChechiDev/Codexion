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
#define EVENT_CAP 4096

enum e_state { TAKE = 1, COMPILE, DEBUG, REFACTOR, BURNOUT };

typedef struct s_event
{
	long ts;
	int id;
	int state;
} t_event;

typedef struct s_run
{
	int status;
	size_t out_len;
	size_t err_len;
	char out[OUTPUT_CAP];
	char err[OUTPUT_CAP];
	t_event events[EVENT_CAP];
	int count;
} t_run;

static t_run run;

static const char *get_binary(void)
{
	const char *path = getenv("CODEXION_BIN");

	if (path == NULL || *path == '\0')
		return ("coders/codexion");
	return (path);
}

static size_t read_stream(FILE *stream, char *buffer)
{
	size_t length;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	length = fread(buffer, 1, OUTPUT_CAP - 1, stream);
	assert(!ferror(stream));
	assert(fgetc(stream) == EOF && !ferror(stream));
	buffer[length] = '\0';
	return (length);
}

static void run_case(const char *coders, const char *burnout,
		const char *compile, const char *cooldown, const char *required,
		const char *mode)
{
	const char *path = get_binary();
	FILE *out = tmpfile();
	FILE *err = tmpfile();
	pid_t child;
	pid_t waited;
	int status;

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
		execl(path, path, coders, burnout, compile, "100", "100",
			required, cooldown, mode, (char *)NULL);
		_exit(127);
	}
	do
	{
		waited = waitpid(child, &status, 0);
	} while (waited < 0 && errno == EINTR);
	assert(waited == child && WIFEXITED(status));
	run.status = WEXITSTATUS(status);
	run.out_len = read_stream(out, run.out);
	run.err_len = read_stream(err, run.err);
	assert(fclose(out) == 0 && fclose(err) == 0);
	run.count = 0;
}

static int get_state(const char *text, size_t length)
{
	static const char *states[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring", "burned out"};
	int i;

	for (i = 0; i < 5; i++)
		if (strlen(states[i]) == length
			&& memcmp(states[i], text, length) == 0)
			return (i + 1);
	return (0);
}

static void parse_log(int max_id)
{
	const char *cursor = run.out;
	const char *end = run.out + run.out_len;
	const char *line_end;
	char *next;
	long previous = -1;
	long parsed_id;
	t_event *event;

	assert(run.err_len == 0);
	assert(memchr(run.out, '\0', run.out_len) == NULL);
	while (cursor < end)
	{
		assert(run.count < EVENT_CAP);
		event = &run.events[run.count++];
		assert(*cursor >= '0' && *cursor <= '9');
		errno = 0;
		event->ts = strtol(cursor, &next, 10);
		assert(errno == 0 && next < end && *next == ' ');
		assert(event->ts >= previous);
		cursor = next + 1;
		assert(cursor < end && *cursor >= '1' && *cursor <= '9');
		errno = 0;
		parsed_id = strtol(cursor, &next, 10);
		assert(errno == 0 && next < end && *next == ' ');
		assert(parsed_id >= 1 && parsed_id <= max_id);
		event->id = (int)parsed_id;
		cursor = next + 1;
		line_end = memchr(cursor, '\n', (size_t)(end - cursor));
		assert(line_end != NULL);
		event->state = get_state(cursor, (size_t)(line_end - cursor));
		assert(event->state != 0);
		cursor = line_end + 1;
		previous = event->ts;
	}
	assert(run.count > 0);
}

/* AC1: both schedulers emit complete, ordered lines with valid IDs and states. */
static void test_ac1_complete_serialized_logs(void)
{
	int i;

	run_case("5", "5000", "100", "20", "3", "fifo");
	assert(run.status == 0);
	parse_log(5);
	for (i = 0; i < run.count; i++)
		assert(run.events[i].state != BURNOUT);
	run_case("5", "5000", "100", "20", "3", "edf");
	assert(run.status == 0);
	parse_log(5);
	for (i = 0; i < run.count; i++)
		assert(run.events[i].state != BURNOUT);
}

static void assert_two_takes(void)
{
	int takes[6] = {0};
	int i;
	t_event *event;

	for (i = 0; i < run.count; i++)
	{
		event = &run.events[i];
		if (event->state == TAKE)
			takes[event->id]++;
		if (event->state == COMPILE)
		{
			assert(takes[event->id] == 2);
			takes[event->id] = 0;
		}
	}
}

/* AC2: every compile follows exactly two takes by the same coder. */
static void test_ac2_two_dongles_per_compile(void)
{
	run_case("5", "5000", "100", "20", "3", "fifo");
	parse_log(5);
	assert_two_takes();
	run_case("5", "5000", "100", "20", "3", "edf");
	parse_log(5);
	assert_two_takes();
}

static void assert_cycles(void)
{
	int phase[6] = {0};
	int i;
	int id;
	int state;

	for (i = 0; i < run.count; i++)
	{
		id = run.events[i].id;
		state = run.events[i].state;
		if (state == BURNOUT)
		{
			assert(i == run.count - 1);
			continue;
		}
		if (phase[id] == 0 || phase[id] == REFACTOR)
			assert(state == TAKE);
		else if (phase[id] == TAKE)
			assert(state == TAKE || state == COMPILE);
		else if (phase[id] == COMPILE)
			assert(state == DEBUG);
		else if (phase[id] == DEBUG)
			assert(state == REFACTOR);
		phase[id] = state;
	}
}

/* AC3: per-coder state order follows the compile, debug, refactor cycle. */
static void test_ac3_state_cycle(void)
{
	run_case("5", "5000", "100", "20", "3", "fifo");
	parse_log(5);
	assert_cycles();
	run_case("5", "5000", "100", "20", "3", "edf");
	parse_log(5);
	assert_cycles();
}

static int is_neighbour(int a, int b, int count)
{
	return (a != b && (abs(a - b) == 1 || abs(a - b) == count - 1));
}

static void assert_exclusion(int coders)
{
	long last[6] = {-1, -1, -1, -1, -1, -1};
	int i;
	int other;
	t_event *event;

	for (i = 0; i < run.count; i++)
	{
		event = &run.events[i];
		if (event->state != COMPILE)
			continue;
		for (other = 1; other <= coders; other++)
			if (is_neighbour(event->id, other, coders) && last[other] >= 0)
				assert(event->ts >= last[other] + 100 - 1);
		last[event->id] = event->ts;
	}
}

/* AC4: neighbours' compile intervals never overlap beyond 1 ms rounding. */
static void test_ac4_neighbour_exclusion(void)
{
	run_case("5", "5000", "100", "20", "3", "fifo");
	parse_log(5);
	assert_exclusion(5);
	run_case("5", "5000", "100", "20", "3", "edf");
	parse_log(5);
	assert_exclusion(5);
}

/* AC5: a neighbour cannot take a shared dongle before compile plus cooldown. */
static void test_ac5_shared_dongle_cooldown(void)
{
	long last[5] = {-1, -1, -1, -1, -1};
	int i;
	int other;
	int checked = 0;
	t_event *event;

	run_case("4", "5000", "100", "50", "3", "fifo");
	parse_log(4);
	assert(run.status == 0);
	for (i = 0; i < run.count; i++)
	{
		event = &run.events[i];
		if (event->state == TAKE)
			for (other = 1; other <= 4; other++)
				if (is_neighbour(event->id, other, 4) && last[other] >= 0)
				{
					assert(event->ts >= last[other] + 100 + 50 - 1);
					checked++;
				}
		if (event->state == COMPILE)
			last[event->id] = event->ts;
	}
	assert(checked > 0);
}

static void assert_alternation(void)
{
	int previous = 0;
	int count[3] = {0};
	int i;
	t_event *event;

	for (i = 0; i < run.count; i++)
	{
		event = &run.events[i];
		if (event->state != COMPILE)
			continue;
		assert(event->id != previous);
		previous = event->id;
		count[event->id]++;
	}
	assert(count[1] >= 4 && count[2] >= 4);
}

/* AC6: two coders' compile lines strictly alternate under FIFO and EDF. */
static void test_ac6_two_coder_fairness(void)
{
	run_case("2", "5000", "100", "0", "4", "fifo");
	parse_log(2);
	assert(run.status == 0);
	assert_alternation();
	run_case("2", "5000", "100", "0", "4", "edf");
	parse_log(2);
	assert(run.status == 0);
	assert_alternation();
}

static long get_slack(void)
{
	const char *text = getenv("CODEXION_TIME_SLACK_MS");
	char *end;
	long value;

	if (text == NULL)
		return (0);
	errno = 0;
	value = strtol(text, &end, 10);
	assert(errno == 0 && end != text && *end == '\0');
	assert(value >= 0 && value <= LONG_MAX - 310);
	return (value);
}

/* AC7: burnout is the single final line, logged at 300..310 ms plus slack. */
static void test_ac7_single_final_burnout(void)
{
	int i;
	int count = 0;
	t_event *event;

	run_case("2", "300", "400", "0", "3", "fifo");
	parse_log(2);
	for (i = 0; i < run.count; i++)
	{
		event = &run.events[i];
		if (event->state != BURNOUT)
			continue;
		count++;
		assert(i == run.count - 1);
		assert(event->ts >= 300 && event->ts <= 310 + get_slack());
	}
	assert(count == 1);
}

/* AC8: the EDF run finishes after every coder compiles three times. */
static void test_ac8_target_met_without_burnout(void)
{
	int count[5] = {0};
	int i;

	run_case("4", "5000", "100", "0", "3", "edf");
	parse_log(4);
	assert(run.status == 0);
	for (i = 0; i < run.count; i++)
	{
		assert(run.events[i].state != BURNOUT);
		if (run.events[i].state == COMPILE)
			count[run.events[i].id]++;
	}
	for (i = 1; i <= 4; i++)
		assert(count[i] >= 3);
}

int main(void)
{
	alarm(getenv("CODEXION_UNDER_VALGRIND") || getenv("TSAN_OPTIONS")
		? 300 : 90);
	test_ac1_complete_serialized_logs();
	test_ac2_two_dongles_per_compile();
	test_ac3_state_cycle();
	test_ac4_neighbour_exclusion();
	test_ac5_shared_dongle_cooldown();
	test_ac6_two_coder_fairness();
	test_ac7_single_final_burnout();
	test_ac8_target_met_without_burnout();
	alarm(0);
	return (0);
}
