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

#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

#include "at.h"
#include "errno.h"

static FILE *f_gsm;

#define GSM_DEVICE "/dev/ttyUSB1"
#define APN "online.telia.se"
#define SERVER_HOST "www.mag32.net"
#define SERVER_PORT "80"

#define NO_CARRIER "\r\nNO CARRIER\r\n"

#define ASSERT(cond, msg, ...) do {	\
	if (!(cond)) {					\
		printf("%s:", __func__);	\
		printf(msg, __VA_ARGS__);	\
		printf("\n");				\
		return false;				\
	} 								\
} while (0)

static bool test_basic_at(void)
{
	int ret;

	ret = at_simple(f_gsm, "AT", 1 * HZ);
	ASSERT(ret == 0, "AT command (%d)", ret);

	printf("%s: OK!\n", __func__);
	return true;
}

static bool test_basic_info(void)
{
	int ret;
	char buf[100];

	ret = at_info(f_gsm, "AT+CGMR", buf, sizeof(buf), 1 * HZ);
	ASSERT(ret > 0, "AT+CGMR (%d)", ret);

	printf("%s: OK! AT+CGMR = '%s' (%d bytes)\n", __func__, buf, ret);
	return true;
}

static bool test_invalid_info(void)
{
	int ret;
	char buf[100];

	ret = at_info(f_gsm, "AT+INVALIDCOMMAND", buf, sizeof(buf), 1 * HZ);
	ASSERT(ret == -EFAULT, "AT+INVALIDCOMMAND (%d)", ret);

	printf("%s: OK!\n", __func__);
	return true;
}

static bool wait_gsm_status(int status)
{
	int gsm_status;
	int attempt;
	int len;
	char buf[12];

	attempt = 0;
	while (attempt < 10) {
		len = at_info(f_gsm, "AT+CREG?", buf, sizeof(buf), 1 * HZ);
		ASSERT(len > 0, "AT+CREG? (%d), attempt %d", len, attempt);
		sscanf(buf, "+CREG: 0,%d", &gsm_status);

		if (gsm_status == status)
			return true;

		switch (gsm_status) {
		case 0:
			printf("%s: gsm not registered, idle\n", __func__);
			break;
		case 2:
			printf("%s: not registered, searching\n", __func__);
			break;
		case 3:
			printf("%s: not registered, denied\n", __func__);
			break;
		case 4:
			printf("%s: not registered, unknown\n", __func__);
			break;
		case 5:
			printf("%s: not registered, roaming\n", __func__);
			break;
		}

		usleep(1000000);
		attempt++;
	}

	return false;
}

static bool activate_gprs(void)
{
	int len;
	int gprs_status;
	char buf[50];

	len = at_info(f_gsm, "AT#GPRS?", buf, sizeof(buf), 10 * HZ);
	ASSERT(len == 8, "AT#GPRS? (%d)\n", len);
	len = sscanf(buf, "#GPRS: %d", &gprs_status);
	ASSERT(len == 1, "sscanf %d", len);

	if (gprs_status == 0) {
		char ip[20];

		len = at_info(f_gsm, "AT#GPRS=1", buf, sizeof(buf), 10 * HZ);
		ASSERT(len > 5, "AT#GPRS first activate (%d)", len);
		ASSERT(strncmp(buf, "+IP: ", 5) == 0, "buf mismatch '%s'", buf);
		len = sscanf(buf, "+IP: %s", ip);
		ASSERT(len == 1, "sscanf(IP) = %d", len);
		printf("%s: IP is '%s'\n", __func__, ip);
	} else
		printf("%s: IP already assigned\n", __func__);

	return true;
}

static bool test_multiple_commands(void)
{
	int len;
	char buf[100];

	if (!wait_gsm_status(1))
		return false;

	len = at_simple(f_gsm, "AT+CGDCONT=1,\"IP\",\"" APN "\"", 1 * HZ);
	ASSERT(len == 0, "AT+CGDCONT (%d)\n", len);

	len = at_simple(f_gsm, "AT#USERID=\"\"", 1 * HZ);
	ASSERT(len == 0, "AT#USERID (%d)\n", len);

	len = at_simple(f_gsm, "AT#PASSW=\"\"", 1 * HZ);
	ASSERT(len == 0, "AT#USERID (%d)\n", len);

	if (!activate_gprs())
		return false;

	/* GPRS should be active at this point. Activating it again should result
	   in EFAULT. Verify! */
	len = at_info(f_gsm, "AT#GPRS=1", buf, sizeof(buf), 10 * HZ);
	ASSERT(len == -EFAULT, "AT#GPRS=1 (already active) (%d)", len);

	printf("%s: OK!\n", __func__);
	return true;
}

