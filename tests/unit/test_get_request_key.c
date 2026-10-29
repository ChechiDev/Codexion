#include "main.h"

#include <assert.h>
#include <limits.h>

int main(void)
{
	assert(get_request_key(MODE_FIFO, 5, -1, 0) == 5);
	assert(get_request_key(MODE_FIFO, LONG_MAX, LONG_MAX, -1) == LONG_MAX);
	assert(get_request_key(MODE_FIFO, -1, 0, 1) == -1);
	assert(get_request_key(MODE_EDF, 9, 1000, 410) == 1410);
	assert(get_request_key(MODE_EDF, 0, 0, 1) == 1);
	assert(get_request_key(MODE_EDF, 0, LONG_MAX - INT_MAX, INT_MAX)
		== LONG_MAX);
	assert(get_request_key(MODE_EDF, 0, LONG_MAX - INT_MAX + 1, INT_MAX)
		== -1);
	assert(get_request_key(MODE_EDF, 0, LONG_MAX, 1) == -1);
	assert(get_request_key(MODE_EDF, 0, -1, 5) == -1);
	assert(get_request_key(MODE_EDF, 0, 5, 0) == -1);
	assert(get_request_key(MODE_EDF, 0, 5, INT_MIN) == -1);
	assert(get_request_key((t_mode)7, 0, 5, 5) == -1);
	return (0);
}
