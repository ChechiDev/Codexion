#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>

static t_request make_request(long key, long ticket, int coder)
{
	t_request request;

	request.key = key;
	request.ticket = ticket;
	request.coder = coder;
	return (request);
}

static void assert_request(const t_request *actual, const t_request *expected)
{
	assert(actual->key == expected->key);
	assert(actual->ticket == expected->ticket);
	assert(actual->coder == expected->coder);
}

static void assert_heap_fields(const t_heap *actual, const t_heap *expected)
{
	assert(actual->items == expected->items);
	assert(actual->size == expected->size);
	assert(actual->capacity == expected->capacity);
}

static void assert_permutation(const t_heap *heap, const t_request *input,
	int count)
{
	int i;
	int j;
	int matches;

	assert(heap->size == count);
	for (i = 0; i < count; i++)
	{
		matches = 0;
		for (j = 0; j < count; j++)
			if (heap->items[j].key == input[i].key
				&& heap->items[j].ticket == input[i].ticket
				&& heap->items[j].coder == input[i].coder)
				matches++;
		assert(matches == 1);
	}
}

static void assert_heap_property(const t_heap *heap)
{
	int i;

	for (i = 1; i < heap->size; i++)
		assert(is_request_before(&heap->items[i],
				&heap->items[(i - 1) / 2]) == 0);
}

static void fill_sample(t_request *input)
{
	const long keys[8] = {5, 3, 8, 3, 1, 9, 1, 7};
	const long tickets[8] = {1, 2, 3, 0, 4, 5, 9, 6};
	int i;

	for (i = 0; i < 8; i++)
		input[i] = make_request(keys[i], tickets[i], i + 1);
}

/* AC1: a first push stores all request fields at the root. */
static void test_ac1_first_request(void)
{
	t_heap heap = {0};
	t_request request = make_request(9, 1, 2);

	assert(init_heap(&heap, 4) == 0);
	assert(push_request(&heap, &request) == 0);
	assert(heap.size == 1);
	assert_request(&heap.items[0], &request);
	free_heap(&heap);
}

/* AC2: each prefix has the specified minimum request at its head. */
static void test_ac2_successive_heads(void)
{
	const int heads[8] = {0, 1, 1, 3, 4, 4, 4, 4};
	t_request input[8];
	t_heap heap = {0};
	int i;

	fill_sample(input);
	assert(init_heap(&heap, 8) == 0);
	for (i = 0; i < 8; i++)
	{
		assert(push_request(&heap, &input[i]) == 0);
		assert(heap.size == i + 1);
		assert_request(&heap.items[0], &input[heads[i]]);
		for (int j = 0; j <= i; j++)
			assert(is_request_before(&heap.items[0], &input[j]) == 1
				|| (heap.items[0].key == input[j].key
					&& heap.items[0].ticket == input[j].ticket));
	}
	free_heap(&heap);
}

/* AC3: eight inserted requests form a valid heap and preserve every item. */
static void test_ac3_heap_property_and_content(void)
{
	t_request input[8];
	t_heap heap = {0};
	int i;

	fill_sample(input);
	assert(init_heap(&heap, 8) == 0);
	for (i = 0; i < 8; i++)
		assert(push_request(&heap, &input[i]) == 0);
	assert_heap_property(&heap);
	assert_permutation(&heap, input, 8);
	free_heap(&heap);
}

/* AC4: ticket breaks EDF key ties and orders FIFO keys. */
static void test_ac4_edf_and_fifo_ties(void)
{
	const long edf_tickets[4] = {4, 2, 3, 1};
	const long fifo_tickets[3] = {3, 1, 2};
	t_request request;
	t_heap heap = {0};
	int i;

	assert(init_heap(&heap, 4) == 0);
	for (i = 0; i < 4; i++)
	{
		request = make_request(100, edf_tickets[i], i + 1);
		assert(push_request(&heap, &request) == 0);
	}
	request = make_request(100, 1, 4);
	assert_request(&heap.items[0], &request);
	free_heap(&heap);
	assert(init_heap(&heap, 3) == 0);
	for (i = 0; i < 3; i++)
	{
		request = make_request(fifo_tickets[i], fifo_tickets[i], i + 1);
		assert(push_request(&heap, &request) == 0);
	}
	request = make_request(1, 1, 2);
	assert_request(&heap.items[0], &request);
	free_heap(&heap);
}

