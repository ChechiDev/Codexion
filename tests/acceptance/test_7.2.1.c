#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct s_capture
{
	int saved;
	int read_fd;
	pthread_t reader;
	char data[1024];
	size_t len;
} t_capture;

static t_config get_config(int count)
{
	t_config config;

	config.number_of_coders = count;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
	return (config);
}

static void init_sim(t_sim *sim, int count)
{
	t_config config = get_config(count);

	assert(init_simulation(sim, &config) == 0);
	sim->start_ms = get_time_ms();
	assert(sim->start_ms >= 0);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

static void assert_all_unlocked(t_sim *sim)
{
	int i;

	assert_unlocked(&sim->stop_mutex);
	assert_unlocked(&sim->log_mutex);
	assert_unlocked(&sim->ticket_mutex);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		assert_unlocked(&sim->coders[i].mutex);
		assert_unlocked(&sim->dongles[i].mutex);
		i++;
	}
}

static int call_take(t_coder *coder, int *taken)
{
	int result;

	alarm(5);
	result = take_coder_dongles(coder, taken);
	alarm(0);
	return (result);
}

static void *read_capture(void *arg)
{
	t_capture *cap = arg;
	ssize_t n;

	while (cap->len < sizeof(cap->data) - 1)
	{
		n = read(cap->read_fd, cap->data + cap->len,
			sizeof(cap->data) - 1 - cap->len);
		if (n < 0 && errno == EINTR)
			continue;
		assert(n >= 0);
		if (n == 0)
			break;
		cap->len += (size_t)n;
	}
	assert(cap->len < sizeof(cap->data) - 1);
	cap->data[cap->len] = '\0';
	return (NULL);
}

static void start_capture(t_capture *cap)
{
	int fds[2];

	memset(cap, 0, sizeof(*cap));
	assert(pipe(fds) == 0);
	cap->saved = dup(STDOUT_FILENO);
	assert(cap->saved >= 0);
	cap->read_fd = fds[0];
	assert(dup2(fds[1], STDOUT_FILENO) >= 0);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&cap->reader, NULL, read_capture, cap) == 0);
}

static void stop_capture(t_capture *cap)
{
	assert(dup2(cap->saved, STDOUT_FILENO) >= 0);
	assert(close(cap->saved) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->read_fd) == 0);
}

static void assert_taken_lines(const t_capture *cap, int coder, int count)
{
	const char *p = cap->data;
	char *end;
	long timestamp;
	char id[16];
	int n;

	n = 0;
	while (n < count)
	{
		assert(*p >= '0' && *p <= '9');
		errno = 0;
		timestamp = strtol(p, &end, 10);
		assert(errno == 0 && timestamp >= 0 && end > p);
		assert(*end == ' ');
		assert(snprintf(id, sizeof(id), "%d", coder) > 0);
		p = end + 1;
		assert(strncmp(p, id, strlen(id)) == 0);
		p += strlen(id);
		assert(strlen(p) >= 20);
		assert(strncmp(p, " has taken a dongle\n", 20) == 0);
		p += 20;
		n++;
	}
	assert(*p == '\0');
}

