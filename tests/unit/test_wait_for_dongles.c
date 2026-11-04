/* Unit tests for wait_for_dongles: validation and ownership on success. */

#include <assert.h>
#include <errno.h>

#include "main.h"

static t_config	get_config(int count)
{
	t_config	config;

	config.number_of_coders = count;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
	return (config);
}

static void	test_invalid_input(void)
{
	t_coder	coder;

	assert(wait_for_dongles(NULL) == -1);
	coder.sim = NULL;
	coder.first_dongle = 0;
	coder.second_dongle = 1;
	assert(wait_for_dongles(&coder) == -1);
}

static void	test_success_keeps_both_locked(void)
{
	t_config	config;
	t_sim		sim;

	config = get_config(4);
	assert(init_simulation(&sim, &config) == 0);
	assert(pthread_mutex_lock(&sim.dongles[0].mutex) == 0);
	assert(pthread_mutex_lock(&sim.dongles[1].mutex) == 0);
	assert(wait_for_dongles(&sim.coders[0]) == 0);
	assert(pthread_mutex_trylock(&sim.dongles[1].mutex) == EBUSY);
	assert(pthread_mutex_unlock(&sim.dongles[1].mutex) == 0);
	assert(pthread_mutex_unlock(&sim.dongles[0].mutex) == 0);
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	test_invalid_input();
	test_success_keeps_both_locked();
	return (0);
}
