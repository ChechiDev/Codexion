/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_simulation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 12:26:47 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/09 12:26:47 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	join_simulation(t_sim *sim)
{
	int	failed;
	int	freed;

	if (join_threads(sim, sim->config.number_of_coders) != 0)
		return (-1);
	failed = sim->failed;
	freed = free_sim(sim);
	if (failed != 0 || freed != 0)
		return (-1);
	return (0);
}

int	run_simulation(t_sim *sim, const t_config *config)
{
	int	started;

	if (sim == NULL || config == NULL)
		return (-1);
	if (config->number_of_compiles_required == 0)
		return (0);
	if (init_simulation(sim, config) != 0)
		return (-1);
	started = start_threads(sim);
	if (started == -1)
	{
		free_sim(sim);
		return (-1);
	}
	if (started != 0)
		return (-1);
	return (join_simulation(sim));
}
