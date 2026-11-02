#include "main.h"

#include <assert.h>
#include <string.h>

static void	teardown(t_dongle *dongle)
{
	free_heap(&dongle->heap);
	assert(pthread_cond_destroy(&dongle->cond) == 0);
	assert(pthread_mutex_destroy(&dongle->mutex) == 0);
}

static void	check_success(int fill)
{
	t_dongle	dongle;

	memset(&dongle, fill, sizeof(dongle));
	assert(init_dongle(&dongle) == 0);
	assert(dongle.available == 1);
	assert(dongle.released_at == -1);
	assert(dongle.heap.items != NULL);
	assert(dongle.heap.size == 0);
	assert(dongle.heap.capacity == 2);
	assert(pthread_mutex_trylock(&dongle.mutex) == 0);
	assert(pthread_mutex_unlock(&dongle.mutex) == 0);
	teardown(&dongle);
}

int	main(void)
{
	check_success(0);
	check_success(0x5a);
	check_success(0xff);
	assert(init_dongle(NULL) == -1);
	return (0);
}
