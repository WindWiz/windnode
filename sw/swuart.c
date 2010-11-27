/* SWUART - A software USART implementation.
 * Stdio interface to a software 8N1 UART port. The implementation is full 
 * duplex and driven by the two 8-bit timers (Timer #0 & #2) found on the 
 * ATmega168.
 * It can be used to add additional USART ports to your MCU at the cost of
 * CPU cycles.
 * 
 * Hardware setup:
 * - Transmitter pin is PD1
 * - Receiver pin is PD0
 * 
 * The TX pin can be changed freely, however the RX pin is bound to either INT0
 * or INT1 because it relies on external interrupts to detect incoming data.
 * This implemention assumes incoming data arrives on INT0/PD0.
 *
 * NOTE: As Software USART is quite heavy on the CPU, this port is limited to
 * 2400 baud. The implementation has been optimized for running at 1 MHz CPU
 * frequency. 
 *
 * Usage:
 * #include <stdio.h>
 * #include "swuart.h"
 * 
 * ...
 *  
 * FILE swuart;
 * swuart_init(SWUART_BAUD(2400, F_CPU), &swuart);
 * 
 * ... 
 * 
 * - Use fputc(), fprintf() etc to write.
 * - Use fgetc(), fread() etc to read.
 *
 * You may also assign the UART stream to become your stdin/stdout:
 * stdin = &swuart;
 * stdout = &swuart;
 * 
 * After this all standard functions will use the UART for I/O, such as:
 * printf, scanf etc.
 * 
 * For more information on the asynchronous serial protocol, please refer to:
 * http://en.wikipedia.org/wiki/Asynchronous_serial
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ringbuf.h"

/* RX must be INT0 (PD2 on ATmega168) */
#define RX      (PD2)
#define RX_PORT (PORTD)
#define RX_PIN  (PIND)
#define RX_DDR  (DDRD)

#define TX      (PD4)
#define TX_PORT (PORTD)
#define TX_DDR  (DDRD)

#define DATAWIDTH (8)
#define TX_PRESCALER (_BV(CS22))		/* 64 */
#define RX_PRESCALER (_BV(CS01) | _BV(CS00))	/* 64 */

#define RX_IRQ_DISABLE()	TIMSK0 &= ~(_BV(OCIE0A))
#define RX_IRQ_ENABLE()		TIMSK0 |= _BV(OCIE0A)
#define TX_IRQ_DISABLE()	TIMSK2 &= ~(_BV(OCIE2A))
#define TX_IRQ_ENABLE()		TIMSK2 |= _BV(OCIE2A)

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

ISR(INT0_vect)
{
	/* Disable INT0 and start RX timer */
	EIMSK &= ~(_BV(INT0));
	
	/* Setup to sample startbit in btime/2 */
	port.rx_state = UART_STARTBIT;
	TCNT0 = port.btime >> 1;	
	TCCR0B |= RX_PRESCALER;
}

ISR(TIMER0_COMPA_vect) 
{
	uint8_t rxbit = RX_PIN & _BV(RX);

	switch (port.rx_state) {
		case UART_IDLE:
			/* We should never end up here */
			break;

		case UART_STARTBIT:
			/* Check for valid startbit */
			if (!rxbit)	{
				port.rx_bitsleft = DATAWIDTH;
				port.rx_state = UART_DATA;
			} else {
				/* Glitch-protection: return to idle if startbit has vanished */
				port.rx_state = UART_IDLE;
				TCCR0B &= ~RX_PRESCALER;
				TIFR0 |= _BV(OCF0A);
				EIFR  |= _BV(INTF0);
				EIMSK |= _BV(INT0);
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
			if (rxbit) {
				if (!rb_is_full(&port.rx_buf))
					rb_insert_tail(&port.rx_buf, port.rx_sample);
			} else {
				/* XXX: Data is most likely corrupt. Signal someone? */
			}

			/* Disable timer and re-enable startbit detector */
			port.rx_state = UART_IDLE;
			TCCR0B &= ~RX_PRESCALER;
			TIFR0 |= _BV(OCF0A);
			EIFR  |= _BV(INTF0);
			EIMSK |= _BV(INT0);
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
			TX_PORT &= ~(_BV(TX));
			port.tx_state = UART_DATA;
			break;

		case UART_DATA:
			if (port.tx_sample & 0x1)
				TX_PORT |= _BV(TX);
			else
				TX_PORT &= ~(_BV(TX));

			port.tx_sample = port.tx_sample >> 1;
			port.tx_bitsleft--;
	
			if (port.tx_bitsleft == 0)
				port.tx_state = UART_STOPBIT;
			break;

		case UART_STOPBIT:
			TX_PORT |= _BV(TX);

			/* Schedule work for next cycle, if available */
			if (!rb_is_empty(&port.tx_buf))	{
				port.tx_bitsleft = DATAWIDTH;
				port.tx_sample = rb_remove_head(&port.tx_buf);
				port.tx_state = UART_STARTBIT;
			} else {
				TCCR2B &= ~TX_PRESCALER;
				TIFR2 |= _BV(OCF2A);
				port.tx_state = UART_IDLE;
			}
			break;
	}
}

static int swuart_getc(FILE *stream)
{
	int ret = 0;

	RX_IRQ_DISABLE();
		if (rb_is_empty(&port.rx_buf))
			ret = EOF;
		else
			ret = rb_remove_head(&port.rx_buf);
	RX_IRQ_ENABLE();

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
			TCCR2B |= TX_PRESCALER;
			queued = 1;
		} else {
			TX_IRQ_DISABLE();
			if (!rb_is_full(&port.tx_buf)) {
				rb_insert_tail(&port.tx_buf, c);
				queued = 1;
			}
			TX_IRQ_ENABLE();
		}
	} while (!queued);

	return 0;
}

void swuart_init(unsigned int btime, FILE *stream)
{
	rb_init(&port.rx_buf);
	rb_init(&port.tx_buf);
	port.tx_state = UART_IDLE;
	port.rx_state = UART_IDLE;
	port.btime = btime;

	RX_DDR  &= ~(_BV(RX));
	RX_PORT |= _BV(RX);		
	TX_DDR  |= _BV(TX);
	TX_PORT |= _BV(TX); 

	/* Falling edge triggers RX sampling */
	EICRA |= _BV(ISC01); 
	EIMSK |= _BV(INT0);

	/* Baudrate */
	OCR0A = btime;
	OCR2A = btime;

	/* CTC Mode */
	TCCR0A = _BV(WGM01);
	TCCR2A = _BV(WGM21);
		
	fdev_setup_stream(stream, swuart_putc, swuart_getc, _FDEV_SETUP_RW);

	/* At this point, the timers are not started so its safe to unmask */
	RX_IRQ_ENABLE();
	TX_IRQ_ENABLE();
}

