#include "main.h"

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int	main(void)
{
	t_config	cfg;
	t_sim		sim;
	int			null_fd;

	assert(start_threads(NULL) == -1);
	null_fd = open("/dev/null", O_WRONLY);
	assert(null_fd >= 0);
	assert(dup2(null_fd, STDOUT_FILENO) == STDOUT_FILENO);
	memset(&cfg, 0, sizeof(cfg));
	cfg.number_of_coders = 2;
	cfg.time_to_burnout = 5000;
	cfg.time_to_compile = 10;
	cfg.time_to_debug = 10;
	cfg.time_to_refactor = 10;
	cfg.number_of_compiles_required = 2;
	cfg.dongle_cooldown = 5;
	cfg.mode = MODE_FIFO;
	assert(init_simulation(&sim, &cfg) == 0);
	assert(start_threads(&sim) == 0);
	assert(sim.start_ms >= 0);
	assert(join_threads(&sim, 2) == 0);
	assert(sim.failed == 0 && sim.stopped == 1);
	assert(free_sim(&sim) == 0);
	return (0);
}
