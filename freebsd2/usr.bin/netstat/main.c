/*
 * Copyright (c) 1983, 1988, 1993
 *	Regents of the University of California.  All rights reserved.
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
char const copyright[] =
"@(#) Copyright (c) 1983, 1988, 1993\n\
	Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)main.c	8.4 (Berkeley) 3/1/94";
#endif
static const char rcsid[] =
	"$Id: main.c,v 1.16.2.3 1997/08/29 05:29:39 imp Exp $";
#endif /* not lint */

#include <sys/param.h>
#include <sys/file.h>
#include <sys/protosw.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <kvm.h>
#include <limits.h>
#include <netdb.h>
#include <nlist.h>
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "netstat.h"

static struct nlist nl[] = {
#define	N_MBSTAT	0
	{ "_mbstat" },
#define	N_IPSTAT	1
	{ "_ipstat" },
#define	N_TCB		2
	{ "_tcb" },
#define	N_TCPSTAT	3
	{ "_tcpstat" },
#define	N_UDB		4
	{ "_udb" },
#define	N_UDPSTAT	5
	{ "_udpstat" },
#define	N_IFNET		6
	{ "_ifnet" },
#define	N_IMP		7
	{ "_imp_softc" },
#define	N_ICMPSTAT	8
	{ "_icmpstat" },
#define	N_RTSTAT	9
	{ "_rtstat" },
#define	N_UNIXSW	10
	{ "_localsw" },
#define N_IDP		11
	{ "_nspcb"},
#define N_IDPSTAT	12
	{ "_idpstat"},
#define N_SPPSTAT	13
	{ "_spp_istat"},
#define N_NSERR		14
	{ "_ns_errstat"},
#define	N_CLNPSTAT	15
	{ "_clnp_stat"},
#define	IN_NOTUSED	16
	{ "_tp_inpcb" },
#define	ISO_TP		17
	{ "_tp_refinfo" },
#define	N_TPSTAT	18
	{ "_tp_stat" },
#define	N_ESISSTAT	19
	{ "_esis_stat"},
#define N_NIMP		20
	{ "_nimp"},
#define N_RTREE		21
	{ "_rt_tables"},
#define N_CLTP		22
	{ "_cltb"},
#define N_CLTPSTAT	23
	{ "_cltpstat"},
#define	N_NFILE		24
	{ "_nfile" },
#define	N_FILE		25
	{ "_file" },
#define N_IGMPSTAT	26
	{ "_igmpstat" },
#define N_MRTPROTO	27
	{ "_ip_mrtproto" },
#define N_MRTSTAT	28
	{ "_mrtstat" },
#define N_MFCTABLE	29
	{ "_mfctable" },
#define N_VIFTABLE	30
	{ "_viftable" },
#define N_IPX		31
	{ "_ipxpcb"},
#define N_IPXSTAT	32
	{ "_ipxstat"},
#define N_SPXSTAT	33
	{ "_spx_istat"},
#define N_DDPSTAT	34
	{ "_ddpstat"},
#define N_DDPCB		35
	{ "_ddpcb"},
#define N_DIVPCB	36
	{ "_divcb"},
#define N_DIVSTAT	37
	{ "_divstat"},
#define N_IP6STAT	38
	{ "_ip6stat" },
#define N_TCB6		39
	{ "_tcb6" },
#define N_TCP6STAT	40
	{ "_tcp6stat" },
#define N_UDB6		41
	{ "_udb6" },
#define N_UDP6STAT	42
	{ "_udp6stat" },
#define N_ICMP6STAT	43
	{ "_icmp6stat" },
#define N_IPSECSTAT	44
	{ "_ipsecstat" },
#define N_IPSEC6STAT	45
	{ "_ipsec6stat" },
#define N_PIM6STAT	46
	{ "_pim6stat" },
#define N_MRT6PROTO	47
	{ "_ip6_mrtproto" },
#define N_MRT6STAT	48
	{ "_mrt6stat" },
#define N_MF6CTABLE	49
	{ "_mf6ctable" },
#define N_MIF6TABLE	50
	{ "_mif6table" },
#define N_PFKEYSTAT	51
	{ "_pfkeystat" },
	{ "" },
};

