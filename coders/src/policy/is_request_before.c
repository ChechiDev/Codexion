/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_request_before.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 18:19:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/29 18:19:00 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	is_request_before(const t_request *a, const t_request *b)
{
	if (a == NULL || b == NULL)
		return (0);
	if (a->key < b->key)
		return (1);
	if (a->key > b->key)
		return (0);
	return (a->ticket < b->ticket);
}
