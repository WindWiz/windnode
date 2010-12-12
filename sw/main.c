#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "swuart.h"

static FILE debug;

int main(void)
{
	swuart_init(SWUART_BAUD(CONFIG_DEBUG_BAUDRATE, F_CPU), &debug);
	stdout = &debug;
	stdin = &debug;

	sei();

	DDRB |= _BV(PB1);
	PORTB |= _BV(PB1);
	while (1) {
	}

	return 0;
}

