/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_threads.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/08 18:08:23 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/08 18:08:23 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	create_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, run_coder,
				&sim->coders[i]) != 0)
			return (i);
		i++;
	}
	return (i);
}

static int	recover_start(t_sim *sim, int created)
{
	set_failed(sim);
	set_stopped(sim);
	wake_dongles(sim);
	if (join_threads(sim, created) != 0)
		return (-2);
	return (-1);
}

int	start_threads(t_sim *sim)
{
	long	now;
	int		created;

	if (sim == NULL)
		return (-1);
	now = get_time_ms();
	if (now < 0)
		return (-1);
	sim->start_ms = now;
	if (pthread_create(&sim->monitor, NULL, run_monitor, sim) != 0)
	{
		set_failed(sim);
		return (-1);
	}
	created = create_coders(sim);
	if (created < sim->config.number_of_coders)
		return (recover_start(sim, created));
	return (0);
}
