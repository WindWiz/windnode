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
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ringbuf.h"
#include "config.h"

#define STARTBIT (0)
#define STOPBIT (1)
#define DATAWIDTH (8)
#define TX_PRESCALER (_BV(CS22))		/* 64 */
#define RX_PRESCALER (_BV(CS01) | _BV(CS00))	/* 64 */

enum {
	UART_IDLE,
	UART_STARTBIT,
	UART_DATA,
	UART_STOPBIT,
};

struct swuart_private {
	uint8_t tx_sample;
	uint8_t tx_bitsleft;
	struct ringbuf tx_buf;
	uint8_t tx_state;
	uint8_t rx_sample;
	uint8_t rx_bitsleft;
	struct ringbuf rx_buf;
	uint8_t rx_state;
	uint8_t btime;
};

static struct swuart_private port;

static uint8_t rx_pin(void)
{
	return (CONFIG_SWUART_RX_PIN & _BV(CONFIG_SWUART_RX_BIT)) != 0;
}

static void rx_pinchange_irq(uint8_t en)
{
	if (en)
		CONFIG_SWUART_RX_PCMSK |= _BV(CONFIG_SWUART_RX_PCBIT);
	else {		
		CONFIG_SWUART_RX_PCMSK &= ~(_BV(CONFIG_SWUART_RX_PCBIT));
		PCIFR |= _BV(CONFIG_SWUART_RX_PCBIT);
	}
}

static void rx_timer_irq(uint8_t en)
{
	if (en)
		TIMSK0 |= _BV(OCIE0A);
	else
		TIMSK0 &= ~(_BV(OCIE0A));
}

static void rx_timer_toggle(uint8_t on)
{
	if (on)
		TCCR0B |= RX_PRESCALER;
	else {
		TCCR0B &= ~RX_PRESCALER;
		TIFR0 |= _BV(OCF0A);
	}
}

static void tx_pin(uint8_t hi)
{
	if (hi)
		CONFIG_SWUART_TX_PORT |= _BV(CONFIG_SWUART_TX_BIT);
	else
		CONFIG_SWUART_TX_PORT &= ~(_BV(CONFIG_SWUART_TX_BIT));
}

static void tx_timer_irq(uint8_t en)
{
	if (en)
		TIMSK2 |= _BV(OCIE2A);
	else
		TIMSK2 &= ~(_BV(OCIE2A));
}

static void tx_timer_toggle(uint8_t on)
{
	if (on)
		TCCR2B |= TX_PRESCALER;
	else {
		TCCR2B &= ~TX_PRESCALER;
		TIFR2 |= _BV(OCF2A);
	}
}

ISR(CONFIG_SWUART_RX_PCVECT)
{
	uint8_t rxbit = rx_pin();

	/* Glitch-protection: Check for start bit on RX PIN */
	if (rxbit == STARTBIT) {
		/* Setup to sample startbit in btime/2 */
		TCNT0 = port.btime/2;
		rx_timer_toggle(true);

		port.rx_state = UART_STARTBIT;
		rx_pinchange_irq(false);
	}
}

ISR(TIMER0_COMPA_vect) 
{
	uint8_t rxbit = rx_pin();

	switch (port.rx_state) {
		case UART_IDLE:
			/* We should never end up here */
			break;

		case UART_STARTBIT:
			/* Check for valid startbit */
			if (rxbit == STARTBIT)	{
				port.rx_bitsleft = DATAWIDTH;
				port.rx_state = UART_DATA;
			} else {
				/* Glitch-protection: return to idle if startbit has vanished */
				port.rx_state = UART_IDLE;
				rx_timer_toggle(false);
				rx_pinchange_irq(true);
			}
			break;

		case UART_DATA:
			port.rx_sample = port.rx_sample >> 1;
			port.rx_bitsleft--;

			if (rxbit)
				port.rx_sample |= _BV(DATAWIDTH-1);

			if (port.rx_bitsleft == 0)
				port.rx_state = UART_STOPBIT;
			break;

		case UART_STOPBIT:
			/* Check for valid stopbit */
			if (rxbit == STOPBIT) {
				if (!rb_is_full(&port.rx_buf))
					rb_insert_tail(&port.rx_buf, port.rx_sample);
			}

			/* Disable timer and re-enable startbit detector */
			port.rx_state = UART_IDLE;
			rx_timer_toggle(false);
			rx_pinchange_irq(true);
			break;			
	}
}

