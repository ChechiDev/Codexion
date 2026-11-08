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
	config.time_to_compile = 100;
	config.time_to_debug = 100;
	config.time_to_refactor = 100;
	config.number_of_compiles_required = 1;
	config.dongle_cooldown = 0;
	config.mode = MODE_FIFO;
	assert(init_simulation(sim, &config) == 0);
	sim->start_ms = get_time_ms();
}

static void	check_invalid_input(void)
{
	t_sim	sim;
	t_coder	bare;
	int		taken;

	init_sim(&sim, 2);
	taken = -9;
	assert(take_coder_dongles(NULL, &taken) == -1);
	assert(take_coder_dongles(&sim.coders[0], NULL) == -1);
	memset(&bare, 0, sizeof(bare));
	assert(take_coder_dongles(&bare, &taken) == -1);
	assert(taken == -9);
	assert(sim.dongles[0].available == 1);
	assert(free_sim(&sim) == 0);
}

static void	check_take_and_stop(int count)
{
	t_sim	sim;
	int		taken;
	int		saved;
	int		null_fd;

	init_sim(&sim, count);
	saved = dup(STDOUT_FILENO);
	null_fd = open("/dev/null", O_WRONLY);
	assert(saved >= 0 && null_fd >= 0);
	assert(dup2(null_fd, STDOUT_FILENO) >= 0);
	taken = -9;
	assert(take_coder_dongles(&sim.coders[0], &taken) == 0 && taken == 1);
	assert(sim.dongles[0].available == 0);
	assert(release_dongles(&sim.coders[0]) == 0);
	assert(set_stopped(&sim) == 0);
	assert(take_coder_dongles(&sim.coders[0], &taken) == 0 && taken == 0);
	assert(dup2(saved, STDOUT_FILENO) >= 0);
	assert(close(saved) == 0 && close(null_fd) == 0);
	assert(free_sim(&sim) == 0);
}

static void	check_ticket_exhaustion(void)
{
	t_sim	sim;
	int		taken;

	init_sim(&sim, 3);
	sim.next_ticket = 0x7fffffffffffffffL;
	taken = -9;
	assert(take_coder_dongles(&sim.coders[0], &taken) == -1);
	assert(taken == -9 && sim.dongles[0].available == 1);
	assert(free_sim(&sim) == 0);
}

static void	check_print_failure(void)
{
	t_sim	sim;
	int		taken;
	int		saved;
	int		full_fd;

	init_sim(&sim, 3);
	saved = dup(STDOUT_FILENO);
	full_fd = open("/dev/full", O_WRONLY);
	assert(saved >= 0 && full_fd >= 0);
	assert(dup2(full_fd, STDOUT_FILENO) >= 0);
	taken = -9;
	assert(take_coder_dongles(&sim.coders[0], &taken) == -1);
	assert(dup2(saved, STDOUT_FILENO) >= 0);
	assert(close(saved) == 0 && close(full_fd) == 0);
	assert(taken == -9 && sim.stopped == 0);
	assert(sim.dongles[0].available == 1 && sim.dongles[1].available == 1);
	assert(free_sim(&sim) == 0);
}

int	main(void)
{
	check_invalid_input();
	check_take_and_stop(3);
	check_take_and_stop(1);
	check_ticket_exhaustion();
	check_print_failure();
	return (0);
}
