/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_run_error.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 13:16:54 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/10 12:58:18 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include "main.h"

void	print_run_error(void)
{
	const char	*text;

	text = "Error: simulation failed\n";
	if (write(2, text, strlen(text)) < 0)
		return ;
}
