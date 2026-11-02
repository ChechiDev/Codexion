/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 16:47:34 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "main.h"

static void	print_arg_errors(const int *invalid)
{
	int	position;

	position = 1;
	while (position <= 8)
	{
		if (invalid[position])
			print_arg_error(position);
		position++;
	}
}

int	main(int argc, char **argv)
{
	t_config	config;
	int			invalid[9];

	if (argc != 9)
	{
		print_usage();
		return (EXIT_USAGE);
	}
	if (parse_config(argv, &config, invalid))
	{
		print_arg_errors(invalid);
		print_usage();
		return (EXIT_USAGE);
	}
	return (0);
}
