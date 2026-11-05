#include "main.h"

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

void	*run_monitor(void *arg);

static t_config	get_config(int target)
{
	t_config	config;

	memset(&config, 0, sizeof(config));
	config.number_of_coders = 3;
	config.time_to_burnout = 300;
	config.time_to_compile = 100;
	config.time_to_debug = 100;
	config.time_to_refactor = 100;
	config.number_of_compiles_required = target;
	config.mode = MODE_FIFO;
	return (config);
}

static void	run_case(int target, int count)
{
	t_config	config;
	t_sim		sim;
	int			i;

	config = get_config(target);
	assert(init_simulation(&sim, &config) == 0);
	for (i = 0; i < 3; ++i)
	{
		sim.coders[i].compile_count = count;
		sim.coders[i].last_compile_start = (i == 0) ? 0 : LONG_MAX;
	}
	sim.start_ms = get_time_ms() - 1000;
	assert(run_monitor(&sim) == NULL);
	assert(sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	assert(run_monitor(NULL) == NULL);
	run_case(3, 3);
	run_case(0, 0);
	run_case(3, 0);
	return (0);
}
