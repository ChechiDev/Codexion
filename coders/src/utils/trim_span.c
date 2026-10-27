/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim_span.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 17:11:07 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/27 18:08:19 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

t_span	trim_span(const char *str)
{
	t_span	span;
	t_slice	slice;

	slice = trim_slice(str);
	span.start = slice.start;
	span.len = slice.len;
	return (span);
}
