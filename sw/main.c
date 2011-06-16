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

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"
#include "errno.h"
#include "swuart.h"
#include "gsm.h"
#include "time.h"
#include "stackmon.h"

static FILE f_debug;

static int cmd_stackdepth(char *arg, char *buf, size_t buflen)
{
	char tmp[30];

	sprintf(tmp, "Depth %d Limit %d", stackmon_maxdepth(),
		stackmon_stacksize());

	if (buflen >= strlen(tmp)) {
		strcat(buf, tmp);
		return strlen(tmp);
	} else
		return -ENOMEM;
}

static int cmd_gsmpwr(char *arg, char *buf, size_t buflen)
{
	int ret;
	if (buflen < 2)
		return -ENOMEM;

	if (!memcmp(arg, "ON", 2)) {
		strcat(buf, "UP");
		gsm_power_up();
	} else {
		strcat(buf, "DN");
		gsm_power_down();
	}

	return 2;
}

static int cmd_gsmrst(char *arg, char *buf, size_t buflen)
{
	if (buflen < 2)
		return -ENOMEM;

	gsm_reset();
	strcat(buf, "OK");
	return 2;
}

static int cmd_gsmat(char *arg, char *buf, size_t buflen)
{
	int read;

	gsm_at(arg);
	read = gsm_read_response(buf, buflen, 1*HZ);
	if (read < 0) {
		sprintf(buf, "ERROR %d", read);
		return 7; /* "approximation" */
	} else {
		return read ;
	}
}

static int cmd_gsmcfg(char *arg, char *buf, size_t buflen)
{
	int ret;

	gsm_at("ATE0");
	if (gsm_check_response("ATE0\r\n\r\nOK\r\n", 1*HZ)) {
		strcat(buf, "ATE FAIL");
		return 8;
	}

	strcat(buf, "OK");
	return 2;
}

/* Sorted by priority */
static struct command *commandlist[] = {
	CREATE_COMMAND("STACK", cmd_stackdepth),
	CREATE_COMMAND("GSMAT", cmd_gsmat),
	CREATE_COMMAND("GSMPWR", cmd_gsmpwr),
	CREATE_COMMAND("GSMRST", cmd_gsmrst),
	CREATE_COMMAND("GSMCFG", cmd_gsmcfg),
	COMMANDLIST_END,
};

static void cmdline_loop(void)
{
	char in[200], out[200];
	size_t inlen;
	bool in_cmd;

	while (1) {
		in_cmd = true;
		inlen = 0;
		out[0] = '\0';
		in[0] = '\0';
		printf("> ");

		while (in_cmd) {
			int c = fgetc(stdin);
			if (c != EOF) {
				if (c == '\n' || c == '\r' || inlen == sizeof(in) - 1) {
					in[inlen] = '\0';
					cmd_exec(commandlist, in, out, sizeof(out));

					printf("%s (%d bytes)\n", out, strlen(out));
					in_cmd = false;
				} else
					in[inlen++] = c;
			}
		}
	}
}

int main(void)
{
	uint8_t cause = MCUSR;
	MCUSR = 0;

	swuart_init(SWUART_BAUD(CONFIG_DEBUG_BAUDRATE, F_CPU), &f_debug);

	stdout = &f_debug;
	stdin = &f_debug;

	gsm_init();
	time_init();

	sei();

	printf("WindNode initialized, reset cause 0x%x, startup stack %d bytes\n",
		cause, stackmon_maxdepth());

	cmdline_loop();

	while (1) {
		/* SLEEP */
	}

	return 0;
}

