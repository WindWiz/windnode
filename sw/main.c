#include "hwparam.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#include "uart.h"

static uint16_t halfrevs;
static uint8_t speed;

ISR(INT0_vect)
{
	halfrevs++;
}

ISR(TIMER0_OVF_vect)
{
	/* calculate revs/s */
	speed = (uint8_t) (halfrevs);
	printf("SPD=%d\n", halfrevs);
	halfrevs = 0;
}

static void adc_init(void)
{
	ADMUX = _BV(REFS0);
	ADCSRB = 0;
	ADCSRA = _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

static void adc_disable(void)
{
	ADCSRA &= ~(_BV(ADEN));
}

static void adc_enable(void)
{
	ADCSRA |= _BV(ADEN);
}

static uint16_t adc_sample(uint8_t channel)
{
	uint16_t value;

	ADMUX = (ADMUX & 0xF0) | (channel & 0xF);
	ADCSRA |= _BV(ADSC);
	loop_until_bit_is_clear(ADCSRA, ADSC);

	value = ADC;
	return value;
}

int main(void)
{
	uart_init(UART_BAUD(2400, F_CPU));

	/* setup ADC for sampling wind direction */
	adc_init();

	/* setup INT0 for wind speed cycle counting */
	EICRA = _BV(ISC01) | _BV(ISC00); /* trigger INT0 on rising edge*/
	EIMSK = _BV(INT0);

	/* setup TIMER1 */
	TCCR1A = 0;
	TCCR1B = _BV(CS02) | _BV(CS00); /* Prescaler 1024 */
	TIMSK1 = _BV(TOIE0);

	adc_enable();
	while (1) {
		cli();
		printf("DIR=%d\n", adc_sample(0));
		sei();
	}
	adc_disable();
	
	return 0;
}

