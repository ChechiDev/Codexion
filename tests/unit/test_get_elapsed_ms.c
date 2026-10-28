/* Unit tests for get_elapsed_ms: non-negative, clamped, tracks the clock. */

#include <assert.h>
#include <limits.h>

#include "main.h"

static void	test_recent_start(void)
{
	long	start;
	long	elapsed;

	start = get_time_ms();
	elapsed = get_elapsed_ms(start);
	assert(elapsed >= 0 && elapsed < 1000);
}

static void	test_past_start(void)
{
	long	start;

	start = get_time_ms() - 250;
	assert(get_elapsed_ms(start) >= 250);
}

static void	test_future_start_clamps(void)
{
	assert(get_elapsed_ms(LONG_MAX) == 0);
	assert(get_elapsed_ms(get_time_ms() + 100000) == 0);
}

int	main(void)
{
	test_recent_start();
	test_past_start();
	test_future_start_clamps();
	return (0);
}
