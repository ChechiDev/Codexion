#include "main.h"

#include <assert.h>

int	main(void)
{
	t_sim	sim;
	t_coder	coder;

	assert(destroy_coder(NULL) == -1);
	assert(init_coder(&coder, &sim, 1, 4) == 0);
	coder.compile_count = 3;
	assert(destroy_coder(&coder) == 0);
	assert(coder.compile_count == 3);
	assert(coder.sim == &sim);
	assert(init_coder(&coder, &sim, 1, 4) == 0);
	assert(destroy_coder(&coder) == 0);
	return (0);
}
