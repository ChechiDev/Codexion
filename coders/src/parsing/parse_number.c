/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_number.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/27 18:01:46 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stddef.h>
#include "main.h"

int	parse_number(const char *str, int *out)
{
	t_slice	slice;
	int		value;
	int		digit;
	int		i;

	if (str == NULL || out == NULL)
		return (-1);
	slice = trim_slice(str);
	if (slice.len == 0)
		return (-1);
	value = 0;
	i = 0;
	while (i < slice.len)
	{
		if (slice.start[i] < '0' || slice.start[i] > '9')
			return (-1);
		digit = slice.start[i] - '0';
		if (value > (INT_MAX - digit) / 10)
			return (-1);
		value = value * 10 + digit;
		i++;
	}
	*out = value;
	return (0);
}
