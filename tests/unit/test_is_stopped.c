/* Unit tests for is_stopped: reads the guarded flag, fails safe. */

#include <assert.h>

#include "main.h"

static void	test_reads_flag(void)
{
	t_sim	sim;

	sim.stopped = 0;
	assert(pthread_mutex_init(&sim.stop_mutex, NULL) == 0);
	assert(is_stopped(&sim) == 0);
	sim.stopped = 1;
	assert(is_stopped(&sim) == 1);
	assert(pthread_mutex_destroy(&sim.stop_mutex) == 0);
}

int	main(void)
{
	test_reads_flag();
	return (0);
}
