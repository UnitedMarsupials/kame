/*	$KAME: route6.c,v 1.65 2007/06/14 12:09:44 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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

#ifdef __FreeBSD__
#include "opt_inet.h"
#include "opt_inet6.h"
#include "opt_mip6.h"
#endif
#ifdef __NetBSD__
#include "opt_inet.h"
#include "opt_mip6.h"
#endif

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/systm.h>
#include <sys/queue.h>

#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet6/in6_var.h>
#include <netinet/ip6.h>
#include <netinet6/ip6_var.h>
#include <netinet6/scope6_var.h>

#ifdef MIP6
#include "mip.h"
#include <netinet/ip6mh.h>
#include <netinet6/mip6_var.h>
#endif

#include <netinet/icmp6.h>

#if 0
static int ip6_rthdr0(struct mbuf *, struct ip6_hdr *, struct ip6_rthdr0 *);
#endif

#if defined(MIP6) && NMIP > 0
static int ip6_rthdr2(struct mbuf *, struct ip6_hdr *, struct ip6_rthdr2 *);
#endif /* defined(MIP6) && NMIP > 0 */

/*
 * proto is unused
 */

int
route6_input(struct mbuf **mp, int *offp, int proto)
{
	struct ip6_hdr *ip6;
	struct mbuf *m = *mp;
	struct ip6_rthdr *rh;
	int off = *offp, rhlen;
	struct m_tag *mtag;

	mtag = ip6_findaux(m);
	if (mtag) {
		struct ip6aux *ip6a = (struct ip6aux *)(mtag + 1);
		/* XXX reject home-address option before rthdr */
		if (ip6a->ip6a_flags & IP6A_SWAP) {
			ip6stat.ip6s_badoptions++;
			m_freem(m);
			return IPPROTO_DONE;
		}
	}

#ifndef PULLDOWN_TEST
	IP6_EXTHDR_CHECK(m, off, sizeof(*rh), IPPROTO_DONE);
	ip6 = mtod(m, struct ip6_hdr *);
	rh = (struct ip6_rthdr *)((caddr_t)ip6 + off);
#else
	ip6 = mtod(m, struct ip6_hdr *);
	IP6_EXTHDR_GET(rh, struct ip6_rthdr *, m, off, sizeof(*rh));
	if (rh == NULL) {
		ip6stat.ip6s_tooshort++;
		return IPPROTO_DONE;
	}
#endif

	switch (rh->ip6r_type) {
#if 0
	/*
	 * See http://www.secdev.org/conf/IPv6_RH_security-csw07.pdf
	 * for why IPV6_RTHDR_TYPE_0 is banned here.
	 *
	 * We return ICMPv6 parameter problem so that innocent people
	 * (not an attacker) would notice about the use of IPV6_RTHDR_TYPE_0.
	 * Since there's no amplification, and ICMPv6 error will be rate-
	 * controlled, it shouldn't cause any problem.
	 * If you are concerned about this, you may want to use the following
	 * code fragment:
	 *
	 * case IPV6_RTHDR_TYPE_0:
	 *	m_freem(m);
	 *	return (IPPROTO_DONE);
	 */
	case IPV6_RTHDR_TYPE_0:
		rhlen = (rh->ip6r_len + 1) << 3;
		if (rh->ip6r_segleft == 0)
			break;	/* Final dst. Just ignore the header. */
#ifndef PULLDOWN_TEST
		/*
		 * note on option length:
		 * due to IP6_EXTHDR_CHECK assumption, we cannot handle
		 * very big routing header (max rhlen == 2048).
		 */
		IP6_EXTHDR_CHECK(m, off, rhlen, IPPROTO_DONE);
#else
		/*
		 * note on option length:
		 * maximum rhlen: 2048
		 * max mbuf m_pulldown can handle: MCLBYTES == usually 2048
		 * so, here we are assuming that m_pulldown can handle
		 * rhlen == 2048 case.  this may not be a good thing to
		 * assume - we may want to avoid pulling it up altogether.
		 */
		IP6_EXTHDR_GET(rh, struct ip6_rthdr *, m, off, rhlen);
		if (rh == NULL) {
			ip6stat.ip6s_tooshort++;
			return IPPROTO_DONE;
		}
#endif
		if (ip6_rthdr0(m, ip6, (struct ip6_rthdr0 *)rh))
			return (IPPROTO_DONE);
		break;
#endif
#if defined(MIP6) && NMIP > 0
	case IPV6_RTHDR_TYPE_2:
		rhlen = (rh->ip6r_len + 1) << 3;

#ifndef PULLDOWN_TEST
		IP6_EXTHDR_CHECK(m, off, rhlen, IPPROTO_DONE);
#else
		IP6_EXTHDR_GET(rh, struct ip6_rthdr *, m, off, rhlen);
		if (rh == NULL) {
			ip6stat.ip6s_tooshort++;
			return IPPROTO_DONE;
		}
#endif

		if (ip6_rthdr2(m, ip6, (struct ip6_rthdr2 *)rh)) 
			return(IPPROTO_DONE);

		break;
#endif /* defined(MIP6) && NMIP > 0 */
	default:
		/* unknown routing type */
		if (rh->ip6r_segleft == 0) {
			rhlen = (rh->ip6r_len + 1) << 3;
			break;	/* Final dst. Just ignore the header. */
		}
		ip6stat.ip6s_badoptions++;
		icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER,
			    (caddr_t)&rh->ip6r_type - (caddr_t)ip6);
		return (IPPROTO_DONE);
	}

	*offp += rhlen;
	return (rh->ip6r_nxt);
}

