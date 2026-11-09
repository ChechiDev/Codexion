#include "main.h"

#include <assert.h>
#include <string.h>

static t_config	get_config(int coders, int required)
{
	t_config	cfg;

	cfg.number_of_coders = coders;
	cfg.time_to_burnout = 2000;
	cfg.time_to_compile = 10;
	cfg.time_to_debug = 10;
	cfg.time_to_refactor = 10;
	cfg.number_of_compiles_required = required;
	cfg.dongle_cooldown = 0;
	cfg.mode = MODE_FIFO;
	return (cfg);
}

int	main(void)
{
	t_config		cfg;
	t_sim			sim;
	unsigned char	before[sizeof(t_sim)];

	cfg = get_config(2, 1);
	assert(run_simulation(NULL, &cfg) == -1);
	assert(run_simulation(&sim, NULL) == -1);
	cfg = get_config(2, 0);
	memset(&sim, 0x5a, sizeof(sim));
	memcpy(before, &sim, sizeof(sim));
	assert(run_simulation(&sim, &cfg) == 0);
	assert(memcmp(before, &sim, sizeof(sim)) == 0);
	cfg = get_config(3, 1);
	assert(run_simulation(&sim, &cfg) == 0);
	return (0);
}
