#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#if defined(__has_include)
# if __has_include(<valgrind/valgrind.h>)
#  include <valgrind/valgrind.h>
#  define HAVE_VALGRIND_HEADER 1
# endif
#endif

#if defined(__has_feature)
# if __has_feature(address_sanitizer) || __has_feature(thread_sanitizer)
#  define SKIP_RESOURCE_LIMIT 1
# endif
#endif
#if defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__)
# define SKIP_RESOURCE_LIMIT 1
#endif

typedef struct s_capture
{
	int saved;
	int read_fd;
	pthread_t reader;
	char data[16384];
	size_t len;
} t_capture;

static void set_watchdog(void)
{
	if (getenv("CODEXION_UNDER_VALGRIND") != NULL
		|| getenv("TSAN_OPTIONS") != NULL)
		alarm(120);
	else
		alarm(20);
}

static t_config get_config(int coders, int burnout, t_mode mode)
{
	t_config cfg;

	cfg.number_of_coders = coders;
	cfg.time_to_burnout = burnout;
	cfg.time_to_compile = 200;
	cfg.time_to_debug = 200;
	cfg.time_to_refactor = 200;
	cfg.number_of_compiles_required = 3;
	cfg.dongle_cooldown = 50;
	cfg.mode = mode;
	return (cfg);
}

static int get_slack(void)
{
	const char *value;
	char *end;
	long slack;

	value = getenv("CODEXION_TIME_SLACK_MS");
	if (value == NULL)
		return (0);
	errno = 0;
	slack = strtol(value, &end, 10);
	assert(errno == 0 && end != value && *end == '\0');
	assert(slack >= 0 && slack <= INT_MAX - 310);
	return ((int)slack);
}

static void *drain_output(void *arg)
{
	t_capture *cap;
	char buf[256];
	ssize_t n;

	cap = arg;
	while (1)
	{
		n = read(cap->read_fd, buf, sizeof(buf));
		assert(n >= 0);
		if (n == 0)
			break;
		assert(cap->len + (size_t)n < sizeof(cap->data));
		memcpy(cap->data + cap->len, buf, (size_t)n);
		cap->len += (size_t)n;
	}
	cap->data[cap->len] = '\0';
	return (NULL);
}

static void capture_start(t_capture *cap)
{
	int fds[2];

	memset(cap, 0, sizeof(*cap));
	assert(pipe(fds) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->read_fd = fds[0];
	assert(dup2(fds[1], STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, drain_output, cap) == 0);
}

static void capture_end(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(cap->saved) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->read_fd) == 0);
}

static void assert_normal_log(const t_capture *cap)
{
	char copy[sizeof(cap->data)];
	char *line;
	char *save;
	long ts;
	long previous;
	int id;
	int used;
	int lines;
	int newlines;
	size_t i;
	char prefix[64];

	assert(cap->len > 0 && cap->data[cap->len - 1] == '\n');
	memcpy(copy, cap->data, cap->len + 1);
	newlines = 0;
	for (i = 0; i < cap->len; ++i)
		if (cap->data[i] == '\n')
			newlines++;
	previous = -1;
	lines = 0;
	line = strtok_r(copy, "\n", &save);
	while (line != NULL)
	{
		used = -1;
		assert(sscanf(line, "%ld %d %n", &ts, &id, &used) == 2);
		assert(used > 0 && ts >= 0 && ts >= previous);
		assert(id >= 1 && id <= 4);
		assert(snprintf(prefix, sizeof(prefix), "%ld %d ", ts, id) > 0);
		assert(strlen(prefix) == (size_t)used);
		assert(strncmp(line, prefix, (size_t)used) == 0);
		assert(strcmp(line + used, "has taken a dongle") == 0
			|| strcmp(line + used, "is compiling") == 0
			|| strcmp(line + used, "is debugging") == 0
			|| strcmp(line + used, "is refactoring") == 0);
		previous = ts;
		lines++;
		line = strtok_r(NULL, "\n", &save);
	}
	assert(lines > 0 && lines == newlines);
}

/* AC1: startup sets the clock, and all coders reach the target cleanly. */
static void test_ac1_normal_start_reaches_target(void)
{
	t_config cfg;
	t_sim sim;
	t_capture cap;
	long before;
	long after;
	int i;

	cfg = get_config(4, 5000, MODE_FIFO);
	assert(init_simulation(&sim, &cfg) == 0);
	capture_start(&cap);
	set_watchdog();
	before = get_time_ms();
	assert(before >= 0);
	assert(start_threads(&sim) == 0);
	after = get_time_ms();
	assert(after >= before && sim.start_ms >= before && sim.start_ms <= after);
	assert(join_threads(&sim, 4) == 0);
	capture_end(&cap);
	alarm(0);
	assert(sim.failed == 0 && sim.stopped == 1);
	for (i = 0; i < 4; ++i)
		assert(sim.coders[i].compile_count >= 3);
	assert_normal_log(&cap);
	assert(free_sim(&sim) == 0);
}

