#include "main.h"

#include <assert.h>
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

static void assert_heap(const t_heap *heap, const t_heap *before,
	const t_request *slots)
{
	int i;

	assert(heap->items == before->items);
	assert(heap->size == before->size);
	assert(heap->capacity == before->capacity);
	for (i = 0; i < heap->capacity; i++)
		assert_request(&heap->items[i], &slots[i]);
}

static void snapshot_slots(const t_heap *heap, t_request *slots)
{
	int i;

	for (i = 0; i < heap->capacity; i++)
		slots[i] = heap->items[i];
}

/* AC1: a single pushed request is copied field by field to out. */
static void test_ac1_single_head(void)
{
	t_heap heap = {0};
	t_request request = make_request(9, 1, 2);
	t_request out = make_request(-1, -1, -1);

	assert(init_heap(&heap, 4) == 0);
	assert(push_request(&heap, &request) == 0);
	assert(get_head_request(&heap, &out) == 0);
	assert_request(&out, &request);
	free_heap(&heap);
}

/* AC2: each insertion prefix returns its specified minimum request. */
static void test_ac2_successive_minimums(void)
{
	const long keys[8] = {5, 3, 8, 3, 1, 9, 1, 7};
	const long tickets[8] = {1, 2, 3, 0, 4, 5, 9, 6};
	const int heads[8] = {0, 1, 1, 3, 4, 4, 4, 4};
	t_request input[8];
	t_request out;
	t_heap heap = {0};
	int i;

	assert(init_heap(&heap, 8) == 0);
	for (i = 0; i < 8; i++)
	{
		input[i] = make_request(keys[i], tickets[i], i + 1);
		assert(push_request(&heap, &input[i]) == 0);
		assert(get_head_request(&heap, &out) == 0);
		assert_request(&out, &input[heads[i]]);
	}
	free_heap(&heap);
}

/* AC3: repeated reads leave heap fields and all capacity slots unchanged. */
static void test_ac3_read_only(void)
{
	t_heap heap = {0};
	t_heap before;
	t_request slots[4];
	t_request request;
	t_request first;
	t_request second;
	int i;

	assert(init_heap(&heap, 4) == 0);
	for (i = 0; i < 4; i++)
		heap.items[i] = make_request(100 + i, 200 + i, 300 + i);
	request = make_request(9, 1, 2);
	assert(push_request(&heap, &request) == 0);
	before = heap;
	snapshot_slots(&heap, slots);
	assert(get_head_request(&heap, &first) == 0);
	assert_heap(&heap, &before, slots);
	assert(get_head_request(&heap, &second) == 0);
	assert_request(&first, &second);
	assert_heap(&heap, &before, slots);
	free_heap(&heap);
}

/* AC4: both empty heap forms fail without changing out or heap storage. */
static void test_ac4_empty_unchanged(void)
{
	t_heap heap = {0};
	t_heap before;
	t_request slots[4];
	t_request request = make_request(9, 1, 2);
	t_request sentinel = make_request(-7, -8, -9);
	t_request out;
	int capacity;
	int i;

	for (capacity = 1; capacity <= 4; capacity += 3)
	{
		assert(init_heap(&heap, capacity) == 0);
		for (i = 0; i < capacity; i++)
			heap.items[i] = make_request(100 + i, 200 + i, 300 + i);
		if (capacity == 1)
		{
			assert(push_request(&heap, &request) == 0);
			heap.size = 0;
		}
		before = heap;
		snapshot_slots(&heap, slots);
		out = sentinel;
		assert(get_head_request(&heap, &out) == -1);
		assert_request(&out, &sentinel);
		assert_heap(&heap, &before, slots);
		free_heap(&heap);
	}
}

/* AC5: null arguments fail and preserve valid outputs and heap storage. */
static void test_ac5_null_arguments(void)
{
	t_heap heap = {0};
	t_heap before;
	t_request slots[4];
	t_request request = make_request(9, 1, 2);
	t_request sentinel = make_request(-7, -8, -9);
	t_request out = sentinel;
	int i;

	assert(get_head_request(NULL, &out) == -1);
	assert_request(&out, &sentinel);
	assert(get_head_request(NULL, NULL) == -1);
	assert(init_heap(&heap, 4) == 0);
	for (i = 0; i < 4; i++)
		heap.items[i] = make_request(100 + i, 200 + i, 300 + i);
	assert(push_request(&heap, &request) == 0);
	before = heap;
	snapshot_slots(&heap, slots);
	assert(get_head_request(&heap, NULL) == -1);
	assert_heap(&heap, &before, slots);
	free_heap(&heap);
}

/* AC6: changing out cannot change the stored head. */
static void test_ac6_copy_by_value(void)
{
	t_heap heap = {0};
	t_request request = make_request(9, 1, 2);
	t_request out;

	assert(init_heap(&heap, 1) == 0);
	assert(push_request(&heap, &request) == 0);
	assert(get_head_request(&heap, &out) == 0);
	out = make_request(-9, -8, -7);
	assert(get_head_request(&heap, &out) == 0);
	assert_request(&out, &request);
	free_heap(&heap);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC7: deliverable builds, exports get_head_request, and passes Norm. */
static void test_ac7_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]get_head_request$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_single_head();
	test_ac2_successive_minimums();
	test_ac3_read_only();
	test_ac4_empty_unchanged();
	test_ac5_null_arguments();
	test_ac6_copy_by_value();
	test_ac7_build_symbol_and_norm();
	return (0);
}
