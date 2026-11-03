#include "main.h"

#include <assert.h>
#include <string.h>

static void	build(t_sim *sim, int n)
{
	memset(sim, 0, sizeof(*sim));
	sim->config.number_of_coders = n;
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->ticket_mutex, NULL) == 0);
	assert(init_dongles(&sim->dongles, n) == 0);
	assert(init_coders(&sim->coders, sim, n) == 0);
}

int	main(void)
{
	t_sim	sim;

	build(&sim, 3);
	sim.stopped = 1;
	sim.next_ticket = 7;
	assert(free_sim(&sim) == 0);
	assert(sim.dongles == NULL && sim.coders == NULL);
	assert(sim.stopped == 1 && sim.next_ticket == 7);
	assert(sim.config.number_of_coders == 3);
	build(&sim, 1);
	assert(free_sim(&sim) == 0);
	assert(free_sim(NULL) == -1);
	return (0);
}
