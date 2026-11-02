/* Unit tests for parse_config: fill, invalid flags, no early exit. */

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
	int			invalid[9];

	fill(argv);
	assert(parse_config(argv, &config, invalid) == 0);
	assert(invalid[1] == 0 && invalid[8] == 0);
	assert(config.number_of_coders == 4 && config.dongle_cooldown == 50);
	assert(config.mode == MODE_FIFO);
}

static void	test_flags(void)
{
	char		*argv[10];
	t_config	config;
	int			invalid[9];
	int			position;

	fill(argv);
	argv[1] = "0";
	argv[7] = "x";
	argv[8] = "no";
	position = 0;
	while (position < 9)
		invalid[position++] = 42;
	assert(parse_config(argv, &config, invalid) == 3);
	assert(invalid[0] == 42);
	assert(invalid[1] == 1 && invalid[7] == 1 && invalid[8] == 1);
	assert(invalid[2] == 0 && invalid[3] == 0 && invalid[4] == 0);
	assert(invalid[5] == 0 && invalid[6] == 0);
}

int	main(void)
{
	test_valid();
	test_flags();
	return (0);
}
