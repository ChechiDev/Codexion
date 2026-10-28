/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_mode.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 11:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 11:00:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

#include "main.h"

int	parse_mode(const char *str, t_mode *out)
{
	if (str == NULL || out == NULL)
		return (-1);
	if (strcmp(str, "fifo") == 0)
		*out = MODE_FIFO;
	else if (strcmp(str, "edf") == 0)
		*out = MODE_EDF;
	else
		return (-1);
	return (0);
}
