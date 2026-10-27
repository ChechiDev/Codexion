/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_number.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/27 11:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stddef.h>
#include "main.h"

int	parse_number(const char *str, int *out)
{
	t_span	span;
	int		value;
	int		digit;
	int		i;

	if (str == NULL || out == NULL)
		return (-1);
	span = trim_span(str);
	if (span.len == 0)
		return (-1);
	value = 0;
	i = 0;
	while (i < span.len)
	{
		if (span.start[i] < '0' || span.start[i] > '9')
			return (-1);
		digit = span.start[i] - '0';
		if (value > (INT_MAX - digit) / 10)
			return (-1);
		value = value * 10 + digit;
		i++;
	}
	*out = value;
	return (0);
}
