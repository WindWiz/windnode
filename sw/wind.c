#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "wind.h"
#include "adc.h"

static uint16_t revs;

ISR(PCINT0_vect)
{
	revs++;
}

static void power_up(void)
{
	PORTC |= _BV(PC2);
}

static void power_down(void)
{
	PORTC &= ~(_BV(PC2));
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
}

uint16_t wind_sample_direction(void)
{
	uint16_t dir;

	power_up();
	adc_enable();
	dir = adc_sample(0);
	adc_disable();
	power_down();
	
	return dir;
}

void wind_sample_speed_start(void)
{
	power_up();
	PCMSK0 |= _BV(PCINT1);	
}

uint16_t wind_sample_speed_stop(void)
{
	uint16_t speed;

	PCMSK0 &= ~(_BV(PCINT1));
	power_down();
	
	speed = revs;
	revs = 0;
	
	return speed;
}
