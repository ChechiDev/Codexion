/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 13:34:41 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/04 13:52:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	get_dongles(t_coder *coder, int *taken, t_dongle **first,
		t_dongle **second)
{
	if (coder == NULL || taken == NULL || coder->sim == NULL
		|| coder->sim->dongles == NULL)
		return (-1);
	if (coder->first_dongle < 0 || coder->first_dongle >= coder->second_dongle
		|| coder->second_dongle >= coder->sim->config.number_of_coders)
		return (-1);
	*first = &coder->sim->dongles[coder->first_dongle];
	*second = &coder->sim->dongles[coder->second_dongle];
	return (0);
}

static int	release_pair(t_dongle *first, t_dongle *second)
{
	int	status;

	status = 0;
	if (pthread_mutex_unlock(&second->mutex) != 0)
		status = -1;
	if (pthread_mutex_unlock(&first->mutex) != 0)
		status = -1;
	return (status);
}

static int	push_requests(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_request	req;

	if (build_request(coder, &req) != 0)
		return (-1);
	if (pthread_mutex_lock(&first->mutex) != 0)
		return (-1);
	if (pthread_mutex_lock(&second->mutex) != 0)
	{
		if (pthread_mutex_unlock(&first->mutex) != 0)
			return (-2);
		return (-1);
	}
	if (push_request(&first->heap, &req) != 0)
	{
		if (release_pair(first, second) != 0)
			return (-2);
		return (-1);
	}
	if (push_request(&second->heap, &req) != 0)
	{
		(void)release_pair(first, second);
		return (-2);
	}
	return (1);
}

static int	take_pair(t_coder *coder, t_dongle *first, t_dongle *second,
		int *out)
{
	long		now;
	int			cd;
	t_request	req;

	*out = 0;
	if (is_stopped(coder->sim))
		return (0);
	now = get_elapsed_ms(coder->sim->start_ms);
	cd = coder->sim->config.dongle_cooldown;
	if (now < 0)
		return (-1);
	if (!is_dongle_ready(first, coder->id, now, cd)
		|| !is_dongle_ready(second, coder->id, now, cd))
	{
		if (wait_for_dongles(coder) != 0)
			return (-2);
		return (1);
	}
	if (pop_request(&first->heap, &req) != 0
		|| pop_request(&second->heap, &req) != 0)
		return (-1);
	first->available = 0;
	second->available = 0;
	*out = 1;
	return (0);
}

int	take_dongles(t_coder *coder, int *taken)
{
	t_dongle	*first;
	t_dongle	*second;
	int			status;
	int			out;

	if (get_dongles(coder, taken, &first, &second) != 0)
		return (-1);
	status = push_requests(coder, first, second);
	if (status == -1)
		return (-1);
	while (status == 1)
		status = take_pair(coder, first, second, &out);
	if (status != -2 && release_pair(first, second) != 0)
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
