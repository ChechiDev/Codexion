/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_config.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 12:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	parse_field(const char *str, int position, int *out)
{
	int	value;

	if (parse_number(str, &value) != 0)
		return (-1);
	if (value < get_arg_minimum(position))
		return (-1);
	*out = value;
	return (0);
}

static void	set_field_addresses(int **fields, t_config *config)
{
	fields[1] = &config->number_of_coders;
	fields[2] = &config->time_to_burnout;
	fields[3] = &config->time_to_compile;
	fields[4] = &config->time_to_debug;
	fields[5] = &config->time_to_refactor;
	fields[6] = &config->number_of_compiles_required;
	fields[7] = &config->dongle_cooldown;
}

int	parse_config(char **argv, t_config *config, int *invalid)
{
	int	*fields[8];
	int	count;
	int	position;

	count = 0;
	set_field_addresses(fields, config);
	position = 1;
	while (position <= 7)
	{
		invalid[position] = parse_field(argv[position], position,
				fields[position]) != 0;
		count += invalid[position];
		position++;
	}
	invalid[8] = parse_mode(argv[8], &config->mode) != 0;
	count += invalid[8];
	return (count);
}
