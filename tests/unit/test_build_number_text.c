/* Unit tests for build_number_text: digits, bounds and invalid input. */

#include <assert.h>
#include <limits.h>
#include <string.h>

#include "main.h"

static void	test_values(void)
{
	char	buf[NUMBER_TEXT_MAX];

	assert(build_number_text(0, buf) == 1 && buf[0] == '0');
	assert(build_number_text(10, buf) == 2 && memcmp(buf, "10", 2) == 0);
	assert(build_number_text(1234567, buf) == 7);
	assert(memcmp(buf, "1234567", 7) == 0);
	assert(build_number_text(LONG_MAX, buf) == 19);
	assert(memcmp(buf, "9223372036854775807", 19) == 0);
}

static void	test_bounds(void)
{
	char	buf[NUMBER_TEXT_MAX + 2];

	memset(buf, 'Q', sizeof(buf));
	assert(build_number_text(305, buf + 1) == 3);
	assert(buf[0] == 'Q' && memcmp(buf + 1, "305", 3) == 0);
	assert(buf[4] == 'Q' && buf[NUMBER_TEXT_MAX + 1] == 'Q');
}

static void	test_invalid(void)
{
	char	buf[NUMBER_TEXT_MAX];

	memset(buf, 'Q', sizeof(buf));
	assert(build_number_text(-1, buf) == -1);
	assert(build_number_text(LONG_MIN, buf) == -1);
	assert(buf[0] == 'Q');
	assert(build_number_text(5, NULL) == -1);
}

int	main(void)
{
	test_values();
	test_bounds();
	test_invalid();
	return (0);
}
