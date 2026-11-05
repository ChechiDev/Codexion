/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_compile_count.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 18:51:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/04 18:51:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include <limits.h>

int	set_compile_count(t_coder *coder)
{
	int	status;

	if (coder == NULL)
		return (-1);
	if (pthread_mutex_lock(&coder->mutex) != 0)
		return (-1);
	status = 0;
	if (coder->compile_count < 0 || coder->compile_count >= INT_MAX)
		status = -1;
	else
		coder->compile_count++;
	if (pthread_mutex_unlock(&coder->mutex) != 0)
		status = -1;
	return (status);
}
