# $FreeBSD: src/sys/modules/sbni/Makefile,v 1.1.2.1 2001/12/19 20:59:29 fjoe Exp $

.PATH: ${.CURDIR}/../../dev/sbni

KMOD=	if_sbni
SRCS=	if_sbni.c if_sbni_pci.c

SRCS+=	bus_if.h device_if.h isa_if.h pci_if.h

.include <bsd.kmod.mk>
