/*	$KAME: if_dummy.c,v 1.24 2003/03/28 05:29:21 suz Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
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
/*
 * derived from
 *	@(#)if_loop.c	8.1 (Berkeley) 6/10/93
 * Id: if_loop.c,v 1.22 1996/06/19 16:24:10 wollman Exp
 */

/*
 * Loopback interface driver for protocol testing and timing.
 */
#include "dummy.h"
#if NDUMMY > 0

#if defined(__FreeBSD__) && __FreeBSD__ >= 3
#include "opt_inet.h"
#include "opt_inet6.h"
#endif
#ifdef __NetBSD__
#include "opt_inet.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/errno.h>
#if defined(__FreeBSD__) && __FreeBSD__ >= 3
#include <sys/sockio.h>
#else
#include <sys/ioctl.h>
#endif
#include <sys/time.h>
#ifdef __bsdi__
#include <machine/cpu.h>
#endif

#include <net/if.h>
#include <net/if_types.h>
#include <net/netisr.h>
#include <net/route.h>
#include <net/bpf.h>

#ifdef	INET
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#endif

#ifdef IPX
#include <netipx/ipx.h>
#include <netipx/ipx_if.h>
#endif

#ifdef INET6
#ifndef INET
#include <netinet/in.h>
#endif
#include <netinet6/in6_var.h>
#include <netinet/ip6.h>
#endif

#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#ifdef ISO
#include <netiso/iso.h>
#include <netiso/iso_var.h>
#endif

#ifdef NETATALK
#include <netatalk/at.h>
#include <netatalk/at_var.h>
#endif

#if defined(__FreeBSD__) && __FreeBSD__ >= 4
#include "bpf.h"
#define NBPFILTER	NBPF
#else
#include "bpfilter.h"
#endif

#include <net/net_osdep.h>

#if defined(__FreeBSD__) && __FreeBSD__ < 3
static int dummyioctl __P((struct ifnet *, int, caddr_t));
#else
static int dummyioctl __P((struct ifnet *, u_long, caddr_t));
#endif
int dummyoutput __P((struct ifnet *, struct mbuf *, struct sockaddr *,
	struct rtentry *));
#if (defined(__bsdi__) && _BSDI_VERSION >= 199802) || defined(__NetBSD__) || defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ >= 4)
static void dummyrtrequest __P((int, struct rtentry *, struct rt_addrinfo *));
#else
static void dummyrtrequest __P((int, struct rtentry *, struct sockaddr *));
#endif

#ifdef __FreeBSD__
void dummyattach __P((void *));
PSEUDO_SET(dummyattach, if_dummy);
#else
void dummyattach __P((int));
#endif

#ifdef TINY_DUMMYMTU
#define	DUMMYMTU	(1024+512)
#else
#define DUMMYMTU	16384
#endif

static struct	ifnet dummyif[NDUMMY];

/* ARGSUSED */
void
dummyattach(dummy)
#ifdef __FreeBSD__
	void *dummy;
#else
	int dummy;
#endif
{
	struct ifnet *ifp;
	int i;

	for (i = 0; i < NDUMMY; i++) {
		ifp = &dummyif[i];
#if defined(__NetBSD__) || defined(__OpenBSD__)
		sprintf(ifp->if_xname, "dummy%d", i);
#else
		ifp->if_name = "dummy";
		ifp->if_unit = i;
#endif
#ifndef __bsdi__
		ifp->if_softc = NULL;
#endif
		ifp->if_mtu = DUMMYMTU;
		/* Change to BROADCAST experimentaly to announce its prefix. */
		ifp->if_flags = /* IFF_LOOPBACK */ IFF_BROADCAST | IFF_MULTICAST;
		ifp->if_ioctl = dummyioctl;
		ifp->if_output = dummyoutput;
		ifp->if_type = IFT_DUMMY;
#if defined(__FreeBSD__) && __FreeBSD__ >= 4
		ifp->if_snd.ifq_maxlen = IFQ_MAXLEN;
#endif
		ifp->if_hdrlen = 0;
		ifp->if_addrlen = 0;
#ifdef __NetBSD__
		ifp->if_dlt = DLT_NULL;
#endif
		if_attach(ifp);
#if defined(__NetBSD__) || defined(__OpenBSD__)
		if_alloc_sadl(ifp);
#endif
#if NBPFILTER > 0
#ifdef HAVE_NEW_BPFATTACH
		bpfattach(ifp, DLT_NULL, sizeof(u_int));
#else
		bpfattach(&ifp->if_bpf, ifp, DLT_NULL, sizeof(u_int));
#endif
#endif
	}
}

int
dummyoutput(ifp, m, dst, rt)
	struct ifnet *ifp;
	struct mbuf *m;
	struct sockaddr *dst;
	struct rtentry *rt;
{
#if (defined(__FreeBSD__) && __FreeBSD_version >= 500000)
	int error;
#else
	int s;
#endif
	int isr;
	struct ifqueue *ifq = 0;

	if ((m->m_flags & M_PKTHDR) == 0)
		panic("dummyoutput no HDR");
#if NBPFILTER > 0
	/* BPF write needs to be handled specially */
	if (dst->sa_family == AF_UNSPEC) {
		dst->sa_family = *(mtod(m, int *));
		m->m_len -= sizeof(int);
		m->m_pkthdr.len -= sizeof(int);
		m->m_data += sizeof(int);
	}

