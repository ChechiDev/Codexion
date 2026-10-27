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
