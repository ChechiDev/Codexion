/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 18:10:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/02 18:10:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	init_dongle(t_dongle *dongle)
{
	if (dongle == NULL)
		return (-1);
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (-1);
	if (pthread_cond_init(&dongle->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->mutex);
		return (-1);
	}
	if (init_heap(&dongle->heap, 2) != 0)
	{
		pthread_cond_destroy(&dongle->cond);
		pthread_mutex_destroy(&dongle->mutex);
		return (-1);
	}
	dongle->available = 1;
	dongle->released_at = -1;
	return (0);
}
