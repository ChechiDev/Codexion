/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim_slice.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 18:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/27 18:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

t_slice	trim_slice(const char *str)
{
	t_span	span;
	t_slice	slice;

	span = trim_span(str);
	slice.start = span.start;
	slice.len = span.len;
	return (slice);
}
