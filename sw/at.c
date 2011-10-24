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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "errno.h"
#include "time.h"

#define D(x)

#define AT_ERROR "ERROR"

static int at_getc(FILE * stream, uint16_t timeout)
{
	uint32_t t0 = time_jiffies();
	int c;

	do {
		c = fgetc(stream);
		if (c != EOF)
			return c;
	} while (time_jiffies() - t0 < timeout);

	return -ETIMEOUT;	/* timeout */
}

static void at_discard_until(FILE * stream, uint8_t x)
{
	int c;

	do {
		c = at_getc(stream, 1 * HZ);
		if (c < 0)
			break;	/* timeout */
#ifdef D
		if (c >= 32 && c < 127)
			printf("Throw '%c'\n", (uint8_t) c);
		else
			printf("Throw 0x%x\n", (uint8_t) c);
#endif
	} while ((uint8_t) c != x);
}

int at_verify_response(FILE * stream, char *expected, uint16_t timeout)
{
	size_t expected_len = strlen(expected);
	size_t len = 0;
	int ret = 0;
	int c;

	do {
		c = at_getc(stream, timeout);
		if (c < 0)
			return c;

		if (expected[len] != (uint8_t) c) {
#ifdef D
			if (c >= 32 && c < 127)
				printf("Expected %dth char '%c' but got '%c'\n", len,
				    expected[len], (uint8_t) c);
			else
				printf("Expected %dth char 0x%x but got 0x%x\n", len,
				    expected[len], (uint8_t) c);
#endif
			at_discard_until(stream, '\n');
			ret = -EINVAL;
			break;
		}

		len++;
	} while (len < expected_len);

	return ret;
}

int at_cmd(FILE * stream, char *cmd)
{
	int c;
	fputs(cmd, stream);
	fputc('\r', stream);

	c = at_verify_response(stream, cmd, 1 * HZ);
	switch (c) {
	case -EINVAL:
		/* XXX: Flush RX ringbuf */
		break;

	case -ETIMEOUT:
		/* Not much we can do .. */
		break;

	default:
		c = at_getc(stream, 1 * HZ);
		if (c < 0)
			return c;
		if ((uint8_t) c == '\r')
			return 0;
		else
			return -EINVAL;
	}

	return c;
}

int at_response(FILE * stream, char *buf, size_t buf_len, uint16_t timeout)
{
	int ret;
	size_t len = 0;
	int c;

	ret = at_verify_response(stream, "\r\n", timeout);
	if (ret)
		return ret;

	D(printf("Response: "));
	do {
		c = at_getc(stream, timeout);
		if (c < 0) {
			D(printf("Timeout!\n"));
			return -EURUN;
		}

		buf[len++] = (uint8_t) c;

		D(printf("0x%x ", (uint8_t) c));
		if (len >= 2 && buf[len - 1] == '\n' && buf[len - 2] == '\r') {
			D(printf("\n"));
			buf[len - 2] = '\0';	/* \r */
			buf[len - 1] = '\0';	/* \n */
			return len - 2;
		}
	} while (len < buf_len);

	D(printf("GSM buf too small!\n"));
	buf[0] = '\0';
	at_discard_until(stream, '\n');

	return -ENOMEM;
}

int at_info(FILE * stream, char *cmd, char *buf, size_t buflen, uint16_t timeout)
{
	int ret;
	int size;

	ret = at_cmd(stream, cmd);
	if (ret)
		return ret;

	size = at_response(stream, buf, buflen, timeout);
	if (size <= 0) {
		at_discard_until(stream, '\n');
		return size;
	}

	if (size >= strlen(AT_ERROR) && !strcmp(buf, AT_ERROR))
		return -EFAULT;

	ret = at_verify_response(stream, "\r\nOK\r\n", 1 * HZ);
	if (ret)
		return ret;

	return size;
}

int at_simple(FILE * stream, char *cmd, uint16_t timeout)
{
	int ret;

	ret = at_cmd(stream, cmd);
	if (ret)
		return ret;

	return at_verify_response(stream, "\r\nOK\r\n", timeout);
}
