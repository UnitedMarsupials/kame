SRCDIR=	${.CURDIR}/../../kame/libinet6

LIB=	inet6
CFLAGS+=-DDNS
CFLAGS+=-DINET6 -DHAVE_SOCKADDR_SA_LEN \
	-DHAVE_PORTABLE_PROTOTYPE -DHAVE_U_INT8_T -DHAVE_U_INT16_T \
	-DHAVE_U_INT32_T -DHAVE_ADDRINFO -DHAVE_SOCKADDR_STORAGE \
	-DHAVE_INADDRSZ -DHAVE_IN6ADDRSZ -DHAVE_RES_USE_INET6 -DHAVE_AAAA \
	-DHAVE_RES_STATE_EXT -DHAVE_NEW_RES_STATE -DHAVE_H_ERRNO \
	-DHAVE_IF_NAMEINDEX -DFILTER_V4MAPPED -DHAVE_SOCKLEN_T \
	-DDEBUG
CFLAGS+=-DUSE_LOG_REORDER
CFLAGS+=-I/usr/pkg/include/bind -I${.CURDIR}/../../include
CFLAGS+=	-g -Wall -Dlint

SRCS=	rresvport_af.c rcmd.c
SRCS+=	if_indextoname.c if_nameindex.c if_nametoindex.c
SRCS+=	rthdr.c ip6opt.c
SRCS+=	bindresvport.c
SRCS+=	getifaddrs.c
SRCS+=	strlcpy.c strlcat.c

MAN3=	rresvport_af.3 if_indextoname.3 getipnodebyname.3 inet6_option_space.3 \
	inet6_rthdr_space.3 bindresvport.3 inet6_opt_init.3 inet6_rth_space.3 \
	getifaddrs.3 strlcpy.3
MLINKS+=	if_indextoname.3 if_nametoindex.3 \
		if_indextoname.3 if_nameindex.3 \
		if_indextoname.3 if_freenameindex.3 \
		inet6_option_space.3 inet6_option_init.3 \
		inet6_option_space.3 inet6_option_append.3 \
		inet6_option_space.3 inet6_option_alloc.3 \
		inet6_option_space.3 inet6_option_next.3 \
		inet6_option_space.3 inet6_option_find.3 \
		inet6_rthdr_space.3 inet6_rthdr_init.3 \
		inet6_rthdr_space.3 inet6_rthdr_add.3 \
		inet6_rthdr_space.3 inet6_rthdr_lasthop.3 \
		inet6_rthdr_space.3 inet6_rthdr_reverse.3 \
		inet6_rthdr_space.3 inet6_rthdr_segments.3 \
		inet6_rthdr_space.3 inet6_rthdr_getaddr.3 \
		inet6_rthdr_space.3 inet6_rthdr_getflags.3 \
		bindresvport.3 bindresvport_sa.3 \
		inet6_opt_init.3 inet6_opt_append.3 \
		inet6_opt_init.3 inet6_opt_finish.3 \
		inet6_opt_init.3 inet6_opt_set_val.3 \
		inet6_opt_init.3 inet6_opt_next.3 \
		inet6_opt_init.3 inet6_opt_find.3 \
		inet6_opt_init.3 inet6_opt_get_val.3 \
		inet6_rth_space.3 inet6_rth_init.3 \
		inet6_rth_space.3 inet6_rth_add.3 \
		inet6_rth_space.3 inet6_rth_reverse.3 \
		inet6_rth_space.3 inet6_rth_segments.3 \
		inet6_rth_space.3 inet6_rth_getaddr.3 \
		getifaddrs.3 freeifaddrs.3 \
		strlcpy.3 strlcat.3

.PATH:	${SRCDIR} ${SRCDIR}/resolv

# If you attempt to build shared library, enable the following lines.
# Do it at your own risk!  libinet6 is updated frequently so version
# management would be very hard...
#SHLIB_MAJOR=	1
#SHLIB_MINOR=	0

MKPIC=no

.include <bsd.lib.mk>
