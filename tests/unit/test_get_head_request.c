#include "main.h"

#include <assert.h>

int	main(void)
{
	t_heap		heap;
	t_request	req;
	t_request	out;

	req.key = 4;
	req.ticket = 7;
	req.coder = 3;
	out.key = -1;
	out.ticket = -2;
	out.coder = -3;
	assert(init_heap(&heap, 2) == 0);
	assert(get_head_request(&heap, &out) == -1 && out.key == -1);
	assert(get_head_request(NULL, &out) == -1 && out.ticket == -2);
	assert(push_request(&heap, &req) == 0);
	assert(get_head_request(&heap, NULL) == -1);
	assert(get_head_request(&heap, &out) == 0);
	assert(out.key == 4 && out.ticket == 7 && out.coder == 3);
	assert(heap.size == 1);
	free_heap(&heap);
	return (0);
}
