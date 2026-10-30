#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>

static void assert_reset(const t_heap *heap)
{
	assert(heap->items == NULL);
	assert(heap->size == 0);
	assert(heap->capacity == 0);
}

/* AC1: freeing an initialized heap releases its storage and resets its fields. */
static void test_ac1_free_initialized_heap(void)
{
	t_heap heap = {0};

	assert(init_heap(&heap, 4) == 0);
	free_heap(&heap);
	assert_reset(&heap);
}

/* AC2: empty, partly filled, and full heaps all reset after free. */
static void test_ac2_free_at_each_occupancy(void)
{
	const int occupied[] = {0, 2, 4};
	t_heap heap;
	int case_index;
	int i;

	for (case_index = 0; case_index < 3; case_index++)
	{
		heap = (t_heap){0};
		assert(init_heap(&heap, 4) == 0);
		for (i = 0; i < occupied[case_index]; i++)
			heap.items[i] = (t_request){i, i, i + 1};
		heap.size = occupied[case_index];
		free_heap(&heap);
		assert_reset(&heap);
	}
}

/* AC3: repeated calls on the same heap are harmless and preserve reset state. */
static void test_ac3_repeated_free(void)
{
	t_heap heap = {0};

	assert(init_heap(&heap, 4) == 0);
	free_heap(&heap);
	assert_reset(&heap);
	free_heap(&heap);
	assert_reset(&heap);
	free_heap(&heap);
	assert_reset(&heap);
}

/* AC4: a never-initialized zeroed heap and a NULL pointer are accepted. */
static void test_ac4_zeroed_and_null_heap(void)
{
	t_heap heap = {0};

	free_heap(&heap);
	assert_reset(&heap);
	free_heap(NULL);
}

/* AC5: a freed heap can be initialized and used again, then freed. */
static void test_ac5_reinitialize_after_free(void)
{
	t_heap heap = {0};

	assert(init_heap(&heap, 4) == 0);
	free_heap(&heap);
	assert_reset(&heap);
	assert(init_heap(&heap, 2) == 0);
	assert(heap.items != NULL);
	assert(heap.size == 0);
	assert(heap.capacity == 2);
	heap.items[0] = (t_request){11, 21, 1};
	heap.items[1] = (t_request){12, 22, 2};
	assert(heap.items[0].key == 11 && heap.items[0].ticket == 21);
	assert(heap.items[0].coder == 1);
	assert(heap.items[1].key == 12 && heap.items[1].ticket == 22);
	assert(heap.items[1].coder == 2);
	free_heap(&heap);
	assert_reset(&heap);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC6: the deliverable builds, exports free_heap, and passes Norm. */
static void test_ac6_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]free_heap$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_free_initialized_heap();
	test_ac2_free_at_each_occupancy();
	test_ac3_repeated_free();
	test_ac4_zeroed_and_null_heap();
	test_ac5_reinitialize_after_free();
	test_ac6_build_symbol_and_norm();
	return (0);
}
