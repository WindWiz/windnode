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
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include "time.h"

volatile uint32_t jiffies = 0;	/* Don't support direct read-out on host */

uint32_t time_jiffies(void)
{
	struct timeval t;

	if (gettimeofday(&t, NULL)) {
		perror("time_jiffies");
		return ~0;
	}

	return (t.tv_sec * HZ) + msecs_to_jiffies((t.tv_usec / 1000));
}

void time_init(void)
{
	/* NO-OP */
}
