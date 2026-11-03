#include "main.h"

#include <assert.h>
#include <limits.h>

static void	check_valid(int n)
{
	t_dongle	*dongles;
	int			i;

	dongles = NULL;
	assert(init_dongles(&dongles, n) == 0);
	assert(dongles != NULL);
	i = 0;
	while (i < n)
	{
		assert(dongles[i].available == 1);
		assert(dongles[i].released_at == -1);
		assert(dongles[i].heap.capacity == 2);
		assert(dongles[i].heap.items != NULL);
		i++;
	}
	assert(free_dongles(dongles, n) == 0);
}

static void	check_invalid(int n)
{
	t_dongle	*sentinel;
	t_dongle	*dongles;

	sentinel = (t_dongle *)&sentinel;
	dongles = sentinel;
	assert(init_dongles(&dongles, n) == -1);
	assert(dongles == sentinel);
}

int	main(void)
{
	check_valid(1);
	check_valid(4);
	check_valid(50);
	check_invalid(0);
	check_invalid(-1);
	check_invalid(INT_MIN);
	assert(init_dongles(NULL, 4) == -1);
	return (0);
}