/* AC5: a full heap rejects a push and preserves its fields and slots. */
static void test_ac5_full_heap_unchanged(void)
{
	t_heap heap = {0};
	t_heap before;
	t_request slots[2];
	t_request request;
	int capacity;
	int i;

	for (capacity = 1; capacity <= 2; capacity++)
	{
		assert(init_heap(&heap, capacity) == 0);
		for (i = 0; i < capacity; i++)
		{
			request = make_request(20 - i, i, i + 1);
			assert(push_request(&heap, &request) == 0);
		}
		before = heap;
		for (i = 0; i < capacity; i++)
			slots[i] = heap.items[i];
		request = make_request(-1, 99, 9);
		assert(push_request(&heap, &request) == -1);
		assert_heap_fields(&heap, &before);
		for (i = 0; i < capacity; i++)
			assert_request(&heap.items[i], &slots[i]);
		free_heap(&heap);
	}
}

/* AC6: null arguments fail; a null request preserves all heap storage. */
static void test_ac6_null_arguments_unchanged(void)
{
	t_heap heap = {0};
	t_heap before;
	t_request request = make_request(4, 1, 2);
	t_request slots[4];
	int i;

	assert(push_request(NULL, &request) == -1);
	assert(push_request(NULL, NULL) == -1);
	assert(init_heap(&heap, 4) == 0);
	assert(push_request(&heap, &request) == 0);
	for (i = heap.size; i < heap.capacity; i++)
		heap.items[i] = make_request(100 + i, 200 + i, 300 + i);
	before = heap;
	for (i = 0; i < heap.capacity; i++)
		slots[i] = heap.items[i];
	assert(push_request(&heap, NULL) == -1);
	assert_heap_fields(&heap, &before);
	for (i = 0; i < heap.capacity; i++)
		assert_request(&heap.items[i], &slots[i]);
	free_heap(&heap);
}

/* AC7: insertion copies the request and leaves its argument untouched. */
static void test_ac7_copy_by_value(void)
{
	t_heap heap = {0};
	t_request request = make_request(7, 8, 9);
	t_request original = request;

	assert(init_heap(&heap, 2) == 0);
	assert(push_request(&heap, &request) == 0);
	assert_request(&request, &original);
	request = make_request(-9, -8, -7);
	assert_request(&heap.items[0], &original);
	free_heap(&heap);
}

/* AC8: 64 pseudo-random pushes preserve each prefix and its minimum. */
static void test_ac8_extremes_and_large_heap(void)
{
	t_request input[64];
	t_heap heap = {0};
	uint32_t state = 1;
	int minimum;
	int i;

	assert(init_heap(&heap, 64) == 0);
	minimum = 0;
	for (i = 0; i < 64; i++)
	{
		state = state * UINT32_C(1664525) + UINT32_C(1013904223);
		input[i] = make_request((long)(state % 2001u) - 1000, i, i + 1);
		if (i == 31)
			input[i].key = LONG_MIN;
		if (i == 63)
			input[i].key = LONG_MAX;
		if (is_request_before(&input[i], &input[minimum]))
			minimum = i;
		assert(push_request(&heap, &input[i]) == 0);
		assert(heap.size == i + 1);
		assert_permutation(&heap, input, i + 1);
		assert_heap_property(&heap);
		assert_request(&heap.items[0], &input[minimum]);
	}
	free_heap(&heap);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC9: the deliverable builds, exports push_request, and passes Norm. */
static void test_ac9_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]push_request$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_first_request();
	test_ac2_successive_heads();
	test_ac3_heap_property_and_content();
	test_ac4_edf_and_fifo_ties();
	test_ac5_full_heap_unchanged();
	test_ac6_null_arguments_unchanged();
	test_ac7_copy_by_value();
	test_ac8_extremes_and_large_heap();
	test_ac9_build_symbol_and_norm();
	return (0);
}
