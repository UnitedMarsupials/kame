# Cronyx Id: sys.modules.ct.Makefile,v 1.1.2.2 2003/12/08 15:39:01 rik Exp $
# $FreeBSD: src/sys/modules/ctau/Makefile,v 1.2 2004/06/23 14:22:14 rik Exp $
.PATH:  ${.CURDIR}/../../dev/ctau
KMOD=   if_ct
SRCS=   if_ct.c ctddk.c ctau.c opt_netgraph.h opt_ng_cronyx.h bpf.h sppp.h \
	device_if.h bus_if.h isa_if.h

NBPF?=		0
NCTAU?=		3
PROTOS?=	-DINET
NG_CRONYX?=	0
NETGRAPH=	${NG_CRONYX}

CFLAGS+= ${PROTOS}
CLEANFILES+= opt_ng_cronyx.h opt_netgraph.h bpf.h sppp.h

opt_netgraph.h:
	echo "#define NETGRAPH $(NETGRAPH)" > opt_netgraph.h

opt_ng_cronyx.h:
.if ${NG_CRONYX} != 0
	echo "#define NETGRAPH_CRONYX 1" > opt_ng_cronyx.h
.else
	echo "" > opt_ng_cronyx.h
.endif

sppp.h:
.if ${NG_CRONYX} == 0
	echo "#define NSPPP 1" > sppp.h
.else
	echo "#define NSPPP 0" > sppp.h
.endif	

bpf.h:
	echo "#define NBPF ${NBPF}" > bpf.h

.include <bsd.kmod.mk>
