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

#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "errno.h"
#include "uart.h"
#include "time.h"

static uint8_t tx_buf[CONFIG_GSM_TX_BUF];
static uint8_t rx_buf[CONFIG_GSM_RX_BUF];

static void telit_power(uint16_t delay)
{
	/* ON pin is inverted by transistor on GSM playground board */
	CONFIG_GSM_EN_PORT |= _BV(CONFIG_GSM_EN_PIN);
	CONFIG_GSM_EN_DIR |= _BV(CONFIG_GSM_EN_PIN);
	_delay_ms(delay);
	CONFIG_GSM_EN_PORT &= ~(_BV(CONFIG_GSM_EN_PIN));
	CONFIG_GSM_EN_DIR &= ~(_BV(CONFIG_GSM_EN_PIN));
}

void telit_power_down(void)
{
	telit_power(2000);
}

void telit_power_up(void)
{
	telit_power(1000);
}

void telit_reset(void)
{
	/* RST pin is inverted by transistor on GSM playground board */
	CONFIG_GSM_RST_PORT |= _BV(CONFIG_GSM_RST_PIN);
	CONFIG_GSM_RST_DIR |= _BV(CONFIG_GSM_RST_PIN);
	_delay_ms(200);
	CONFIG_GSM_RST_PORT &= ~(_BV(CONFIG_GSM_RST_PIN));
	CONFIG_GSM_RST_DIR &= ~(_BV(CONFIG_GSM_RST_PIN));
}

int telit_init(FILE * stream)
{
	uart_init(UART_BAUD(CONFIG_GSM_BAUDRATE, F_CPU), stream, rx_buf,
	    CONFIG_GSM_RX_BUF, tx_buf, CONFIG_GSM_TX_BUF);

	/* Tri-state ON+RST pins with no pull-ups */
	CONFIG_GSM_EN_DIR &= ~(_BV(CONFIG_GSM_EN_PIN));
	CONFIG_GSM_EN_PORT &= ~(_BV(CONFIG_GSM_EN_PIN));

	CONFIG_GSM_RST_DIR &= ~(_BV(CONFIG_GSM_RST_PIN));
	CONFIG_GSM_RST_PORT &= ~(_BV(CONFIG_GSM_RST_PIN));

	_delay_ms(100);		/* Allow for signals to stabilize */

	telit_reset();

	return 0;
}
