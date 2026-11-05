#include "main.h"

#include <assert.h>
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

static void	check_null_inputs(void)
{
	t_coder	coder;

	assert(set_compile_start(NULL) == -1);
	memset(&coder, 0, sizeof(coder));
	coder.last_compile_start = 77;
	assert(set_compile_start(&coder) == -1);
	assert(coder.last_compile_start == 77);
}

static void	check_stores_and_unlocks(void)
{
	t_config	config;
	t_sim		sim;

	config = get_config();
	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 300;
	assert(set_compile_start(&sim.coders[0]) == 0);
	assert(sim.coders[0].last_compile_start >= 300);
	assert(sim.coders[1].last_compile_start == 0);
	assert(pthread_mutex_trylock(&sim.coders[0].mutex) == 0);
	assert(pthread_mutex_unlock(&sim.coders[0].mutex) == 0);
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	check_null_inputs();
	check_stores_and_unlocks();
	return (0);
}