static int at_recv(char *buf, size_t * buf_len)
{
	int ret;
	size_t size = 0;
	char *p = buf;
	int attempts = 20;

	while (attempts--) {
		ret = fread(p, 1, (size_t) (*buf_len - size), f_gsm);
		if (ret > 0) {
			p += ret;
			size += ret;
			if (!memcmp(p - strlen(NO_CARRIER), NO_CARRIER,
				strlen(NO_CARRIER))) {
				/* Socket hung up */
				*buf_len = size - strlen(NO_CARRIER);
				return 0;
			}

			if (size == *buf_len) {
				/* Buffer is filled up */
				printf("%s: buffer overflow\n", __func__);
				return -ENOMEM;
			}
		}

		usleep(1000000);
	}

	*buf_len = size;
	return -ETIMEOUT;
}

static bool test_rawdata_transfer(void)
{
	int ret;
	char http_req[] = "GET /404.file HTTP/1.0\r\nHost: " SERVER_HOST "\r\n\r\n";
	char http_resp_expected[] = "HTTP/1.0 404 Not Found";
	char http_resp[1024 * 100];
	size_t resp_size = sizeof(http_resp);

	if (!wait_gsm_status(1))
		return false;

	if (!activate_gprs())
		return false;

	ret = at_cmd(f_gsm, "AT#SKTD=0," SERVER_PORT ",\"" SERVER_HOST "\",0,0");
	ASSERT(ret == 0, "at_cmd(AT#SKTD) = %d\n", ret);

	ret = at_verify_response(f_gsm, "\r\nCONNECT\r\n", 60 * HZ);
	ASSERT(ret == 0, "at_verify_response(CONNECT) = %d\n", ret);

	ret = fwrite(http_req, 1, sizeof(http_req) - 1, f_gsm);
	ASSERT(ret == sizeof(http_req) - 1, "fwrite(HTTP) = %d\n", ret);

	ret = at_recv(http_resp, &resp_size);
	ASSERT((ret == 0 || ret == -ETIMEOUT) && resp_size > 100,
	    "at_recv(http_resp) = %d\n", ret);

	http_resp[resp_size] = '\0';
	ret = memcmp(http_resp, http_resp_expected, strlen(http_resp_expected));
	ASSERT(ret == 0, "HTTP response mismatch, buf = %s", http_resp);

	printf("%s: HTTP response from %s OK\n", __func__, SERVER_HOST);

	return true;
}

int main(void)
{
	int fd;
	struct termios t;
	size_t bufsize = CONFIG_GSM_RX_BUF < CONFIG_GSM_TX_BUF ? CONFIG_GSM_RX_BUF :
	    CONFIG_GSM_TX_BUF;

	printf("Please connect the GSM modem '%s' and power it up.\n", GSM_DEVICE);
	getchar();

	fd = open(GSM_DEVICE, 0, O_RDWR);
	if (fd == -1) {
		perror("open serial");
		return EXIT_FAILURE;
	}

	tcgetattr(fd, &t);
	t.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL
	    | IXON);
	t.c_oflag &= ~OPOST;
	t.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	t.c_cflag &= ~(CSIZE | PARENB);
	t.c_cflag |= CS8;
	t.c_iflag = IGNBRK | IGNPAR;
	t.c_cflag |= CLOCAL;

	/* Make stream non-blocking, AT module assumes it is */
	t.c_cc[VMIN] = 0;
	t.c_cc[VTIME] = 0;
	cfsetospeed(&t, CONFIG_GSM_BAUDRATE);
	cfsetispeed(&t, CONFIG_GSM_BAUDRATE);
	tcsetattr(fd, TCSANOW, &t);

	close(fd);

	f_gsm = fopen(GSM_DEVICE, "r+b");
	if (!f_gsm) {
		perror("open stream");
		return EXIT_FAILURE;
	}

	if (setvbuf(f_gsm, NULL, _IOLBF, bufsize)) {
		perror("stream buffering");
		return EXIT_FAILURE;
	}

	if (!test_basic_at())
		return EXIT_FAILURE;

	if (!test_basic_info())
		return EXIT_FAILURE;

	if (!test_invalid_info())
		return EXIT_FAILURE;

	if (!test_multiple_commands())
		return EXIT_FAILURE;

	if (!test_rawdata_transfer())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
