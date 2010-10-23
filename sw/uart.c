#include <stdio.h>
#include <avr/io.h>

static void uart_putc(unsigned char);

static FILE uart = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);

void uart_init(unsigned int ubrr)
{
	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) (ubrr);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);

	/* Async 8N1 */
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	/* Re-route stdout to UART */
	stdout = &uart;
}

static void uart_putc(unsigned char c)
{
	/* Wait for empty transmit buffer */
	while(!(UCSR0A & (1 << UDRE0)));

	/* Transmit */
	UDR0 = c;
}

