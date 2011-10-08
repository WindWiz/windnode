/* Copyright (C) 2010-2011 Magnus Olsson
 * 
 * This file is part of Windnode
 * Windnode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Windnode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Windnode.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include "ringbuf.h"
#include "errno.h"

void rb_init(struct ringbuf *rb, uint8_t * buf, uint8_t capacity)
{
	rb->size = 0;
	rb->data = buf;
	rb->capacity = capacity;
	rb->head = &rb->data[0];
	rb->tail = &rb->data[0];
}

uint8_t rb_is_full(struct ringbuf *rb)
{
	return rb->size == rb->capacity;
}

uint8_t rb_is_empty(struct ringbuf * rb)
{
	return rb->size == 0;
}

static inline void __rb_insert_tail(struct ringbuf *rb, uint8_t data)
{
	*rb->tail = data;
	rb->tail++;
	if (rb->tail == &rb->data[rb->capacity])
		rb->tail = rb->data;
}

void rb_insert_tail(struct ringbuf *rb, uint8_t data)
{
	__rb_insert_tail(rb, data);
	rb->size++;
}

static inline uint8_t __rb_remove_head(struct ringbuf *rb)
{
	uint8_t data = *rb->head;
	rb->head++;
	if (rb->head == &rb->data[rb->capacity])
		rb->head = rb->data;
	return data;
}

uint8_t rb_remove_head(struct ringbuf * rb)
{
	rb->size--;
	return __rb_remove_head(rb);
}
