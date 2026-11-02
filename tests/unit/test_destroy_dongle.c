#include "main.h"

#include <assert.h>

static void	check_destroy(int queued)
{
	t_dongle	dongle;
	t_request	req;

	req.key = 1;
	req.ticket = 1;
	req.coder = 1;
	assert(init_dongle(&dongle) == 0);
	if (queued)
		assert(push_request(&dongle.heap, &req) == 0);
	dongle.available = 0;
	dongle.released_at = 77;
	assert(destroy_dongle(&dongle) == 0);
	assert(dongle.heap.items == NULL);
	assert(dongle.heap.size == 0);
	assert(dongle.heap.capacity == 0);
	assert(dongle.available == 0);
	assert(dongle.released_at == 77);
}

int	main(void)
{
	check_destroy(0);
	check_destroy(1);
	assert(destroy_dongle(NULL) == -1);
	return (0);
}
