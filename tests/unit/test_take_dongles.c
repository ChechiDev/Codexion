#include "main.h"

#include <assert.h>

static t_config	get_config(int n)
{
	t_config	config;

	config.number_of_coders = n;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 0;
	config.mode = MODE_EDF;
	return (config);
}

static void	check_take(void)
{
	t_config	config;
	t_sim		sim;
	int			taken;

	config = get_config(3);
	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms();
	taken = -1;
	assert(take_dongles(&sim.coders[1], &taken) == 0 && taken == 1);
	assert(sim.dongles[1].available == 0 && sim.dongles[2].available == 0);
	assert(sim.dongles[1].heap.size == 0 && sim.dongles[2].heap.size == 0);
	assert(release_dongles(&sim.coders[1]) == 0);
	assert(set_stopped(&sim) == 0);
	taken = -1;
	assert(take_dongles(&sim.coders[1], &taken) == 0 && taken == 0);
	assert(sim.dongles[1].available == 1);
	assert(free_sim(&sim) == 0);
}

static void	check_invalid(void)
{
	t_coder	coder = {0};
	t_sim	sim = {0};
	int		taken;

	taken = 5;
	assert(take_dongles(NULL, &taken) == -1 && taken == 5);
	assert(take_dongles(&coder, &taken) == -1);
	assert(take_dongles(&coder, NULL) == -1);
	coder.sim = &sim;
	assert(take_dongles(&coder, &taken) == -1 && taken == 5);
}

int	main(void)
{
	check_take();
	check_invalid();
	return (0);
}
