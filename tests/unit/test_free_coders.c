#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void	check_prefix(int total, int count)
{
	t_coder	*coders;
	t_sim	sim;
	int		i;

	coders = malloc(total * sizeof(*coders));
	assert(coders != NULL);
	memset(coders, 0xa5, total * sizeof(*coders));
	i = 0;
	while (i < count)
	{
		assert(init_coder(&coders[i], &sim, i, total) == 0);
		i++;
	}
	assert(free_coders(coders, count) == 0);
}

int	main(void)
{
	check_prefix(3, 3);
	check_prefix(4, 2);
	check_prefix(1, 0);
	assert(free_coders(NULL, 0) == 0);
	assert(free_coders(NULL, 2) == -1);
	assert(free_coders(NULL, -1) == -1);
	return (0);
}
