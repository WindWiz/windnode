#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "wind.h"
#include "adc.h"

static volatile uint16_t revs;

ISR(PCINT0_vect)
{
	revs++;
}

void wind_init(void)
{
	/* Setup wind direction on ADC0 / PC0 */
	adc_init();

	/* Setup wind speed on PC1 */ 
	DDRC &= ~(_BV(PC1));	
	PCICR |= _BV(PCIE0);
	PCMSK0 &= ~(_BV(PCINT1)); /* Disable "pinchange" IRQs initially */

	/* Setup wind power pin on PC2 */
	DDRC |= _BV(PC2);
	wind_power_down();
}

void wind_power_up(void)
{
	PORTC |= _BV(PC2);
}

void wind_power_down(void)
{
	PORTC &= ~(_BV(PC2));
}

uint16_t wind_direction_sample(void)
{
	uint16_t dir;

	adc_enable();
	dir = adc_sample(0);
	adc_disable();
	
	return dir;
}

void wind_speed_start(void)
{
	PCMSK0 |= _BV(PCINT1);	
	revs = 0;
}

uint16_t wind_speed_sample(void)
{
	return revs;
}

uint16_t wind_speed_stop(void)
{
	PCMSK0 &= ~(_BV(PCINT1));
	
	return revs;
}
