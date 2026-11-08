/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_coder.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 12:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/08 14:03:53 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	*release_run(t_coder *coder, int *held, int res)
{
	if (res != -1 && *held)
	{
		*held = 0;
		if (release_dongles(coder) == -1)
			res = -1;
	}
	if (res != -1)
		return (NULL);
	set_failed(coder->sim);
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
	int		res;

	sim = coder->sim;
	res = set_compile_start(coder);
	if (res != 0)
		return (res);
	res = print_state(sim, coder->id, STATE_COMPILING);
	if (res != 0)
		return (res);
	res = sleep_ms(sim, sim->config.time_to_compile);
	if (res != 0)
		return (res);
	res = set_compile_count(coder);
	if (res != 0)
		return (res);
	*held = 0;
	return (release_dongles(coder));
}

static int	sleep_rest(t_coder *coder)
{
	t_sim	*sim;
	int		res;

	sim = coder->sim;
	res = print_state(sim, coder->id, STATE_DEBUGGING);
	if (res != 0)
		return (res);
	res = sleep_ms(sim, sim->config.time_to_debug);
	if (res != 0)
		return (res);
	res = print_state(sim, coder->id, STATE_REFACTORING);
	if (res != 0)
		return (res);
	res = sleep_ms(sim, sim->config.time_to_refactor);
	if (res != 0)
		return (res);
	return (is_stopped(sim));
}

static int	wait_single(t_coder *coder)
{
	int	res;

	res = sleep_ms(coder->sim, 1000);
	while (res == 0)
		res = sleep_ms(coder->sim, 1000);
	return (res);
}

void	*run_coder(void *arg)
{
	t_coder	*coder;
	int		held;
	int		taken;
	int		res;

	coder = arg;
	if (coder == NULL || coder->sim == NULL)
		return (NULL);
	held = 0;
	while (1)
	{
		taken = 0;
		res = take_coder_dongles(coder, &taken);
		if (res != 0 || taken == 0)
			return (release_run(coder, &held, res));
		held = 1;
		if (coder->first_dongle == coder->second_dongle)
			return (release_run(coder, &held, wait_single(coder)));
		res = sleep_compile(coder, &held);
		if (res == 0)
			res = sleep_rest(coder);
		if (res != 0)
			return (release_run(coder, &held, res));
	}
}
