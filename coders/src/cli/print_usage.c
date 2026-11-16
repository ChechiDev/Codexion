/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_usage.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 17:05:14 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/16 17:11:02 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include "main.h"

void	print_usage(void)
{
	const char	*text;

	text = "\nUsage: ./codexion coders burnout compile debug refactor "
		"compiles cooldown fifo|edf\n\n"
		"  e.g. ./codexion 4 800 200 200 200 3 50 edf\n";
	if (write(2, text, strlen(text)) < 0)
		return ;
}