struct protox {
	u_char	pr_index;		/* index into nlist of cb head */
	u_char	pr_sindex;		/* index into nlist of stat block */
	u_char	pr_wanted;		/* 1 if wanted, 0 otherwise */
	void	(*pr_cblocks)();	/* control blocks printing routine */
	void	(*pr_stats)();		/* statistics printing routine */
	void	(*pr_istats)();		/* per/if statistics printing routine */
	char	*pr_name;		/* well-known name */
} protox[] = {
	{ N_TCB,	N_TCPSTAT,	1,	protopr,
	  tcp_stats,	NULL,		"tcp" },
	{ N_UDB,	N_UDPSTAT,	1,	protopr,
	  udp_stats,	NULL,		"udp" },
	{ N_DIVPCB,	N_DIVSTAT,	1,	protopr,
	  NULL,		NULL,		"divert" }, 	/* no stat structure yet */
	{ -1,		N_IPSTAT,	1,	0,
	  ip_stats,	NULL,		"ip" },
	{ -1,		N_ICMPSTAT,	1,	0,
	  icmp_stats,	NULL,		"icmp" },
	{ -1,		N_IGMPSTAT,	1,	0,
	  igmp_stats,	NULL,		"igmp" },
#ifdef IPSEC
	{ -1,		N_IPSECSTAT,	1,	0,
	  ipsec_stats,	NULL,		"ipsec" },
#endif
	{ -1,		-1,		0,	0,
	  0,		NULL,		0 }
};

#ifdef INET6
struct protox ip6protox[] = {
	{ -1,		N_IP6STAT,	1,	0,
	  ip6_stats,	ip6_ifstats,	"ip6" },
	{ -1,		N_ICMP6STAT,	1,	0,
	  icmp6_stats,	icmp6_ifstats,	"icmp6" },
	{ N_TCB6,	N_TCP6STAT,	1,	ip6protopr,
	  tcp6_stats,	NULL,		"tcp6" },
	{ N_UDB6,	N_UDP6STAT,	1,	ip6protopr,
	  udp6_stats,	NULL,		"udp6" },
#ifdef IPSEC
	{ -1,		N_IPSEC6STAT,	1,	0,
	  ipsec_stats,	NULL,		"ipsec6" },
#endif
	{ -1,		N_PIM6STAT,	1,	0,
	  pim6_stats,	NULL,		"pim6" },
	{ -1,		-1,		1,	0,
	  rip6_stats,	NULL,		"rip6" },
	{ -1,		-1,		0,	0,
	  0,		NULL,		0 }
};
#endif /*INET6*/

#ifdef IPSEC
struct protox pfkeyprotox[] = {
	{ N_PFKEYSTAT,	N_PFKEYSTAT,	1,	0,
	  pfkey_stats,	NULL,		"pfkey" },
	{ -1,		-1,		0,	0,
	  0,		NULL,		0 }
};
#endif

struct protox atalkprotox[] = {
	{ N_DDPCB,	N_DDPSTAT,	1,	atalkprotopr,
	  ddp_stats,	NULL,		"ddp" },
	{ -1,		-1,		0,	0,
	  0,		NULL,		0 }
};

struct protox ipxprotox[] = {
	{ N_IPX,	N_IPXSTAT,	1,	ipxprotopr,
	  ipx_stats,	NULL,		"ipx" },
	{ N_IPX,	N_SPXSTAT,	1,	ipxprotopr,
	  spx_stats,	NULL,		"spx" },
	{ -1,		-1,		0,	0,
	  0,		NULL,		0 }
};

#ifdef NS
struct protox nsprotox[] = {
	{ N_IDP,	N_IDPSTAT,	1,	nsprotopr,
	  idp_stats,	NULL,		"idp" },
	{ N_IDP,	N_SPPSTAT,	1,	nsprotopr,
	  spp_stats,	NULL,		"spp" },
	{ -1,		N_NSERR,	1,	0,
	  nserr_stats,	NULL,		"ns_err" },
	{ -1,		-1,		0,	0,
	  0,		NULL,		0 }
};
#endif

