#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* AC1: a fully initialized coder array is destroyed and freed. */
static void test_ac1_free_full_array(void)
{
	t_coder	*coders;
	t_sim	sim;
	int		i;

	coders = malloc(3 * sizeof(*coders));
	assert(coders != NULL);
	i = 0;
	while (i < 3)
	{
		assert(init_coder(&coders[i], &sim, i, 3) == 0);
		i++;
	}
	assert(free_coders(coders, 3) == 0);
}

/* AC2: only the initialized prefix of sentinel-filled storage is freed. */
static void test_ac2_free_partial_array(void)
{
	t_coder	*coders;
	t_sim	sim;

	coders = malloc(4 * sizeof(*coders));
	assert(coders != NULL);
	memset(coders, 0xa5, 4 * sizeof(*coders));
	assert(init_coder(&coders[0], &sim, 0, 4) == 0);
	assert(init_coder(&coders[1], &sim, 1, 4) == 0);
	assert(free_coders(coders, 2) == 0);
}

/* AC3: count zero frees allocated storage without reading an element. */
static void test_ac3_free_zero_count(void)
{
	t_coder	*coders;

	coders = malloc(sizeof(*coders));
	assert(coders != NULL);
	assert(free_coders(coders, 0) == 0);
}

/* AC4: NULL with zero count is valid; positive count fails safely. */
static void test_ac4_null_array(void)
{
	assert(free_coders(NULL, 0) == 0);
	assert(free_coders(NULL, 3) == -1);
}

/* AC5: negative count leaves the live array available for cleanup. */
static void test_ac5_negative_count(void)
{
	t_coder	*coders;
	t_sim	sim;

	coders = malloc(sizeof(*coders));
	assert(coders != NULL);
	assert(init_coder(&coders[0], &sim, 0, 1) == 0);
	assert(free_coders(coders, -1) == -1);
	assert(free_coders(coders, 1) == 0);
}

int	main(void)
{
	test_ac1_free_full_array();
	test_ac2_free_partial_array();
	test_ac3_free_zero_count();
	test_ac4_null_array();
	test_ac5_negative_count();
	return (0);
}
