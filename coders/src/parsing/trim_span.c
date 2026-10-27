/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim_span.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 10:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/27 10:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "main.h"

static int	is_space(char c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
}

t_span	trim_span(const char *str)
{
	t_span	span;
	int		end;

	span.start = NULL;
	span.len = 0;
	if (str == NULL)
		return (span);
	while (is_space(*str))
		str++;
	span.start = str;
	end = 0;
	while (str[end] != '\0')
		end++;
	while (end > 0 && is_space(str[end - 1]))
		end--;
	span.len = end;
	return (span);
}
