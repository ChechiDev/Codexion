/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 10:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 10:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include <stdlib.h>

int	free_dongles(t_dongle *dongles, int count)
{
	int	result;
	int	i;

	if (count < 0 || (dongles == NULL && count > 0))
		return (-1);
	result = 0;
	i = count - 1;
	while (i >= 0)
	{
		if (destroy_dongle(&dongles[i]) != 0)
			result = -1;
		i--;
	}
	free(dongles);
	return (result);
}
