#include <avr/io.h>
#include <stdint.h>

void adc_init()
{
	ADMUX = _BV(REFS0);
	ADCSRB = 0;
	ADCSRA = _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

void adc_disable()
{
	ADCSRA &= ~(_BV(ADEN));
}

void adc_enable()
{
	ADCSRA |= _BV(ADEN);
}

uint16_t adc_sample(uint8_t channel)
{
	uint16_t value;

	ADMUX = (ADMUX & 0xF0) | (channel & 0xF);
	ADCSRA |= _BV(ADSC);
	loop_until_bit_is_clear(ADCSRA, ADSC);

	value = ADC;
	return value;
}
