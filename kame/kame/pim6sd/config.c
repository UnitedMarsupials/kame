/*	$KAME: config.c,v 1.27 2002/05/29 05:08:21 suz Exp $	*/

/*
 * Copyright (c) 1998-2001
 * The University of Southern California/Information Sciences Institute.
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
/*
 *  Questions concerning this software should be directed to
 *  Mickael Hoerdt (hoerdt@clarinet.u-strasbg.fr) LSIIT Strasbourg.
 *
 */
/*
 * This program has been derived from pim6dd.
 * The pim6dd program is covered by the license in the accompanying file
 * named "LICENSE.pim6dd".
 */
/*
 * This program has been derived from pimd.
 * The pimd program is covered by the license in the accompanying file
 * named "LICENSE.pimd".
 *
 */
/*
 * Part of this program has been derived from mrouted.
 * The mrouted program is covered by the license in the accompanying file
 * named "LICENSE.mrouted".
 *
 * The mrouted program is COPYRIGHT 1989 by The Board of Trustees of
 * Leland Stanford Junior University.
 *
 */


#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <net/if.h>
#if defined(__FreeBSD__) && __FreeBSD__ >= 3
#include <net/if_var.h>
#endif
#include <net/route.h>
#include <netinet/in.h>
#include <netinet6/ip6_mroute.h>
#include <netinet6/in6_var.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <stdlib.h>
#ifdef HAVE_GETIFADDRS
#include <ifaddrs.h>
#endif 
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include "defs.h"
#include "config.h"
#include "vif.h"
#include "pim6.h"
#include "inet6.h"
#include "mrt.h"
#include "rp.h"
#include "pimd.h"
#include "timer.h"
#include "route.h"
#include "debug.h"

void add_phaddr(struct uvif *v, struct sockaddr_in6 *addr,
		struct in6_addr *mask);

