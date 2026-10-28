/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sleep_ms.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:18:36 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 18:18:36 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "main.h"

static useconds_t	get_slice_us(long remaining_ms)
{
	if (remaining_ms > SLEEP_SLICE_US)
		return (SLEEP_SLICE_US);
	if (remaining_ms * 1000 > SLEEP_SLICE_US)
		return (SLEEP_SLICE_US);
	return ((useconds_t)(remaining_ms * 1000));
}

int	sleep_ms(t_sim *sim, long ms)
{
	long	start;
	long	elapsed;

	if (ms <= 0)
		return (0);
	start = get_time_ms();
	if (start < 0)
		return (-1);
	while (1)
	{
		if (is_stopped(sim))
			return (-1);
		elapsed = get_elapsed_ms(start);
		if (elapsed < 0)
			return (-1);
		if (elapsed >= ms)
			return (0);
		usleep(get_slice_us(ms - elapsed));
	}
}
