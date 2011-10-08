#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "errno.h"
#include "command.h"

static int cmd_pi(char *arg, char *buf, size_t buflen)
{
	if (buflen < 4) {
		return -ENOMEM;
	}

	strcat(buf, "3.14");
	return strlen(buf);
}

static DECLARE_COMMAND(pi, "PI", cmd_pi);

static int cmd_square(char *arg, char *buf, size_t buflen)
{
	unsigned long int num = strtoul(arg, NULL, 0);
	char tmp[21];

	sprintf(tmp, "%ld", num * num);

	if (strlen(tmp) > buflen)
		return -ENOMEM;

	strcat(buf, tmp);
	return strlen(tmp);
}

static DECLARE_COMMAND(square, "SQUARE", cmd_square);

static int cmd_mul(char *arg, char *buf, size_t buflen)
{
	char tmp[21];
	char *op1;
	int op1_num;
	char *op2;
	int op2_num;

	/* Split arg into two */
	op1 = arg;
	op2 = strstr(arg, " ");
	if (op2 == NULL)
		return 0;
	*op2 = '\0';
	op2++;

	op1_num = strtoul(op1, NULL, 0);
	op2_num = strtoul(op2, NULL, 0);

	sprintf(tmp, "%d", op1_num * op2_num);

	if (strlen(tmp) > buflen)
		return -ENOMEM;

	strcat(buf, tmp);
	return strlen(tmp);
}

static DECLARE_COMMAND(mul, "MUL", cmd_mul);

static struct command *commandlist[] = {
	&pi,
	&square,
	&mul,
	COMMANDLIST_END,
};

struct test {
	char *input;
	char *expected;
};

static struct test testvectors[] = {
	/* Single command */
	{"PI", "3.14;"},
	/* SHORT (nonexisting) command */
	{"H", "ENOENT;"},
	/* Space-prefixed command */
	{"  PI", "3.14;"},
	/* Space-padded command */
	{"  PI    ", "3.14;"},
	/* Space-suffixed command */
	{"PI          ;", "3.14;"},
	/* Space-padded with separator */
	{"   PI     ;", "3.14;"},
	/* Single command with separator */
	{"PI;", "3.14;"},
	/* Command with argument */
	{"SQUARE 2", "4;"},
	/* Multiple commands */
	{"PI;      SQUARE 4", "3.14;16;"},
	/* Unknown command */
	{"THIS COMMAND DOES NOT EXIST", "ENOENT;"},
	/* Multiple commands including unknown command */
	{"PI; SECOND MISSING; SQUARE 2", "3.14;ENOENT;4;"},
	/* Null-commands */
	{";;;;;PI;;;", "3.14;"},
	/* Multiple arguments */
	{"MUL 2 4", "8;"},
	/* Outbuffer overruns */
	{"PI;PI;PI;PI;PI;PI", "3.14;3.14;3.14;3.14;3.14;E;"},
	{"MUL 10000 10000; MUL 10000 10000; MUL 10000 10000;",
		    "100000000;100000000;ENOMEM;"},
	{"INVALID; COMMAND; UNKNOWN; COMMAND", "ENOENT;ENOENT;ENOENT;ENOEN;"}
};

int main(void)
{
	int i;
	char outbuf[28];

	srand(time(NULL));
	for (i = 0; i < sizeof(testvectors) / sizeof(testvectors[0]); i++) {
		struct test *t = &testvectors[i];
		char *inbuf = malloc(strlen(t->input) + 1);
		int k;

		/* Reset output buffer with some fuzzy garbage.. */
		for (k = 0; k < sizeof(outbuf); k++)
			outbuf[k] = rand() % 256;

		/* Duplicate string in writable memory for strtok */
		strcpy(inbuf, t->input);

		cmd_exec(commandlist, inbuf, outbuf, sizeof(outbuf));

		if (!strcmp(outbuf, t->expected))
			printf("'%s' -> '%s' OK!\n", t->input, outbuf);
		else {
			printf("'%s' -> '%s' (expected '%s') FAILED!\n", t->input,
			    outbuf, t->expected);

			free(inbuf);
			return EXIT_FAILURE;
		}

		free(inbuf);
	}

	return EXIT_SUCCESS;
}
