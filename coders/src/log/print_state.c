/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 10:59:48 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/29 10:59:48 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "main.h"

static const char	*get_state_text(t_state state)
{
	static const char	*texts[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring"};

	if ((int)state < 0 || (int)state > STATE_REFACTORING)
		return (NULL);
	return (texts[state]);
}

static int	read_stopped(t_sim *sim, int *stopped)
{
	if (pthread_mutex_lock(&sim->stop_mutex) != 0)
		return (-1);
	*stopped = sim->stopped;
	if (pthread_mutex_unlock(&sim->stop_mutex) != 0)
		return (-1);
	return (0);
}

static int	write_line(t_sim *sim, int coder, const char *text)
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

/*
** Writes "<ts> <coder> <text>\n" to stdout with one unbuffered write, under
** log_mutex. Returns 0 on success, -1 if rejected (bad state or coder,
** stopped) or on any failure. No retries.
*/
int	print_state(t_sim *sim, int coder, t_state state)
{
	const char	*text;
	int			stopped;
	int			status;

	text = get_state_text(state);
	if (text == NULL)
		return (-1);
	if (pthread_mutex_lock(&sim->log_mutex) != 0)
		return (-1);
	stopped = 0;
	status = read_stopped(sim, &stopped);
	if (status == 0 && stopped)
		status = -1;
	if (status == 0)
		status = write_line(sim, coder, text);
	if (pthread_mutex_unlock(&sim->log_mutex) != 0)
		status = -1;
	return (status);
}