#if 0
/*
 * Type0 routing header processing
 *
 * RFC2292 backward compatibility warning: no support for strict/loose bitmap,
 * as it was dropped between RFC1883 and RFC2460.
 */
static int
ip6_rthdr0(struct mbuf *m, struct ip6_hdr *ip6, struct ip6_rthdr0 *rh0)
{
	int addrs, index;
	struct in6_addr *nextaddr, tmpaddr;
	struct in6_ifaddr *ifa;

	if (rh0->ip6r0_segleft == 0)
		return (0);

	if (rh0->ip6r0_len % 2) {
		/*
		 * Type 0 routing header can't contain more than 23 addresses.
		 * RFC 2460: this limitation was removed since strict/loose
		 * bitmap field was deleted.
		 */
		ip6stat.ip6s_badoptions++;
		icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER,
			    (caddr_t)&rh0->ip6r0_len - (caddr_t)ip6);
		return (-1);
	}

	if ((addrs = rh0->ip6r0_len / 2) < rh0->ip6r0_segleft) {
		ip6stat.ip6s_badoptions++;
		icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER,
			    (caddr_t)&rh0->ip6r0_segleft - (caddr_t)ip6);
		return (-1);
	}

	index = addrs - rh0->ip6r0_segleft;
	rh0->ip6r0_segleft--;
	nextaddr = ((struct in6_addr *)(rh0 + 1)) + index;

	/*
	 * reject invalid addresses.  be proactive about malicious use of
	 * IPv4 mapped/compat address.
	 * XXX need more checks?
	 */
	if (IN6_IS_ADDR_MULTICAST(nextaddr) ||
	    IN6_IS_ADDR_UNSPECIFIED(nextaddr) ||
	    IN6_IS_ADDR_V4MAPPED(nextaddr) ||
	    IN6_IS_ADDR_V4COMPAT(nextaddr)) {
		ip6stat.ip6s_badoptions++;
		goto bad;
	}
	if (IN6_IS_ADDR_MULTICAST(&ip6->ip6_dst) ||
	    IN6_IS_ADDR_UNSPECIFIED(&ip6->ip6_dst) ||
	    IN6_IS_ADDR_V4MAPPED(&ip6->ip6_dst) ||
	    IN6_IS_ADDR_V4COMPAT(&ip6->ip6_dst)) {
		ip6stat.ip6s_badoptions++;
		goto bad;
	}

	/*
	 * Determine the scope zone of the next hop, based on the interface
	 * of the current hop. [RFC4007, Section 9]
	 * Then disambiguate the scope zone for the next hop (if necessary). 
	 */
	if ((ifa = ip6_getdstifaddr(m)) == NULL)
		goto bad;
	if (in6_setscope(nextaddr, ifa->ia_ifp, NULL) != 0) {
		ip6stat.ip6s_badscope++;
		goto bad;
	}

	/*
	 * Swap the IPv6 destination address and nextaddr. Forward the packet.
	 */
	tmpaddr = *nextaddr;
	*nextaddr = ip6->ip6_dst;
	in6_clearscope(nextaddr); /* XXX */
	ip6->ip6_dst = tmpaddr;

	ip6_forward(m, 1);

	return (-1);			/* m would be freed in ip6_forward() */

  bad:
	m_freem(m);
	return (-1);
}
#endif

