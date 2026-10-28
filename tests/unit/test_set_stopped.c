/* Unit tests for set_stopped: guarded stop flag, lock order, failure paths. */

#include <assert.h>

#include "main.h"

static void	test_sets_flag_and_releases(void)
{
	t_sim	sim;

	sim.stopped = 0;
	assert(pthread_mutex_init(&sim.log_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim.stop_mutex, NULL) == 0);
	assert(set_stopped(&sim) == 0);
	assert(set_stopped(&sim) == 0);
	assert(is_stopped(&sim) == 1);
	assert(pthread_mutex_trylock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_destroy(&sim.stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim.log_mutex) == 0);
}

static void	test_stop_lock_failure(void)
{
	t_sim				sim;
	pthread_mutexattr_t	attr;

	sim.stopped = 0;
	assert(pthread_mutexattr_init(&attr) == 0);
	assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	assert(pthread_mutex_init(&sim.log_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim.stop_mutex, &attr) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(set_stopped(&sim) == -1);
	assert(sim.stopped == 0);
	assert(pthread_mutex_trylock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim.stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim.log_mutex) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
}

int	main(void)
{
	test_sets_flag_and_releases();
	test_stop_lock_failure();
	return (0);
}
