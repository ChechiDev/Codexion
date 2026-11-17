/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_compile_start.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 18:25:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/12 17:30:41 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	set_compile_start(t_coder *coder)
{
	long	now;
	int		status;

	if (coder == NULL || coder->sim == NULL)
		return (-1);
	if (pthread_mutex_lock(&coder->mutex) != 0)
		return (-1);
	now = get_elapsed_ms(coder->sim->start_ms);
	if (now < 0)
	{
		pthread_mutex_unlock(&coder->mutex);
		return (-1);
	}
	status = 1;
	if (coder->last_compile_start > now - coder->sim->config.time_to_burnout)
	{
		coder->last_compile_start = now;
		status = 0;
	}
	if (pthread_mutex_unlock(&coder->mutex) != 0)
		status = -1;
	return (status);
}
