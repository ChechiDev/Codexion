/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wake_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 11:17:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/04 11:17:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	wake_dongle(t_dongle *dongle)
{
	int	status;

	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (-1);
	status = 0;
	if (pthread_cond_broadcast(&dongle->cond) != 0)
		status = -1;
	if (pthread_mutex_unlock(&dongle->mutex) != 0)
		status = -1;
	return (status);
}

int	wake_dongles(t_sim *sim)
{
	int	i;
	int	status;

	if (sim == NULL || sim->dongles == NULL)
		return (-1);
	status = 0;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (wake_dongle(&sim->dongles[i]) != 0)
			status = -1;
		i++;
	}
	return (status);
}
