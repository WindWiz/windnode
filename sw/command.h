#ifndef _COMMAND_H_

#include <stdint.h>

struct command {
	char *name;
	uint8_t namelen;
	int (*exec)(char *arg, char *buf, size_t buflen);
};

#define CREATE_COMMAND(cmd, exec) \
	(&(struct command) __COMMAND_INITIALIZER(cmd, exec))

#define COMMANDLIST_END (NULL)

#define DECLARE_COMMAND(name, cmd, exec ) \
	struct command name = __COMMAND_INITIALIZER(cmd, exec)

#define __COMMAND_INITIALIZER(cmd, exec) \
	{ cmd, sizeof(cmd) - 1, exec }

void cmd_exec(struct command *cmds[], char *in, char *buf, size_t buflen);


#endif
