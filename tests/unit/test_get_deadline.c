/* Unit tests for get_deadline: validation, normalisation and bracketing. */

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <sys/time.h>

#include "main.h"

static void	test_invalid_input(void)
{
	struct timespec	out;
	struct timespec	copy;

	memset(&out, 0x5a, sizeof(out));
	copy = out;
	assert(get_deadline(-1, &out) == -1);
	assert(memcmp(&out, &copy, sizeof(out)) == 0);
	assert(get_deadline(10, NULL) == -1);
}

static void	test_bracket_and_normalised(void)
{
	struct timeval	before;
	struct timespec	out;
	long			ms;

	ms = 0;
	while (ms < 2000)
	{
		gettimeofday(&before, NULL);
		assert(get_deadline(ms, &out) == 0);
		assert(out.tv_nsec >= 0 && out.tv_nsec < 1000000000L);
		assert(out.tv_sec >= before.tv_sec + ms / 1000);
		ms += 137;
	}
}

static void	test_long_max(void)
{
	struct timespec	out;

	assert(get_deadline(LONG_MAX, &out) == 0);
	assert(out.tv_nsec >= 0 && out.tv_nsec < 1000000000L);
	assert(out.tv_sec > LONG_MAX / 1000);
}

int	main(void)
{
	test_invalid_input();
	test_bracket_and_normalised();
	test_long_max();
	return (0);
}
