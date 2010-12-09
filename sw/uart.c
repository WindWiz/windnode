/* Stdio interface to the ATmega hardware UART. The implementation is interrupt
 * -driven and uses ringbuffers for each direction and allows for full duplex
 * communication.
 *
 * Usage:
 * #include <stdio.h>
 *
 * ...
 *
 * FILE uart;
 * uart_init(UART_BAUD(2400, F_CPU), &uart, NULL);
 *
 * You may also (optionally) register an overrun handler which will be called
 * whenever an receiver overrun occurs. This can be used as a warning for the
 * consumer that,
 *  1) producer is too fast (throttle)
 *  2) data consumption is too slow
 *  3) data is being lost
 *
 * uart_init(UART_BAD(2400, F_CPU), &uart, orun_handler);
 *
 * ...
 *
 * - Use fputc(), fprintf() etc to write.
 * - Use fgetc(), fread() etc to read.
 *
 * You may also assign the UART stream to become your stdin/stdout:
 * stdin = &uart;
 * stdout = &uart;
 *
 * After this all standard functions will use the UART for I/O, such as:
 * printf, scanf etc.
 *
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
	rb_init(&port.rx, CONFIG_UART_RX_BUF);
	rb_init(&port.tx, CONFIG_UART_TX_BUF);
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
	rb_free(&port.tx);
	rb_free(&port.rx);
}

