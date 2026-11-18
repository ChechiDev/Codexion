/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_config.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 12:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/17 18:21:30 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	parse_field(const char *str, int position, int *out)
{
	int	value;
	int	status;

	status = parse_number(str, &value);
	if (status == -2)
		return (ARG_BAD_SIGN);
	if (status != 0 || value < get_arg_minimum(position))
		return (ARG_BAD_RANGE);
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
				fields[position]);
		count += invalid[position] != 0;
		position++;
	}
	invalid[8] = 0;
	if (parse_mode(argv[8], &config->mode) != 0)
		invalid[8] = ARG_BAD_RANGE;
	count += invalid[8] != 0;
	return (count);
}
