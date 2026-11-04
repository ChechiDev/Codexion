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

static void	check_running_sim(int n)
{
	t_config	config;
	t_sim		sim;
	int			i;

	config = get_config(n);
	assert(init_simulation(&sim, &config) == 0);
	assert(wake_dongles(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	assert(is_stopped(&sim) == 0);
	i = 0;
	while (i < n)
	{
		assert(pthread_mutex_trylock(&sim.dongles[i].mutex) == 0);
		assert(pthread_mutex_unlock(&sim.dongles[i].mutex) == 0);
		i++;
	}
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	t_sim	sim = {0};

	check_running_sim(1);
	check_running_sim(5);
	assert(wake_dongles(NULL) == -1);
	assert(wake_dongles(&sim) == -1);
	return (0);
}
