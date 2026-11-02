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

static void fill_sample(t_request *input)
{
	const long keys[8] = {5, 3, 8, 3, 1, 9, 1, 7};
	const long tickets[8] = {1, 2, 3, 0, 4, 5, 9, 6};
	int i;

	for (i = 0; i < 8; i++)
		input[i] = make_request(keys[i], tickets[i], i + 1);
}

static void fill_large(t_request *input)
{
	uint32_t state;
	int i;

	state = 1;
	for (i = 0; i < 64; i++)
	{
		state = state * UINT32_C(1664525) + UINT32_C(1013904223);
		input[i] = make_request((long)(state % 2001u) - 1000, i, i + 1);
		if (i == 31)
			input[i].key = LONG_MIN;
		if (i == 63)
			input[i].key = LONG_MAX;
	}
}

static void push_all(t_heap *heap, const t_request *input, int count)
{
	int i;

	for (i = 0; i < count; i++)
		assert(push_request(heap, &input[i]) == 0);
}

static void assert_remaining(const t_heap *heap, const t_request *input,
	const int *removed, int count)
{
	int i;
	int j;
	int matches;

	for (i = 0; i < count; i++)
	{
		if (removed[i])
			continue ;
		matches = 0;
		for (j = 0; j < heap->size; j++)
			if (heap->items[j].key == input[i].key
				&& heap->items[j].ticket == input[i].ticket
				&& heap->items[j].coder == input[i].coder)
				matches++;
		assert(matches == 1);
	}
}

static void assert_property_and_permutation(const t_heap *heap,
	const t_request *input, const int *removed, int count)
{
	int i;
	int remaining;

	remaining = 0;
	for (i = 0; i < count; i++)
		if (!removed[i])
			remaining++;
	assert(heap->size == remaining);
	for (i = 1; i < heap->size; i++)
		assert(is_request_before(&heap->items[i],
				&heap->items[(i - 1) / 2]) == 0);
	assert_remaining(heap, input, removed, count);
}

static void mark_removed(const t_request *out, const t_request *input,
	int *removed, int count)
{
	int i;
	int matches;

	matches = 0;
	for (i = 0; i < count; i++)
		if (out->key == input[i].key && out->ticket == input[i].ticket
			&& out->coder == input[i].coder && !removed[i])
		{
			removed[i] = 1;
			matches++;
		}
	assert(matches == 1);
}

/* AC1: popping the only request returns every field and empties the heap. */
static void test_ac1_pop_single_request(void)
{
	t_heap heap = {0};
	t_request request = make_request(9, 1, 2);
	t_request out = {0};

	assert(init_heap(&heap, 4) == 0);
	assert(push_request(&heap, &request) == 0);
	assert(pop_request(&heap, &out) == 0);
	assert_request(&out, &request);
	assert(heap.size == 0);
	free_heap(&heap);
}

/* AC2: eight pops follow key and ticket order, then the heap rejects a pop. */
static void test_ac2_sorted_order(void)
{
	const int order[8] = {4, 6, 3, 1, 0, 7, 2, 5};
	t_request input[8];
	t_request out;
	t_heap heap = {0};
	int i;

	fill_sample(input);
	assert(init_heap(&heap, 8) == 0);
	push_all(&heap, input, 8);
	for (i = 0; i < 8; i++)
	{
		assert(pop_request(&heap, &out) == 0);
		assert_request(&out, &input[order[i]]);
		assert(heap.size == 7 - i);
	}
	assert(pop_request(&heap, &out) == -1);
	free_heap(&heap);
}

/* AC3: each pop preserves the heap property and all queued requests. */
static void test_ac3_heap_property_and_content(void)
{
	t_request small[8];
	t_request large[64];
	t_request out;
	t_heap heap = {0};
	int removed[64] = {0};
	int i;

	fill_sample(small);
	fill_large(large);
	assert(init_heap(&heap, 8) == 0);
	push_all(&heap, small, 8);
	for (i = 0; i < 8; i++)
	{
		assert(pop_request(&heap, &out) == 0);
		mark_removed(&out, small, removed, 8);
		assert_property_and_permutation(&heap, small, removed, 8);
	}
	free_heap(&heap);
	for (i = 0; i < 64; i++)
		removed[i] = 0;
	assert(init_heap(&heap, 64) == 0);
	push_all(&heap, large, 64);
	for (i = 0; i < 64; i++)
	{
		assert(pop_request(&heap, &out) == 0);
		mark_removed(&out, large, removed, 64);
		assert_property_and_permutation(&heap, large, removed, 64);
	}
	free_heap(&heap);
}

static void assert_empty_unchanged(t_heap *heap)
{
	t_heap before;
	t_request slots[4];
	t_request out;
	t_request sentinel;
	int i;

	for (i = heap->size; i < heap->capacity; i++)
		heap->items[i] = make_request(100 + i, 200 + i, 300 + i);
	before = *heap;
	for (i = 0; i < heap->capacity; i++)
		slots[i] = heap->items[i];
	sentinel = make_request(-11, -12, -13);
	out = sentinel;
	assert(pop_request(heap, &out) == -1);
	assert_request(&out, &sentinel);
	assert_heap_fields(heap, &before);
	for (i = 0; i < heap->capacity; i++)
		assert_request(&heap->items[i], &slots[i]);
}

