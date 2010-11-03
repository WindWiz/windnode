#include "hwparam.h" /* F_CPU; must be first */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#include "uart.h"
#include "wind.h"

int main(void)
{
	uart_init(UART_BAUD(2400, F_CPU));
	
	wind_init();		
	
	cli();

	while (1) {
		printf("DIR=%d ", wind_sample_direction());
		wind_sample_speed_start();
		_delay_ms(200);
		printf("SPD=%d\n", wind_sample_speed_stop());
	}

	return 0;
}