/* AC1: a pair is acquired and logged twice without changing coder state. */
static void test_ac1_pair_acquisition_and_logs(void)
{
	t_sim sim;
	t_capture cap;
	int taken;
	int i;

	init_sim(&sim, 4);
	sim.coders[2].compile_count = 7;
	sim.coders[2].last_compile_start = 12345;
	taken = -9;
	start_capture(&cap);
	assert(call_take(&sim.coders[2], &taken) == 0);
	stop_capture(&cap);
	assert(taken == 1);
	assert_taken_lines(&cap, 3, 2);
	assert(sim.coders[2].compile_count == 7);
	assert(sim.coders[2].last_compile_start == 12345);
	i = 0;
	while (i < 4)
	{
		assert(sim.dongles[i].heap.size == 0);
		assert(sim.dongles[i].available == (i == 2 || i == 3 ? 0 : 1));
		if (i != 2 && i != 3)
			assert(sim.dongles[i].released_at == -1);
		i++;
	}
	assert_all_unlocked(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC2: the sole coder acquires and logs its distinct dongle once. */
static void test_ac2_single_acquisition_and_log(void)
{
	t_sim sim;
	t_capture cap;
	int taken;

	init_sim(&sim, 1);
	sim.coders[0].compile_count = 7;
	sim.coders[0].last_compile_start = 12345;
	taken = -9;
	start_capture(&cap);
	assert(call_take(&sim.coders[0], &taken) == 0);
	stop_capture(&cap);
	assert(taken == 1);
	assert_taken_lines(&cap, 1, 1);
	assert(sim.dongles[0].available == 0);
	assert(sim.dongles[0].heap.size == 0);
	assert(sim.coders[0].compile_count == 7);
	assert(sim.coders[0].last_compile_start == 12345);
	assert_all_unlocked(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC3: a preexisting stop leaves every dongle available and logs nothing. */
static void test_ac3_stopped_before_acquisition(void)
{
	t_sim sim;
	t_capture cap;
	int count;
	int taken;
	int i;

	for (count = 1; count <= 4; count += 3)
	{
		init_sim(&sim, count);
		assert(set_stopped(&sim) == 0);
		taken = -9;
		start_capture(&cap);
		assert(call_take(&sim.coders[0], &taken) == 0);
		stop_capture(&cap);
		assert(taken == 0 && cap.len == 0);
		for (i = 0; i < count; i++)
			assert(sim.dongles[i].available == 1);
		assert_all_unlocked(&sim);
		assert(free_sim(&sim) == 0);
	}
}

static void check_log_failure_child(int count)
{
	t_sim sim;
	int fd;
	int taken;
	int i;

	init_sim(&sim, count);
	fd = open("/dev/full", O_WRONLY);
	assert(fd >= 0);
	assert(dup2(fd, STDOUT_FILENO) >= 0);
	assert(close(fd) == 0);
	taken = -9;
	assert(call_take(&sim.coders[0], &taken) == -1);
	assert(taken == -9 && sim.stopped == 0);
	for (i = 0; i < count; i++)
	{
		assert(sim.dongles[i].available == 1);
		if (i == 0 || (count > 1 && i == 1))
			assert(sim.dongles[i].released_at >= 0);
	}
	assert_all_unlocked(&sim);
	assert(free_sim(&sim) == 0);
	_exit(0);
}

/* AC4: a failed log returns an error after giving back each held dongle. */
static void test_ac4_log_failure_releases_dongles(void)
{
	pid_t child;
	int status;
	int count;

	for (count = 1; count <= 4; count += 3)
	{
		child = fork();
		assert(child >= 0);
		if (child == 0)
			check_log_failure_child(count);
		alarm(5);
		assert(waitpid(child, &status, 0) == child);
		alarm(0);
		assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	}
}

/* AC5: invalid pointers do not log, take resources, or overwrite the out value. */
static void test_ac5_invalid_input(void)
{
	t_sim sim;
	t_capture cap;
	t_coder invalid;
	int taken;
	int i;

	init_sim(&sim, 4);
	memset(&invalid, 0, sizeof(invalid));
	invalid.sim = NULL;
	taken = -9;
	start_capture(&cap);
	assert(call_take(NULL, &taken) == -1);
	assert(taken == -9);
	assert(call_take(&sim.coders[0], NULL) == -1);
	assert(call_take(&invalid, &taken) == -1);
	stop_capture(&cap);
	assert(taken == -9 && cap.len == 0);
	for (i = 0; i < 4; i++)
		assert(sim.dongles[i].available == 1);
	assert_all_unlocked(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC6: ticket exhaustion fails both acquisition paths without side effects. */
static void test_ac6_ticket_exhaustion(void)
{
	t_sim sim;
	t_capture cap;
	int count;
	int taken;
	int i;

	for (count = 1; count <= 4; count += 3)
	{
		init_sim(&sim, count);
		sim.next_ticket = LONG_MAX;
		taken = -9;
		start_capture(&cap);
		assert(call_take(&sim.coders[0], &taken) == -1);
		stop_capture(&cap);
		assert(taken == -9 && cap.len == 0);
		for (i = 0; i < count; i++)
		{
			assert(sim.dongles[i].available == 1);
			assert(sim.dongles[i].heap.size == 0);
		}
		assert_all_unlocked(&sim);
		assert(free_sim(&sim) == 0);
	}
}

int main(void)
{
	test_ac1_pair_acquisition_and_logs();
	test_ac2_single_acquisition_and_log();
	test_ac3_stopped_before_acquisition();
	test_ac4_log_failure_releases_dongles();
	test_ac5_invalid_input();
	test_ac6_ticket_exhaustion();
	return (0);
}
