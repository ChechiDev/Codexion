/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 11:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 11:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include <stdint.h>
#include <stdlib.h>

int	init_dongles(t_dongle **out, int n)
{
	t_dongle	*array;
	int			i;

	if (out == NULL || n < 1)
		return (-1);
	if ((size_t)n > SIZE_MAX / sizeof(t_dongle))
		return (-1);
	array = malloc((size_t)n * sizeof(t_dongle));
	if (array == NULL)
		return (-1);
	i = 0;
	while (i < n)
	{
		if (init_dongle(&array[i]) != 0)
		{
			free_dongles(array, i);
			return (-1);
		}
		i++;
	}
	*out = array;
	return (0);
}
