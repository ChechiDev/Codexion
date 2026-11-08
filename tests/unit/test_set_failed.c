/* Unit tests for set_failed: guarded failure flag, no stop, lock failure. */

#include <assert.h>

#include "main.h"

static void	test_marks_without_stopping(void)
{
	t_sim	sim;

	sim.failed = 0;
	sim.stopped = 0;
	assert(pthread_mutex_init(&sim.log_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim.stop_mutex, NULL) == 0);
	assert(set_failed(&sim) == 0);
	assert(set_failed(&sim) == 0);
	assert(sim.failed == 1 && sim.stopped == 0);
	assert(pthread_mutex_trylock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim.stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim.log_mutex) == 0);
}

static void	test_lock_failure(void)
{
	t_sim				sim;
	pthread_mutexattr_t	attr;

	sim.failed = 0;
	assert(pthread_mutexattr_init(&attr) == 0);
	assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	assert(pthread_mutex_init(&sim.stop_mutex, &attr) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(set_failed(&sim) == -1);
	assert(sim.failed == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim.stop_mutex) == 0);
}

int	main(void)
{
	test_marks_without_stopping();
	test_lock_failure();
	return (0);
}
