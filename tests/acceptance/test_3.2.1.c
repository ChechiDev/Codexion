#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

static void init_sim(t_sim *sim)
{
	sim->start_ms = 0;
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
}

static void destroy_sim(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
}

/* AC1: repeated reads reflect both values of the guarded stop flag. */
static void test_ac1_reads_running_and_stopped(void)
{
	t_sim	sim;
	int		i;

	init_sim(&sim);
	i = 0;
	while (i++ < 3)
		assert(is_stopped(&sim) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	sim.stopped = 1;
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	i = 0;
	while (i++ < 3)
		assert(is_stopped(&sim) == 1);
	destroy_sim(&sim);
}

/* AC2: a read preserves the flag and releases stop_mutex. */
static void test_ac2_preserves_flag_and_unlocks(void)
{
	t_sim	sim;
	int		value;

	init_sim(&sim);
	value = 0;
	while (value <= 1)
	{
		sim.stopped = value;
		assert(is_stopped(&sim) == value);
		assert(sim.stopped == value);
		assert(pthread_mutex_trylock(&sim.stop_mutex) == 0);
		assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
		value++;
	}
	destroy_sim(&sim);
}

/* AC3: failed lock reports stopped without changing the flag or ownership. */
static void test_ac3_lock_failure_stops_caller(void)
{
	t_sim				sim;
	pthread_mutexattr_t	attr;

	assert(pthread_mutexattr_init(&attr) == 0);
	assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	sim.start_ms = 0;
	sim.stopped = 0;
	assert(pthread_mutex_init(&sim.stop_mutex, &attr) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
	assert(pthread_mutex_init(&sim.log_mutex, NULL) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(is_stopped(&sim) == 1);
	assert(sim.stopped == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	destroy_sim(&sim);
}

/* AC4: a caller holding log_mutex can still read either stop value. */
static void test_ac4_does_not_take_log_mutex(void)
{
	t_sim				sim;
	pthread_mutexattr_t	attr;
	int				value;

	assert(pthread_mutexattr_init(&attr) == 0);
	assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	sim.start_ms = 0;
	sim.stopped = 0;
	assert(pthread_mutex_init(&sim.stop_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim.log_mutex, &attr) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	value = 0;
	while (value <= 1)
	{
		sim.stopped = value;
		assert(is_stopped(&sim) == value);
		value++;
	}
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	destroy_sim(&sim);
}

static void run_command(const char *command)
{
	int	status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC5: the deliverable builds, exports is_stopped, and passes Norm. */
static void test_ac5_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]is_stopped$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int	main(void)
{
	test_ac1_reads_running_and_stopped();
	test_ac2_preserves_flag_and_unlocks();
	test_ac3_lock_failure_stops_caller();
	test_ac4_does_not_take_log_mutex();
	test_ac5_build_symbol_and_norm();
	return (0);
}
