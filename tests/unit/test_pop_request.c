#include "main.h"

#include <assert.h>

static void	push_pair(t_heap *heap, long key, long ticket)
{
	t_request	req;

	req.key = key;
	req.ticket = ticket;
	req.coder = (int)ticket;
	assert(push_request(heap, &req) == 0);
}

int	main(void)
{
	t_heap		heap;
	t_request	out;
	int			i;
	long		want[4];

	out.key = -1;
	out.ticket = -2;
	out.coder = -3;
	assert(init_heap(&heap, 4) == 0);
	assert(pop_request(&heap, &out) == -1 && out.key == -1);
	assert(pop_request(NULL, &out) == -1 && out.ticket == -2);
	push_pair(&heap, 4, 1);
	push_pair(&heap, 2, 2);
	push_pair(&heap, 3, 3);
	push_pair(&heap, 1, 4);
	assert(pop_request(&heap, NULL) == -1 && heap.size == 4);
	want[0] = 4;
	want[1] = 2;
	want[2] = 3;
	want[3] = 1;
	i = 0;
	while (i < 4)
	{
		assert(pop_request(&heap, &out) == 0);
		assert(out.ticket == want[i] && out.coder == want[i]);
		assert(heap.size == 3 - i);
		i++;
	}
	assert(pop_request(&heap, &out) == -1);
	free_heap(&heap);
	return (0);
}
