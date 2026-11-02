/* Unit tests for get_dongle_pair: mapping, order, wrap and invalid input. */

#include <assert.h>
#include <limits.h>
#include <stddef.h>

#include "main.h"

static void	test_ring_pairs(void)
{
	int	first;
	int	second;
	int	n;
	int	seat;

	n = 1;
	while (n <= 6)
	{
		seat = 0;
		while (seat < n)
		{
			assert(get_dongle_pair(seat, n, &first, &second) == 0);
			assert(first <= second);
			assert((first == second) == (n == 1));
			assert(first == seat || second == seat);
			assert(first == (seat + 1) % n || second == (seat + 1) % n);
			seat++;
		}
		n++;
	}
}

static void	test_int_max(void)
{
	int	first;
	int	second;

	assert(get_dongle_pair(INT_MAX - 1, INT_MAX, &first, &second) == 0);
	assert(first == 0 && second == INT_MAX - 1);
	assert(get_dongle_pair(INT_MAX - 2, INT_MAX, &first, &second) == 0);
	assert(first == INT_MAX - 2 && second == INT_MAX - 1);
}

static void	test_invalid(void)
{
	int	first;
	int	second;

	first = -42;
	second = -42;
	assert(get_dongle_pair(-1, 4, &first, &second) == -1);
	assert(get_dongle_pair(4, 4, &first, &second) == -1);
	assert(get_dongle_pair(0, 0, &first, &second) == -1);
	assert(get_dongle_pair(0, INT_MIN, &first, &second) == -1);
	assert(get_dongle_pair(0, 4, NULL, &second) == -1);
	assert(get_dongle_pair(0, 4, &first, NULL) == -1);
	assert(first == -42 && second == -42);
}

int	main(void)
{
	test_ring_pairs();
	test_int_max();
	test_invalid();
	return (0);
}
