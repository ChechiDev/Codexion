/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_single_dongle.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 14:10:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/04 14:10:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#include <errno.h>

static int	get_single_dongle(t_coder *coder, int *taken, t_dongle **out)
{
	if (coder == NULL || taken == NULL || coder->sim == NULL
		|| coder->sim->dongles == NULL)
		return (-1);
	if (coder->first_dongle != coder->second_dongle
		|| coder->first_dongle < 0
		|| coder->first_dongle >= coder->sim->config.number_of_coders)
		return (-1);
	*out = &coder->sim->dongles[coder->first_dongle];
	return (0);
}

/* 1: queued, held; -1: nothing queued, released; -2: fatal, unlock failed. */
static int	push_single_request(t_coder *coder, t_dongle *dongle)
{
	t_request	req;

	if (build_request(coder, &req) != 0)
		return (-1);
	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (-1);
	if (push_request(&dongle->heap, &req) != 0)
	{
		if (pthread_mutex_unlock(&dongle->mutex) != 0)
			return (-2);
		return (-1);
	}
	return (1);
}

/* Holding the mutex: 1 waited, -1 fatal (mutex still treated as held). */
static int	wait_for_single_dongle(t_dongle *dongle)
{
	struct timespec	deadline;
	int				status;

	if (get_deadline(1, &deadline) != 0)
		return (-1);
	status = pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &deadline);
	if (status != 0 && status != ETIMEDOUT)
		return (-1);
	return (1);
}

/* Holding the mutex: 0 done (out set), 1 waited, -1 fatal. */
static int	take_single_step(t_coder *coder, t_dongle *dongle, int *out)
{
	long		now;
	t_request	req;

	*out = 0;
	if (is_stopped(coder->sim))
		return (0);
	now = get_elapsed_ms(coder->sim->start_ms);
	if (now < 0)
		return (-1);
	if (!is_dongle_ready(dongle, coder->id, now,
			coder->sim->config.dongle_cooldown))
		return (wait_for_single_dongle(dongle));
	if (pop_request(&dongle->heap, &req) != 0)
		return (-1);
	dongle->available = 0;
	*out = 1;
	return (0);
}

int	take_single_dongle(t_coder *coder, int *taken)
{
	t_dongle	*dongle;
	int			status;
	int			out;

	if (get_single_dongle(coder, taken, &dongle) != 0)
		return (-1);
	status = push_single_request(coder, dongle);
	if (status == -1)
		return (-1);
	while (status == 1)
		status = take_single_step(coder, dongle, &out);
	if (status != -2 && pthread_mutex_unlock(&dongle->mutex) != 0)
		status = -1;
	if (status == 0)
	{
		*taken = out;
		return (0);
	}
	(void)set_stopped(coder->sim);
	(void)wake_dongles(coder->sim);
	return (-1);
}
