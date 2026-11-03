#include "main.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

int is_dongle_ready(const t_dongle *dongle, int coder, long now,
	int cooldown);

static t_request get_request(long key, long ticket, int coder)
{
	t_request request;

	memset(&request, 0, sizeof(request));
	request.key = key;
	request.ticket = ticket;
	request.coder = coder;
	return (request);
}

static void destroy_test_dongle(t_dongle *dongle)
{
	free_heap(&dongle->heap);
	assert(pthread_cond_destroy(&dongle->cond) == 0);
	assert(pthread_mutex_destroy(&dongle->mutex) == 0);
}

/* AC1: a never-released dongle is ready without an initial cooldown. */
static void test_ac1_never_released(void)
{
	t_dongle dongle;
	t_request request = get_request(10, 1, 3);

	assert(init_dongle(&dongle) == 0);
	assert(dongle.available == 1 && dongle.released_at == -1);
	assert(push_request(&dongle.heap, &request) == 0);
	assert(is_dongle_ready(&dongle, 3, 0, 50) == 1);
	destroy_test_dongle(&dongle);
}

/* AC2: only the current heap head can take the dongle. */
static void test_ac2_head_only(void)
{
	t_dongle dongle;
	t_request first = get_request(10, 1, 3);
	t_request second = get_request(20, 2, 1);

	assert(init_dongle(&dongle) == 0);
	assert(push_request(&dongle.heap, &second) == 0);
	assert(push_request(&dongle.heap, &first) == 0);
	assert(is_dongle_ready(&dongle, 3, 0, 0) == 1);
	assert(is_dongle_ready(&dongle, 1, 0, 0) == 0);
	assert(is_dongle_ready(&dongle, 2, 0, 0) == 0);
	dongle.heap.size = 0;
	assert(is_dongle_ready(&dongle, 3, 0, 0) == 0);
	assert(is_dongle_ready(&dongle, 1, 0, 0) == 0);
	destroy_test_dongle(&dongle);
}

/* AC3: an unavailable dongle cannot be taken by its head. */
static void test_ac3_unavailable(void)
{
	t_dongle dongle;
	t_request request = get_request(10, 1, 3);

	assert(init_dongle(&dongle) == 0);
	assert(push_request(&dongle.heap, &request) == 0);
	dongle.available = 0;
	assert(is_dongle_ready(&dongle, 3, 0, 0) == 0);
	destroy_test_dongle(&dongle);
}

/* AC4: cooldown starts at release and expires at its exact boundary. */
static void test_ac4_cooldown_boundary(void)
{
	t_dongle dongle;
	t_request request = get_request(10, 1, 3);

	assert(init_dongle(&dongle) == 0);
	assert(push_request(&dongle.heap, &request) == 0);
	dongle.released_at = 1000;
	assert(is_dongle_ready(&dongle, 3, 1049, 50) == 0);
	assert(is_dongle_ready(&dongle, 3, 1050, 50) == 1);
	assert(is_dongle_ready(&dongle, 3, 5000, 50) == 1);
	assert(is_dongle_ready(&dongle, 3, 1000, 0) == 1);
	assert(is_dongle_ready(&dongle, 3, 999, 0) == 0);
	destroy_test_dongle(&dongle);
}

/* AC5: large elapsed values are compared without signed overflow. */
static void test_ac5_large_times(void)
{
	t_dongle dongle;
	t_request request = get_request(10, 1, 3);

	assert(init_dongle(&dongle) == 0);
	assert(push_request(&dongle.heap, &request) == 0);
	dongle.released_at = 0;
	assert(is_dongle_ready(&dongle, 3, LONG_MAX, INT_MAX) == 1);
	dongle.released_at = LONG_MAX - 10;
	assert(is_dongle_ready(&dongle, 3, LONG_MAX, INT_MAX) == 0);
	destroy_test_dongle(&dongle);
}

