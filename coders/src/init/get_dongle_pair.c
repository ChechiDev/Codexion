/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_dongle_pair.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 17:51:31 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/02 17:51:31 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	get_dongle_pair(int seat, int n, int *first, int *second)
{
	int	right;

	if (first == NULL || second == NULL || n < 1 || seat < 0 || seat >= n)
		return (-1);
	if (seat == n - 1)
		right = 0;
	else
		right = seat + 1;
	if (seat < right)
	{
		*first = seat;
		*second = right;
	}
	else
	{
		*first = right;
		*second = seat;
	}
	return (0);
}
