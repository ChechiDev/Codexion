/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_monitor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 14:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/12 16:58:26 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include <unistd.h>

#define KEEP_SCANNING 0
#define LEAVE_SILENT 1
#define LEAVE_STOP 2
#define LEAVE_BURNED 3
#define LEAVE_ERROR 4

static int	get_burned_index(t_sim *sim, int *index)
{
	int	burned;
	int	i;

	*index = -1;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		burned = 0;
		if (get_burned_out(&sim->coders[i], &burned) != 0)
			return (-1);
		if (burned)
		{
			*index = i;
			return (0);
		}
		i++;
	}
	return (0);
}

static int	handle_burnout(t_sim *sim)
{
	int	index;

	if (get_burned_index(sim, &index) != 0)
		return (LEAVE_ERROR);
	if (index < 0)
		return (KEEP_SCANNING);
	if (print_burnout(sim, sim->coders[index].id) != 0)
		return (LEAVE_ERROR);
	return (LEAVE_BURNED);
}

static int	scan_step(t_sim *sim)
{
	int	met;
	int	result;

	if (is_stopped(sim))
		return (LEAVE_SILENT);
	result = handle_burnout(sim);
	if (result != KEEP_SCANNING)
		return (result);
	if (get_target_met(sim, &met) != 0)
		return (LEAVE_ERROR);
	if (met)
		return (LEAVE_STOP);
	if (usleep(1000) != 0)
		return (LEAVE_ERROR);
	return (KEEP_SCANNING);
}

static void	set_monitor_end(t_sim *sim, int result)
{
	int	bad;

	if (result == LEAVE_SILENT)
		return ;
	bad = 0;
	if (result == LEAVE_ERROR)
	{
		set_failed(sim);
		set_stopped(sim);
		wake_dongles(sim);
		return ;
	}
	if (result == LEAVE_STOP && set_stopped(sim) != 0)
		bad = 1;
	if (wake_dongles(sim) != 0)
		bad = 1;
	if (bad)
		set_failed(sim);
}

void	*run_monitor(void *arg)
{
	t_sim	*sim;
	int		result;

	sim = arg;
	if (sim == NULL)
		return (NULL);
	result = KEEP_SCANNING;
	while (result == KEEP_SCANNING)
		result = scan_step(sim);
	set_monitor_end(sim, result);
	return (NULL);
}
