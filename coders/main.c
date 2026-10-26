/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 16:47:34 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/26 18:50:17 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

void	print_usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler\n"
		"  scheduler is either fifo or edf\n");
}

int	main(int argc, char **argv)
{
	(void)argv;
	if (argc != 9)
	{
		print_usage();
		return (EXIT_USAGE);
	}
	return (0);
}
