/*
 * Copyright (c) 1983, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 1983, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)lpd.c	8.7 (Berkeley) 5/10/95";
#endif
static const char rcsid[] =
	"$Id: lpd.c,v 1.3.2.5 1997/09/25 06:32:39 charnier Exp $";
#endif /* not lint */

/*
 * lpd -- line printer daemon.
 *
 * Listen for a connection and perform the requested operation.
 * Operations are:
 *	\1printer\n
 *		check the queue for jobs and print any found.
 *	\2printer\n
 *		receive a job from another machine and queue it.
 *	\3printer [users ...] [jobs ...]\n
 *		return the current state of the queue (short form).
 *	\4printer [users ...] [jobs ...]\n
 *		return the current state of the queue (long form).
 *	\5printer person [users ...] [jobs ...]\n
 *		remove jobs from the queue.
 *
 * Strategy to maintain protected spooling area:
 *	1. Spooling area is writable only by daemon and spooling group
 *	2. lpr runs setuid root and setgrp spooling group; it uses
 *	   root to access any file it wants (verifying things before
 *	   with an access call) and group id to know how it should
 *	   set up ownership of files in the spooling area.
 *	3. Files in spooling area are owned by root, group spooling
 *	   group, with mode 660.
 *	4. lpd, lpq and lprm run setuid daemon and setgrp spooling group to
 *	   access files and printer.  Users can't get to anything
 *	   w/o help of lpq and lprm programs.
 */

#include <sys/param.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <ctype.h>
#include "lp.h"
#include "lp.local.h"
#include "pathnames.h"
#include "extern.h"

int	lflag;				/* log requests flag */
int	from_remote;			/* from remote socket */

static void       reapchild __P((int));
static void       mcleanup __P((int));
static void       doit __P((void));
static void       startup __P((void));
static void       chkhost __P((struct sockaddr *));
static int	  ckqueue __P((char *));
static void	  usage __P((void));

uid_t	uid, euid;

u_char family = AF_INET;