#ifdef ISO
struct protox isoprotox[] = {
	{ ISO_TP,	N_TPSTAT,	1,	iso_protopr,
	  tp_stats,	NULL,		"tp" },
	{ N_CLTP,	N_CLTPSTAT,	1,	iso_protopr,
	  cltp_stats,	NULL,		"cltp" },
	{ -1,		N_CLNPSTAT,	1,	 0,
	  clnp_stats,	NULL,		"clnp"},
	{ -1,		N_ESISSTAT,	1,	 0,
	  esis_stats,	NULL,		"esis"},
	{ -1,		-1,		0,	0,
	  0,		NULL,		0 }
};
#endif

struct protox *protoprotox[] = {
	protox,
#ifdef INET6
	ip6protox,
#endif
#ifdef IPSEC
	pfkeyprotox,
#endif
	ipxprotox, atalkprotox,
#ifdef NS
	nsprotox, 
#endif
#ifdef ISO
	isoprotox, 
#endif
	NULL
};

static void printproto __P((struct protox *, char *));
static void usage __P((void));
static struct protox *name2protox __P((char *));
static struct protox *knownname __P((char *));

kvm_t *kvmd;

int
main(argc, argv)
	int argc;
	char *argv[];
{
	register struct protox *tp = NULL; /* for printing cblocks & stats */
	int ch;
	char *nlistf = NULL, *memf = NULL;
	char buf[_POSIX2_LINE_MAX];

	af = AF_UNSPEC;

	while ((ch = getopt(argc, argv, "Aabdf:ghI:liM:mN:np:rstuw:")) !=  -1)
		switch(ch) {
		case 'A':
			Aflag = 1;
			break;
		case 'a':
			aflag = 1;
			break;
		case 'b':
			bflag = 1;
			break;
		case 'd':
			dflag = 1;
			break;
		case 'f':
#ifdef NS
			if (strcmp(optarg, "ns") == 0)
				af = AF_NS;
			else
#endif
			if (strcmp(optarg, "ipx") == 0)
				af = AF_IPX;
			else if (strcmp(optarg, "inet") == 0)
				af = AF_INET;
#ifdef INET6
			else if (strcmp(optarg, "inet6") == 0)
				af = AF_INET6;
#endif /*INET6*/
#ifdef IPSEC
			else if (strcmp(optarg, "pfkey") == 0)
				af = PF_KEY;
#endif /*IPSEC*/
			else if (strcmp(optarg, "unix") == 0)
				af = AF_UNIX;
			else if (strcmp(optarg, "atalk") == 0)
				af = AF_APPLETALK;
#ifdef ISO
			else if (strcmp(optarg, "iso") == 0)
				af = AF_ISO;
#endif
			else {
				errx(1, "%s: unknown address family", optarg);
			}
			break;
		case 'g':
			gflag = 1;
			break;
		case 'I': {
			char *cp;

			iflag = 1;
			for (cp = interface = optarg; isalpha(*cp); cp++)
				continue;
			unit = atoi(cp);
			break;
		}
		case 'i':
			iflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'M':
			memf = optarg;
			break;
		case 'm':
			mflag = 1;
			break;
		case 'N':
			nlistf = optarg;
			break;
		case 'n':
			nflag = 1;
			break;
		case 'p':
			if ((tp = name2protox(optarg)) == NULL) {
				errx(1, 
				     "%s: unknown or uninstrumented protocol",
				     optarg);
			}
			pflag = 1;
			break;
		case 'r':
			rflag = 1;
			break;
		case 's':
			++sflag;
			break;
		case 't':
			tflag = 1;
			break;
		case 'u':
			af = AF_UNIX;
			break;
		case 'w':
			interval = atoi(optarg);
			iflag = 1;
			break;
		case '?':
		default:
			usage();
		}
	argv += optind;
	argc -= optind;

#define	BACKWARD_COMPATIBILITY
#ifdef	BACKWARD_COMPATIBILITY
	if (*argv) {
		if (isdigit(**argv)) {
			interval = atoi(*argv);
			if (interval <= 0)
				usage();
			++argv;
			iflag = 1;
		}
		if (*argv) {
			nlistf = *argv;
			if (*++argv)
				memf = *argv;
		}
	}
#endif

	/*
	 * Discard setgid privileges if not the running kernel so that bad
	 * guys can't print interesting stuff from kernel memory.
	 */
	if (nlistf != NULL || memf != NULL)
		setgid(getgid());

	kvmd = kvm_openfiles(nlistf, memf, NULL, O_RDONLY, buf);
	if (kvmd == NULL) {
		errx(1, "kvm_open: %s", buf);
	}
	if (kvm_nlist(kvmd, &nl[0]) < 0) {
		if(nlistf)
			errx(1, "%s: kvm_nlist: %s", nlistf, kvm_geterr(kvmd));
		else
			errx(1, "kvm_nlist: %s", kvm_geterr(kvmd));
	}

	if (nl[0].n_type == 0) {
		if(nlistf)
			errx(1, "%s: no namelist", nlistf);
		else
			errx(1, "no namelist");
	}
	if (mflag) {
		mbpr(nl[N_MBSTAT].n_value);
		exit(0);
	}
	if (pflag && tp) {
		if (iflag && tp->pr_istats)
			intpr(interval, nl[N_IFNET].n_value, tp->pr_istats);
		else if (tp->pr_stats)
			(*tp->pr_stats)(nl[tp->pr_sindex].n_value,
				tp->pr_name);
		else
			printf("%s: no stats routine\n", tp->pr_name);
		exit(0);
	}
#if 0
	/*
	 * Keep file descriptors open to avoid overhead
	 * of open/close on each call to get* routines.
	 */
	sethostent(1);
	setnetent(1);
#else
	/*
	 * This does not make sense any more with DNS being default over
	 * the files.  Doing a setXXXXent(1) causes a tcp connection to be
	 * used for the queries, which is slower.
	 */
#endif
	if (iflag) {
		if (af != AF_UNSPEC)
			goto protostat;

		intpr(interval, nl[N_IFNET].n_value, NULL);
		exit(0);
	}
	if (rflag) {
		if (sflag)
			rt_stats(nl[N_RTSTAT].n_value);
		else
			routepr(nl[N_RTREE].n_value);
		exit(0);
	}
	if (gflag) {
		if (sflag) {
			if (af == AF_INET || af == AF_UNSPEC)
				mrt_stats(nl[N_MRTPROTO].n_value,
					  nl[N_MRTSTAT].n_value);
			if (af == AF_INET6 || af == AF_UNSPEC)
				mrt6_stats(nl[N_MRT6PROTO].n_value,
					   nl[N_MRT6STAT].n_value);
		}
		else {
			if (af == AF_INET || af == AF_UNSPEC)
				mroutepr(nl[N_MRTPROTO].n_value,
					 nl[N_MFCTABLE].n_value,
					 nl[N_VIFTABLE].n_value);
			if (af == AF_INET6 || af == AF_UNSPEC)
				mroute6pr(nl[N_MRT6PROTO].n_value,
					  nl[N_MF6CTABLE].n_value,
					  nl[N_MIF6TABLE].n_value);
		}
		exit(0);
	}

  protostat:
#if 0
	if (af == AF_INET || af == AF_UNSPEC) {
		setprotoent(1);
		setservent(1);
		/* ugh, this is O(MN) ... why do we do this? */
		while ((p = getprotoent())) {
			for (tp = protox; tp->pr_name; tp++)
				if (strcmp(tp->pr_name, p->p_name) == 0)
					break;
			if (tp->pr_name == 0 || tp->pr_wanted == 0)
				continue;
			printproto(tp, p->p_name);
		}
		endprotoent();
	}
#endif
	if (af == AF_INET || af == AF_UNSPEC)
		for (tp = protox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name);
#ifdef INET6
	if (af == AF_INET6 || af == AF_UNSPEC)
		for (tp = ip6protox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name);
#endif /*INET6*/
#ifdef IPSEC
	if (af == PF_KEY || af == AF_UNSPEC)
		for (tp = pfkeyprotox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name);
#endif /*IPSEC*/
	if (af == AF_IPX || af == AF_UNSPEC)
		for (tp = ipxprotox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name);
	if (af == AF_APPLETALK || af == AF_UNSPEC)
		for (tp = atalkprotox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name);
#ifdef NS
	if (af == AF_NS || af == AF_UNSPEC)
		for (tp = nsprotox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name);
#endif
#ifdef ISO
	if (af == AF_ISO || af == AF_UNSPEC)
		for (tp = isoprotox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name);
#endif
	if ((af == AF_UNIX || af == AF_UNSPEC) && !sflag)
		unixpr(nl[N_UNIXSW].n_value);
	exit(0);
}

