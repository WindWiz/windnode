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

#include <avr/io.h>
#include <stdint.h>

void adc_init()
{
	ADMUX = _BV(REFS0);
	ADCSRB = 0;
	ADCSRA = _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

void adc_disable()
{
	ADCSRA &= ~(_BV(ADEN));
}

void adc_enable()
{
	ADCSRA |= _BV(ADEN);
}

uint16_t adc_sample(uint8_t channel)
{
	uint16_t value;

	ADMUX = (ADMUX & 0xF0) | (channel & 0xF);
	ADCSRA |= _BV(ADSC);
	loop_until_bit_is_clear(ADCSRA, ADSC);

	value = ADC;
	return value;
}
