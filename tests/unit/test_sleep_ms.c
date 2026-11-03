/* Unit tests for sleep_ms: return contract, duration, stop observed. */

#include <assert.h>
#include <limits.h>

#include "main.h"

static void	setup_sim(t_sim *sim)
{
	sim->start_ms = 0;
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
}

static void	destroy_sim(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
}

static void	test_nonpositive_returns_zero(void)
{
	t_sim	sim;

	setup_sim(&sim);
	assert(sleep_ms(&sim, 0) == 0);
	assert(sleep_ms(&sim, -5) == 0);
	assert(sleep_ms(&sim, LONG_MIN) == 0);
	assert(set_stopped(&sim) == 0);
	assert(sleep_ms(&sim, 0) == 0);
	assert(sleep_ms(&sim, -1) == 0);
	destroy_sim(&sim);
}

static void	test_running_sleep_lasts_duration(void)
{
	t_sim	sim;
	long	before;
	long	after;

	setup_sim(&sim);
	before = get_time_ms();
	assert(sleep_ms(&sim, 30) == 0);
	after = get_time_ms();
	assert(after - before >= 30 && after - before < 200);
	destroy_sim(&sim);
}

static void	test_stopped_returns_minus_one_fast(void)
{
	t_sim	sim;
	long	before;

	setup_sim(&sim);
	assert(set_stopped(&sim) == 0);
	before = get_time_ms();
	assert(sleep_ms(&sim, LONG_MAX) == -1);
	assert(get_time_ms() - before < 100);
	destroy_sim(&sim);
}

int	main(void)
{
	test_nonpositive_returns_zero();
	test_running_sleep_lasts_duration();
	test_stopped_returns_minus_one_fast();
	return (0);
}
