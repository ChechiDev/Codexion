/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_for_dongles.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 12:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/12 17:30:41 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#include <errno.h>

static int	release_pair(t_dongle *first, t_dongle *second, int hold_second)
{
	if (hold_second)
		(void)pthread_mutex_unlock(&second->mutex);
	(void)pthread_mutex_unlock(&first->mutex);
	return (-1);
}

int	wait_for_dongles(t_coder *coder)
{
	t_dongle		*first;
	t_dongle		*second;
	struct timespec	deadline;
	int				status;

	if (coder == NULL || coder->sim == NULL || coder->sim->dongles == NULL
		|| coder->first_dongle >= coder->second_dongle)
		return (-1);
	first = &coder->sim->dongles[coder->first_dongle];
	second = &coder->sim->dongles[coder->second_dongle];
	if (get_deadline(1, &deadline) != 0)
		return (release_pair(first, second, 1));
	if (pthread_mutex_unlock(&second->mutex) != 0)
		return (release_pair(first, second, 1));
	status = pthread_cond_timedwait(&first->cond, &first->mutex, &deadline);
	if (status != 0 && status != ETIMEDOUT)
		return (release_pair(first, second, 0));
	if (pthread_mutex_lock(&second->mutex) != 0)
		return (release_pair(first, second, 0));
	return (0);
}
