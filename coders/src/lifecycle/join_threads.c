/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join_threads.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/08 12:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/12 17:30:41 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	join_threads(t_sim *sim, int coders)
{
	int	status;
	int	i;

	if (sim == NULL || coders < 0 || coders > sim->config.number_of_coders)
		return (-1);
	status = 0;
	i = 0;
	while (i < coders)
	{
		if (pthread_join(sim->coders[i].thread, NULL) != 0)
			status = -1;
		i++;
	}
	if (pthread_join(sim->monitor, NULL) != 0)
		status = -1;
	return (status);
}