void
config_vifs_from_kernel()
{
	register struct uvif *v;
	register mifi_t vifi;
	int i;
	struct sockaddr_in6 addr;
	struct in6_addr mask;
	short flags;
#ifdef HAVE_GETIFADDRS
	struct ifaddrs *ifap, *ifa;
#else
	int n;
	int num_ifreq = 64;
	struct ifconf ifc;
	struct ifreq *ifrp,*ifend;
#endif

	total_interfaces= 0;	/* The total number of physical interfaces */

#ifdef HAVE_GETIFADDRS
	if (getifaddrs(&ifap))
		log(LOG_ERR, errno, "getifaddrs");

	/*
	 * Loop through all of the interfaces.
	 */
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		struct in6_ifreq ifr6;

		/*
		 * Ignore any interface for an address family other than IPv6.
		 */
		if (ifa->ifa_addr->sa_family != AF_INET6) {
			/* Eventually may have IPv6 address later */
			total_interfaces++;
			continue;
		}

		memcpy(&addr, ifa->ifa_addr, sizeof(struct sockaddr_in6));

		flags = ifa->ifa_flags;


		/*
		 * Get netmask of the address.
		 */
		memcpy(&mask,
		       &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr,
		       sizeof(mask));

		/*
		 * Get IPv6 specific flags, and ignore an anycast address.
		 * XXX: how about a deprecated, tentative, duplicated or
		 * detached address?
		 */
		strncpy(ifr6.ifr_name, ifa->ifa_name, sizeof(ifr6.ifr_name));
		ifr6.ifr_addr = *(struct sockaddr_in6 *)ifa->ifa_addr;
		if (ioctl(udp_socket, SIOCGIFAFLAG_IN6, &ifr6) < 0) {
			log(LOG_ERR, errno, "ioctl SIOCGIFAFLAG_IN6 for %s",
			    inet6_fmt(&ifr6.ifr_addr.sin6_addr));
		}
		else {
			if (ifr6.ifr_ifru.ifru_flags6 & IN6_IFF_ANYCAST) {
				log(LOG_DEBUG, 0, "config_vifs_from_kernel: "
				    "%s on %s is an anycast address, ignored",
				    inet6_fmt(&ifr6.ifr_addr.sin6_addr),
				    ifa->ifa_name);
				continue;
			}
		}

		if (IN6_IS_ADDR_LINKLOCAL(&addr.sin6_addr))
		{
			addr.sin6_scope_id = if_nametoindex(ifa->ifa_name);
#ifdef __KAME__
			/*
			 * Hack for KAME kernel.
			 * Set sin6_scope_id field of a link local address and clear
			 * the index embedded in the address.
			 */
			/* clear interface index */
			addr.sin6_addr.s6_addr[2] = 0;
			addr.sin6_addr.s6_addr[3] = 0;
#endif
		}

		/*
		 * If the address is connected to the same subnet as one
		 * already installed in the uvifs array, just add the address
		 * to the list of addresses of the uvif.
		 */
		for (vifi = 0, v = uvifs; vifi < numvifs; ++vifi, ++v)
		{
			if (strlen(v->uv_name) == strlen(ifa->ifa_name) &&
			    strcmp(v->uv_name, ifa->ifa_name) == 0)
			{
				add_phaddr(v, &addr, &mask);
				break;
			}
		}	

		if (vifi != numvifs)
			continue;

		/*
		 * If there is room in the uvifs array, install this interface.
		 */
		if (numvifs == MAXMIFS)
		{
			log(LOG_WARNING, 0,
			    "too many vifs, ignoring %s", ifa->ifa_name);
			continue;
		}

		/*
		 * Everyone below is a potential vif interface.
		 * We don't care if it has wrong configuration or not
		 * configured at all.
		 */
		total_interfaces++;

		v  = &uvifs[numvifs];
		v->uv_dst_addr = allpim6routers_group;
		v->uv_subnetmask = mask;
		strncpy(v->uv_name, ifa->ifa_name, IFNAMSIZ);
		v->uv_ifindex = if_nametoindex(v->uv_name);
		add_phaddr(v, &addr,&mask);
	
		/* prefix local calc. (and what about add_phaddr?...) */
		for (i = 0; i < sizeof(struct in6_addr); i++)
			v->uv_prefix.sin6_addr.s6_addr[i] =
				addr.sin6_addr.s6_addr[i] & mask.s6_addr[i];
	
		if(flags & IFF_POINTOPOINT)
			v->uv_flags |=(VIFF_REXMIT_PRUNES | VIFF_POINT_TO_POINT);

		/*
		 * Disable multicast routing on loopback interfaces and
		 * interfaces that do not support multicast. But they are
		 * still necessary, since global addresses maybe assigned only
		 * on such interfaces.
		 */
		if ((flags & IFF_LOOPBACK) != 0 ||
		    (flags & IFF_MULTICAST) == 0)
			v->uv_flags |= VIFF_DISABLED;

		IF_DEBUG(DEBUG_IF)
			log(LOG_DEBUG,0,
			    "Installing %s (%s on subnet %s) ,"
			    "as vif #%u - rate = %d",
			    v->uv_name,inet6_fmt(&addr.sin6_addr),
			    net6name(&v->uv_prefix.sin6_addr,&mask),
			    numvifs,v->uv_rate_limit);

		++numvifs;

		
		if( !(flags & IFF_UP)) 
		{
			v->uv_flags |= VIFF_DOWN;
			vifs_down = TRUE;
		}

	}

	freeifaddrs(ifap);