/*
 * Print out protocol statistics or control blocks (per sflag).
 * If the interface was not specifically requested, and the symbol
 * is not in the namelist, ignore this one.
 */
static void
printproto(tp, name)
	register struct protox *tp;
	char *name;
{
	void (*pr)();
	u_long off;

	if (sflag) {
		if (iflag) {
			if (tp->pr_istats)
				intpr(interval, nl[N_IFNET].n_value,
				      tp->pr_istats);
			return;
		}
		else {
			pr = tp->pr_stats;
			off = nl[tp->pr_sindex].n_value;
		}
	} else {
		pr = tp->pr_cblocks;
		off = nl[tp->pr_index].n_value;
	}
	if (pr != NULL && (off || af != AF_UNSPEC))
		(*pr)(off, name);
}

/*
 * Read kernel memory, return 0 on success.
 */
int
kread(addr, buf, size)
	u_long addr;
	char *buf;
	int size;
{

	if (kvm_read(kvmd, addr, buf, size) != size) {
		warnx("%s", kvm_geterr(kvmd));
		return (-1);
	}
	return (0);
}

char *
plural(n)
	int n;
{
	return (n != 1 ? "s" : "");
}

char *
plurales(n)
	int n;
{
	return (n != 1 ? "es" : "");
}

/*
 * Find the protox for the given "well-known" name.
 */