int
main(argc, argv)
	int argc;
	char **argv;
{
	int f, funix, finet, finet6, options, fromlen, i, errs;
	fd_set defreadfds;
	struct sockaddr_un un, fromunix;
	struct sockaddr_in sin, frominet;
	struct sockaddr_in6 sin6, frominet6;
	int omask, lfd;
	struct servent *sp, serv;
	int inet_flag = 0, inet6_flag = 0;

	euid = geteuid();	/* these shouldn't be different */
	uid = getuid();
	options = 0;
	gethostname(host, sizeof(host));

	name = "lpd";

	if (euid != 0)
		errx(EX_NOPERM,"must run as root");

	errs = 0;
	while ((i = getopt(argc, argv, "dl46")) != -1)
		switch (i) {
		case 'd':
			options |= SO_DEBUG;
			break;
		case 'l':
			lflag++;
			break;
		case '4':
			family = AF_INET;
			inet_flag++;
			break;
		case '6':
			family = AF_INET6;
			inet6_flag++;
			break;
		default:
			errs++;
		}
	if (inet_flag && inet6_flag)
		family = AF_UNSPEC;
	argc -= optind;
	argv += optind;
	if (errs)
		usage();

	if (argc == 1) {
		if ((i = atoi(argv[0])) == 0)
			usage();
		if (i < 0 || i > USHRT_MAX)
			errx(EX_USAGE, "port # %d is invalid", i);

		serv.s_port = htons(i);
		sp = &serv;
		argc--;
	} else {
		sp = getservbyname("printer", "tcp");
		if (sp == NULL)
			errx(EX_OSFILE, "printer/tcp: unknown service");
	}

	if (argc != 0)
		usage();

#ifndef DEBUG
	/*
	 * Set up standard environment by detaching from the parent.
	 */
	daemon(0, 0);
#endif

	openlog("lpd", LOG_PID, LOG_LPR);
	syslog(LOG_INFO, "restarted");
	(void) umask(0);
	lfd = open(_PATH_MASTERLOCK, O_WRONLY|O_CREAT, 0644);
	if (lfd < 0) {
		syslog(LOG_ERR, "%s: %m", _PATH_MASTERLOCK);
		exit(1);
	}
	if (flock(lfd, LOCK_EX|LOCK_NB) < 0) {
		if (errno == EWOULDBLOCK)	/* active deamon present */
			exit(0);
		syslog(LOG_ERR, "%s: %m", _PATH_MASTERLOCK);
		exit(1);
	}
	ftruncate(lfd, 0);
	/*
	 * write process id for others to know
	 */
	sprintf(line, "%u\n", getpid());
	f = strlen(line);
	if (write(lfd, line, f) != f) {
		syslog(LOG_ERR, "%s: %m", _PATH_MASTERLOCK);
		exit(1);
	}
	signal(SIGCHLD, reapchild);
	/*
	 * Restart all the printers.
	 */
	startup();
	(void) unlink(_PATH_SOCKETNAME);
	funix = socket(AF_UNIX, SOCK_STREAM, 0);
	if (funix < 0) {
		syslog(LOG_ERR, "socket: %m");
		exit(1);
	}
#define	mask(s)	(1 << ((s) - 1))
	omask = sigblock(mask(SIGHUP)|mask(SIGINT)|mask(SIGQUIT)|mask(SIGTERM));
	(void) umask(07);
	signal(SIGHUP, mcleanup);
	signal(SIGINT, mcleanup);
	signal(SIGQUIT, mcleanup);
	signal(SIGTERM, mcleanup);
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, _PATH_SOCKETNAME);
#ifndef SUN_LEN
#define SUN_LEN(unp) (strlen((unp)->sun_path) + 2)
#endif
	if (bind(funix, (struct sockaddr *)&un, SUN_LEN(&un)) < 0) {
		syslog(LOG_ERR, "ubind: %m");
		exit(1);
	}
	(void) umask(0);
	sigsetmask(omask);
	FD_ZERO(&defreadfds);
	FD_SET(funix, &defreadfds);
	listen(funix, 5);
    if (family == AF_INET || family == AF_UNSPEC) {
	finet = socket(AF_INET, SOCK_STREAM, 0);
	if (finet >= 0) {
		if (options & SO_DEBUG)
			if (setsockopt(finet, SOL_SOCKET, SO_DEBUG, 0, 0) < 0) {
				syslog(LOG_ERR, "setsockopt (SO_DEBUG): %m");
				mcleanup(0);
			}
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = sp->s_port;
		if (bind(finet, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
			syslog(LOG_ERR, "bind: %m");
			mcleanup(0);
		}
		FD_SET(finet, &defreadfds);
		listen(finet, 5);
	}
    }
    if (family == AF_INET6 || family == AF_UNSPEC) {
	finet6 = socket(AF_INET6, SOCK_STREAM, 0);
	if (finet6 >= 0) {
		if (options & SO_DEBUG)
			if (setsockopt(finet6, SOL_SOCKET, SO_DEBUG, 0, 0) < 0) {
				syslog(LOG_ERR, "setsockopt (SO_DEBUG): %m");
				mcleanup(0);
			}
		memset(&sin6, 0, sizeof(sin6));
		sin6.sin6_family = AF_INET6;
		sin6.sin6_port = sp->s_port;
		if (bind(finet6, (struct sockaddr *)&sin6, sizeof(sin6)) < 0) {
			syslog(LOG_ERR, "bind: %m");
			mcleanup(0);
		}
		FD_SET(finet6, &defreadfds);
		listen(finet6, 5);
	}
    }
	/*
	 * Main loop: accept, do a request, continue.
	 */
	memset(&frominet, 0, sizeof(frominet));
	memset(&frominet6, 0, sizeof(frominet6));
	memset(&fromunix, 0, sizeof(fromunix));
	for (;;) {
		int domain, nfds, s;
		fd_set readfds;

		FD_COPY(&defreadfds, &readfds);
		nfds = select(20, &readfds, 0, 0, 0);
		if (nfds <= 0) {
			if (nfds < 0 && errno != EINTR)
				syslog(LOG_WARNING, "select: %m");
			continue;
		}
		if (FD_ISSET(funix, &readfds)) {
			domain = AF_UNIX, fromlen = sizeof(fromunix);
			s = accept(funix,
			    (struct sockaddr *)&fromunix, &fromlen);
		} else if (finet6 >= 0 && FD_ISSET(finet6, &readfds))  {
			domain = AF_INET6, fromlen = sizeof(frominet6);
			s = accept(finet6,
			    (struct sockaddr *)&frominet6, &fromlen);
			if (frominet6.sin6_port == htons(20)) {
				close(s);
				continue;
			}
		} else if (finet >= 0 && FD_ISSET(finet, &readfds)) {
			domain = AF_INET, fromlen = sizeof(frominet);
			s = accept(finet,
			    (struct sockaddr *)&frominet, &fromlen);
			if (frominet.sin_port == htons(20)) {
				close(s);
				continue;
			}
		}
		if (s < 0) {
			if (errno != EINTR)
				syslog(LOG_WARNING, "accept: %m");
			continue;
		}
		if (fork() == 0) {
			signal(SIGCHLD, SIG_IGN);
			signal(SIGHUP, SIG_IGN);
			signal(SIGINT, SIG_IGN);
			signal(SIGQUIT, SIG_IGN);
			signal(SIGTERM, SIG_IGN);
			(void) close(funix);
			if (finet >= 0)
				(void) close(finet);
			if (finet6 >= 0)
				(void) close(finet6);
			dup2(s, 1);
			(void) close(s);
			if (domain == AF_INET || domain == AF_INET6) {
				from_remote = 1;
				chkhost((domain == AF_INET ?
					(struct sockaddr *) &frominet :
					(struct sockaddr *) &frominet6));
			} else
				from_remote = 0;
			doit();
			exit(0);
		}
		(void) close(s);
	}
}

