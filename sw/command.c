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

	buflen -= CMD_SEP_LEN;
	switch (err) {
		case -ENOENT:
			msg = "Command not found";
			break;

		case -ENOMEM:
			msg = "Not enough memory";
			break;

		default:
			msg = "Command failed (unknown error)";
	}

	len = buflen < strlen(msg) ? buflen : strlen(msg);

	strncat(buf, msg, len);
	strcat(buf, CONFIG_CMDLINE_SEPARATOR);
	return len + CMD_SEP_LEN;
}

static int __cmd_exec(struct command *cmdlist[], char *cmd, char *buf, 
	size_t buflen)
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

			/* Do we have enough buffer space to execute? */
			if (buflen < CMD_SEP_LEN)
				return -ENOMEM;

			arg = ltrim(arg);
			written = acmd->exec(arg, buf, buflen - CMD_SEP_LEN); 
			if (written > 0) {
				strcat(buf, CONFIG_CMDLINE_SEPARATOR);
				written++;
			}

			return written;
		}
	}

	return -ENOENT;
}

void cmd_exec(struct command *cmdlist[], char *in, char *buf, size_t buflen)
{
	char *bufpos = buf;
	int written;
	char *token;

	buflen--; 	/* Reserve NULL terminator */

	token = strtok(in, CONFIG_CMDLINE_SEPARATOR);
	while (token) {
		written = __cmd_exec(cmdlist, token, bufpos, buflen);
		if (written < 0)
			written = cmd_error(written, bufpos, buflen);

		bufpos += written;
		buflen -= written;
		token = strtok(NULL, CONFIG_CMDLINE_SEPARATOR);
	}
}

