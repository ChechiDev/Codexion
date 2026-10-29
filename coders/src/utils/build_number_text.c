/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_number_text.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:38:26 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 18:38:26 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	get_digit_count(long n)
{
	int	count;

	count = 1;
	while (n >= 10)
	{
		n /= 10;
		count++;
	}
	return (count);
}

/*
** Writes the decimal digits of n (>= 0) at the start of buf, which must hold
** NUMBER_TEXT_MAX bytes. No sign, no '\0'. Returns the digit count, or -1
** (writing nothing) if n < 0 or buf is NULL.
*/
int	build_number_text(long n, char *buf)
{
	int	count;
	int	index;

	if (n < 0 || buf == NULL)
		return (-1);
	count = get_digit_count(n);
	index = count;
	while (index > 0)
	{
		index--;
		buf[index] = (char)('0' + n % 10);
		n /= 10;
	}
	return (count);
}
