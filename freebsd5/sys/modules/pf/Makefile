# $FreeBSD: src/sys/modules/pf/Makefile,v 1.4 2004/08/14 15:32:40 dwmalone Exp $

.PATH: ${.CURDIR}/../../contrib/pf/net
.PATH: ${.CURDIR}/../../contrib/pf/netinet
.PATH: ${.CURDIR}/../../netinet

KMOD=	pf
SRCS = 	pf.c pf_if.c pf_subr.c pf_osfp.c pf_ioctl.c pf_norm.c pf_table.c \
	if_pflog.c \
	in4_cksum.c \
	opt_pf.h opt_inet.h opt_inet6.h opt_bpf.h

CFLAGS+=  -Wall -I${.CURDIR}/../../contrib/pf

opt_pf.h:
	echo "#define DEV_PF 1" > opt_pf.h
	echo "#define DEV_PFLOG 1" >> opt_pf.h

opt_inet.h:
	echo "#define INET 1" > opt_inet.h

opt_inet6.h:
.if defined(NOINET6)
	echo > opt_inet6.h
.else
	echo "#define INET6 1" > opt_inet6.h
.endif

opt_bpf.h:
	echo "#define DEV_BPF 1" > opt_bpf.h

.include <bsd.kmod.mk>
