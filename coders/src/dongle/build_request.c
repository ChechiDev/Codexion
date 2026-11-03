/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_request.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:40:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 19:40:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	get_last_start(t_coder *coder, long *out)
{
	if (pthread_mutex_lock(&coder->mutex) != 0)
		return (-1);
	*out = coder->last_compile_start;
	if (pthread_mutex_unlock(&coder->mutex) != 0)
		return (-1);
	return (0);
}

int	build_request(t_coder *coder, t_request *out)
{
	t_request	req;
	long		last_start;

	if (coder == NULL || out == NULL || coder->sim == NULL)
		return (-1);
	if (get_last_start(coder, &last_start) != 0)
		return (-1);
	if (take_ticket(coder->sim, &req.ticket) != 0)
		return (-1);
	req.key = get_request_key(coder->sim->config.mode, req.ticket,
			last_start, coder->sim->config.time_to_burnout);
	if (req.key == -1)
		return (-1);
	req.coder = coder->id;
	*out = req;
	return (0);
}
