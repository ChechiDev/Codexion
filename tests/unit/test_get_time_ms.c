/* Unit tests for get_time_ms: plausible epoch value and monotonic reads. */

#include <assert.h>

#include "main.h"

static void	test_epoch_scale(void)
{
	long	now;

	now = get_time_ms();
	assert(now > 1700000000000L);
}

static void	test_non_decreasing(void)
{
	long	first;
	long	second;

	first = get_time_ms();
	second = get_time_ms();
	assert(second >= first);
}

int	main(void)
{
	test_epoch_scale();
	test_non_decreasing();
	return (0);
}
