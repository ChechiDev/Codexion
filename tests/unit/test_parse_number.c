/* Unit tests for parse_number: edge cases beyond the acceptance contract. */

#include <assert.h>
#include <limits.h>

#include "main.h"

static void	test_max_boundary_with_padding(void)
{
	int	value;

	value = -1;
	assert(parse_number(" 0002147483647\t", &value) == 0);
	assert(value == INT_MAX);
	value = -1;
	assert(parse_number("0002147483648", &value) == -1 && value == -1);
}

static void	test_ten_times_overflow(void)
{
	int	value;

	value = -1;
	assert(parse_number("21474836470", &value) == -1 && value == -1);
	assert(parse_number("2147483650", &value) == -1 && value == -1);
}

static void	test_embedded_garbage_after_digits(void)
{
	int	value;

	value = -1;
	assert(parse_number("12\xd9\xa1", &value) == -1 && value == -1);
	assert(parse_number("/", &value) == -1 && value == -1);
	assert(parse_number(":", &value) == -1 && value == -1);
}

int	main(void)
{
	test_max_boundary_with_padding();
	test_ten_times_overflow();
	test_embedded_garbage_after_digits();
	return (0);
}
