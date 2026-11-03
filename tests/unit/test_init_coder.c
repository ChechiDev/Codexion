#include "main.h"

#include <assert.h>
#include <string.h>

static void	check_success(int seat, int n, int first, int second)
{
	t_sim		sim;
	t_coder		coder;

	memset(&coder, 0x5a, sizeof(coder));
	assert(init_coder(&coder, &sim, seat, n) == 0);
	assert(coder.id == seat + 1);
	assert(coder.first_dongle == first);
	assert(coder.second_dongle == second);
	assert(coder.last_compile_start == 0);
	assert(coder.compile_count == 0);
	assert(coder.sim == &sim);
	assert(pthread_mutex_trylock(&coder.mutex) == 0);
	assert(pthread_mutex_unlock(&coder.mutex) == 0);
	assert(pthread_mutex_destroy(&coder.mutex) == 0);
}

static void	check_invalid(t_sim *sim, int seat, int n)
{
	t_coder			coder;
	unsigned char	before[sizeof(t_coder)];

	memset(&coder, 0x5a, sizeof(coder));
	memcpy(before, &coder, sizeof(coder));
	assert(init_coder(&coder, sim, seat, n) == -1);
	assert(memcmp(&coder, before, sizeof(coder)) == 0);
}

int	main(void)
{
	t_sim	sim;

	check_success(0, 4, 0, 1);
	check_success(3, 4, 0, 3);
	check_success(0, 1, 0, 0);
	check_invalid(NULL, 0, 4);
	check_invalid(&sim, -1, 4);
	check_invalid(&sim, 4, 4);
	check_invalid(&sim, 0, 0);
	check_invalid(&sim, 0, -1);
	assert(init_coder(NULL, &sim, 0, 4) == -1);
	return (0);
}
