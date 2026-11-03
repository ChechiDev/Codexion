/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_dongle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 18:30:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/02 18:30:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	destroy_dongle(t_dongle *dongle)
{
	int	result;

	if (dongle == NULL)
		return (-1);
	result = 0;
	free_heap(&dongle->heap);
	if (pthread_cond_destroy(&dongle->cond) != 0)
		result = -1;
	if (pthread_mutex_destroy(&dongle->mutex) != 0)
		result = -1;
	return (result);
}
