/* Copyright (C) 2010-2011 Magnus Olsson
 * 
 * This file is part of Windnode
 * Windnode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Windnode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Windnode.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "errno.h"
#include "uart.h"
#include "time.h"

#define D(x)

static FILE f_gsm;

static void gsm_power(uint16_t delay)
{
	/* ON pin is inverted by transistor on GSM playground board */
	CONFIG_GSM_EN_PORT |= _BV(CONFIG_GSM_EN_PIN);
	CONFIG_GSM_EN_DIR |= _BV(CONFIG_GSM_EN_PIN);
	_delay_ms(delay);
	CONFIG_GSM_EN_PORT &= ~(_BV(CONFIG_GSM_EN_PIN));
	CONFIG_GSM_EN_DIR &= ~(_BV(CONFIG_GSM_EN_PIN));
}

void gsm_power_down(void)
{
	gsm_power(2000);
}

void gsm_power_up(void)
{
	gsm_power(1000);
}

void gsm_reset(void)
{
	/* RST pin is inverted by transistor on GSM playground board */
	CONFIG_GSM_RST_PORT |= _BV(CONFIG_GSM_RST_PIN);
	CONFIG_GSM_RST_DIR |= _BV(CONFIG_GSM_RST_PIN);
	_delay_ms(200);
	CONFIG_GSM_RST_PORT &= ~(_BV(CONFIG_GSM_RST_PIN));
	CONFIG_GSM_RST_DIR &= ~(_BV(CONFIG_GSM_RST_PIN));
}

int gsm_init(void)
{	
	uart_init(UART_BAUD(CONFIG_GSM_BAUDRATE, F_CPU), &f_gsm, NULL);

	/* Tri-state ON+RST pins with no pull-ups */
	CONFIG_GSM_EN_DIR &= ~(_BV(CONFIG_GSM_EN_PIN));
	CONFIG_GSM_EN_PORT &= ~(_BV(CONFIG_GSM_EN_PIN));

	CONFIG_GSM_RST_DIR &= ~(_BV(CONFIG_GSM_RST_PIN));
	CONFIG_GSM_RST_PORT &= ~(_BV(CONFIG_GSM_RST_PIN));

	_delay_ms(100); /* Allow for signals to stabilize */

	gsm_reset();
	
	return 0;
}

static int gsm_getc(uint8_t timeout)
{
	uint32_t t0 = time_jiffies();
	int c;

	do {
		c = fgetc(&f_gsm);
		if (c != EOF)
			return c;
	} while(time_jiffies() - t0 < timeout);

	return -1; /* timeout */
}

/* Discard response up to and including next <LF> */
static void gsm_discard_response(void)
{
	int c;

	do {
		c = gsm_getc(1*HZ);
		if (c < 0)
			break;	/* timeout */
	} while ((uint8_t) c != '\n');
}

int gsm_read_response(char *buf, size_t buf_len, uint8_t timeout)
{
	size_t len = 0;
	int c;

	gsm_discard_response();
	D(printf("Response: "));
	do {
		c = gsm_getc(timeout);
		if (c < 0) {
			D(printf("Timeout!\n"));
			return -ETIMEOUT;
		}

		buf[len++] = (uint8_t) c;

		D(printf("0x%x ", (uint8_t) c));
		if (len >= 2 && buf[len-1] == '\n' && buf[len-2] == '\r') {
			D(printf("\n"));
			buf[len-2] = '\0'; /* \r */
			buf[len-1] = '\0'; /* \n */
			return len-2;
		}
	} while (len < buf_len);

	D(printf("Mismatch!\n"));
	buf[0] = '\0';
	gsm_discard_response();

	return -ENOMEM;
}

int gsm_check_response(char *expected, uint8_t timeout)
{
	size_t expected_len = strlen(expected);
	size_t len = 0;
	int c;
	int ret = 0;

	do {
		c = gsm_getc(timeout);
		if (c < 0)
			return -ETIMEOUT;

		if (expected[len] != (uint8_t) c) {
			ret = -EINVAL;
			break;
		}

		len++;
	} while (len < expected_len);

	return ret;
}

int gsm_at(char *cmd)
{
	fputs(cmd, &f_gsm);
	fputs("\r\n", &f_gsm);

	D(printf("Write: %s\n", cmd));
	return 0;
}