#else  /* !HAVE_GETIFADDRS */
	ifc.ifc_len = num_ifreq * sizeof (struct ifreq);
	ifc.ifc_buf = calloc(ifc.ifc_len,sizeof(char));
	while (ifc.ifc_buf) {
		caddr_t newbuf;

		if (ioctl(udp_socket,SIOCGIFCONF,(char *)&ifc) <0)
		      log(LOG_ERR, errno, "ioctl SIOCGIFCONF");	
		/*
		 * If the buffer was large enough to hold all the addresses
		 * then break out, otherwise increase the buffer size and
		 * try again.
		 *
		 * The only way to know that we definitely had enough space
		 * is to know that there was enough space for at least one
		 * more struct ifreq. ???
		 */
		if( (num_ifreq * sizeof (struct ifreq)) >=
		    ifc.ifc_len + sizeof(struct ifreq))
			break;

		num_ifreq *= 2;
		ifc.ifc_len = num_ifreq * sizeof(struct ifreq);
		newbuf = realloc(ifc.ifc_buf, ifc.ifc_len);
		if (newbuf == NULL)
			free(ifc.ifc_buf);
		ifc.ifc_buf = newbuf;
	}
	if (ifc.ifc_buf == NULL)
	    log(LOG_ERR, 0, "config_vifs_from_kernel: ran out of memory");
	

	ifrp = (struct ifreq *) ifc.ifc_buf;
	ifend = (struct ifreq * ) (ifc.ifc_buf + ifc.ifc_len);

	/*
	 * Loop through all of the interfaces.
	 */
	for(;ifrp < ifend;ifrp = (struct ifreq *)((char *)ifrp+n))
	{
		struct ifreq ifr;
		struct in6_ifreq ifr6;

#ifdef HAVE_SA_LEN
		n = ifrp->ifr_addr.sa_len + sizeof(ifrp->ifr_name);
		if(n < sizeof(*ifrp))
			n=sizeof(*ifrp);
#else
		n=sizeof(*ifrp);
#endif 

		/*
		 * Ignore any interface for an address family other than IPv6.
		 */
		if ( ifrp->ifr_addr.sa_family != AF_INET6)
		{
			/* Eventually may have IP address later */
			total_interfaces++;
			continue;
		}

		memcpy(&addr,&ifrp->ifr_addr,sizeof(struct sockaddr_in6));

		/*
		 * Need a template to preserve address info that is
		 * used below to locate the next entry.  (Otherwise,
		 * SIOCGIFFLAGS stomps over it because the requests
		 * are returned in a union.)
		 */
		strncpy(ifr.ifr_name,ifrp->ifr_name,sizeof(ifr.ifr_name));
		strncpy(ifr6.ifr_name,ifrp->ifr_name,sizeof(ifr6.ifr_name));

		if(ioctl(udp_socket,SIOCGIFFLAGS,(char *)&ifr) <0)
        	log(LOG_ERR, errno, "ioctl SIOCGIFFLAGS for %s", ifr.ifr_name);
		flags = ifr.ifr_flags;

#if 0
		/*
		 * Ignore loopback interfaces and interfaces that do not
		 * support multicast.
		 */
		if((flags & (IFF_LOOPBACK | IFF_MULTICAST ))!= IFF_MULTICAST)
			continue;
#endif

		/*
		 * Get netmask of the address.
		 */
		ifr6.ifr_addr = *(struct sockaddr_in6 *)&ifrp->ifr_addr;
		if(ioctl(udp_socket, SIOCGIFNETMASK_IN6, (char *)&ifr6) <0)
			log(LOG_ERR, errno, "ioctl SIOCGIFNETMASK_IN6 for %s",
			    inet6_fmt(&ifr6.ifr_addr.sin6_addr));
		memcpy(&mask,&ifr6.ifr_addr.sin6_addr,sizeof(mask));

		/*
		 * Get IPv6 specific flags, and ignore an anycast address.
		 * XXX: how about a deprecated, tentative, duplicated or
		 * detached address?
		 */
		ifr6.ifr_addr = *(struct sockaddr_in6 *)&ifrp->ifr_addr;
		if (ioctl(udp_socket, SIOCGIFAFLAG_IN6, &ifr6) < 0) {
			log(LOG_ERR, errno, "ioctl SIOCGIFAFLAG_IN6 for %s",
			    inet6_fmt(&ifr6.ifr_addr.sin6_addr));
		}
		else {
			if (ifr6.ifr_ifru.ifru_flags6 & IN6_IFF_ANYCAST) {
				log(LOG_DEBUG, 0, "config_vifs_from_kernel: "
				    "%s on %s is an anycast address, ignored",
				    inet6_fmt(&ifr6.ifr_addr.sin6_addr),
				    ifr.ifr_name);
				continue;
			}
		}

		if (IN6_IS_ADDR_LINKLOCAL(&addr.sin6_addr))
		{
			addr.sin6_scope_id = if_nametoindex(ifrp->ifr_name);
#ifdef __KAME__
			/*
			 * Hack for KAME kernel.
			 * Set sin6_scope_id field of a link local address and clear
			 * the index embedded in the address.
			 */
			/* clear interface index */
			addr.sin6_addr.s6_addr[2] = 0;
			addr.sin6_addr.s6_addr[3] = 0;
#endif
		}

		/*
		 * If the address is connected to the same subnet as one
		 * already installed in the uvifs array, just add the address
		 * to the list of addresses of the uvif.
		 */
		for (vifi = 0, v = uvifs; vifi < numvifs; ++vifi, ++v)
		{
			if (strlen(v->uv_name) == strlen(ifr.ifr_name) &&
			    strcmp(v->uv_name , ifr.ifr_name) == 0)
			{
				add_phaddr(v, &addr,&mask);
				break;
			}
		}	

		if( vifi != numvifs )
			continue;

		/*
		 * If there is room in the uvifs array, install this interface.
		 */
		if( numvifs == MAXMIFS )
		{
			log(LOG_WARNING, 0,
			    "too many vifs, ignoring %s", ifr.ifr_name);	
			continue;
		}		

		/*
		 * Everyone below is a potential vif interface.
		 * We don't care if it has wrong configuration or not
		 * configured at all.
		 */
		total_interfaces++;

		v  = &uvifs[numvifs];
		v->uv_dst_addr = allpim6routers_group;
		v->uv_subnetmask = mask;
		strncpy ( v->uv_name , ifr.ifr_name,IFNAMSIZ);
		v->uv_ifindex = if_nametoindex(v->uv_name);
		add_phaddr(v,&addr,&mask);
	
		/* prefix local calc. (and what about add_phaddr?...) */
		for (i = 0; i < sizeof(struct in6_addr); i++)
			v->uv_prefix.sin6_addr.s6_addr[i] =
				addr.sin6_addr.s6_addr[i] & mask.s6_addr[i];
	
		if(flags & IFF_POINTOPOINT)
			v->uv_flags |=(VIFF_REXMIT_PRUNES | VIFF_POINT_TO_POINT);

		/*
		 * Disable multicast routing on loopback interfaces and
		 * interfaces that do not support multicast. But they are
		 * still necessary, since global addresses maybe assigned only
		 * on such interfaces.
		 */
		if ((flags & IFF_LOOPBACK) != 0 || (flags & IFF_MULTICAST) == 0)
			v->uv_flags |= VIFF_DISABLED;

		IF_DEBUG(DEBUG_IF)
			log(LOG_DEBUG,0,
			    "Installing %s (%s on subnet %s) ,"
			    "as vif #%u - rate = %d",
			    v->uv_name,inet6_fmt(&addr.sin6_addr),
			    net6name(&v->uv_prefix.sin6_addr,&mask),
			    numvifs,v->uv_rate_limit);

		++numvifs;

		
		if( !(flags & IFF_UP)) 
		{
			v->uv_flags |= VIFF_DOWN;
			vifs_down = TRUE;
		}

	}
#endif /* HAVE_GETIFADDRS */
}

void
add_phaddr(struct uvif *v,struct sockaddr_in6 *addr,struct in6_addr *mask)
{
	struct phaddr *pa;
	int i;
	
	if( (pa=malloc(sizeof(*pa))) == NULL)
		        log(LOG_ERR, 0, "add_phaddr: memory exhausted");


	memset(pa,0,sizeof(*pa));
	pa->pa_addr= *addr;
	pa->pa_subnetmask = *mask;

	for(i = 0; i < sizeof(struct in6_addr); i++)
		pa->pa_prefix.sin6_addr.s6_addr[i] =
			addr->sin6_addr.s6_addr[i] & mask->s6_addr[i];
	pa->pa_prefix.sin6_scope_id = addr->sin6_scope_id;


	if(IN6_IS_ADDR_LINKLOCAL(&addr->sin6_addr)) {
		if(v->uv_linklocal)
            log(LOG_WARNING, 0,
               "add_phaddr: found more than one link-local "
               "address on %s",
               v->uv_name);

	v->uv_linklocal = pa;
	}

	pa->pa_next = v->uv_addrs;
	v->uv_addrs = pa;
}
