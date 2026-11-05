/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_coder_dongles.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 11:15:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/05 11:15:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	print_taken(t_coder *coder)
{
	int	count;
	int	i;

	count = 2;
	if (coder->first_dongle == coder->second_dongle)
		count = 1;
	i = 0;
	while (i < count)
	{
		if (print_state(coder->sim, coder->id, STATE_TAKEN_DONGLE) != 0)
			return (-1);
		i++;
	}
	return (0);
}

static int	release_taken(t_coder *coder, int *taken)
{
	if (release_dongles(coder) != 0)
		return (-1);
	if (is_stopped(coder->sim) == 0)
		return (-1);
	*taken = 0;
	return (0);
}

static int	take_pair(t_coder *coder, int *got)
{
	if (coder->first_dongle == coder->second_dongle)
		return (take_single_dongle(coder, got));
	return (take_dongles(coder, got));
}

int	take_coder_dongles(t_coder *coder, int *taken)
{
	int	got;

	if (coder == NULL || taken == NULL || coder->sim == NULL)
		return (-1);
	got = 0;
	if (take_pair(coder, &got) != 0)
		return (-1);
	if (got == 0)
	{
		*taken = 0;
		return (0);
	}
	if (print_taken(coder) != 0)
		return (release_taken(coder, taken));
	*taken = 1;
	return (0);
}
