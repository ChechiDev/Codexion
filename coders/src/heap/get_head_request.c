/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_head_request.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 13:31:25 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/02 13:31:25 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	get_head_request(const t_heap *heap, t_request *out)
{
	if (heap == NULL || out == NULL || heap->size == 0)
		return (-1);
	*out = heap->items[0];
	return (0);
}
