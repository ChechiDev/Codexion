#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void	*run_quick(void *arg)
{
	(void)arg;
	return (NULL);
}

int	main(void)
{
	t_sim	sim;
	int		i;

	assert(join_threads(NULL, 0) == -1);
	memset(&sim, 0, sizeof(sim));
	sim.config.number_of_coders = 3;
	assert(join_threads(&sim, -1) == -1);
	assert(join_threads(&sim, 4) == -1);
	sim.coders = calloc(3, sizeof(t_coder));
	assert(sim.coders != NULL);
	i = 0;
	while (i < 2)
		assert(pthread_create(&sim.coders[i++].thread, NULL, run_quick,
				NULL) == 0);
	assert(pthread_create(&sim.monitor, NULL, run_quick, NULL) == 0);
	assert(join_threads(&sim, 2) == 0);
	free(sim.coders);
	return (0);
}
