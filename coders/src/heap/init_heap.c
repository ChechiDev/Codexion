/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_heap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 11:32:49 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/30 11:32:49 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include <stdint.h>
#include <stdlib.h>

int	init_heap(t_heap *heap, int capacity)
{
	t_request	*items;
	size_t		bytes;

	if (heap == NULL || capacity < 1)
		return (-1);
	if ((size_t)capacity > SIZE_MAX / sizeof(t_request))
		return (-1);
	bytes = (size_t)capacity * sizeof(t_request);
	items = malloc(bytes);
	if (items == NULL)
		return (-1);
	heap->items = items;
	heap->size = 0;
	heap->capacity = capacity;
	return (0);
}
