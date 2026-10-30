#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static void write_request(t_request *item, long key, long ticket, int coder)
{
	item->key = key;
	item->ticket = ticket;
	item->coder = coder;
}

static void assert_request(const t_request *item, long key, long ticket,
	int coder)
{
	assert(item->key == key);
	assert(item->ticket == ticket);
	assert(item->coder == coder);
}

/* AC1: capacity two initializes zeroed and non-live sentinel-filled storage. */
static void test_ac1_init_two_slots(void)
{
	t_heap heap;
	int pass;

	for (pass = 0; pass < 2; pass++)
	{
		if (pass == 0)
			memset(&heap, 0, sizeof(heap));
		else
			memset(&heap, 0xa5, sizeof(heap));
		assert(init_heap(&heap, 2) == 0);
		assert(heap.items != NULL);
		assert(heap.size == 0 && heap.capacity == 2);
		write_request(&heap.items[0], 11, 21, 31);
		write_request(&heap.items[1], 12, 22, 32);
		assert_request(&heap.items[0], 11, 21, 31);
		assert_request(&heap.items[1], 12, 22, 32);
		free(heap.items);
	}
}

/* AC2: capacities one and 1000 allocate every requested slot. */
static void test_ac2_boundary_and_large_capacity(void)
{
	const int capacities[] = {1, 1000};
	t_heap heap;
	int choice;
	int i;

	for (choice = 0; choice < 2; choice++)
	{
		memset(&heap, 0, sizeof(heap));
		assert(init_heap(&heap, capacities[choice]) == 0);
		assert(heap.items != NULL);
		assert(heap.size == 0 && heap.capacity == capacities[choice]);
		for (i = 0; i < capacities[choice]; i++)
			write_request(&heap.items[i], i + 1, i + 1001, i + 2001);
		for (i = 0; i < capacities[choice]; i++)
			assert_request(&heap.items[i], i + 1, i + 1001, i + 2001);
		free(heap.items);
	}
}

/* AC3: invalid capacities fail without changing any byte of the heap. */
static void test_ac3_invalid_capacity_preserves_heap(void)
{
	const int capacities[] = {0, -1, INT_MIN};
	unsigned char before[sizeof(t_heap)];
	t_heap heap;
	int i;

	for (i = 0; i < 3; i++)
	{
		memset(&heap, 0xa5, sizeof(heap));
		memcpy(before, &heap, sizeof(heap));
		assert(init_heap(&heap, capacities[i]) == -1);
		assert(memcmp(&heap, before, sizeof(heap)) == 0);
	}
}

/* AC4: a null output pointer fails without crashing. */
static void test_ac4_null_heap(void)
{
	assert(init_heap(NULL, 2) == -1);
}

/* AC5: two live heaps own independent arrays and preserve each other's data. */
static void test_ac5_independent_heaps(void)
{
	t_heap first;
	t_heap second;

	memset(&first, 0, sizeof(first));
	memset(&second, 0, sizeof(second));
	assert(init_heap(&first, 2) == 0);
	assert(init_heap(&second, 2) == 0);
	assert(first.items != NULL && second.items != NULL);
	write_request(&first.items[0], 1, 11, 101);
	write_request(&first.items[1], 2, 12, 102);
	write_request(&second.items[0], 3, 13, 103);
	write_request(&second.items[1], 4, 14, 104);
	assert_request(&first.items[0], 1, 11, 101);
	assert_request(&first.items[1], 2, 12, 102);
	assert_request(&second.items[0], 3, 13, 103);
	assert_request(&second.items[1], 4, 14, 104);
	write_request(&first.items[0], 5, 15, 105);
	assert_request(&second.items[0], 3, 13, 103);
	assert_request(&second.items[1], 4, 14, 104);
	write_request(&second.items[1], 6, 16, 106);
	assert_request(&first.items[0], 5, 15, 105);
	assert_request(&first.items[1], 2, 12, 102);
	free(first.items);
	free(second.items);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC6: the deliverable builds, defines init_heap, and passes Norm. */
static void test_ac6_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]init_heap$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_init_two_slots();
	test_ac2_boundary_and_large_capacity();
	test_ac3_invalid_capacity_preserves_heap();
	test_ac4_null_heap();
	test_ac5_independent_heaps();
	test_ac6_build_symbol_and_norm();
	return (0);
}
