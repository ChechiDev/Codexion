#define _GNU_SOURCE

#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
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

static t_sim *armed_sim;
static int armed_writes;

ssize_t write(int fd, const void *buf, size_t count)
{
	if (fd == STDOUT_FILENO && armed_sim != NULL)
	{
		assert(pthread_mutex_lock(&armed_sim->stop_mutex) == 0);
		armed_sim->stopped = 1;
		assert(pthread_mutex_unlock(&armed_sim->stop_mutex) == 0);
		armed_writes++;
		errno = EIO;
		return (-1);
	}
	return (syscall(SYS_write, fd, buf, count));
}

static void init_sim(t_sim *sim, int count)
{
	t_config cfg;

	cfg.number_of_coders = count;
	cfg.time_to_burnout = 800;
	cfg.time_to_compile = 200;
	cfg.time_to_debug = 200;
	cfg.time_to_refactor = 200;
	cfg.number_of_compiles_required = 3;
	cfg.dongle_cooldown = 50;
	cfg.mode = MODE_FIFO;
	assert(init_simulation(sim, &cfg) == 0);
	sim->start_ms = get_time_ms();
	assert(sim->start_ms >= 0);
}

static int call_take(t_coder *coder, int *taken)
{
	int result;

	alarm(15);
	result = take_coder_dongles(coder, taken);
	alarm(0);
	return (result);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

static void assert_dongles(t_sim *sim, int available)
{
	int first;
	int second;
	int i;

	first = sim->coders[0].first_dongle;
	second = sim->coders[0].second_dongle;
	assert(sim->dongles[first].available == available);
	assert(sim->dongles[second].available == available);
	i = 0;
	while (i < sim->config.number_of_coders)
		assert_unlocked(&sim->dongles[i++].mutex);
}

static void assert_released(t_sim *sim)
{
	int first;
	int second;

	first = sim->coders[0].first_dongle;
	second = sim->coders[0].second_dongle;
	assert_dongles(sim, 1);
	assert(sim->dongles[first].released_at >= 0);
	assert(sim->dongles[second].released_at >= 0);
}

static void *read_capture(void *arg)
{
	t_capture *cap;
	ssize_t n;

	cap = arg;
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
	alarm(15);
	assert(dup2(cap->saved, STDOUT_FILENO) >= 0);
	assert(close(cap->saved) == 0);
	assert(pthread_join(cap->reader, NULL) == 0);
	assert(close(cap->read_fd) == 0);
	alarm(0);
}

static void assert_lines(const t_capture *cap, int id, int count)
{
	const char *p;
	char *end;
	long ts;
	int found;
	int line_id;
	int used;

	p = cap->data;
	found = 0;
	while (*p != '\0')
	{
		errno = 0;
		ts = strtol(p, &end, 10);
		assert(errno == 0 && ts >= 0 && end > p);
		assert(sscanf(end, " %d %n", &line_id, &used) == 1);
		assert(line_id == id);
		p = end + used;
		assert(strncmp(p, "has taken a dongle\n", 19) == 0);
		p += 19;
		found++;
	}
	assert(found == count);
}

/* AC1: a running pair is acquired, logged twice, and left held. */
static void test_ac1_running_pair(void)
{
	t_sim sim;
	t_capture cap;
	int taken;
	int first;
	int second;

	init_sim(&sim, 4);
	first = sim.coders[2].first_dongle;
	second = sim.coders[2].second_dongle;
	taken = -9;
	start_capture(&cap);
	assert(call_take(&sim.coders[2], &taken) == 0);
	stop_capture(&cap);
	assert(taken == 1);
	assert_lines(&cap, 3, 2);
	assert(sim.dongles[first].available == 0);
	assert(sim.dongles[second].available == 0);
	assert_unlocked(&sim.dongles[first].mutex);
	assert_unlocked(&sim.dongles[second].mutex);
	assert(release_dongles(&sim.coders[2]) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC2: a preexisting stop reports no acquisition or log line. */
static void test_ac2_stopped_before_take(void)
{
	t_sim sim;
	t_capture cap;
	int taken;

	init_sim(&sim, 4);
	assert(set_stopped(&sim) == 0);
	taken = -9;
	start_capture(&cap);
	assert(call_take(&sim.coders[0], &taken) == 0);
	stop_capture(&cap);
	assert(taken == 0 && cap.len == 0);
	assert_dongles(&sim, 1);
	assert(free_sim(&sim) == 0);
}

static void full_child(int count)
{
	t_sim sim;
	int fd;
	int taken;

	init_sim(&sim, count);
	fd = open("/dev/full", O_WRONLY);
	if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0 || close(fd) != 0)
		_exit(100);
	taken = -9;
	assert(call_take(&sim.coders[0], &taken) == -1);
	assert(taken == -9 && sim.stopped == 0);
	assert_released(&sim);
	assert_unlocked(&sim.log_mutex);
	assert_unlocked(&sim.stop_mutex);
	assert(free_sim(&sim) == 0);
	_exit(0);
}

static void assert_full_child(int count)
{
	pid_t child;
	int status;

	child = fork();
	assert(child >= 0);
	if (child == 0)
		full_child(count);
	alarm(15);
	assert(waitpid(child, &status, 0) == child);
	alarm(0);
	assert(WIFEXITED(status));
	if (WEXITSTATUS(status) == 100)
	{
		fprintf(stderr, "/dev/full child setup failed\n");
		abort();
	}
	assert(WEXITSTATUS(status) == 0);
}

/* AC3: a real print error restores both dongles and preserves taken. */
static void test_ac3_real_print_error(void)
{
	assert_full_child(4);
}

/* AC4: one coder logs once, and a failed print releases its single dongle. */
static void test_ac4_single_coder(void)
{
	t_sim sim;
	t_capture cap;
	int taken;

	init_sim(&sim, 1);
	taken = -9;
	start_capture(&cap);
	assert(call_take(&sim.coders[0], &taken) == 0);
	stop_capture(&cap);
	assert(taken == 1);
	assert_lines(&cap, 1, 1);
	assert_dongles(&sim, 0);
	assert(release_dongles(&sim.coders[0]) == 0);
	assert(free_sim(&sim) == 0);
	assert_full_child(1);
}

/* AC5: a real write error remains an error when stop appears during write. */
static void test_ac5_print_error_with_stop(void)
{
	t_sim sim;
	int taken;
	int result;

	init_sim(&sim, 4);
	taken = -9;
	armed_writes = 0;
	armed_sim = &sim;
	result = call_take(&sim.coders[0], &taken);
	armed_sim = NULL;
	assert(result == -1 && taken == -9);
	assert(armed_writes == 1 && sim.stopped == 1);
	assert_released(&sim);
	assert_unlocked(&sim.log_mutex);
	assert_unlocked(&sim.stop_mutex);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	test_ac1_running_pair();
	test_ac2_stopped_before_take();
	test_ac3_real_print_error();
	test_ac4_single_coder();
	test_ac5_print_error_with_stop();
	return (0);
}
