#include "main.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

static t_config	get_config(int coders, int target)
{
	t_config	config;

	memset(&config, 0, sizeof(config));
	config.number_of_coders = coders;
	config.time_to_burnout = 800;
	config.time_to_compile = 100;
	config.time_to_debug = 100;
	config.time_to_refactor = 100;
	config.number_of_compiles_required = target;
	config.dongle_cooldown = 0;
	config.mode = MODE_FIFO;
	return (config);
}

static void	check(t_sim *sim, int a, int b, int expected)
{
	int	met;

	sim->coders[0].compile_count = a;
	sim->coders[1].compile_count = b;
	met = 7;
	assert(get_target_met(sim, &met) == 0);
	assert(met == expected);
	assert(sim->coders[0].compile_count == a);
	assert(sim->coders[1].compile_count == b);
	assert(pthread_mutex_trylock(&sim->coders[0].mutex) == 0);
	assert(pthread_mutex_unlock(&sim->coders[0].mutex) == 0);
	assert(pthread_mutex_trylock(&sim->coders[1].mutex) == 0);
	assert(pthread_mutex_unlock(&sim->coders[1].mutex) == 0);
}

static void	check_counts(void)
{
	t_config	config;
	t_sim		sim;

	config = get_config(2, 2);
	assert(init_simulation(&sim, &config) == 0);
	check(&sim, 2, 2, 1);
	check(&sim, 1, 2, 0);
	check(&sim, 2, 1, 0);
	check(&sim, INT_MAX, 3, 1);
	assert(free_sim(&sim) == 0);
	config = get_config(2, 0);
	assert(init_simulation(&sim, &config) == 0);
	check(&sim, 0, 0, 1);
	assert(free_sim(&sim) == 0);
}

static void	check_invalid(void)
{
	t_config	config;
	t_sim		sim;
	int			met;

	config = get_config(2, 2);
	assert(init_simulation(&sim, &config) == 0);
	met = 7;
	assert(get_target_met(NULL, &met) == -1 && met == 7);
	assert(get_target_met(&sim, NULL) == -1);
	sim.config.number_of_coders = 0;
	assert(get_target_met(&sim, &met) == -1 && met == 7);
	sim.config.number_of_coders = 2;
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	check_counts();
	check_invalid();
	return (0);
}
