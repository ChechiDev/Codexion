/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_burnout.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 12:44:39 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/29 12:44:39 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	set_stopped_once(t_sim *sim)
{
	int	status;

	if (pthread_mutex_lock(&sim->stop_mutex) != 0)
		return (-1);
	status = -1;
	if (!sim->stopped)
	{
		sim->stopped = 1;
		status = 0;
	}
	if (pthread_mutex_unlock(&sim->stop_mutex) != 0)
		return (-1);
	return (status);
}

int	print_burnout(t_sim *sim, int coder)
{
	int	status;

	if (pthread_mutex_lock(&sim->log_mutex) != 0)
		return (-1);
	status = set_stopped_once(sim);
	if (status == 0)
		status = print_log_line(sim, coder, "burned out");
	if (pthread_mutex_unlock(&sim->log_mutex) != 0)
		status = -1;
	return (status);
}
