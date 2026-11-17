/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_number.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/17 18:25:24 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stddef.h>
#include "main.h"

static int	skip_plus(const t_slice *slice, int *i)
{
	if (slice->start[0] != '+')
		return (0);
	if (slice->len < 2 || !is_digit(slice->start[1]))
		return (-2);
	*i = 1;
	return (0);
}

static int	read_digits(const t_slice *slice, int i, int *value)
{
	int	digit;

	*value = 0;
	while (i < slice->len)
	{
		if (!is_digit(slice->start[i]))
			return (-1);
		digit = slice->start[i] - '0';
		if (*value > (INT_MAX - digit) / 10)
			return (-1);
		*value = *value * 10 + digit;
		i++;
	}
	return (0);
}

int	parse_number(const char *str, int *out)
{
	t_slice	slice;
	int		value;
	int		i;

	if (str == NULL || out == NULL)
		return (-1);
	slice = trim_slice(str);
	if (slice.len == 0)
		return (-1);
	i = 0;
	if (skip_plus(&slice, &i) != 0)
		return (-2);
	if (read_digits(&slice, i, &value) != 0)
		return (-1);
	*out = value;
	return (0);
}
