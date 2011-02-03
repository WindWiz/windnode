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
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "swuart.h"
#include "uart.h"

static FILE debug;
static FILE gsm;

int main(void)
{
	swuart_init(SWUART_BAUD(CONFIG_DEBUG_BAUDRATE, F_CPU), &debug);
	uart_init(UART_BAUD(CONFIG_GSM_BAUDRATE, F_CPU), &gsm, NULL);

	stdout = &debug;
	stdin = &debug;

	sei();

	while (1) {

	}

	return 0;
}

