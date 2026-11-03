#include "main.h"

#include <assert.h>
#include <limits.h>

/* AC1: four seats have the specified initial state and can be freed. */
static void test_ac1_four_initialized_coders(void)
{
	const int expected[4][2] = {{0, 1}, {1, 2}, {2, 3}, {0, 3}};
	t_coder *coders = NULL;
	t_sim sim;
	int i;

	assert(init_coders(&coders, &sim, 4) == 0);
	assert(coders != NULL);
	i = 0;
	while (i < 4)
	{
		assert(coders[i].id == i + 1);
		assert(coders[i].first_dongle == expected[i][0]);
		assert(coders[i].second_dongle == expected[i][1]);
		assert(coders[i].last_compile_start == 0);
		assert(coders[i].compile_count == 0);
		assert(coders[i].sim == &sim);
		assert(pthread_mutex_trylock(&coders[i].mutex) == 0);
		assert(pthread_mutex_unlock(&coders[i].mutex) == 0);
		i++;
	}
	assert(free_coders(coders, 4) == 0);
}

/* AC2: the one-coder and 200-coder boundaries use valid seat pairs. */
static void test_ac2_one_and_two_hundred_coders(void)
{
	t_coder *coders = NULL;
	t_sim sim;
	int i;

	assert(init_coders(&coders, &sim, 1) == 0);
	assert(coders != NULL);
	assert(coders[0].id == 1);
	assert(coders[0].first_dongle == 0);
	assert(coders[0].second_dongle == 0);
	assert(free_coders(coders, 1) == 0);
	coders = NULL;
	assert(init_coders(&coders, &sim, 200) == 0);
	assert(coders != NULL);
	i = 0;
	while (i < 200)
	{
		assert(coders[i].id == i + 1);
		assert(coders[i].first_dongle < coders[i].second_dongle);
		i++;
	}
	assert(coders[199].first_dongle == 0);
	assert(coders[199].second_dongle == 199);
	assert(free_coders(coders, 200) == 0);
}

/* AC3: locking one coder does not lock another coder in the array. */
static void test_ac3_independent_mutexes(void)
{
	t_coder *coders = NULL;
	t_sim sim;

	assert(init_coders(&coders, &sim, 2) == 0);
	assert(coders != NULL);
	assert(pthread_mutex_lock(&coders[0].mutex) == 0);
	assert(pthread_mutex_trylock(&coders[1].mutex) == 0);
	assert(pthread_mutex_unlock(&coders[1].mutex) == 0);
	assert(pthread_mutex_unlock(&coders[0].mutex) == 0);
	assert(free_coders(coders, 2) == 0);
}

/* AC4: invalid arguments fail without replacing the caller's pointer. */
static void test_ac4_invalid_arguments_preserve_output(void)
{
	t_coder sentinel;
	t_coder *coders = &sentinel;
	t_sim sim;

	assert(init_coders(&coders, &sim, 0) == -1);
	assert(coders == &sentinel);
	assert(init_coders(&coders, &sim, -1) == -1);
	assert(coders == &sentinel);
	assert(init_coders(&coders, &sim, INT_MIN) == -1);
	assert(coders == &sentinel);
	assert(init_coders(&coders, NULL, 4) == -1);
	assert(coders == &sentinel);
	assert(init_coders(NULL, &sim, 4) == -1);
}

int main(void)
{
	test_ac1_four_initialized_coders();
	test_ac2_one_and_two_hundred_coders();
	test_ac3_independent_mutexes();
	test_ac4_invalid_arguments_preserve_output();
	return (0);
}
