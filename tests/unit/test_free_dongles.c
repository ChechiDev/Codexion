#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void	check_prefix(int total, int count)
{
	t_dongle	*dongles;
	int			i;

	dongles = malloc(total * sizeof(*dongles));
	assert(dongles != NULL);
	memset(dongles, 0xa5, total * sizeof(*dongles));
	i = 0;
	while (i < count)
	{
		assert(init_dongle(&dongles[i]) == 0);
		i++;
	}
	assert(free_dongles(dongles, count) == 0);
}

int	main(void)
{
	check_prefix(3, 3);
	check_prefix(4, 2);
	check_prefix(1, 0);
	assert(free_dongles(NULL, 0) == 0);
	assert(free_dongles(NULL, 2) == -1);
	return (0);
}