static void assert_read_only(t_dongle *dongle, int coder, long now,
	int cooldown, int expected)
{
	t_request before[2];
	t_request *items = dongle->heap.items;
	int available = dongle->available;
	long released_at = dongle->released_at;
	int size = dongle->heap.size;
	int capacity = dongle->heap.capacity;

	assert(size <= 2);
	memcpy(before, items, sizeof(t_request) * size);
	assert(is_dongle_ready(dongle, coder, now, cooldown) == expected);
	assert(dongle->available == available);
	assert(dongle->released_at == released_at);
	assert(dongle->heap.items == items);
	assert(dongle->heap.size == size);
	assert(dongle->heap.capacity == capacity);
	assert(memcmp(before, items, sizeof(t_request) * size) == 0);
}

/* AC6: all readiness outcomes preserve dongle fields and queued bytes. */
static void test_ac6_read_only(void)
{
	t_dongle dongle;
	t_request first = get_request(10, 1, 3);
	t_request second = get_request(20, 2, 1);

	assert(init_dongle(&dongle) == 0);
	assert(push_request(&dongle.heap, &second) == 0);
	assert(push_request(&dongle.heap, &first) == 0);
	assert_read_only(&dongle, 3, 0, 50, 1);
	assert_read_only(&dongle, 1, 0, 50, 0);
	assert_read_only(&dongle, 2, 0, 50, 0);
	dongle.available = 0;
	assert_read_only(&dongle, 3, 0, 50, 0);
	dongle.available = 1;
	dongle.released_at = 1000;
	assert_read_only(&dongle, 3, 1049, 50, 0);
	assert_read_only(&dongle, 3, 1050, 50, 1);
	assert_read_only(&dongle, 3, 5000, 50, 1);
	assert_read_only(&dongle, 3, 1000, 0, 1);
	assert_read_only(&dongle, 3, 999, 0, 0);
	dongle.released_at = 0;
	assert_read_only(&dongle, 3, LONG_MAX, INT_MAX, 1);
	dongle.released_at = LONG_MAX - 10;
	assert_read_only(&dongle, 3, LONG_MAX, INT_MAX, 0);
	assert_read_only(&dongle, 0, 0, 0, 0);
	assert_read_only(&dongle, -1, 0, 0, 0);
	assert_read_only(&dongle, 3, -1, 0, 0);
	assert_read_only(&dongle, 3, 0, -1, 0);
	dongle.released_at = -7;
	assert_read_only(&dongle, 3, 0, 0, 0);
	dongle.heap.size = 0;
	assert_read_only(&dongle, 3, 0, 0, 0);
	assert_read_only(&dongle, 1, 0, 0, 0);
	destroy_test_dongle(&dongle);
}

/* AC7: malformed inputs return false without dereferencing a null dongle. */
static void test_ac7_invalid_input(void)
{
	t_dongle dongle;
	t_request request = get_request(10, 1, 3);

	assert(is_dongle_ready(NULL, 3, 0, 0) == 0);
	assert(init_dongle(&dongle) == 0);
	assert(push_request(&dongle.heap, &request) == 0);
	assert(is_dongle_ready(&dongle, 3, 0, -1) == 0);
	assert(is_dongle_ready(&dongle, 3, -1, 0) == 0);
	dongle.released_at = -7;
	assert(is_dongle_ready(&dongle, 3, 0, 0) == 0);
	dongle.released_at = -1;
	assert(is_dongle_ready(&dongle, 0, 0, 0) == 0);
	assert(is_dongle_ready(&dongle, -1, 0, 0) == 0);
	destroy_test_dongle(&dongle);
}

int main(void)
{
	test_ac1_never_released();
	test_ac2_head_only();
	test_ac3_unavailable();
	test_ac4_cooldown_boundary();
	test_ac5_large_times();
	test_ac6_read_only();
	test_ac7_invalid_input();
	return (0);
}
