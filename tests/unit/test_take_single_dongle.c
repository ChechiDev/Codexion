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
	config.mode = MODE_FIFO;
	return (config);
}

static void	check_take_and_release(void)
{
	t_config	config;
	t_sim		sim;
	int			taken;

	config = get_config(1);
	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms();
	taken = -1;
	assert(take_single_dongle(&sim.coders[0], &taken) == 0 && taken == 1);
	assert(sim.dongles[0].available == 0 && sim.dongles[0].heap.size == 0);
	assert(release_dongles(&sim.coders[0]) == 0);
	taken = -1;
	assert(take_single_dongle(&sim.coders[0], &taken) == 0 && taken == 1);
	assert(set_stopped(&sim) == 0);
	assert(release_dongles(&sim.coders[0]) == 0);
	taken = -1;
	assert(take_single_dongle(&sim.coders[0], &taken) == 0 && taken == 0);
	assert(free_sim(&sim) == 0);
}

static void	check_invalid(void)
{
	t_config	config;
	t_sim		sim;
	int			taken;

	taken = -1;
	assert(take_single_dongle(NULL, &taken) == -1 && taken == -1);
	config = get_config(3);
	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms();
	assert(take_single_dongle(&sim.coders[0], NULL) == -1);
	assert(take_single_dongle(&sim.coders[0], &taken) == -1 && taken == -1);
	assert(sim.next_ticket == 0);
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	check_take_and_release();
	check_invalid();
	return (0);
}
