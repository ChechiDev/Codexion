#include "main.h"

#include <assert.h>

static void	check_reset(const t_heap *heap)
{
	assert(heap->items == NULL);
	assert(heap->size == 0 && heap->capacity == 0);
}

int	main(void)
{
	t_heap	heap = {0};

	free_heap(NULL);
	free_heap(&heap);
	check_reset(&heap);
	assert(init_heap(&heap, 3) == 0);
	heap.items[0].key = 1;
	heap.size = 1;
	free_heap(&heap);
	check_reset(&heap);
	free_heap(&heap);
	check_reset(&heap);
	assert(init_heap(&heap, 2) == 0);
	free_heap(&heap);
	check_reset(&heap);
	return (0);
}
