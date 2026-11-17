/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_log_line.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:58:49 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 18:58:49 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "main.h"

static int	has_line_break(const char *text)
{
	while (*text != '\0')
	{
		if (*text == '\n' || *text == '\r')
			return (1);
		text++;
	}
	return (0);
}

static int	is_valid_log_input(char *line, long ts, int coder, const char *text)
{
	size_t	length;

	if (line == NULL || text == NULL || ts < 0 || coder < 1)
		return (0);
	length = strlen(text);
	if (length < 1 || length > LOG_TEXT_MAX)
		return (0);
	return (!has_line_break(text));
}

static int	build_log_text(char *dest, const char *text)
{
	int	index;

	index = 0;
	while (text[index] != '\0')
	{
		dest[index] = text[index];
		index++;
	}
	return (index);
}

int	build_log_line(char *line, long ts, int coder, const char *text)
{
	int	pos;

	if (!is_valid_log_input(line, ts, coder, text))
		return (-1);
	pos = build_number_text(ts, line);
	line[pos++] = ' ';
	pos += build_number_text(coder, line + pos);
	line[pos++] = ' ';
	pos += build_log_text(line + pos, text);
	line[pos++] = '\n';
	return (pos);
}
