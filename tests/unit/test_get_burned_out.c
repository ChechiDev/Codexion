#include "main.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

static t_config	get_config(int burnout)
{
	t_config	config;

	memset(&config, 0, sizeof(config));
	config.number_of_coders = 2;
	config.time_to_burnout = burnout;
	config.time_to_compile = 100;
	config.time_to_debug = 100;
	config.time_to_refactor = 100;
	config.number_of_compiles_required = 1;
	config.dongle_cooldown = 0;
	config.mode = MODE_FIFO;
	return (config);
}

static void	check_one(t_sim *sim, long start, int expected)
{
	int	burned;

	sim->coders[0].last_compile_start = start;
	burned = 7;
	assert(get_burned_out(&sim->coders[0], &burned) == 0);
	assert(burned == expected);
	assert(sim->coders[0].last_compile_start == start);
	assert(pthread_mutex_trylock(&sim->coders[0].mutex) == 0);
	assert(pthread_mutex_unlock(&sim->coders[0].mutex) == 0);
}

static void	check_boundary(void)
{
	t_config	config;
	t_sim		sim;

	config = get_config(10);
	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = LONG_MAX;
	check_one(&sim, 0, 0);
	check_one(&sim, -9, 0);
	check_one(&sim, -10, 1);
	check_one(&sim, -11, 1);
	check_one(&sim, LONG_MIN, 1);
	check_one(&sim, LONG_MAX, 0);
	assert(free_sim(&sim) == 0);
}

static void	check_real_clock(void)
{
	t_config	config;
	t_sim		sim;

	config = get_config(10);
	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 500;
	check_one(&sim, 0, 1);
	sim.start_ms = get_time_ms();
	check_one(&sim, 0, 0);
	assert(free_sim(&sim) == 0);
}

static void	check_invalid(void)
{
	t_config	config;
	t_sim		sim;
	t_sim		*saved;
	int			burned;

	config = get_config(10);
	burned = 7;
	assert(get_burned_out(NULL, &burned) == -1);
	assert(init_simulation(&sim, &config) == 0);
	assert(get_burned_out(&sim.coders[0], NULL) == -1);
	saved = sim.coders[0].sim;
	sim.coders[0].sim = NULL;
	assert(get_burned_out(&sim.coders[0], &burned) == -1);
	sim.coders[0].sim = saved;
	assert(burned == 7);
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	check_boundary();
	check_real_clock();
	check_invalid();
	return (0);
}
