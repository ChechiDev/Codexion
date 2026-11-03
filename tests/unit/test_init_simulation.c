#include "main.h"

#include <assert.h>
#include <string.h>

static t_config	get_config(int n)
{
	t_config	config;

	memset(&config, 0, sizeof(config));
	config.number_of_coders = n;
	config.time_to_burnout = 800;
	config.mode = MODE_EDF;
	return (config);
}

int	main(void)
{
	t_sim		sim;
	t_sim		copy;
	t_config	config;

	config = get_config(4);
	assert(init_simulation(&sim, &config) == 0);
	assert(sim.config.time_to_burnout == 800 && sim.stopped == 0);
	assert(sim.dongles != NULL && sim.coders != NULL);
	assert(sim.coders[3].sim == &sim && sim.coders[3].id == 4);
	assert(free_sim(&sim) == 0);
	memset(&sim, 0x5A, sizeof(sim));
	memcpy(&copy, &sim, sizeof(sim));
	assert(init_simulation(&sim, NULL) == -1);
	config.number_of_coders = 0;
	assert(init_simulation(&sim, &config) == -1);
	assert(memcmp(&sim, &copy, sizeof(sim)) == 0);
	assert(init_simulation(NULL, &config) == -1);
	return (0);
}
