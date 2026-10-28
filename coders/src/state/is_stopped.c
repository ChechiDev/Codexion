/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_stopped.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 17:53:34 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 17:53:34 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	is_stopped(t_sim *sim)
{
	int	stopped;

	if (pthread_mutex_lock(&sim->stop_mutex) != 0)
		return (1);
	stopped = sim->stopped;
	if (pthread_mutex_unlock(&sim->stop_mutex) != 0)
		return (1);
	return (stopped != 0);
}