static void
reapchild(signo)
	int signo;
{
	union wait status;

	while (wait3((int *)&status, WNOHANG, 0) > 0)
		;
}

static void
mcleanup(signo)
	int signo;
{
	if (lflag)
		syslog(LOG_INFO, "exiting");
	unlink(_PATH_SOCKETNAME);
	exit(0);
}

/*
 * Stuff for handling job specifications
 */
char	*user[MAXUSERS];	/* users to process */
int	users;			/* # of users in user array */
int	requ[MAXREQUESTS];	/* job number of spool entries */
int	requests;		/* # of spool requests */
char	*person;		/* name of person doing lprm */

char	fromb[MAXHOSTNAMELEN];	/* buffer for client's machine name */
char	cbuf[BUFSIZ];		/* command line buffer */
char	*cmdnames[] = {
	"null",
	"printjob",
	"recvjob",
	"displayq short",
	"displayq long",
	"rmjob"
};

static void
doit()
{
	register char *cp;
	register int n;

	for (;;) {
		cp = cbuf;
		do {
			if (cp >= &cbuf[sizeof(cbuf) - 1])
				fatal("Command line too long");
			if ((n = read(1, cp, 1)) != 1) {
				if (n < 0)
					fatal("Lost connection");
				return;
			}
		} while (*cp++ != '\n');
		*--cp = '\0';
		cp = cbuf;
		if (lflag) {
			if (*cp >= '\1' && *cp <= '\5')
				syslog(LOG_INFO, "%s requests %s %s",
					from, cmdnames[*cp], cp+1);
			else
				syslog(LOG_INFO, "bad request (%d) from %s",
					*cp, from);
		}
		switch (*cp++) {
		case '\1':	/* check the queue and print any jobs there */
			printer = cp;
			printjob();
			break;
		case '\2':	/* receive files to be queued */
			if (!from_remote) {
				syslog(LOG_INFO, "illegal request (%d)", *cp);
				exit(1);
			}
			printer = cp;
			recvjob();
			break;
		case '\3':	/* display the queue (short form) */
		case '\4':	/* display the queue (long form) */
			printer = cp;
			while (*cp) {
				if (*cp != ' ') {
					cp++;
					continue;
				}
				*cp++ = '\0';
				while (isspace(*cp))
					cp++;
				if (*cp == '\0')
					break;
				if (isdigit(*cp)) {
					if (requests >= MAXREQUESTS)
						fatal("Too many requests");
					requ[requests++] = atoi(cp);
				} else {
					if (users >= MAXUSERS)
						fatal("Too many users");
					user[users++] = cp;
				}
			}
			displayq(cbuf[0] - '\3');
			exit(0);
		case '\5':	/* remove a job from the queue */
			if (!from_remote) {
				syslog(LOG_INFO, "illegal request (%d)", *cp);
				exit(1);
			}
			printer = cp;
			while (*cp && *cp != ' ')
				cp++;
			if (!*cp)
				break;
			*cp++ = '\0';
			person = cp;
			while (*cp) {
				if (*cp != ' ') {
					cp++;
					continue;
				}
				*cp++ = '\0';
				while (isspace(*cp))
					cp++;
				if (*cp == '\0')
					break;
				if (isdigit(*cp)) {
					if (requests >= MAXREQUESTS)
						fatal("Too many requests");
					requ[requests++] = atoi(cp);
				} else {
					if (users >= MAXUSERS)
						fatal("Too many users");
					user[users++] = cp;
				}
			}
			rmjob();
			break;
		}
		fatal("Illegal service request");
	}
}

/*
 * Make a pass through the printcap database and start printing any
 * files left from the last time the machine went down.
 */
