/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_coder.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 12:30:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 12:30:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	destroy_coder(t_coder *coder)
{
	if (coder == NULL)
		return (-1);
	if (pthread_mutex_destroy(&coder->mutex) != 0)
		return (-1);
	return (0);
}
