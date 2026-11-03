#include "main.h"

#include <assert.h>
#include <string.h>

/* AC1: destroying a freshly initialized coder succeeds. */
static void test_ac1_destroy_initialized_coder(void)
{
	t_sim sim;
	t_coder coder;

	assert(init_coder(&coder, &sim, 0, 4) == 0);
	assert(destroy_coder(&coder) == 0);
}

/* AC2: destroying a used mutex preserves every other coder field. */
static void test_ac2_preserve_coder_fields(void)
{
	t_sim sim;
	t_coder coder;
	unsigned char thread_before[sizeof(coder.thread)];
	int id;
	int first_dongle;
	int second_dongle;
	long last_compile_start;
	int compile_count;
	t_sim *sim_before;

	memset(&coder, 0xa5, sizeof(coder));
	assert(init_coder(&coder, &sim, 0, 4) == 0);
	assert(pthread_mutex_lock(&coder.mutex) == 0);
	assert(pthread_mutex_unlock(&coder.mutex) == 0);
	coder.last_compile_start = 1234;
	coder.compile_count = 3;
	id = coder.id;
	first_dongle = coder.first_dongle;
	second_dongle = coder.second_dongle;
	last_compile_start = coder.last_compile_start;
	compile_count = coder.compile_count;
	sim_before = coder.sim;
	memcpy(thread_before, &coder.thread, sizeof(thread_before));
	assert(destroy_coder(&coder) == 0);
	assert(coder.id == id);
	assert(coder.first_dongle == first_dongle);
	assert(coder.second_dongle == second_dongle);
	assert(coder.last_compile_start == last_compile_start);
	assert(coder.compile_count == compile_count);
	assert(coder.sim == sim_before);
	assert(memcmp(&coder.thread, thread_before, sizeof(thread_before)) == 0);
}

/* AC3: a destroyed coder can be initialized and destroyed again. */
static void test_ac3_reinitialize_after_destroy(void)
{
	t_sim sim;
	t_coder coder;

	assert(init_coder(&coder, &sim, 0, 4) == 0);
	assert(destroy_coder(&coder) == 0);
	assert(init_coder(&coder, &sim, 0, 4) == 0);
	assert(pthread_mutex_trylock(&coder.mutex) == 0);
	assert(pthread_mutex_unlock(&coder.mutex) == 0);
	assert(destroy_coder(&coder) == 0);
}

/* AC4: a null coder is rejected without a crash. */
static void test_ac4_null_coder(void)
{
	assert(destroy_coder(NULL) == -1);
}

int main(void)
{
	test_ac1_destroy_initialized_coder();
	test_ac2_preserve_coder_fields();
	test_ac3_reinitialize_after_destroy();
	test_ac4_null_coder();
	return (0);
}
