/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 16:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 17:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (-1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		return (-1);
	}
	if (pthread_mutex_init(&sim->ticket_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (-1);
	}
	return (0);
}

int	init_simulation(t_sim *sim, const t_config *config)
{
	if (sim == NULL || config == NULL || config->number_of_coders < 1)
		return (-1);
	sim->config = *config;
	sim->start_ms = 0;
	sim->stopped = 0;
	sim->next_ticket = 0;
	sim->dongles = NULL;
	sim->coders = NULL;
	if (init_mutexes(sim) != 0)
		return (-1);
	if (init_dongles(&sim->dongles, config->number_of_coders) != 0
		|| init_coders(&sim->coders, sim, config->number_of_coders) != 0)
	{
		free_sim(sim);
		return (-1);
	}
	return (0);
}
