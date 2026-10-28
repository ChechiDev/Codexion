/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_elapsed_ms.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 17:38:57 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 17:38:57 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

long	get_elapsed_ms(long start_ms)
{
	long	now;

	now = get_time_ms();
	if (now < 0)
		return (-1);
	if (now < start_ms)
		return (0);
	return (now - start_ms);
}
