/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_burned_out.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 13:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/05 13:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	get_burned_out(t_coder *coder, int *burned)
{
	long	now;
	long	last;
	int		result;

	if (coder == NULL || coder->sim == NULL || burned == NULL)
		return (-1);
	if (pthread_mutex_lock(&coder->mutex) != 0)
		return (-1);
	last = coder->last_compile_start;
	now = get_elapsed_ms(coder->sim->start_ms);
	if (now < 0)
	{
		pthread_mutex_unlock(&coder->mutex);
		return (-1);
	}
	result = last <= now - coder->sim->config.time_to_burnout;
	if (pthread_mutex_unlock(&coder->mutex) != 0)
		return (-1);
	*burned = result;
	return (0);
}
