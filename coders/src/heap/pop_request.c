/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pop_request.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 15:31:22 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/02 15:31:22 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	sift_down(t_heap *heap)
{
	int			i;
	int			child;
	t_request	tmp;

	i = 0;
	while (i < heap->size / 2)
	{
		child = 2 * i + 1;
		if (child + 1 < heap->size && is_request_before(
				&heap->items[child + 1], &heap->items[child]))
			child++;
		if (!is_request_before(&heap->items[child], &heap->items[i]))
			return ;
		tmp = heap->items[i];
		heap->items[i] = heap->items[child];
		heap->items[child] = tmp;
		i = child;
	}
}

int	pop_request(t_heap *heap, t_request *out)
{
	if (heap == NULL || out == NULL || heap->size == 0)
		return (-1);
	*out = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		sift_down(heap);
	}
	return (0);
}
