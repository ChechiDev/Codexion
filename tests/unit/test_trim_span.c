/* Unit tests for trim_span: edge cases beyond the acceptance contract. */

#include <assert.h>
#include <string.h>

#include "main.h"

static void	test_single_char(void)
{
	const char	*s = "x";
	t_span		span;

	span = trim_span(s);
	assert(span.start == s && span.len == 1);
}

static void	test_only_leading_and_only_trailing(void)
{
	const char	*lead = "\t\tab";
	const char	*trail = "ab\n\n";
	t_span		span;

	span = trim_span(lead);
	assert(span.start == lead + 2 && span.len == 2);
	span = trim_span(trail);
	assert(span.start == trail && span.len == 2);
}

static void	test_blank_is_non_null(void)
{
	t_span	span;

	span = trim_span("");
	assert(span.start != NULL && span.len == 0);
	span = trim_span(" \t\r");
	assert(span.start != NULL && span.len == 0);
}

static void	test_non_space_controls_kept(void)
{
	const char	*s = "\x01 a\x7f";
	t_span		span;

	span = trim_span(s);
	assert(span.start == s && span.len == 4);
}

int	main(void)
{
	test_single_char();
	test_only_leading_and_only_trailing();
	test_blank_is_non_null();
	test_non_space_controls_kept();
	return (0);
}
