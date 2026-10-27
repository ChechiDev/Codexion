/* Unit tests for is_digit: decimal digits and rejected characters. */

#include <assert.h>

#include "main.h"

static void	test_digits(void)
{
	char	c;

	c = '0';
	while (c <= '9')
	{
		assert(is_digit(c) == 1);
		c++;
	}
}

static void	test_other_chars(void)
{
	assert(is_digit('/') == 0);
	assert(is_digit(':') == 0);
	assert(is_digit('\0') == 0);
	assert(is_digit(' ') == 0);
	assert(is_digit('+') == 0);
	assert(is_digit('-') == 0);
	assert(is_digit('a') == 0);
	assert(is_digit((char)-1) == 0);
}

int	main(void)
{
	test_digits();
	test_other_chars();
	return (0);
}
