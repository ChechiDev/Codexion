#include "main.h"

#include <assert.h>
#include <errno.h>
#include <time.h>

static t_request make_request(long key, long ticket, int coder)
{
	t_request request;

	request.key = key;
	request.ticket = ticket;
	request.coder = coder;
	return (request);
}

static void assert_heap_reset(const t_dongle *dongle)
{
	assert(dongle->heap.items == NULL);
	assert(dongle->heap.size == 0);
	assert(dongle->heap.capacity == 0);
}

/* AC1: destroying a newly initialized dongle resets its owned heap. */
static void test_ac1_destroy_empty_dongle(void)
{
	t_dongle dongle;

	assert(init_dongle(&dongle) == 0);
	assert(destroy_dongle(&dongle) == 0);
	assert_heap_reset(&dongle);
}

/* AC2: queued requests and changed state do not prevent heap teardown. */
static void test_ac2_destroy_queued_dongle(void)
{
	t_request first = make_request(10, 1, 1);
	t_request second = make_request(20, 2, 2);
	t_dongle dongle;

	assert(init_dongle(&dongle) == 0);
	assert(push_request(&dongle.heap, &first) == 0);
	assert(push_request(&dongle.heap, &second) == 0);
	assert(dongle.heap.size == 2);
	dongle.available = 0;
	dongle.released_at = 1234;
	assert(destroy_dongle(&dongle) == 0);
	assert_heap_reset(&dongle);
	assert(dongle.available == 0);
	assert(dongle.released_at == 1234);
}

/* AC3: used mutex and condition variable can both be destroyed. */
static void test_ac3_destroy_used_sync(void)
{
	struct timespec past = {0, 0};
	t_dongle dongle;

	assert(init_dongle(&dongle) == 0);
	assert(pthread_mutex_lock(&dongle.mutex) == 0);
	assert(pthread_mutex_unlock(&dongle.mutex) == 0);
	assert(pthread_cond_broadcast(&dongle.cond) == 0);
	assert(pthread_mutex_lock(&dongle.mutex) == 0);
	assert(pthread_cond_timedwait(&dongle.cond, &dongle.mutex, &past)
		== ETIMEDOUT);
	assert(pthread_mutex_unlock(&dongle.mutex) == 0);
	assert(destroy_dongle(&dongle) == 0);
}

/* AC4: the same storage can be initialized and used after destruction. */
static void test_ac4_reinitialize_after_destroy(void)
{
	t_request first = make_request(10, 1, 1);
	t_request second = make_request(20, 2, 2);
	t_request third = make_request(30, 3, 3);
	t_dongle dongle;

	assert(init_dongle(&dongle) == 0);
	assert(destroy_dongle(&dongle) == 0);
	assert(init_dongle(&dongle) == 0);
	assert(pthread_mutex_trylock(&dongle.mutex) == 0);
	assert(pthread_mutex_unlock(&dongle.mutex) == 0);
	assert(push_request(&dongle.heap, &first) == 0);
	assert(push_request(&dongle.heap, &second) == 0);
	assert(push_request(&dongle.heap, &third) == -1);
	assert(destroy_dongle(&dongle) == 0);
	assert_heap_reset(&dongle);
}

/* AC5: a null dongle is rejected without a crash. */
static void test_ac5_null_dongle(void)
{
	assert(destroy_dongle(NULL) == -1);
}

int main(void)
{
	test_ac1_destroy_empty_dongle();
	test_ac2_destroy_queued_dongle();
	test_ac3_destroy_used_sync();
	test_ac4_reinitialize_after_destroy();
	test_ac5_null_dongle();
	return (0);
}
