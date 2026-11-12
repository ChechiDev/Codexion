#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct s_capture
{
	int			read_fd;
	int			saved_out;
	pthread_t	reader;
	char		data[4096];
	size_t		length;
} t_capture;

static t_config get_config(int coders, int cooldown)
{
	t_config config;

	config.number_of_coders = coders;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = cooldown;
	config.mode = MODE_FIFO;
	return (config);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

static void *read_output(void *arg)
{
	t_capture *capture;
	ssize_t count;

	capture = arg;
	while (capture->length < sizeof(capture->data) - 1)
	{
		count = read(capture->read_fd, capture->data + capture->length,
			sizeof(capture->data) - 1 - capture->length);
		assert(count >= 0);
		if (count == 0)
			break ;
		capture->length += (size_t)count;
	}
	capture->data[capture->length] = '\0';
	return (NULL);
}

static void start_capture(t_capture *capture)
{
	int fds[2];

	memset(capture, 0, sizeof(*capture));
	assert(pipe(fds) == 0);
	capture->read_fd = fds[0];
	capture->saved_out = dup(STDOUT_FILENO);
	assert(capture->saved_out >= 0);
	assert(dup2(fds[1], STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(fds[1]) == 0);
	assert(pthread_create(&capture->reader, NULL, read_output, capture) == 0);
}

static void stop_capture(t_capture *capture)
{
	assert(dup2(capture->saved_out, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(capture->saved_out) == 0);
	assert(pthread_join(capture->reader, NULL) == 0);
	assert(close(capture->read_fd) == 0);
}

/* AC1: a live deadline stores the elapsed start and preserves other fields. */
static void test_ac1_store_live_deadline(void)
{
	t_config config = get_config(4, 50);
	t_sim sim;
	long before;
	long after;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 500;
	assert(sim.start_ms > 0);
	sim.coders[1].compile_count = 7;
	sim.coders[1].last_compile_start = 100;
	before = get_elapsed_ms(sim.start_ms);
	assert(before >= 0);
	assert(set_compile_start(&sim.coders[1]) == 0);
	after = get_elapsed_ms(sim.start_ms);
	assert(after >= before);
	assert(sim.coders[1].last_compile_start >= before);
	assert(sim.coders[1].last_compile_start <= after);
	assert(sim.coders[1].id == 2);
	assert(sim.coders[1].first_dongle == 1);
	assert(sim.coders[1].second_dongle == 2);
	assert(sim.coders[1].compile_count == 7);
	assert_unlocked(&sim.coders[1].mutex);
	assert(free_sim(&sim) == 0);
}

/* AC2: an expired deadline stays visible with no stop, failure, or output. */
static void test_ac2_keep_expired_deadline(void)
{
	t_config config = get_config(4, 50);
	t_sim sim;
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 1000;
	sim.coders[1].compile_count = 7;
	assert(sim.start_ms > 0);
	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) == STDOUT_FILENO);
	assert(dup2(fileno(err), STDERR_FILENO) == STDERR_FILENO);
	assert(set_compile_start(&sim.coders[1]) == 1);
	assert(dup2(saved_out, STDOUT_FILENO) == STDOUT_FILENO);
	assert(dup2(saved_err, STDERR_FILENO) == STDERR_FILENO);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	assert(fseek(out, 0, SEEK_END) == 0 && ftell(out) == 0);
	assert(fseek(err, 0, SEEK_END) == 0 && ftell(err) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
	assert(sim.coders[1].last_compile_start == 0);
	assert(sim.coders[1].compile_count == 7);
	assert_unlocked(&sim.coders[1].mutex);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(sim.stopped == 0 && sim.failed == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC3: the reached boundary is inclusive; a later deadline can be renewed. */
static void test_ac3_inclusive_boundary(void)
{
	t_config config = get_config(4, 50);
	t_sim sim;
	long before;
	long after;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 1000;
	assert(sim.start_ms > 0);
	sim.coders[1].last_compile_start = 200;
	assert(set_compile_start(&sim.coders[1]) == 1);
	assert(sim.coders[1].last_compile_start == 200);
	sim.coders[1].last_compile_start = 400;
	before = get_elapsed_ms(sim.start_ms);
	assert(before >= 0 && before < 1200);
	assert(set_compile_start(&sim.coders[1]) == 0);
	after = get_elapsed_ms(sim.start_ms);
	assert(after >= before);
	assert(sim.coders[1].last_compile_start >= before);
	assert(sim.coders[1].last_compile_start <= after);
	assert(free_sim(&sim) == 0);
}

/* AC4: the monitor predicate can still read the expired deadline. */
static void test_ac4_monitor_sees_expired_deadline(void)
{
	t_config config = get_config(4, 50);
	t_sim sim;
	int burned;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 1000;
	assert(sim.start_ms > 0);
	assert(set_compile_start(&sim.coders[1]) == 1);
	burned = -1;
	assert(get_burned_out(&sim.coders[1], &burned) == 0);
	assert(burned == 1);
	assert(free_sim(&sim) == 0);
}

/* AC5: run_coder releases both dongles and leaves burnout to the monitor. */
static void test_ac5_caller_releases_without_compiling(void)
{
	t_config config = get_config(2, 0);
	t_sim sim;
	t_capture capture;
	pthread_t thread;
	void *result;
	int burned;
	int first;
	int second;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 1000;
	assert(sim.start_ms > 0);
	start_capture(&capture);
	assert(pthread_create(&thread, NULL, run_coder, &sim.coders[0]) == 0);
	alarm(1);
	assert(pthread_join(thread, &result) == 0);
	alarm(0);
	assert(result == NULL);
	stop_capture(&capture);
	assert(strstr(capture.data, "1 is compiling") == NULL);
	assert(sim.coders[0].compile_count == 0);
	assert(sim.coders[0].last_compile_start == 0);
	first = sim.coders[0].first_dongle;
	second = sim.coders[0].second_dongle;
	assert(sim.dongles[first].available == 1);
	assert(sim.dongles[second].available == 1);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(sim.stopped == 0 && sim.failed == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	burned = -1;
	assert(get_burned_out(&sim.coders[0], &burned) == 0);
	assert(burned == 1);
	assert_unlocked(&sim.coders[0].mutex);
	assert_unlocked(&sim.coders[1].mutex);
	assert_unlocked(&sim.dongles[first].mutex);
	assert_unlocked(&sim.dongles[second].mutex);
	assert_unlocked(&sim.stop_mutex);
	assert_unlocked(&sim.log_mutex);
	assert_unlocked(&sim.ticket_mutex);
	assert(free_sim(&sim) == 0);
}

/* AC6: null input fails, and future simulation time still clamps to zero. */
static void test_ac6_unchanged_paths(void)
{
	t_config config = get_config(4, 50);
	t_sim sim;
	t_coder coder;

	assert(set_compile_start(NULL) == -1);
	memset(&coder, 0, sizeof(coder));
	coder.last_compile_start = 123;
	assert(set_compile_start(&coder) == -1);
	assert(coder.last_compile_start == 123);
	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = LONG_MAX;
	sim.coders[1].last_compile_start = 100;
	assert(set_compile_start(&sim.coders[1]) == 0);
	assert(sim.coders[1].last_compile_start == 0);
	assert(free_sim(&sim) == 0);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC7: deliverable builds, exports set_compile_start, and passes Norm. */
static void test_ac7_build_symbol_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]set_compile_start$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	alarm(30);
	test_ac1_store_live_deadline();
	test_ac2_keep_expired_deadline();
	test_ac3_inclusive_boundary();
	test_ac4_monitor_sees_expired_deadline();
	test_ac5_caller_releases_without_compiling();
	test_ac6_unchanged_paths();
	test_ac7_build_symbol_norm();
	alarm(0);
	return (0);
}