static void
startup()
{
	char *buf;
	register char *cp;
	int pid;
	char *spooldirs[16];        /* Which spooldirs are active? */
	int i;                      /* Printer index presently processed */
	int j;                      /* Printer index of potential conflict */
	char *spooldir;             /* Spooldir of present printer */
	int  canfreespool;          /* Is the spooldir malloc()ed? */

	/*
	 * Restart the daemons and test for spooldir conflict.
	 */
	i = 0;
	while (cgetnext(&buf, printcapdb) > 0) {

		/* Check for duplicate spooldirs */
		canfreespool = 1;
		if (cgetstr(buf, "sd", &spooldir) <= 0) {
			spooldir = _PATH_DEFSPOOL;
			canfreespool = 0;
		}
		if (i < sizeof(spooldirs)/sizeof(spooldirs[0]))
			spooldirs[i] = spooldir;
		for (j = 0;
			 j < MIN(i,sizeof(spooldirs)/sizeof(spooldirs[0]));
			 j++) {
			if (strcmp(spooldir, spooldirs[j]) == 0) {
				syslog(LOG_ERR,
					"startup: duplicate spool directories: %s",
					spooldir);
				mcleanup(0);
			}
		}
		if (canfreespool && i >= sizeof(spooldirs)/sizeof(spooldirs[0]))
			free(spooldir);
		i++;
		/* Spooldir test done */

		if (ckqueue(buf) <= 0) {
			free(buf);
			continue;	/* no work to do for this printer */
		}
		for (cp = buf; *cp; cp++)
			if (*cp == '|' || *cp == ':') {
				*cp = '\0';
				break;
			}
		if (lflag)
			syslog(LOG_INFO, "work for %s", buf);
		if ((pid = fork()) < 0) {
			syslog(LOG_WARNING, "startup: cannot fork");
			mcleanup(0);
		}
		if (!pid) {
			printer = buf;
			cgetclose();
			printjob();
			/* NOTREACHED */
		}
		else free(buf);
	}
}

/*
 * Make sure there's some work to do before forking off a child
 */
static int
ckqueue(cap)
	char *cap;
{
	register struct dirent *d;
	DIR *dirp;
	char *spooldir;

	if (cgetstr(cap, "sd", &spooldir) == -1)
		spooldir = _PATH_DEFSPOOL;
	if ((dirp = opendir(spooldir)) == NULL)
		return (-1);
	while ((d = readdir(dirp)) != NULL) {
		if (d->d_name[0] != 'c' || d->d_name[1] != 'f')
			continue;	/* daemon control files only */
		closedir(dirp);
		return (1);		/* found something */
	}
	closedir(dirp);
	return (0);
}

#define DUMMY ":nobody::"

/*
 * Check to see if the from host has access to the line printer.
 */
static void
chkhost(f)
	struct sockaddr *f;
{
	register struct hostent *hp;
	register FILE *hostf;
	int first = 1;
	int good = 0;
	int err;
	char numerichost[INET6_ADDRSTRLEN];
	caddr_t addr = f->sa_family == AF_INET ?
			(caddr_t) &((struct sockaddr_in *) f)->sin_addr :
			(caddr_t) &((struct sockaddr_in6 *) f)->sin6_addr;

	/* Need real hostname for temporary filenames */
	err = getnameinfo(f, f->sa_len, numerichost, sizeof(numerichost), 0, 0, NI_NUMERICHOST);
	if (err) {
		syslog(LOG_ERR, "getnameinfo: failed %d", err);
		exit(1);
	}
	err = getnameinfo(f, f->sa_len, fromb, sizeof(fromb), 0, 0, NI_NAMEREQD);
	if (err)
		fatal("Host name for your address (%s) unknown", numerichost);
	from = fromb;

	/* Check for spoof, ala rlogind */
	hp = gethostbyname2(fromb, f->sa_family);
	if (!hp)
		fatal("hostname for your address (%s) unknown", numerichost);
	for (; good == 0 && hp->h_addr_list[0] != NULL; hp->h_addr_list++) {
		if (!bcmp(hp->h_addr_list[0], addr, hp->h_length))
			good = 1;
	}
	if (good == 0) {
		fatal("address for your hostname (%s) not matched", numerichost);
	}

	hostf = fopen(_PATH_HOSTSEQUIV, "r");
again:
	if (hostf) {
		if (__ivaliduser_af(hostf, addr,
		    DUMMY, DUMMY,
		    hp->h_addrtype, hp->h_length) == 0) {
			(void) fclose(hostf);
			return;
		}
		(void) fclose(hostf);
	}
	if (first == 1) {
		first = 0;
		hostf = fopen(_PATH_HOSTSLPD, "r");
		goto again;
	}
	fatal("Your host does not have line printer access");
	/*NOTREACHED*/
}

static void
usage()
{
	fprintf(stderr, "usage: lpd [-dl] [port#]\n");
	exit(EX_USAGE);
}
