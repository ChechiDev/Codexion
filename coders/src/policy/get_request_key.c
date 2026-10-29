/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_request_key.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 18:37:25 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/29 18:37:25 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include "main.h"

long	get_request_key(t_mode mode, long ticket, long last_compile_start,
		int time_to_burnout)
{
	if (ticket < 0)
		return (-1);
	if (mode == MODE_FIFO)
		return (ticket);
	if (mode != MODE_EDF)
		return (-1);
	if (last_compile_start < 0 || time_to_burnout < 1)
		return (-1);
	if (last_compile_start > LONG_MAX - time_to_burnout)
		return (-1);
	return (last_compile_start + time_to_burnout);
}
