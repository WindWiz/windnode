#include "hwparam.h" /* F_CPU; must be first */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#include "uart.h"
#include "wind.h"
#include "swuart.h"

static FILE swuart;

int main(void)
{
	uart_init(UART_BAUD(2400, F_CPU));
	swuart_init(SWUART_BAUD(2400, F_CPU), &swuart);
	wind_init();
	
	sei();

	wind_power_up();
	wind_speed_start();
	while (1) {
		printf("DIR=%d ", wind_direction_sample());
		printf("SPD=%d\n", wind_speed_sample());
		_delay_ms(2000);	
	}
	wind_speed_stop();
	wind_power_down();

	return 0;
}

