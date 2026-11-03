/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_sim.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 15:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	free_arrays(t_sim *sim, int n)
{
	int	result;

	result = 0;
	if (sim->coders != NULL)
	{
		if (free_coders(sim->coders, n) != 0)
			result = -1;
		sim->coders = NULL;
	}
	if (sim->dongles != NULL)
	{
		if (free_dongles(sim->dongles, n) != 0)
			result = -1;
		sim->dongles = NULL;
	}
	return (result);
}

int	free_sim(t_sim *sim)
{
	int	result;

	if (sim == NULL)
		return (-1);
	result = free_arrays(sim, sim->config.number_of_coders);
	if (pthread_mutex_destroy(&sim->ticket_mutex) != 0)
		result = -1;
	if (pthread_mutex_destroy(&sim->log_mutex) != 0)
		result = -1;
	if (pthread_mutex_destroy(&sim->stop_mutex) != 0)
		result = -1;
	return (result);
}
