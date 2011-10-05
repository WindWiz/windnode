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

#ifndef _SWUART_H_
#define _SWUART_H_

#include <stdio.h>

#define SWUART_BAUD(baud, f) ((f)/(baud*64l))

void swuart_init(unsigned int btime, FILE *stream, uint8_t *rx_buf,
	uint8_t rx_size, uint8_t *tx_buf, uint8_t tx_size);
void swuart_free(void);

#endif