static void assert_single_coder_log(const t_capture *cap)
{
	long taken_ts;
	long burned_ts;
	int used;
	int offset;

	used = -1;
	assert(sscanf(cap->data, "%ld 1 has taken a dongle%n",
			&taken_ts, &used) == 1);
	assert(used > 0 && cap->data[used] == '\n');
	assert(taken_ts >= 0);
	offset = used + 1;
	used = -1;
	assert(sscanf(cap->data + offset, "%ld 1 burned out%n",
			&burned_ts, &used) == 1);
	assert(used > 0 && cap->data[offset + used] == '\n');
	assert((size_t)(offset + used + 1) == cap->len);
	assert(burned_ts >= 300 && burned_ts <= 310 + get_slack());
}

/* AC2: a lone coder takes its one dongle and then burns out. */
static void test_ac2_single_coder_burns_out(void)
{
	t_config cfg;
	t_sim sim;
	t_capture cap;

	cfg = get_config(1, 300, MODE_EDF);
	assert(init_simulation(&sim, &cfg) == 0);
	capture_start(&cap);
	set_watchdog();
	assert(start_threads(&sim) == 0);
	assert(join_threads(&sim, 1) == 0);
	capture_end(&cap);
	alarm(0);
	assert(sim.failed == 0);
	assert_single_coder_log(&cap);
	assert(free_sim(&sim) == 0);
}

static int is_resource_test_skipped(void)
{
#if defined(SKIP_RESOURCE_LIMIT)
	return (1);
#else
# if defined(HAVE_VALGRIND_HEADER)
	if (RUNNING_ON_VALGRIND)
		return (1);
# endif
	return (getenv("CODEXION_UNDER_VALGRIND") != NULL);
#endif
}

static rlim_t get_limited_address_space(rlim_t old_soft)
{
	FILE *statm;
	unsigned long long pages;
	unsigned long long bytes;
	long page_size;

	statm = fopen("/proc/self/statm", "r");
	assert(statm != NULL);
	assert(fscanf(statm, "%llu", &pages) == 1);
	assert(fclose(statm) == 0);
	page_size = sysconf(_SC_PAGESIZE);
	assert(page_size > 0);
	assert(pages <= ULLONG_MAX / (unsigned long long)page_size);
	bytes = pages * (unsigned long long)page_size;
	assert(bytes <= ULLONG_MAX - 20ULL * 1024ULL * 1024ULL);
	bytes += 20ULL * 1024ULL * 1024ULL;
	assert((rlim_t)bytes == bytes);
	assert(old_soft == RLIM_INFINITY || (rlim_t)bytes < old_soft);
	return ((rlim_t)bytes);
}

static void child_resource_limit(void)
{
	t_config cfg;
	t_sim sim;
	struct rlimit saved;
	struct rlimit limited;
	int null_fd;
	int result;
	int i;

	set_watchdog();
	cfg = get_config(200, 800, MODE_FIFO);
	assert(init_simulation(&sim, &cfg) == 0);
	null_fd = open("/dev/null", O_WRONLY);
	assert(null_fd >= 0);
	assert(dup2(null_fd, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(null_fd) == 0);
	assert(getrlimit(RLIMIT_AS, &saved) == 0);
	limited = saved;
	limited.rlim_cur = get_limited_address_space(saved.rlim_cur);
	assert(setrlimit(RLIMIT_AS, &limited) == 0);
	result = start_threads(&sim);
	if (result == -1)
	{
		assert(sim.failed == 1);
		for (i = 0; i < 200; ++i)
			assert(sim.dongles[i].available == 1);
		assert(free_sim(&sim) == 0);
		_exit(0);
	}
	assert(result == 0);
	assert(setrlimit(RLIMIT_AS, &saved) == 0);
	assert(set_stopped(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	if (join_threads(&sim, 200) != 0)
		_exit(1);
	assert(free_sim(&sim) == 0);
	assert(fprintf(stderr, "AC3: creation failure not exercised\n") > 0);
	_exit(77);
}

/* AC3: constrained startup either recovers fully or reports no exercise. */
static void test_ac3_creation_failure_recovers(void)
{
	pid_t child;
	int status;

	if (is_resource_test_skipped())
	{
		assert(fprintf(stderr, "AC3: skipped under sanitizer or valgrind\n") > 0);
		return ;
	}
	set_watchdog();
	child = fork();
	assert(child >= 0);
	if (child == 0)
		child_resource_limit();
	assert(waitpid(child, &status, 0) == child);
	alarm(0);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0 || WEXITSTATUS(status) == 77);
}

/* AC4: a NULL simulation is rejected before any thread is created. */
static void test_ac4_null_simulation_rejected(void)
{
	assert(start_threads(NULL) == -1);
}

int main(void)
{
	test_ac1_normal_start_reaches_target();
	test_ac2_single_coder_burns_out();
	test_ac3_creation_failure_recovers();
	test_ac4_null_simulation_rejected();
	return (0);
}
