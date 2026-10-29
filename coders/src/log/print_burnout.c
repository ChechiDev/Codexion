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

/*
** Caller holds log_mutex. Takes stop_mutex, sets stopped to 1 if it was 0 and
** releases stop_mutex. Returns 0 only if this call made the first stop and the
** unlock succeeded; -1 if already stopped or on a lock failure.
*/
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

/*
** Performs the first stop and writes "<ts> <coder> burned out\n" in one
** critical section under log_mutex. Returns 0 if this call made the first
** stop and the line was written; -1 if already stopped or on any failure (the
** stop stays committed once made). No retries.
*/
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
