/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim_slice.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 18:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/27 18:31:45 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <string.h>
#include "main.h"

t_slice	trim_slice(const char *str)
{
	t_slice	slice;
	int		end;

	slice.start = NULL;
	slice.len = 0;
	if (str == NULL)
		return (slice);
	while (is_space(*str))
		str++;
	slice.start = str;
	end = (int)strlen(str);
	while (end > 0 && is_space(str[end - 1]))
		end--;
	slice.len = end;
	return (slice);
}