ISR(TIMER2_COMPA_vect) 
{
	switch (port.tx_state) {
		case UART_IDLE:
			/* We should never end up here */
			break;
		
		case UART_STARTBIT:
			tx_pin(STARTBIT);
			port.tx_state = UART_DATA;
			break;

		case UART_DATA:
			tx_pin(port.tx_sample & 0x1);

			port.tx_sample = port.tx_sample >> 1;
			port.tx_bitsleft--;
	
			if (port.tx_bitsleft == 0)
				port.tx_state = UART_STOPBIT;
			break;

		case UART_STOPBIT:
			tx_pin(STOPBIT);

			/* Schedule work for next cycle, if available */
			if (!rb_is_empty(&port.tx_buf))	{
				port.tx_bitsleft = DATAWIDTH;
				port.tx_sample = rb_remove_head(&port.tx_buf);
				port.tx_state = UART_STARTBIT;
			} else {
				tx_timer_toggle(false);
				port.tx_state = UART_IDLE;
			}
			break;
	}
}

static int swuart_getc(FILE *stream)
{
	int ret = 0;

	rx_timer_irq(false);
	if (rb_is_empty(&port.rx_buf))
		ret = _FDEV_EOF;
	else
		ret = rb_remove_head(&port.rx_buf);
	rx_timer_irq(true);

	return ret;
}

static int swuart_putc(char c, FILE *stream)
{
	uint8_t queued = 0;

	do {
		if (port.tx_state == UART_IDLE) {
			port.tx_bitsleft = DATAWIDTH;
			port.tx_sample = c;
			port.tx_state = UART_STARTBIT;
			TCNT2 = 0;			
			tx_timer_toggle(true);
			queued = 1;
		} else {
			tx_timer_irq(false);
			if (!rb_is_full(&port.tx_buf)) {
				rb_insert_tail(&port.tx_buf, c);
				queued = 1;
			}
			tx_timer_irq(true);
		}
	} while (!queued);

	return 0;
}

void swuart_init(unsigned int btime, FILE *stream, uint8_t *rx_buf,
    uint8_t rx_size, uint8_t *tx_buf, uint8_t tx_size)
{
	rb_init(&port.rx_buf, rx_buf, rx_size);
	rb_init(&port.tx_buf, tx_buf, tx_size);

	port.tx_state = UART_IDLE;
	port.rx_state = UART_IDLE;
	port.btime = btime;

	CONFIG_SWUART_RX_DIR  &= ~(_BV(CONFIG_SWUART_RX_BIT));
	CONFIG_SWUART_RX_PORT |= _BV(CONFIG_SWUART_RX_BIT);	/* Pull-up */
	CONFIG_SWUART_TX_DIR  |= _BV(CONFIG_SWUART_TX_BIT);
	tx_pin(STOPBIT);

	/* Baudrate */
	OCR0A = btime;
	OCR2A = btime;

	/* CTC Mode */
	TCCR0A = _BV(WGM01);
	TCCR2A = _BV(WGM21);
		
	fdev_setup_stream(stream, swuart_putc, swuart_getc, _FDEV_SETUP_RW);

	/* At this point, the timers are not started so its safe to unmask */
	rx_timer_irq(true);
	tx_timer_irq(true);

	/* Trigger interrupt on RX pin changes */
	PCICR |= _BV(CONFIG_SWUART_RX_PCCTRL); 
	rx_pinchange_irq(true);
}

void swuart_free(void)
{
}
