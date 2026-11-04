#include "main.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

static t_config	get_config(void)
{
	t_config	config;

	memset(&config, 0, sizeof(config));
	config.number_of_coders = 2;
	config.time_to_burnout = 800;
	config.time_to_compile = 100;
	config.time_to_debug = 100;
	config.time_to_refactor = 100;
	config.number_of_compiles_required = 1;
	config.dongle_cooldown = 0;
	config.mode = MODE_FIFO;
	return (config);
}

static void	check_increment(void)
{
	t_config	config;
	t_sim		sim;

	config = get_config();
	assert(set_compile_count(NULL) == -1);
	assert(init_simulation(&sim, &config) == 0);
	assert(set_compile_count(&sim.coders[0]) == 0);
	assert(set_compile_count(&sim.coders[0]) == 0);
	assert(sim.coders[0].compile_count == 2);
	assert(sim.coders[1].compile_count == 0);
	assert(pthread_mutex_trylock(&sim.coders[0].mutex) == 0);
	assert(pthread_mutex_unlock(&sim.coders[0].mutex) == 0);
	assert(free_sim(&sim) == 0);
}

static void	check_bounds(void)
{
	t_config	config;
	t_sim		sim;

	config = get_config();
	assert(init_simulation(&sim, &config) == 0);
	sim.coders[0].compile_count = INT_MAX - 1;
	assert(set_compile_count(&sim.coders[0]) == 0);
	assert(set_compile_count(&sim.coders[0]) == -1);
	assert(sim.coders[0].compile_count == INT_MAX);
	sim.coders[0].compile_count = -1;
	assert(set_compile_count(&sim.coders[0]) == -1);
	assert(sim.coders[0].compile_count == -1);
	assert(pthread_mutex_trylock(&sim.coders[0].mutex) == 0);
	assert(pthread_mutex_unlock(&sim.coders[0].mutex) == 0);
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	check_increment();
	check_bounds();
	return (0);
}
