#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "command.h"
#include "errno.h"

#define CMD_SEP_LEN strlen(CONFIG_CMDLINE_SEPARATOR)

static char *ltrim(char *str)
{
	while (*str == ' ')
		str++;

	return str;
}

static int cmd_error(int err, char *buf, size_t buflen)
{
	char *msg;
	int len;

	/* No space left, not much we can do .. */
	if (buflen == 0)
		return 0;

	switch (err) {
	case -ENOENT:
		msg = "ENOENT";
		break;

	case -ENOMEM:
		msg = "ENOMEM";
		break;

	case -EINVAL:
		msg = "EINVAL";
		break;

	case -ETIMEOUT:
		msg = "ETIMEOUT";
		break;

	case -EURUN:
		msg = "EURUN";
		break;

	default:
		msg = "UNKNOWN";
	}

	len = buflen < strlen(msg) ? buflen : strlen(msg);
	strncpy(buf, msg, len);

	return len;
}

static int __cmd_exec(struct command *cmdlist[], char *cmd, char *buf, size_t buflen)
{
	struct command *acmd;
	int cmdlen;
	int i = 0;

	if (buflen == 0)
		return -ENOMEM;

	cmd = ltrim(cmd);
	cmdlen = strlen(cmd);

	if (cmdlen == 0)
		return 0;

	while (cmdlist[i]) {
		acmd = cmdlist[i];

		i++;

		if (cmdlen < acmd->namelen)
			continue;

		if (!strncmp(acmd->name, cmd, acmd->namelen)) {
			char *arg = cmd + acmd->namelen;
			int written;

			arg = ltrim(arg);
			written = acmd->exec(arg, buf, buflen);
			return (written > (int) buflen) ? buflen : written;
		}
	}

	return -ENOENT;
}

void cmd_exec(struct command *cmdlist[], char *in, char *buf, size_t buflen)
{
	char *bufpos = buf;
	int written;
	char *token;

	buflen--;		/* Reserve NULL terminator */

	buf[0] = '\0';		/* Reset output */
	token = strtok(in, CONFIG_CMDLINE_SEPARATOR);
	while (token) {
		written = __cmd_exec(cmdlist, token, bufpos, buflen - CMD_SEP_LEN);
		if (written < 0)
			written = cmd_error(written, bufpos, buflen - CMD_SEP_LEN);

		strcpy(&bufpos[written], CONFIG_CMDLINE_SEPARATOR);
		written += CMD_SEP_LEN;

		bufpos += written;
		buflen -= written;
		token = strtok(NULL, CONFIG_CMDLINE_SEPARATOR);

		if (buflen == 0)
			break;
	}
}