#if defined(MIP6) && NMIP > 0
/* Type2 routing header processing */
static int
ip6_rthdr2(struct mbuf *m, struct ip6_hdr *ip6, struct ip6_rthdr2 *rh2)
{
	struct in6_addr *nextaddr, tmpaddr;
	struct in6_ifaddr *ifa;
	struct in6_ifaddr *ia;
	int  myhoa = 0;
	int  mycoa = 0;

	if (rh2->ip6r2_segleft == 0) {
		nextaddr = (struct in6_addr *)(rh2 + 1);

		if (IN6_IS_ADDR_MULTICAST(nextaddr) ||
		    IN6_IS_ADDR_UNSPECIFIED(nextaddr) ||
		    IN6_IS_ADDR_V4MAPPED(nextaddr) ||
		    IN6_IS_ADDR_V4COMPAT(nextaddr)) {
			ip6stat.ip6s_badoptions++;
			goto bad;
		}

		if (IN6_IS_ADDR_MULTICAST(&ip6->ip6_dst) ||
		    IN6_IS_ADDR_UNSPECIFIED(&ip6->ip6_dst) ||
		    IN6_IS_ADDR_V4MAPPED(&ip6->ip6_dst) ||
		    IN6_IS_ADDR_V4COMPAT(&ip6->ip6_dst)) {
			ip6stat.ip6s_badoptions++;
			goto bad;
		}

		if ((ifa = ip6_getdstifaddr(m)) == NULL) 
			goto bad;
		if (in6_setscope(nextaddr, ifa->ia_ifp, NULL) != 0) {
			ip6stat.ip6s_badscope++;
			goto bad;
		}

		/*
		 * check dst addr = hoa
		 */
		for (ia = in6_ifaddr; ia; ia = ia->ia_next) {
			if (ia->ia6_flags & IN6_IFF_HOME) 
				if (IN6_ARE_ADDR_EQUAL(&ip6->ip6_dst, &ia->ia_addr.sin6_addr))
					myhoa = 1;

			if (IN6_ARE_ADDR_EQUAL(nextaddr, &ia->ia_addr.sin6_addr))
					mycoa = 1;
		} 

		if (!myhoa || !mycoa) {
			ip6stat.ip6s_badoptions++;
			goto bad;
		} 

		return (0);
	} 

	if (rh2->ip6r2_segleft != 1) { 
		ip6stat.ip6s_badoptions++;
		goto bad;
	}
	
	/* section 11.3.3 */
	if (rh2->ip6r2_len != 2) {
		ip6stat.ip6s_badoptions++;
		goto bad;
	}

	rh2->ip6r2_segleft--;
	nextaddr = (struct in6_addr *)(rh2 + 1);

	/*
	 * reject invalid addresses.  be proactive about malicious use of
	 * IPv4 mapped/compat address.
	 * XXX need more checks?
	 */
	if (IN6_IS_ADDR_MULTICAST(nextaddr) ||
		IN6_IS_ADDR_UNSPECIFIED(nextaddr) ||
		IN6_IS_ADDR_V4MAPPED(nextaddr) ||
		IN6_IS_ADDR_V4COMPAT(nextaddr)) {
		ip6stat.ip6s_badoptions++;
		goto bad;
	}

	if (IN6_IS_ADDR_MULTICAST(&ip6->ip6_dst) ||
		IN6_IS_ADDR_UNSPECIFIED(&ip6->ip6_dst) ||
		IN6_IS_ADDR_V4MAPPED(&ip6->ip6_dst) ||
		IN6_IS_ADDR_V4COMPAT(&ip6->ip6_dst)) {
		ip6stat.ip6s_badoptions++;
		goto bad;
	}

	/*
	 * determine the scope zone of the next hop, based on the interface
	 * of the current hop. [RFC4007, Section 9]
	 * Then disambiguate the scope zone for the next hop (if necessary).
	 */
	if ((ifa = ip6_getdstifaddr(m)) == NULL) 
		goto bad;
	if (in6_setscope(nextaddr, ifa->ia_ifp, NULL) != 0) {
		ip6stat.ip6s_badscope++;
		goto bad;
	}

	/*
	 * Swap the IPv6 destination address and nextaddr. Forward the packet.
	 */
  	tmpaddr = *nextaddr;
	*nextaddr = ip6->ip6_dst;
	in6_clearscope(nextaddr); /* XXX */
	ip6->ip6_dst = tmpaddr;

  	ip6_forward(m, 1);

	return (-1);			/* m would be freed in ip6_forward() */

  bad:
	m_freem(m);
	return (-1);

}
#endif /* defined(MIP6) && NMIP > 0 */
