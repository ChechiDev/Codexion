/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 14:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include <stdint.h>
#include <stdlib.h>

int	init_coders(t_coder **out, t_sim *sim, int n)
{
	t_coder	*array;
	int		i;

	if (out == NULL || sim == NULL || n < 1)
		return (-1);
	if ((size_t)n > SIZE_MAX / sizeof(t_coder))
		return (-1);
	array = malloc((size_t)n * sizeof(t_coder));
	if (array == NULL)
		return (-1);
	i = 0;
	while (i < n)
	{
		if (init_coder(&array[i], sim, i, n) != 0)
		{
			free_coders(array, i);
			return (-1);
		}
		i++;
	}
	*out = array;
	return (0);
}
