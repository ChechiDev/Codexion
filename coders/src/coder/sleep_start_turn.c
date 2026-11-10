/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sleep_start_turn.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 19:41:30 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/09 19:41:30 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	sleep_start_turn(t_coder *coder)
{
	if (coder == NULL || coder->sim == NULL)
		return (-1);
	if (coder->id % 2 != 0)
		return (0);
	return (sleep_ms(coder->sim, START_TURN_MS));
}
