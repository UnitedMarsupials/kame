# $FreeBSD: src/sys/modules/if_gif/Makefile,v 1.3 2002/08/12 15:27:17 rwatson Exp $

.PATH: ${.CURDIR}/../../net ${.CURDIR}/../../netinet ${.CURDIR}/../../netinet6

KMOD=	if_gif
SRCS=	if_gif.c in_gif.c in6_gif.c opt_inet.h opt_inet6.h opt_mac.h \
	opt_mrouting.h opt_mip6.h gif.h bpf.h

opt_inet.h:
	echo "#define INET 1" > ${.TARGET}

opt_inet6.h:
	echo "#define INET6 1" > ${.TARGET}

opt_mrouting.h:
	echo "#define MROUTING 1" > ${.TARGET}

gif.h:
	echo "#define NGIF 1" > ${.TARGET}

bpf.h:
	echo "#define NBPF 1" > ${.TARGET}

.include <bsd.kmod.mk>
