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
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ringbuf.h"
#include "uart.h"

static struct ringbuf tx;
static struct ringbuf rx;

#define TX_IRQ_DISABLE()	UCSR0B &= ~(_BV(UDRIE0))
#define TX_IRQ_ENABLE()		UCSR0B |= _BV(UDRIE0)
#define RX_IRQ_DISABLE()	UCSR0B &= ~(_BV(RXCIE0))
#define RX_IRQ_ENABLE()		UCSR0B |= _BV(RXCIE0)

ISR(USART_RX_vect)
{
	unsigned char rx_pin = UDR0;
	if (!rb_is_full(&rx))
		rb_insert_tail(&rx, rx_pin);
}

ISR(USART_UDRE_vect)
{
	if (!rb_is_empty(&tx))
		UDR0 = rb_remove_head(&tx);
	else
		TX_IRQ_DISABLE();
}

static int uart_getc(FILE * stream)
{
	int ret = 0;

	RX_IRQ_DISABLE();
	if (rb_is_empty(&rx))
		ret = _FDEV_EOF;
	else
		ret = rb_remove_head(&rx);
	RX_IRQ_ENABLE();

	return ret;
}

static int uart_putc(char c, FILE * stream)
{
	uint8_t queued = 0;

	do {
		TX_IRQ_DISABLE();
		if (!rb_is_full(&tx)) {
			rb_insert_tail(&tx, c);
			queued = 1;
		}
		TX_IRQ_ENABLE();
	} while (!queued);

	return 0;
}

void uart_init(unsigned int ubrr, FILE * stream, uint8_t * rx_buf,
    uint8_t rx_size, uint8_t * tx_buf, uint8_t tx_size)
{
	/* Setup context */
	rb_init(&rx, rx_buf, rx_size);
	rb_init(&tx, tx_buf, tx_size);

	/* Baudrate */
	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) (ubrr);

	/* Async 8N1 */
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

	fdev_setup_stream(stream, uart_putc, uart_getc, _FDEV_SETUP_RW);

	/* Enable */
	UCSR0B = _BV(TXEN0) | _BV(RXEN0);
	RX_IRQ_ENABLE();
}

void uart_free(void)
{
}