/* AC4: an empty pop preserves output, heap fields and every backing slot. */
static void test_ac4_empty_unchanged(void)
{
	t_heap heap = {0};
	t_request request = make_request(9, 1, 2);
	t_request out;

	assert(init_heap(&heap, 4) == 0);
	assert_empty_unchanged(&heap);
	assert(push_request(&heap, &request) == 0);
	assert(pop_request(&heap, &out) == 0);
	assert_empty_unchanged(&heap);
	free_heap(&heap);
}

/* AC5: 64 extreme and generated requests pop in strict order exactly once. */
static void test_ac5_extremes_and_large_heap(void)
{
	t_request input[64];
	t_request out;
	t_request previous;
	t_heap heap = {0};
	int removed[64] = {0};
	int i;

	fill_large(input);
	assert(init_heap(&heap, 64) == 0);
	push_all(&heap, input, 64);
	for (i = 0; i < 64; i++)
	{
		assert(pop_request(&heap, &out) == 0);
		if (i > 0)
			assert(is_request_before(&previous, &out) == 1);
		mark_removed(&out, input, removed, 64);
		previous = out;
	}
	assert(heap.size == 0);
	for (i = 0; i < 64; i++)
		assert(removed[i] == 1);
	free_heap(&heap);
}

/* AC6: interleaved pushes and pops return tickets 2, 4, 3, 1, 5. */
static void test_ac6_interleaved(void)
{
	const long keys[5] = {4, 2, 3, 1, 5};
	const int order[5] = {1, 3, 2, 0, 4};
	t_request request[5];
	t_request out;
	t_heap heap = {0};
	int i;

	for (i = 0; i < 5; i++)
		request[i] = make_request(keys[i], i + 1, i + 1);
	assert(init_heap(&heap, 4) == 0);
	assert(push_request(&heap, &request[0]) == 0);
	assert(push_request(&heap, &request[1]) == 0);
	assert(pop_request(&heap, &out) == 0);
	assert_request(&out, &request[order[0]]);
	assert(push_request(&heap, &request[2]) == 0);
	assert(push_request(&heap, &request[3]) == 0);
	for (i = 1; i < 3; i++)
	{
		assert(pop_request(&heap, &out) == 0);
		assert_request(&out, &request[order[i]]);
	}
	assert(push_request(&heap, &request[4]) == 0);
	for (i = 3; i < 5; i++)
	{
		assert(pop_request(&heap, &out) == 0);
		assert_request(&out, &request[order[i]]);
	}
	assert(heap.size == 0);
	free_heap(&heap);
}

/* AC7: capacity two breaks ties by ticket; capacity one can be reused. */
static void test_ac7_dongle_sized_heap(void)
{
	t_heap heap = {0};
	t_request first = make_request(7, 1, 1);
	t_request second = make_request(7, 0, 2);
	t_request out;

	assert(init_heap(&heap, 2) == 0);
	assert(push_request(&heap, &first) == 0);
	assert(push_request(&heap, &second) == 0);
	assert(pop_request(&heap, &out) == 0);
	assert_request(&out, &second);
	assert(pop_request(&heap, &out) == 0);
	assert_request(&out, &first);
	free_heap(&heap);
	first = make_request(9, 1, 1);
	second = make_request(3, 2, 2);
	assert(init_heap(&heap, 1) == 0);
	assert(push_request(&heap, &first) == 0);
	assert(pop_request(&heap, &out) == 0);
	assert_request(&out, &first);
	assert(push_request(&heap, &second) == 0);
	assert(pop_request(&heap, &out) == 0);
	assert_request(&out, &second);
	free_heap(&heap);
}

/* AC8: NULL arguments fail without changing output or any heap field or slot. */
static void test_ac8_invalid_arguments(void)
{
	t_heap heap = {0};
	t_heap before;
	t_request slots[4];
	t_request request = make_request(4, 1, 2);
	t_request sentinel = make_request(-1, -2, -3);
	t_request out = sentinel;
	int i;

	assert(pop_request(NULL, &out) == -1);
	assert_request(&out, &sentinel);
	assert(pop_request(NULL, NULL) == -1);
	assert(init_heap(&heap, 4) == 0);
	assert(push_request(&heap, &request) == 0);
	for (i = heap.size; i < heap.capacity; i++)
		heap.items[i] = make_request(100 + i, 200 + i, 300 + i);
	before = heap;
	for (i = 0; i < heap.capacity; i++)
		slots[i] = heap.items[i];
	assert(pop_request(&heap, NULL) == -1);
	assert_heap_fields(&heap, &before);
	for (i = 0; i < heap.capacity; i++)
		assert_request(&heap.items[i], &slots[i]);
	free_heap(&heap);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC9: the deliverable builds, exports pop_request, and passes Norm. */
static void test_ac9_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]pop_request$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_pop_single_request();
	test_ac2_sorted_order();
	test_ac3_heap_property_and_content();
	test_ac4_empty_unchanged();
	test_ac5_extremes_and_large_heap();
	test_ac6_interleaved();
	test_ac7_dongle_sized_heap();
	test_ac8_invalid_arguments();
	test_ac9_build_symbol_and_norm();
	return (0);
}
