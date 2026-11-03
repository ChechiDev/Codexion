#include "main.h"

#include <assert.h>
#include <limits.h>

static void	check_valid(int n)
{
	t_coder	*coders;
	t_sim	sim;
	int		i;

	coders = NULL;
	assert(init_coders(&coders, &sim, n) == 0);
	assert(coders != NULL);
	i = 0;
	while (i < n)
	{
		assert(coders[i].id == i + 1);
		assert(coders[i].compile_count == 0);
		assert(coders[i].sim == &sim);
		i++;
	}
	assert(free_coders(coders, n) == 0);
}

static void	check_invalid(t_sim *sim, int n)
{
	t_coder	*sentinel;
	t_coder	*coders;

	sentinel = (t_coder *)&sentinel;
	coders = sentinel;
	assert(init_coders(&coders, sim, n) == -1);
	assert(coders == sentinel);
}

int	main(void)
{
	t_sim	sim;

	check_valid(1);
	check_valid(4);
	check_valid(50);
	check_invalid(&sim, 0);
	check_invalid(&sim, -1);
	check_invalid(&sim, INT_MIN);
	check_invalid(NULL, 4);
	assert(init_coders(NULL, &sim, 4) == -1);
	return (0);
}
