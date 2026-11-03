#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* AC1: a fully initialized array with queued requests is freed. */
static void test_ac1_free_full_array(void)
{
	t_dongle *dongles;
	t_request request;
	int i;

	dongles = malloc(3 * sizeof(*dongles));
	assert(dongles != NULL);
	i = 0;
	while (i < 3)
	{
		assert(init_dongle(&dongles[i]) == 0);
		request.key = i;
		request.ticket = i;
		request.coder = i + 1;
		assert(push_request(&dongles[i].heap, &request) == 0);
		i++;
	}
	assert(free_dongles(dongles, 3) == 0);
}

/* AC2: only the initialized prefix of sentinel-filled storage is freed. */
static void test_ac2_free_partial_array(void)
{
	t_dongle *dongles;

	dongles = malloc(4 * sizeof(*dongles));
	assert(dongles != NULL);
	memset(dongles, 0xa5, 4 * sizeof(*dongles));
	assert(init_dongle(&dongles[0]) == 0);
	assert(init_dongle(&dongles[1]) == 0);
	assert(free_dongles(dongles, 2) == 0);
}

/* AC3: count zero frees allocated storage without accessing an element. */
static void test_ac3_free_zero_count(void)
{
	t_dongle *dongles;

	dongles = malloc(sizeof(*dongles));
	assert(dongles != NULL);
	assert(free_dongles(dongles, 0) == 0);
}

/* AC4: NULL with count zero is valid; NULL with a positive count fails. */
static void test_ac4_null_array(void)
{
	assert(free_dongles(NULL, 0) == 0);
	assert(free_dongles(NULL, 3) == -1);
}

/* AC5: a negative count fails without consuming the live array. */
static void test_ac5_negative_count(void)
{
	t_dongle *dongles;

	dongles = malloc(sizeof(*dongles));
	assert(dongles != NULL);
	assert(init_dongle(&dongles[0]) == 0);
	assert(free_dongles(dongles, -1) == -1);
	assert(free_dongles(dongles, 1) == 0);
}

int main(void)
{
	test_ac1_free_full_array();
	test_ac2_free_partial_array();
	test_ac3_free_zero_count();
	test_ac4_null_array();
	test_ac5_negative_count();
	return (0);
}
