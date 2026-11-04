/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_deadline.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 22:03:26 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 22:03:26 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stddef.h>
#include <sys/time.h>

#include "main.h"

int	get_deadline(long ms, struct timespec *out)
{
	struct timeval	now;
	long			nsec;
	long			secs;

	if (out == NULL || ms < 0 || gettimeofday(&now, NULL) != 0)
		return (-1);
	nsec = now.tv_usec * 1000L + (ms % 1000) * 1000000L;
	secs = ms / 1000 + nsec / 1000000000L;
	if ((long)now.tv_sec > LONG_MAX - secs)
		return (-1);
	out->tv_sec = (time_t)((long)now.tv_sec + secs);
	out->tv_nsec = nsec % 1000000000L;
	return (0);
}
