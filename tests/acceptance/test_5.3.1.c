#include "main.h"

#include <assert.h>
#include <string.h>

/* AC1: four seats get their ordered dongles, initial state and unlocked mutex. */
static void test_ac1_four_initialized_coders(void)
{
	const int expected[4][2] = {{0, 1}, {1, 2}, {2, 3}, {0, 3}};
	t_sim sim;
	t_coder coder;
	int first;
	int second;
	int seat;

	for (seat = 0; seat < 4; seat++)
	{
		assert(init_coder(&coder, &sim, seat, 4) == 0);
		assert(get_dongle_pair(seat, 4, &first, &second) == 0);
		assert(coder.id == seat + 1);
		assert(coder.first_dongle == expected[seat][0]);
		assert(coder.second_dongle == expected[seat][1]);
		assert(coder.first_dongle == first && coder.second_dongle == second);
		assert(coder.last_compile_start == 0);
		assert(coder.compile_count == 0);
		assert(coder.sim == &sim);
		assert(pthread_mutex_trylock(&coder.mutex) == 0);
		assert(pthread_mutex_unlock(&coder.mutex) == 0);
		assert(pthread_mutex_destroy(&coder.mutex) == 0);
	}
}

/* AC2: sentinel-filled storage is initialized while thread bytes stay intact. */
static void test_ac2_sentinel_storage_and_thread_untouched(void)
{
	t_sim sim;
	t_coder coder;
	unsigned char thread_before[sizeof(coder.thread)];

	memset(&coder, 0xa5, sizeof(coder));
	memcpy(thread_before, &coder.thread, sizeof(thread_before));
	assert(init_coder(&coder, &sim, 3, 4) == 0);
	assert(coder.id == 4);
	assert(coder.first_dongle == 0);
	assert(coder.second_dongle == 3);
	assert(coder.last_compile_start == 0);
	assert(coder.compile_count == 0);
	assert(coder.sim == &sim);
	assert(pthread_mutex_trylock(&coder.mutex) == 0);
	assert(pthread_mutex_unlock(&coder.mutex) == 0);
	assert(memcmp(&coder.thread, thread_before, sizeof(thread_before)) == 0);
	assert(pthread_mutex_destroy(&coder.mutex) == 0);
}

/* AC3: one coder uses the same sole dongle as both endpoints. */
static void test_ac3_single_coder_pair(void)
{
	t_sim sim;
	t_coder coder;

	assert(init_coder(&coder, &sim, 0, 1) == 0);
	assert(coder.id == 1);
	assert(coder.first_dongle == 0);
	assert(coder.second_dongle == 0);
	assert(pthread_mutex_destroy(&coder.mutex) == 0);
}

/* AC4: invalid inputs return -1 without changing caller-owned coder bytes. */
static void test_ac4_invalid_input_preserves_coder(void)
{
	const int seats[3] = {-1, 4, 5};
	const int counts[2] = {0, -1};
	t_sim sim;
	t_coder coder;
	unsigned char before[sizeof(coder)];
	int i;

	memset(&coder, 0xa5, sizeof(coder));
	memcpy(before, &coder, sizeof(coder));
	assert(init_coder(&coder, NULL, 0, 4) == -1);
	assert(memcmp(&coder, before, sizeof(coder)) == 0);
	for (i = 0; i < 3; i++)
	{
		assert(init_coder(&coder, &sim, seats[i], 4) == -1);
		assert(memcmp(&coder, before, sizeof(coder)) == 0);
	}
	for (i = 0; i < 2; i++)
	{
		assert(init_coder(&coder, &sim, 0, counts[i]) == -1);
		assert(memcmp(&coder, before, sizeof(coder)) == 0);
	}
	assert(init_coder(NULL, &sim, 0, 4) == -1);
}

/* AC5: each initialized coder owns an independent mutex. */
static void test_ac5_independent_mutexes(void)
{
	t_sim sim;
	t_coder first;
	t_coder second;

	assert(init_coder(&first, &sim, 0, 4) == 0);
	assert(init_coder(&second, &sim, 1, 4) == 0);
	assert(pthread_mutex_lock(&first.mutex) == 0);
	assert(pthread_mutex_trylock(&second.mutex) == 0);
	assert(pthread_mutex_unlock(&second.mutex) == 0);
	assert(pthread_mutex_unlock(&first.mutex) == 0);
	assert(pthread_mutex_destroy(&second.mutex) == 0);
	assert(pthread_mutex_destroy(&first.mutex) == 0);
}

int main(void)
{
	test_ac1_four_initialized_coders();
	test_ac2_sentinel_storage_and_thread_untouched();
	test_ac3_single_coder_pair();
	test_ac4_invalid_input_preserves_coder();
	test_ac5_independent_mutexes();
	return (0);
}
