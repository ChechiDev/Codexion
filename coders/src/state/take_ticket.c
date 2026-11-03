/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_ticket.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:01:35 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 19:01:35 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include <limits.h>

int	take_ticket(t_sim *sim, long *out)
{
	long	ticket;
	int		status;

	if (sim == NULL || out == NULL)
		return (-1);
	if (pthread_mutex_lock(&sim->ticket_mutex) != 0)
		return (-1);
	status = -1;
	ticket = sim->next_ticket;
	if (ticket >= 0 && ticket < LONG_MAX)
	{
		sim->next_ticket = ticket + 1;
		status = 0;
	}
	if (pthread_mutex_unlock(&sim->ticket_mutex) != 0)
		return (-1);
	if (status == 0)
		*out = ticket;
	return (status);
}
