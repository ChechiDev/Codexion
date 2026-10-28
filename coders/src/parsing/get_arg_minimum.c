/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_arg_minimum.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 10:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 10:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	get_arg_minimum(int position)
{
	if (position == 1 || position == 2)
		return (1);
	if (position >= 3 && position <= 7)
		return (0);
	return (-1);
}
