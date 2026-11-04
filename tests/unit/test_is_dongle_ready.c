#include "main.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

static t_request	build_test_request(long key, long ticket, int coder)
{
	t_request	req;

	memset(&req, 0, sizeof(req));
	req.key = key;
	req.ticket = ticket;
	req.coder = coder;
	return (req);
}

static void	init_fixture(t_dongle *d, int coder)
{
	t_request	req;

	assert(init_dongle(d) == 0);
	req = build_test_request(10, 1, coder);
	assert(push_request(&d->heap, &req) == 0);
}

static void	destroy_fixture(t_dongle *d)
{
	free_heap(&d->heap);
	assert(pthread_cond_destroy(&d->cond) == 0);
	assert(pthread_mutex_destroy(&d->mutex) == 0);
}

static void	check_head_and_flags(void)
{
	t_dongle	d;

	init_fixture(&d, 3);
	assert(is_dongle_ready(&d, 3, 0, 50) == 1);
	assert(is_dongle_ready(&d, 2, 0, 50) == 0);
	d.available = 0;
	assert(is_dongle_ready(&d, 3, 0, 50) == 0);
	d.available = 1;
	d.heap.size = 0;
	assert(is_dongle_ready(&d, 3, 0, 50) == 0);
	destroy_fixture(&d);
}

static void	check_cooldown(void)
{
	t_dongle	d;

	init_fixture(&d, 3);
	d.released_at = 1000;
	assert(is_dongle_ready(&d, 3, 1049, 50) == 0);
	assert(is_dongle_ready(&d, 3, 1050, 50) == 1);
	assert(is_dongle_ready(&d, 3, 999, 0) == 0);
	assert(is_dongle_ready(&d, 3, 1000, 0) == 1);
	d.released_at = 0;
	assert(is_dongle_ready(&d, 3, LONG_MAX, INT_MAX) == 1);
	d.released_at = LONG_MAX - 10;
	assert(is_dongle_ready(&d, 3, LONG_MAX, INT_MAX) == 0);
	destroy_fixture(&d);
}

static void	check_invalid(void)
{
	t_dongle	d;

	assert(is_dongle_ready(NULL, 3, 0, 0) == 0);
	init_fixture(&d, 3);
	assert(is_dongle_ready(&d, 0, 0, 0) == 0);
	assert(is_dongle_ready(&d, -1, 0, 0) == 0);
	assert(is_dongle_ready(&d, 3, -1, 0) == 0);
	assert(is_dongle_ready(&d, 3, 0, -1) == 0);
	d.released_at = -7;
	assert(is_dongle_ready(&d, 3, 0, 0) == 0);
	destroy_fixture(&d);
}

int	main(void)
{
	check_head_and_flags();
	check_cooldown();
	check_invalid();
	return (0);
}
