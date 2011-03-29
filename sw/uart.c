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

struct uart_private {
	struct ringbuf tx;
	struct ringbuf rx;
	uart_orun_handler_t orun;
};

static struct uart_private port;

static uint8_t tx_buffer[CONFIG_UART_TX_BUF];
static uint8_t rx_buffer[CONFIG_UART_RX_BUF];

#define TX_IRQ_DISABLE()	UCSR0B &= ~(_BV(UDRIE0))
#define TX_IRQ_ENABLE()		UCSR0B |= _BV(UDRIE0)
#define RX_IRQ_DISABLE()	UCSR0B &= ~(_BV(RXCIE0))
#define RX_IRQ_ENABLE()		UCSR0B |= _BV(RXCIE0)

ISR(USART_RX_vect)
{
	unsigned char rx = UDR0;
	if (!rb_is_full(&port.rx))
		rb_insert_tail(&port.rx, rx);
	else if (port.orun)
		port.orun();
}

ISR(USART_UDRE_vect)
{
	if (!rb_is_empty(&port.tx))
		UDR0 = rb_remove_head(&port.tx);
	else
		TX_IRQ_DISABLE();
}

static int uart_getc(FILE *stream)
{
	int ret = 0;

	RX_IRQ_DISABLE();
	if (rb_is_empty(&port.rx))
		ret = _FDEV_EOF;
	else
		ret = rb_remove_head(&port.rx);
	RX_IRQ_ENABLE();

	return ret;
}

static int uart_putc(char c, FILE *stream)
{
	uint8_t queued = 0;

	do {
		TX_IRQ_DISABLE();
		if (!rb_is_full(&port.tx)) {
			rb_insert_tail(&port.tx, c);
			queued = 1;
		}
		TX_IRQ_ENABLE();
	} while (!queued);

	return 0;
}

void uart_init(unsigned int ubrr, FILE *stream, uart_orun_handler_t orun)
{
	/* Setup context */
	rb_init(&port.rx, &rx_buffer[0], CONFIG_UART_RX_BUF);
	rb_init(&port.tx, &tx_buffer[0], CONFIG_UART_TX_BUF);

	port.orun = orun;

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

