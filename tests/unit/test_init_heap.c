#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static void	check_success(int capacity, int fill)
{
	t_heap	heap;
	int		i;

	memset(&heap, fill, sizeof(heap));
	assert(init_heap(&heap, capacity) == 0);
	assert(heap.items != NULL);
	assert(heap.size == 0 && heap.capacity == capacity);
	for (i = 0; i < capacity; i++)
	{
		heap.items[i].key = i;
		heap.items[i].ticket = i + 1;
		heap.items[i].coder = i + 2;
	}
	for (i = 0; i < capacity; i++)
		assert(heap.items[i].key == i && heap.items[i].coder == i + 2);
	free(heap.items);
}

static void	check_failure(int capacity)
{
	unsigned char	before[sizeof(t_heap)];
	t_heap			heap;

	memset(&heap, 0xa5, sizeof(heap));
	memcpy(before, &heap, sizeof(heap));
	assert(init_heap(&heap, capacity) == -1);
	assert(memcmp(&heap, before, sizeof(heap)) == 0);
}

int	main(void)
{
	check_success(1, 0);
	check_success(2, 0xa5);
	check_success(1000, 0);
	check_failure(0);
	check_failure(-1);
	check_failure(INT_MIN);
	assert(init_heap(NULL, 2) == -1);
	return (0);
}
