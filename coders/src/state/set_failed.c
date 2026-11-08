/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_failed.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/08 11:37:12 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/08 11:37:12 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	set_failed(t_sim *sim)
{
	if (pthread_mutex_lock(&sim->stop_mutex) != 0)
		return (-1);
	sim->failed = 1;
	if (pthread_mutex_unlock(&sim->stop_mutex) != 0)
		return (-1);
	return (0);
}
