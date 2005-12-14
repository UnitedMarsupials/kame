/*	$KAME: command.c,v 1.3 2005/12/14 08:17:51 t-momose Exp $	*/

/*
 * Copyright (C) 2004 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <syslog.h>
#include <unistd.h>
#include <ctype.h>
#include <poll.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "fdlist.h"
#include "command.h"

static struct sockaddr_in6 sin6_ci;
char *prompt = "> ";

void command_help(int, char *);
void quit_ui(int, char *);
int command_in(int);
int new_connection(int);
#define disp_prompt(s)	write((s), prompt, strlen(prompt))
static void dispatch_command(int, char *, struct command_table *);

struct command_table basic_command_table[] = {
	{"help", command_help, "Show help"},
	{"?", command_help, "Show help"},
	{"quit", quit_ui, "Quit the shell"},
};
struct command_table *commands;

int
command_init(p, cmdset, cmdset_size, port)
	char *p;
	struct command_table *cmdset;
	size_t cmdset_size;
	u_short port;
{
	int i, s;
	int s_optval = 1;
	struct command_table *c;

	s = socket(PF_INET6, SOCK_STREAM, 0);
	if (s < 0) {
		perror("command: socket");
		return (-1);
	}

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
			&s_optval, sizeof(s_optval)) == -1) {
		perror("command: setsockopt");
		return (-1);
	}

	/* Configuration channel is bound to only IPv6 */
	bzero(&sin6_ci, sizeof(sin6_ci));
	sin6_ci.sin6_family = AF_INET6;
	sin6_ci.sin6_len = sizeof(sin6_ci);
	sin6_ci.sin6_addr = in6addr_loopback;
	sin6_ci.sin6_port = htons(port);
	if (bind(s, (struct sockaddr *)&sin6_ci, sizeof(sin6_ci)) < 0) {
		perror("command: bind");
		goto bad;
	}
	if (listen(s, 1) < 0) {
		perror("command: listen");
		goto bad;
	}

	commands = malloc((cmdset_size + sizeof(basic_command_table) / sizeof(struct command_table) + 1) * sizeof(struct command_table));
	if (commands == NULL) {
		perror("command: malloc");
		goto bad;
	}
	c = commands;
	for (i = 0; i < sizeof(basic_command_table) / sizeof(struct command_table); i++)
		*c++ = basic_command_table[i];
	for (i = 0; i < cmdset_size; i++)
		*c++ = cmdset[i];
	bzero(c, sizeof(struct command_table));

	new_fd_list(s, POLLIN, new_connection);
	prompt = p;

	return (s);

 bad:
	close(s);
	return (-1);
}

int
new_connection(s)
	int s;
{
	int ss;
	struct sockaddr_in6 sin6;
	size_t sin6len;

	sin6len = sizeof(struct sockaddr_in6);
	if ((ss = accept(s, (struct sockaddr *)&sin6, &sin6len)) < 0) {
		perror("command: accept");
		return -1;
	}
	
	new_fd_list(ss, POLLIN, command_in);
	disp_prompt(ss);
	return (0);
}

int
command_in(s)
	int s;
{
	int bytes;
	char buffer[2048];
	
	bytes = read(s, buffer, 2048);

	buffer[bytes] = '\0';
	while (strlen(buffer) && isspace(buffer[strlen(buffer) - 1]))
		buffer[strlen(buffer) - 1] = '\0';
	if (strlen(buffer) > 0)
		dispatch_command(s, buffer, commands);

	disp_prompt(s);

	return (0);
}

static void
dispatch_command(s, command_line, command_table)
	int s;
	char *command_line;
	struct command_table *command_table;
{
	char *arg;
	struct command_table *ctbl;
	char *errmsg = "??? unknown command\n";
	
	if ((strncmp(command_line, "help", 4) == 0) ||
	    (strncmp(command_line, "?", 1) == 0)) {
		command_help(s, (char *)command_table);
		return;
	}
	
	for (ctbl = command_table; ctbl->command != NULL; ctbl++) {
		if ((strncmp(ctbl->command, command_line, strlen(ctbl->command)) != 0))
			continue;

		arg = command_line + strlen(ctbl->command);

		while (isspace(*arg))
			arg++;

		if (ctbl->sub_cmds) {
			if (*arg == '\0')
				command_help(s, (char *)ctbl->sub_cmds);
			else
				dispatch_command(s, arg, ctbl->sub_cmds);
		} else {
			(*ctbl->cmdfunc)(s, arg);
		}
		return;
	}

	write(s, errmsg, strlen(errmsg));
	return;
}

void
command_printf(int s, const char *fmt, ...)
{
	va_list ap;
	char buffer[512];
	
	va_start(ap, fmt);
	vsnprintf(buffer, 512, fmt, ap);
	va_end(ap);
	write(s, buffer, strlen(buffer));
}

void
command_help(s, line)
	int s;
	char *line;
{
	struct command_table *ctbl, *base;

	base =(struct command_table *)line;
	
	for (ctbl = base; ctbl->command != NULL; ctbl++) {
		command_printf(s, "%-10s - %s\n",
			       ctbl->command, ctbl->helpmsg);
	}
}

void
quit_ui(s, line)
	int s;
	char *line;
{
	command_printf(s, "bye bye\n");
	delete_fd_list_entry(s);
	close(s);
}
