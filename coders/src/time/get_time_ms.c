/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_time_ms.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 17:23:45 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 17:23:45 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <sys/time.h>
#include "main.h"

long	get_time_ms(void)
{
	struct timeval	now;

	if (gettimeofday(&now, NULL) != 0)
		return (-1);
	return ((long)now.tv_sec * 1000 + now.tv_usec / 1000);
}
