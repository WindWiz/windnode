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

#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include <stdint.h>

struct ringbuf {
	uint8_t *data;
	uint8_t capacity;
	uint8_t size;
	uint8_t *head;
	uint8_t *tail;
};

void rb_init(struct ringbuf *rb, uint8_t * buf, uint8_t capacity);
uint8_t rb_is_full(struct ringbuf *rb);
uint8_t rb_is_empty(struct ringbuf *rb);
void rb_insert_tail(struct ringbuf *rb, uint8_t data);
uint8_t rb_remove_head(struct ringbuf *rb);

#endif
