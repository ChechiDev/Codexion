#include "main.h"

#include <assert.h>
#include <limits.h>

static void assert_initial_array(t_dongle *dongles, int n)
{
	int i;

	assert(dongles != NULL);
	i = 0;
	while (i < n)
	{
		assert(dongles[i].available == 1);
		assert(dongles[i].released_at == -1);
		assert(dongles[i].heap.size == 0);
		assert(dongles[i].heap.capacity == 2);
		assert(dongles[i].heap.items != NULL);
		assert(pthread_mutex_trylock(&dongles[i].mutex) == 0);
		assert(pthread_mutex_unlock(&dongles[i].mutex) == 0);
		i++;
	}
}

/* AC1: four dongles have the documented initial state and can be freed. */
static void test_ac1_four_initialized_dongles(void)
{
	t_dongle *dongles = NULL;

	assert(init_dongles(&dongles, 4) == 0);
	assert_initial_array(dongles, 4);
	assert(free_dongles(dongles, 4) == 0);
}

/* AC2: the one-dongle and 200-dongle boundaries initialize and free. */
static void test_ac2_one_and_two_hundred_dongles(void)
{
	t_dongle *dongles = NULL;
	int counts[2] = {1, 200};
	int i;

	i = 0;
	while (i < 2)
	{
		assert(init_dongles(&dongles, counts[i]) == 0);
		assert_initial_array(dongles, counts[i]);
		assert(free_dongles(dongles, counts[i]) == 0);
		dongles = NULL;
		i++;
	}
}

/* AC3: mutexes and request heaps belong to separate array elements. */
static void test_ac3_independent_elements(void)
{
	t_dongle *dongles = NULL;
	t_request request = {1, 1, 3};
	int i;
	int j;

	assert(init_dongles(&dongles, 4) == 0);
	assert(pthread_mutex_lock(&dongles[0].mutex) == 0);
	assert(pthread_mutex_trylock(&dongles[1].mutex) == 0);
	assert(pthread_mutex_unlock(&dongles[1].mutex) == 0);
	assert(pthread_mutex_unlock(&dongles[0].mutex) == 0);
	i = 0;
	while (i < 4)
	{
		j = i + 1;
		while (j < 4)
		{
			assert(dongles[i].heap.items != dongles[j].heap.items);
			j++;
		}
		i++;
	}
	assert(push_request(&dongles[2].heap, &request) == 0);
	assert(dongles[3].heap.size == 0);
	assert(free_dongles(dongles, 4) == 0);
}

/* AC4: invalid arguments fail without replacing the caller's pointer. */
static void test_ac4_invalid_arguments_preserve_output(void)
{
	t_dongle sentinel;
	t_dongle *dongles = &sentinel;

	assert(init_dongles(&dongles, 0) == -1);
	assert(dongles == &sentinel);
	assert(init_dongles(&dongles, -1) == -1);
	assert(dongles == &sentinel);
	assert(init_dongles(&dongles, INT_MIN) == -1);
	assert(dongles == &sentinel);
	assert(init_dongles(NULL, 4) == -1);
}

int main(void)
{
	test_ac1_four_initialized_dongles();
	test_ac2_one_and_two_hundred_dongles();
	test_ac3_independent_elements();
	test_ac4_invalid_arguments_preserve_output();
	return (0);
}
