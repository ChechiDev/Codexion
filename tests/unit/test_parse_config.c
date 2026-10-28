/* Unit tests for parse_config: fill, mask bits, no early exit. */

#include <assert.h>
#include <stddef.h>

#include "main.h"

static void	fill(char **argv)
{
	argv[0] = "codexion";
	argv[1] = "4";
	argv[2] = "800";
	argv[3] = "200";
	argv[4] = "200";
	argv[5] = "200";
	argv[6] = "3";
	argv[7] = "50";
	argv[8] = "fifo";
	argv[9] = NULL;
}

static void	test_valid(void)
{
	char		*argv[10];
	t_config	config;

	fill(argv);
	assert(parse_config(argv, &config) == 0);
	assert(config.number_of_coders == 4 && config.dongle_cooldown == 50);
	assert(config.mode == MODE_FIFO);
}

static void	test_mask(void)
{
	char		*argv[10];
	t_config	config;

	fill(argv);
	argv[1] = "0";
	argv[7] = "x";
	argv[8] = "no";
	assert(parse_config(argv, &config) == ((1 << 1) | (1 << 7) | (1 << 8)));
}

int	main(void)
{
	test_valid();
	test_mask();
	return (0);
}