	if (ifp->if_bpf) {
		/*
		 * We need to prepend the address family as
		 * a four byte field.  Cons up a dummy header
		 * to pacify bpf.  This is safe because bpf
		 * will only read from the mbuf (i.e., it won't
		 * try to free it or keep a pointer a to it).
		 */
		struct mbuf m0;
		u_int32_t af = dst->sa_family;

		m0.m_next = m;
		m0.m_len = 4;
		m0.m_data = (char *)&af;

#ifdef HAVE_NEW_BPF
		bpf_mtap(ifp, &m0);
#else
		bpf_mtap(ifp->if_bpf, &m0);
#endif
	}
#endif
	m->m_pkthdr.rcvif = ifp;

	if (rt && rt->rt_flags & (RTF_REJECT|RTF_BLACKHOLE)) {
		m_freem(m);
		return (rt->rt_flags & RTF_BLACKHOLE ? 0 :
		        rt->rt_flags & RTF_HOST ? EHOSTUNREACH : ENETUNREACH);
	}
	ifp->if_opackets++;
	ifp->if_obytes += m->m_pkthdr.len;
	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		ifq = &ipintrq;
		isr = NETISR_IP;
		break;
#endif
#ifdef IPX
	case AF_IPX:
		ifq = &ipxintrq;
		isr = NETISR_IPX;
		break;
#endif
#ifdef INET6
	case AF_INET6:
		ifq = &ip6intrq;
		isr = NETISR_IPV6;
		break;
#endif
#ifdef NS
	case AF_NS:
		ifq = &nsintrq;
		isr = NETISR_NS;
		break;
#endif
#ifdef ISO
	case AF_ISO:
		ifq = &clnlintrq;
		isr = NETISR_ISO;
		break;
#endif
#ifdef NETATALK
	case AF_APPLETALK:
	        ifq = &atintrq2;
		isr = NETISR_ATALK;
		break;
#endif
	default:
		printf("%s: can't handle af%d\n",
		       if_name(ifp), dst->sa_family);
		m_freem(m);
		return (EAFNOSUPPORT);
	}

#if (defined(__FreeBSD__) && __FreeBSD_version >= 500000)
	IFQ_HANDOFF(ifp, m, NULL, error);
	if (error)
		return (error);
#else
#ifdef __NetBSD__
	s = splnet();
#else
	s = splimp();
#endif
	if (IF_QFULL(ifq)) {
		IF_DROP(ifq);
		m_freem(m);
		splx(s);
		return (ENOBUFS);
	}
	IF_ENQUEUE(ifq, m);
#endif /* !FreeBSD5 */

	schednetisr(isr);
	ifp->if_ipackets++;
	ifp->if_ibytes += m->m_pkthdr.len;
#if !(defined(__FreeBSD__) && __FreeBSD_version >= 500000)
	splx(s);
#endif
	return (0);
}

/* ARGSUSED */
static void
#if (defined(__bsdi__) && _BSDI_VERSION >= 199802) || defined(__NetBSD__) || defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ >= 4)
dummyrtrequest(cmd, rt, info)
	int cmd;
	struct rtentry *rt;
	struct rt_addrinfo *info;
#else
dummyrtrequest(cmd, rt, sa)
	int cmd;
	struct rtentry *rt;
	struct sockaddr *sa;
#endif
{
	if (rt) {
		rt->rt_rmx.rmx_mtu = rt->rt_ifp->if_mtu; /* for ISO */
#ifdef __FreeBSD__
		/*
		 * For optimal performance, the send and receive buffers
		 * should be at least twice the MTU plus a little more for
		 * overhead.
		 */
		rt->rt_rmx.rmx_recvpipe =
			rt->rt_rmx.rmx_sendpipe = 3 * DUMMYMTU;
#endif
	}
}

/*
 * Process an ioctl request.
 */
/* ARGSUSED */
static int
dummyioctl(ifp, cmd, data)
	struct ifnet *ifp;
#if defined(__FreeBSD__) && __FreeBSD__ < 3
	int cmd;
#else
	u_long cmd;
#endif
	caddr_t data;
{
	struct ifaddr *ifa;
	struct ifreq *ifr = (struct ifreq *)data;
	int error = 0;

	switch (cmd) {

	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP | IFF_RUNNING;
		ifa = (struct ifaddr *)data;
		ifa->ifa_rtrequest = dummyrtrequest;
		/*
		 * Everything else is done at a higher level.
		 */
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		if (ifr == 0) {
			error = EAFNOSUPPORT;		/* XXX */
			break;
		}
		switch (ifr->ifr_addr.sa_family) {

#ifdef INET
		case AF_INET:
			break;
#endif
#ifdef INET6
		case AF_INET6:
			break;
#endif

		default:
			error = EAFNOSUPPORT;
			break;
		}
		break;

#ifdef SIOCSIFMTU
	case SIOCSIFMTU:
		ifp->if_mtu = ifr->ifr_mtu;
		break;
#endif

	case SIOCSIFFLAGS:
		break;

	default:
		error = EINVAL;
	}
	return (error);
}
#endif /* NDUMMY > 0 */
