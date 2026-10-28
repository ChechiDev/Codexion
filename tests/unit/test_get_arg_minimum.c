/* Unit tests for get_arg_minimum: per-position minimums and out-of-range. */

#include <assert.h>

#include "main.h"

static void	test_positive_minimums(void)
{
	assert(get_arg_minimum(1) == 1);
	assert(get_arg_minimum(2) == 1);
}

static void	test_zero_minimums(void)
{
	int	position;

	position = 3;
	while (position <= 7)
	{
		assert(get_arg_minimum(position) == 0);
		position++;
	}
}

static void	test_no_numeric_minimum(void)
{
	assert(get_arg_minimum(8) == -1);
	assert(get_arg_minimum(0) == -1);
	assert(get_arg_minimum(9) == -1);
	assert(get_arg_minimum(-1) == -1);
}

int	main(void)
{
	test_positive_minimums();
	test_zero_minimums();
	test_no_numeric_minimum();
	return (0);
}
