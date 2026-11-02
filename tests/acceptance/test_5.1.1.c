#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>

/* AC1: four seats map to adjacent dongles, including the wraparound seat. */
static void test_ac1_four_seat_mapping(void)
{
	const int expected[4][2] = {{0, 1}, {1, 2}, {2, 3}, {0, 3}};
	int first;
	int second;
	int seat;

	for (seat = 0; seat < 4; seat++)
	{
		first = -1;
		second = -1;
		assert(get_dongle_pair(seat, 4, &first, &second) == 0);
		assert(first == expected[seat][0]);
		assert(second == expected[seat][1]);
	}
}

/* AC2: one coder receives the one shared dongle in both outputs. */
static void test_ac2_single_coder_shared_dongle(void)
{
	int first = -1;
	int second = -1;

	assert(get_dongle_pair(0, 1, &first, &second) == 0);
	assert(first == 0);
	assert(second == 0);
}

/* AC3: both two-coder seats share the same ordered pair; all pairs are ordered. */
static void test_ac3_two_coders_and_ordered_pairs(void)
{
	int first;
	int second;
	int seat;
	int n;

	for (n = 2; n <= 4; n += 2)
	{
		for (seat = 0; seat < n; seat++)
		{
			first = -1;
			second = -1;
			assert(get_dongle_pair(seat, n, &first, &second) == 0);
			if (n == 2)
			{
				assert(first == 0);
				assert(second == 1);
			}
			assert(first < second);
		}
	}
}

/* AC4: the first and last INT_MAX seats map without integer overflow. */
static void test_ac4_int_max_boundary_mapping(void)
{
	const int seats[3] = {0, INT_MAX - 2, INT_MAX - 1};
	const int expected[3][2] = {
		{0, 1}, {INT_MAX - 2, INT_MAX - 1}, {0, INT_MAX - 1}
	};
	int first;
	int second;
	int i;

	for (i = 0; i < 3; i++)
	{
		first = -1;
		second = -1;
		assert(get_dongle_pair(seats[i], INT_MAX, &first, &second) == 0);
		assert(first == expected[i][0]);
		assert(second == expected[i][1]);
	}
}

/* AC5: invalid seats, counts, and output pointers preserve all valid outputs. */
static void test_ac5_invalid_input_preserves_outputs(void)
{
	const int invalid_seats[4] = {-1, INT_MIN, 4, 5};
	const int invalid_counts[3] = {0, -1, INT_MIN};
	int first;
	int second;
	int i;

	for (i = 0; i < 4; i++)
	{
		first = -42;
		second = -42;
		assert(get_dongle_pair(invalid_seats[i], 4, &first, &second) == -1);
		assert(first == -42 && second == -42);
	}
	for (i = 0; i < 3; i++)
	{
		first = -42;
		second = -42;
		assert(get_dongle_pair(0, invalid_counts[i], &first, &second) == -1);
		assert(first == -42 && second == -42);
	}
	first = -42;
	second = -42;
	assert(get_dongle_pair(0, 4, NULL, &second) == -1);
	assert(second == -42);
	assert(get_dongle_pair(0, 4, &first, NULL) == -1);
	assert(first == -42);
}

int main(void)
{
	test_ac1_four_seat_mapping();
	test_ac2_single_coder_shared_dongle();
	test_ac3_two_coders_and_ordered_pairs();
	test_ac4_int_max_boundary_mapping();
	test_ac5_invalid_input_preserves_outputs();
	return (0);
}
