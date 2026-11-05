/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_coder.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 12:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	*set_failure(t_coder *coder, int *held)
{
	if (*held)
	{
		*held = 0;
		release_dongles(coder);
	}
	set_stopped(coder->sim);
	wake_dongles(coder->sim);
	return (NULL);
}

static int	sleep_compile(t_coder *coder, int *held)
{
	t_sim	*sim;

	sim = coder->sim;
	if (set_compile_start(coder) != 0)
		return (-1);
	if (print_state(sim, coder->id, STATE_COMPILING) != 0)
		return (-1);
	if (sleep_ms(sim, sim->config.time_to_compile) != 0)
		return (-1);
	if (set_compile_count(coder) != 0)
		return (-1);
	*held = 0;
	return (release_dongles(coder));
}

static int	sleep_rest(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	if (print_state(sim, coder->id, STATE_DEBUGGING) != 0)
		return (-1);
	if (sleep_ms(sim, sim->config.time_to_debug) != 0)
		return (-1);
	if (print_state(sim, coder->id, STATE_REFACTORING) != 0)
		return (-1);
	return (sleep_ms(sim, sim->config.time_to_refactor));
}

static void	*wait_single(t_coder *coder, int *held)
{
	while (sleep_ms(coder->sim, 1000) == 0)
		;
	*held = 0;
	if (release_dongles(coder) != 0)
		return (set_failure(coder, held));
	if (is_stopped(coder->sim) == 0)
		return (set_failure(coder, held));
	return (NULL);
}

void	*run_coder(void *arg)
{
	t_coder	*coder;
	int		held;
	int		taken;

	coder = arg;
	if (coder == NULL || coder->sim == NULL)
		return (NULL);
	held = 0;
	while (1)
	{
		taken = 0;
		if (take_coder_dongles(coder, &taken) != 0)
			return (set_failure(coder, &held));
		if (taken == 0)
			return (NULL);
		held = 1;
		if (coder->first_dongle == coder->second_dongle)
			return (wait_single(coder, &held));
		if (sleep_compile(coder, &held) != 0 || sleep_rest(coder) != 0)
			return (set_failure(coder, &held));
		if (is_stopped(coder->sim))
			return (NULL);
	}
}
