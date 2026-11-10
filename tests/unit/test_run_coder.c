#include "main.h"

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static void	init_sim(t_sim *sim, int count)
{
	t_config	config;

	memset(&config, 0, sizeof(config));
	config.number_of_coders = count;
	config.time_to_burnout = 800;
	config.time_to_compile = 5;
	config.time_to_debug = 5;
	config.time_to_refactor = 5;
	config.mode = MODE_FIFO;
	assert(init_simulation(sim, &config) == 0);
	sim->start_ms = get_time_ms();
}

static void	check_invalid_and_stopped(void)
{
	t_sim	sim;
	t_coder	bare;

	init_sim(&sim, 2);
	memset(&bare, 0, sizeof(bare));
	assert(run_coder(NULL) == NULL);
	assert(run_coder(&bare) == NULL);
	assert(set_stopped(&sim) == 0);
	assert(run_coder(&sim.coders[0]) == NULL);
	assert(sim.failed == 0);
	assert(sim.dongles[0].available == 1);
	assert(sim.coders[0].compile_count == 0);
	assert(free_sim(&sim) == 0);
}

static void	check_count_failure_stops(void)
{
	t_sim	sim;
	int		saved;
	int		null_fd;

	init_sim(&sim, 2);
	sim.coders[0].compile_count = 2147483647;
	saved = dup(STDOUT_FILENO);
	null_fd = open("/dev/null", O_WRONLY);
	assert(saved >= 0 && null_fd >= 0);
	assert(dup2(null_fd, STDOUT_FILENO) >= 0);
	assert(run_coder(&sim.coders[0]) == NULL);
	assert(dup2(saved, STDOUT_FILENO) >= 0);
	close(saved);
	close(null_fd);
	assert(is_stopped(&sim) == 1);
	assert(sim.failed == 1);
	assert(sim.dongles[0].available == 1);
	assert(sim.dongles[1].available == 1);
	assert(free_sim(&sim) == 0);
}

static void	check_start_turn_order(void)
{
	t_sim	sim;

	init_sim(&sim, 2);
	assert(set_stopped(&sim) == 0);
	assert(run_coder(&sim.coders[1]) == NULL);
	assert(sim.failed == 0);
	assert(sim.next_ticket == 0);
	assert(run_coder(&sim.coders[0]) == NULL);
	assert(sim.failed == 0);
	assert(sim.next_ticket == 1);
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	check_start_turn_order();
	check_invalid_and_stopped();
	check_count_failure_stops();
	return (0);
}
