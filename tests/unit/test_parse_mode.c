/* Unit tests for parse_mode: exact fifo/edf, rejection, NULL safety. */

#include <assert.h>
#include <stddef.h>

#include "main.h"

static void	test_valid(void)
{
	t_mode	mode;

	mode = MODE_EDF;
	assert(parse_mode("fifo", &mode) == 0 && mode == MODE_FIFO);
	assert(parse_mode("edf", &mode) == 0 && mode == MODE_EDF);
}

static void	test_invalid(void)
{
	t_mode	mode;

	mode = MODE_EDF;
	assert(parse_mode("FIFO", &mode) == -1 && mode == MODE_EDF);
	assert(parse_mode("fifo ", &mode) == -1 && mode == MODE_EDF);
	assert(parse_mode("fif", &mode) == -1 && mode == MODE_EDF);
	assert(parse_mode("", &mode) == -1 && mode == MODE_EDF);
}

static void	test_null(void)
{
	t_mode	mode;

	mode = MODE_FIFO;
	assert(parse_mode(NULL, &mode) == -1 && mode == MODE_FIFO);
	assert(parse_mode("edf", NULL) == -1);
}

int	main(void)
{
	test_valid();
	test_invalid();
	test_null();
	return (0);
}
