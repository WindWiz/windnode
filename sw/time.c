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
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "time.h"

volatile uint32_t jiffies;

/* XXX: Calculate depending on F_CPU and HZ, at the moment this is only
   accurate for 16 MHz F_CPU and HZ > 1 */
#define TIMER_PRESCALER (_BV(CS11) | _BV(CS10)) /* 64 */

SIGNAL(TIMER1_COMPA_vect)
{
	jiffies++;
}

uint32_t time_jiffies(void)
{
	uint32_t t;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		t = jiffies;
	}

	return t;
}

void time_init(void)
{
	jiffies = -120*HZ; /* Trigger wraparound early to find bugs */

	/* Setup timer hardware */
	TCCR1B = _BV(WGM12);	/* CTC mode */
	OCR1A = F_CPU / (TIMER_PRESCALER * HZ);
	TIMSK1 |= _BV(OCIE1A);

	/* Start timer */
	TCCR1B |= TIMER_PRESCALER;
}


