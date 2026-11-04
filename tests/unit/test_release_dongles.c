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
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
	return (config);
}

static void	check_unlocked(t_dongle *dongle)
{
	assert(pthread_mutex_trylock(&dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&dongle->mutex) == 0);
}

static void	check_release(int n, int seat)
{
	t_config	config;
	t_sim		sim;
	t_coder		*coder;

	config = get_config(n);
	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms();
	coder = &sim.coders[seat];
	sim.dongles[coder->first_dongle].available = 0;
	sim.dongles[coder->second_dongle].available = 0;
	assert(release_dongles(coder) == 0);
	assert(sim.dongles[coder->first_dongle].available == 1);
	assert(sim.dongles[coder->second_dongle].available == 1);
	assert(sim.dongles[coder->first_dongle].released_at >= 0);
	check_unlocked(&sim.dongles[coder->first_dongle]);
	check_unlocked(&sim.dongles[coder->second_dongle]);
	assert(free_sim(&sim) == 0);
}

static void	check_invalid(void)
{
	t_coder	coder = {0};
	t_sim	sim = {0};

	assert(release_dongles(NULL) == -1);
	assert(release_dongles(&coder) == -1);
	coder.sim = &sim;
	assert(release_dongles(&coder) == -1);
}

int	main(void)
{
	check_release(1, 0);
	check_release(4, 0);
	check_release(4, 3);
	check_invalid();
	return (0);
}
