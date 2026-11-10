#include "main.h"

#include <assert.h>
#include <string.h>

static void	init_case(t_sim *sim, t_coder *coder, int id)
{
	memset(sim, 0, sizeof(*sim));
	memset(coder, 0, sizeof(*coder));
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
	coder->id = id;
	coder->sim = sim;
}

static void	check_invalid(void)
{
	t_coder	coder;

	memset(&coder, 0, sizeof(coder));
	coder.id = 1;
	assert(sleep_start_turn(NULL) == -1);
	assert(sleep_start_turn(&coder) == -1);
}

static void	check_odd_seat(void)
{
	t_sim	sim;
	t_coder	coder;
	int		id;

	id = 1;
	while (id < 8)
	{
		init_case(&sim, &coder, id);
		assert(set_stopped(&sim) == 0);
		assert(sleep_start_turn(&coder) == 0);
		assert(pthread_mutex_destroy(&sim.stop_mutex) == 0);
		id += 2;
	}
}

static void	check_even_seat(void)
{
	t_sim	sim;
	t_coder	coder;
	long	before;

	init_case(&sim, &coder, 2);
	before = get_time_ms();
	assert(sleep_start_turn(&coder) == 0);
	assert(get_time_ms() >= before + START_TURN_MS);
	assert(set_stopped(&sim) == 0);
	assert(sleep_start_turn(&coder) == 1);
	assert(pthread_mutex_destroy(&sim.stop_mutex) == 0);
}

int	main(void)
{
	check_invalid();
	check_odd_seat();
	check_even_seat();
	return (0);
}
