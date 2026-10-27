/* Unit tests for trim_slice: same contract as trim_span. */

#include <assert.h>
#include <string.h>

#include "main.h"

static void	test_both_sides(void)
{
	const char	*s = " \t 0007 \r\n";
	t_slice		slice;

	slice = trim_slice(s);
	assert(slice.start == s + 3 && slice.len == 4);
}

static void	test_inner_whitespace_kept(void)
{
	const char	*s = "  fi fo  ";
	t_slice		slice;

	slice = trim_slice(s);
	assert(slice.start == s + 2 && slice.len == 5);
}

static void	test_blank_inputs(void)
{
	t_slice	slice;

	slice = trim_slice("");
	assert(slice.len == 0);
	slice = trim_slice("   ");
	assert(slice.len == 0);
}

static void	test_null_input(void)
{
	t_slice	slice;

	slice = trim_slice(NULL);
	assert(slice.start == NULL && slice.len == 0);
}

static void	test_input_unchanged(void)
{
	char	buf[16];
	char	copy[16];

	memset(buf, 0, sizeof(buf));
	strcpy(buf, "  ab  ");
	memcpy(copy, buf, sizeof(buf));
	trim_slice(buf);
	assert(memcmp(buf, copy, sizeof(buf)) == 0);
}

int	main(void)
{
	test_both_sides();
	test_inner_whitespace_kept();
	test_blank_inputs();
	test_null_input();
	test_input_unchanged();
	return (0);
}
