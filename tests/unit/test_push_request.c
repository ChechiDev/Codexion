#include "main.h"

#include <assert.h>

static void	push_one(t_heap *heap, long key, long ticket)
{
	t_request	req;

	req.key = key;
	req.ticket = ticket;
	req.coder = (int)ticket;
	assert(push_request(heap, &req) == 0);
}

static void	check_order(const t_heap *heap)
{
	int	i;

	i = 1;
	while (i < heap->size)
	{
		assert(is_request_before(&heap->items[i],
				&heap->items[(i - 1) / 2]) == 0);
		i++;
	}
}

int	main(void)
{
	t_heap		heap;
	t_request	req;
	long		keys[6] = {5, 3, 8, 1, 9, 1};
	int			i;

	req.key = 1;
	req.ticket = 1;
	req.coder = 1;
	assert(push_request(NULL, &req) == -1);
	assert(init_heap(&heap, 6) == 0);
	assert(push_request(&heap, NULL) == -1 && heap.size == 0);
	i = 0;
	while (i < 6)
	{
		push_one(&heap, keys[i], i);
		assert(heap.size == i + 1);
		check_order(&heap);
		i++;
	}
	assert(heap.items[0].key == 1 && heap.items[0].ticket == 3);
	assert(push_request(&heap, &req) == -1 && heap.size == 6);
	free_heap(&heap);
	return (0);
}