static struct protox *
knownname(name)
	char *name;
{
	struct protox **tpp, *tp;

	for (tpp = protoprotox; *tpp; tpp++)
		for (tp = *tpp; tp->pr_name; tp++)
			if (strcmp(tp->pr_name, name) == 0)
				return (tp);
	return (NULL);
}

/*
 * Find the protox corresponding to name.
 */
static struct protox *
name2protox(name)
	char *name;
{
	struct protox *tp;
	char **alias;			/* alias from p->aliases */
	struct protoent *p;

	/*
	 * Try to find the name in the list of "well-known" names. If that
	 * fails, check if name is an alias for an Internet protocol.
	 */
	if ((tp = knownname(name)) != NULL)
		return (tp);

	setprotoent(1);			/* make protocol lookup cheaper */
	while ((p = getprotoent()) != NULL) {
		/* assert: name not same as p->name */
		for (alias = p->p_aliases; *alias; alias++)
			if (strcmp(name, *alias) == 0) {
				endprotoent();
				return (knownname(p->p_name));
			}
	}
	endprotoent();
	return (NULL);
}

static void
usage()
{
	(void)fprintf(stderr, "%s\n%s\n%s\n%s\n",
"usage: netstat [-Aan] [-f address_family] [-M core] [-N system]",
"       netstat [-bdghimnrs] [-f address_family] [-M core] [-N system]",
"       netstat [-bdn] [-I interface] [-M core] [-N system] [-w wait]",
"       netstat [-M core] [-N system] [-p protocol]");
	exit(1);
}

void
trimdomain(cp)
	char *cp;
{
	static char domain[MAXHOSTNAMELEN + 1];
	static int first = 1;
	char *s;

	if (first) {
		first = 0;
		if (gethostname(domain, MAXHOSTNAMELEN) == 0 &&
		    (s = strchr(domain, '.')))
			(void) strcpy(domain, s + 1);
		else
			domain[0] = 0;
	}

	if (domain[0]) {
		while ((cp = strchr(cp, '.'))) {
			if (!strcasecmp(cp + 1, domain)) {
				*cp = 0;	/* hit it */
				break;
			} else {
				cp++;
			}
		}
	}
}

