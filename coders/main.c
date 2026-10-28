/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 16:47:34 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/27 17:06:02 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "main.h"

static void	print_arg_errors(int mask)
{
	int	position;

	position = 1;
	while (position <= 8)
	{
		if (mask & (1 << position))
			print_arg_error(position);
		position++;
	}
}

int	main(int argc, char **argv)
{
	t_config	config;
	int			mask;

	if (argc != 9)
	{
		print_usage();
		return (EXIT_USAGE);
	}
	mask = parse_config(argv, &config);
	if (mask)
	{
		print_arg_errors(mask);
		print_usage();
		return (EXIT_USAGE);
	}
	return (0);
}
