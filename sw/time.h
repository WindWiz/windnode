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
#ifndef _JIFFIES_H_
#define _JIFFIES_H_

#include <stdint.h>

#define MSEC_PER_SEC (1000)

#define HZ 			CONFIG_HZ

/* Do not support sub millisecond precision */
#if HZ > MSEC_PER_SEC
#error Unsupported CONFIG_HZ (> MSEC_PER_SEC)
#endif

/* Do not support non-integer timer periods to simply to/from msecs() */
#if MSEC_PER_SEC % HZ
#error Unsupported CONFIG_HZ (MSEC_PER_SEC % CONFIG_HZ != 0)
#endif

#define jiffies_to_msecs(x) \
	((x) * (MSEC_PER_SEC / HZ))

#define msecs_to_jiffies(x) \
	((x) + (MSEC_PER_SEC / HZ) - 1) / (MSEC_PER_SEC / HZ)

void time_init(void);
uint32_t time_jiffies(void);

extern volatile uint32_t jiffies;

#endif /* _JIFFIES_H_ */
