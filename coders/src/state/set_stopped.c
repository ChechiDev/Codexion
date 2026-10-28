/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_stopped.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:05:41 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 18:05:41 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	set_stopped(t_sim *sim)
{
	int	status;

	if (pthread_mutex_lock(&sim->log_mutex) != 0)
		return (-1);
	if (pthread_mutex_lock(&sim->stop_mutex) != 0)
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return (-1);
	}
	sim->stopped = 1;
	status = 0;
	if (pthread_mutex_unlock(&sim->stop_mutex) != 0)
		status = -1;
	if (pthread_mutex_unlock(&sim->log_mutex) != 0)
		status = -1;
	return (status);
}
