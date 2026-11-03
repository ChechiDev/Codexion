#include "main.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

static void	setup(t_sim *sim, t_coder *coder, t_mode mode)
{
	memset(sim, 0, sizeof(*sim));
	memset(coder, 0, sizeof(*coder));
	assert(pthread_mutex_init(&sim->ticket_mutex, NULL) == 0);
	assert(pthread_mutex_init(&coder->mutex, NULL) == 0);
	sim->config.mode = mode;
	sim->config.time_to_burnout = 800;
	coder->id = 7;
	coder->sim = sim;
}

static void	destroy_fixture(t_sim *sim, t_coder *coder)
{
	assert(pthread_mutex_destroy(&coder->mutex) == 0);
	assert(pthread_mutex_destroy(&sim->ticket_mutex) == 0);
}

static void	check_unlocked(t_sim *sim, t_coder *coder)
{
	assert(pthread_mutex_trylock(&sim->ticket_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->ticket_mutex) == 0);
	assert(pthread_mutex_trylock(&coder->mutex) == 0);
	assert(pthread_mutex_unlock(&coder->mutex) == 0);
}

int	main(void)
{
	t_sim		sim;
	t_coder		coder;
	t_request	req;
	t_request	before;

	setup(&sim, &coder, MODE_FIFO);
	assert(build_request(&coder, &req) == 0);
	assert(req.ticket == 0 && req.key == 0 && req.coder == 7);
	assert(build_request(&coder, &req) == 0);
	assert(req.ticket == 1 && req.key == 1);
	check_unlocked(&sim, &coder);
	destroy_fixture(&sim, &coder);
	setup(&sim, &coder, MODE_EDF);
	coder.last_compile_start = 1500;
	assert(build_request(&coder, &req) == 0);
	assert(req.key == 2300 && req.ticket == 0 && req.coder == 7);
	coder.last_compile_start = -5;
	memset(&req, 0xa5, sizeof(req));
	memcpy(&before, &req, sizeof(req));
	assert(build_request(&coder, &req) == -1);
	assert(memcmp(&req, &before, sizeof(req)) == 0);
	sim.next_ticket = LONG_MAX;
	assert(build_request(&coder, &req) == -1);
	assert(memcmp(&req, &before, sizeof(req)) == 0);
	check_unlocked(&sim, &coder);
	assert(build_request(NULL, &req) == -1);
	assert(build_request(&coder, NULL) == -1);
	coder.sim = NULL;
	assert(build_request(&coder, &req) == -1);
	assert(memcmp(&req, &before, sizeof(req)) == 0);
	destroy_fixture(&sim, &coder);
	return (0);
}
