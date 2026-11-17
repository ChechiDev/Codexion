/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_log_line.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 12:23:12 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/29 12:23:12 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "main.h"

int	print_log_line(t_sim *sim, int coder, const char *text)
{
	char	line[LOG_LINE_MAX];
	long	ts;
	int		len;

	ts = get_elapsed_ms(sim->start_ms);
	if (ts == -1)
		return (-1);
	len = build_log_line(line, ts, coder, text);
	if (len == -1)
		return (-1);
	if (write(1, line, len) != len)
		return (-1);
	return (0);
}
