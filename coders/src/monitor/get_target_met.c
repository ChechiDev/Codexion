/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_target_met.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 13:30:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/05 13:30:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	get_count_reached(t_coder *coder, int target, int *reached)
{
	int	result;

	if (pthread_mutex_lock(&coder->mutex) != 0)
		return (-1);
	result = coder->compile_count >= target;
	if (pthread_mutex_unlock(&coder->mutex) != 0)
		return (-1);
	*reached = result;
	return (0);
}

int	get_target_met(t_sim *sim, int *met)
{
	int	reached;
	int	i;

	if (sim == NULL || met == NULL || sim->coders == NULL
		|| sim->config.number_of_coders < 1)
		return (-1);
	reached = 1;
	i = 0;
	while (reached && sim->config.number_of_compiles_required > 0
		&& i < sim->config.number_of_coders)
	{
		if (get_count_reached(&sim->coders[i],
				sim->config.number_of_compiles_required, &reached) != 0)
			return (-1);
		i++;
	}
	*met = reached;
	return (0);
}
