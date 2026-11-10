/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_arg_error.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 10:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/10 12:58:18 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

void	print_arg_error(int position)
{
	static const char	*names[] = {"number_of_coders", "time_to_burnout",
		"time_to_compile", "time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown"};
	const char			*text;

	if (position >= 1 && position <= 7)
		fprintf(stderr, "Error: %s must be an integer from %d to %d\n",
			names[position - 1], get_arg_minimum(position), INT_MAX);
	else if (position == 8)
	{
		text = "Error: scheduler must be fifo or edf\n";
		if (write(2, text, strlen(text)) < 0)
			return ;
	}
}
