/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_dongle_ready.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 21:15:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/03 21:15:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	is_dongle_ready(const t_dongle *dongle, int coder, long now,
		int cooldown)
{
	t_request	head;

	if (dongle == NULL || coder < 1 || now < 0 || cooldown < 0
		|| dongle->released_at < -1)
		return (0);
	if (get_head_request(&dongle->heap, &head) != 0 || head.coder != coder)
		return (0);
	if (dongle->available != 1)
		return (0);
	if (dongle->released_at == -1)
		return (1);
	if (now < dongle->released_at)
		return (0);
	return (now - dongle->released_at >= cooldown);
}
