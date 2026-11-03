#include "main.h"

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <time.h>

static void destroy_test_dongle(t_dongle *dongle)
{
	free_heap(&dongle->heap);
	assert(pthread_cond_destroy(&dongle->cond) == 0);
	assert(pthread_mutex_destroy(&dongle->mutex) == 0);
}

static t_request make_request(long key, long ticket, int coder)
{
	t_request request;

	request.key = key;
	request.ticket = ticket;
	request.coder = coder;
	return (request);
}

/* AC1: zeroed and non-live sentinel storage gets the documented initial state. */
static void test_ac1_initial_state(void)
{
	t_dongle dongle;
	int pass;

	for (pass = 0; pass < 2; pass++)
	{
		memset(&dongle, pass == 0 ? 0 : 0xa5, sizeof(dongle));
		assert(init_dongle(&dongle) == 0);
		assert(dongle.available == 1);
		assert(dongle.released_at == -1);
		assert(dongle.heap.items != NULL);
		assert(dongle.heap.size == 0);
		assert(dongle.heap.capacity == 2);
		destroy_test_dongle(&dongle);
	}
}

/* AC2: the new mutex can be locked immediately and then unlocked. */
static void test_ac2_mutex_unlocked(void)
{
	t_dongle dongle;

	assert(init_dongle(&dongle) == 0);
	assert(pthread_mutex_trylock(&dongle.mutex) == 0);
	assert(pthread_mutex_unlock(&dongle.mutex) == 0);
	destroy_test_dongle(&dongle);
}

/* AC3: a past-deadline condition wait times out and restores mutex ownership. */
static void test_ac3_condition_wait(void)
{
	struct timespec past = {0, 0};
	t_dongle dongle;

	assert(init_dongle(&dongle) == 0);
	assert(pthread_mutex_lock(&dongle.mutex) == 0);
	assert(pthread_cond_timedwait(&dongle.cond, &dongle.mutex, &past)
		== ETIMEDOUT);
	assert(pthread_mutex_unlock(&dongle.mutex) == 0);
	assert(pthread_cond_broadcast(&dongle.cond) == 0);
	destroy_test_dongle(&dongle);
}

/* AC4: two requests fit, a third does not, and the ordered head is returned. */
static void test_ac4_two_neighbour_heap(void)
{
	t_request later = make_request(20, 2, 2);
	t_request earlier = make_request(10, 1, 1);
	t_request third = make_request(5, 3, 3);
	t_request head;
	t_dongle dongle;

	assert(init_dongle(&dongle) == 0);
	assert(is_request_before(&earlier, &later) == 1);
	assert(push_request(&dongle.heap, &later) == 0);
	assert(push_request(&dongle.heap, &earlier) == 0);
	assert(push_request(&dongle.heap, &third) == -1);
	assert(dongle.heap.size == 2);
	assert(get_head_request(&dongle.heap, &head) == 0);
	assert(head.key == earlier.key);
	assert(head.ticket == earlier.ticket);
	assert(head.coder == earlier.coder);
	destroy_test_dongle(&dongle);
}

/* AC5: separately initialized dongles have separate mutexes and heap arrays. */
static void test_ac5_independent_dongles(void)
{
	t_request request = make_request(1, 1, 1);
	t_dongle first;
	t_dongle second;

	assert(init_dongle(&first) == 0);
	assert(init_dongle(&second) == 0);
	assert(pthread_mutex_lock(&first.mutex) == 0);
	assert(pthread_mutex_trylock(&second.mutex) == 0);
	assert(pthread_mutex_unlock(&second.mutex) == 0);
	assert(pthread_mutex_unlock(&first.mutex) == 0);
	assert(first.heap.items != second.heap.items);
	assert(push_request(&first.heap, &request) == 0);
	assert(second.heap.size == 0);
	destroy_test_dongle(&second);
	destroy_test_dongle(&first);
}

/* AC6: a null dongle pointer is rejected without dereferencing it. */
static void test_ac6_null_dongle(void)
{
	assert(init_dongle(NULL) == -1);
}

int main(void)
{
	test_ac1_initial_state();
	test_ac2_mutex_unlocked();
	test_ac3_condition_wait();
	test_ac4_two_neighbour_heap();
	test_ac5_independent_dongles();
	test_ac6_null_dongle();
	return (0);
}
