# $FreeBSD: src/sys/modules/acpi/acpi_panasonic/Makefile,v 1.2 2004/08/03 21:27:42 njl Exp $

.PATH:	${.CURDIR}/../../../i386/acpica

KMOD=	acpi_panasonic
WARNS?=	2
CFLAGS+= -I${.CURDIR}/../../../contrib/dev/acpica
SRCS=	acpi_panasonic.c opt_acpi.h acpi_if.h bus_if.h device_if.h

.include <bsd.kmod.mk>
