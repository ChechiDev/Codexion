/* Unit tests for is_space: whitespace set and rejected characters. */

#include <assert.h>
#include <string.h>

#include "main.h"

static void	test_whitespace_chars(void)
{
	const char	*spaces = " \t\n\v\f\r";
	size_t		i;

	i = 0;
	while (i < strlen(spaces))
	{
		assert(is_space(spaces[i]) == 1);
		i++;
	}
}

static void	test_other_chars(void)
{
	assert(is_space('\0') == 0);
	assert(is_space('a') == 0);
	assert(is_space('0') == 0);
	assert(is_space('_') == 0);
	assert(is_space((char)127) == 0);
	assert(is_space((char)-1) == 0);
}

int	main(void)
{
	test_whitespace_chars();
	test_other_chars();
	return (0);
}
