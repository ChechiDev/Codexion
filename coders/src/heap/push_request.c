/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_request.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/12 17:30:41 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	sift_up(t_request *items, int i)
{
	int			parent;
	t_request	tmp;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!is_request_before(&items[i], &items[parent]))
			return ;
		tmp = items[i];
		items[i] = items[parent];
		items[parent] = tmp;
		i = parent;
	}
}

int	push_request(t_heap *heap, const t_request *req)
{
	if (heap == NULL || req == NULL || heap->size >= heap->capacity)
		return (-1);
	heap->items[heap->size] = *req;
	heap->size++;
	sift_up(heap->items, heap->size - 1);
	return (0);
}
