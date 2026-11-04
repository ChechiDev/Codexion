/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   release_dongles.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 13:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/04 13:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	pick_dongles(t_coder *coder, t_dongle **low, t_dongle **high)
{
	int	a;
	int	b;

	if (coder == NULL || coder->sim == NULL || coder->sim->dongles == NULL)
		return (-1);
	a = coder->first_dongle;
	b = coder->second_dongle;
	if (a > b)
	{
		a = coder->second_dongle;
		b = coder->first_dongle;
	}
	if (a < 0 || b >= coder->sim->config.number_of_coders)
		return (-1);
	*low = &coder->sim->dongles[a];
	*high = &coder->sim->dongles[b];
	return (0);
}

static int	unlock_pair(t_dongle *low, t_dongle *high)
{
	int	status;

	status = 0;
	if (high != low && pthread_mutex_unlock(&high->mutex) != 0)
		status = -1;
	if (pthread_mutex_unlock(&low->mutex) != 0)
		status = -1;
	return (status);
}

static int	mark_released(t_dongle *low, t_dongle *high, long start_ms)
{
	long	now;

	now = get_elapsed_ms(start_ms);
	if (now < 0)
		return (-1);
	low->available = 1;
	low->released_at = now;
	if (high != low)
	{
		high->available = 1;
		high->released_at = now;
	}
	return (0);
}

static int	broadcast_pair(t_dongle *low, t_dongle *high)
{
	int	status;

	status = 0;
	if (pthread_cond_broadcast(&low->cond) != 0)
		status = -1;
	if (high != low && pthread_cond_broadcast(&high->cond) != 0)
		status = -1;
	return (status);
}

int	release_dongles(t_coder *coder)
{
	t_dongle	*low;
	t_dongle	*high;
	int			status;

	if (pick_dongles(coder, &low, &high) != 0)
		return (-1);
	if (pthread_mutex_lock(&low->mutex) != 0)
		return (-1);
	if (high != low && pthread_mutex_lock(&high->mutex) != 0)
	{
		(void)pthread_mutex_unlock(&low->mutex);
		return (-1);
	}
	if (mark_released(low, high, coder->sim->start_ms) != 0)
	{
		(void)unlock_pair(low, high);
		return (-1);
	}
	status = broadcast_pair(low, high);
	if (unlock_pair(low, high) != 0)
		status = -1;
	return (status);
}
