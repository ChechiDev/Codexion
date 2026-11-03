#include "main.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

static void	build(t_sim *sim)
{
	memset(sim, 0, sizeof(*sim));
	assert(pthread_mutex_init(&sim->ticket_mutex, NULL) == 0);
}

static void	check_unlocked(t_sim *sim)
{
	assert(pthread_mutex_trylock(&sim->ticket_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->ticket_mutex) == 0);
}

int	main(void)
{
	t_sim	sim;
	long	out;

	build(&sim);
	out = -5;
	assert(take_ticket(&sim, &out) == 0 && out == 0 && sim.next_ticket == 1);
	assert(take_ticket(&sim, &out) == 0 && out == 1 && sim.next_ticket == 2);
	check_unlocked(&sim);
	sim.next_ticket = LONG_MAX - 1;
	assert(take_ticket(&sim, &out) == 0 && out == LONG_MAX - 1);
	assert(sim.next_ticket == LONG_MAX);
	out = -5;
	assert(take_ticket(&sim, &out) == -1 && out == -5);
	assert(sim.next_ticket == LONG_MAX);
	check_unlocked(&sim);
	sim.next_ticket = -1;
	assert(take_ticket(&sim, &out) == -1 && out == -5);
	assert(sim.next_ticket == -1);
	check_unlocked(&sim);
	assert(take_ticket(NULL, &out) == -1 && out == -5);
	assert(take_ticket(&sim, NULL) == -1 && sim.next_ticket == -1);
	check_unlocked(&sim);
	assert(pthread_mutex_destroy(&sim.ticket_mutex) == 0);
	return (0);
}
