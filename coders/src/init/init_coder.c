/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coder.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 12:03:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 12:03:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	init_coder(t_coder *coder, t_sim *sim, int seat, int n)
{
	int	first;
	int	second;

	if (coder == NULL || sim == NULL)
		return (-1);
	if (get_dongle_pair(seat, n, &first, &second) != 0)
		return (-1);
	if (pthread_mutex_init(&coder->mutex, NULL) != 0)
		return (-1);
	coder->id = seat + 1;
	coder->first_dongle = first;
	coder->second_dongle = second;
	coder->last_compile_start = 0;
	coder->compile_count = 0;
	coder->sim = sim;
	return (0);
}
