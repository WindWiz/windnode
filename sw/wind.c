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
#include <avr/io.h>
#include <avr/interrupt.h>
#include "wind.h"
#include "adc.h"

static volatile uint16_t revs;

ISR(PCINT1_vect)
{
	revs++;
}

void wind_init(void)
{
	/* Setup wind direction on ADC0 / PC0 */
	adc_init();

	/* Setup wind speed on PC1 */
	DDRC &= ~(_BV(PC1));
	PCICR |= _BV(PCIE0);
	PCMSK0 &= ~(_BV(PCINT1));	/* Disable "pinchange" IRQs initially */

	/* Setup wind power pin on PC2 */
	DDRC |= _BV(PC2);
	wind_power_down();
}

void wind_power_up(void)
{
	PORTC |= _BV(PC2);
}

void wind_power_down(void)
{
	PORTC &= ~(_BV(PC2));
}

uint16_t wind_direction_sample(void)
{
	uint16_t dir;

	adc_enable();
	dir = adc_sample(0);
	adc_disable();

	return dir;
}

void wind_speed_start(void)
{
	PCMSK0 |= _BV(PCINT1);
	revs = 0;
}

uint16_t wind_speed_sample(void)
{
	return revs;
}

uint16_t wind_speed_stop(void)
{
	PCMSK0 &= ~(_BV(PCINT1));

	return revs;
}
