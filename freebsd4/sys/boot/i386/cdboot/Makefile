# $FreeBSD: src/sys/boot/i386/cdboot/Makefile,v 1.4.2.1 2001/12/21 21:01:41 jhb Exp $

MAINTAINER=jhb@FreeBSD.org

ORG=		0x7c00

PROG=		cdboot
NOMAN=
STRIP=
BINDIR?=	/boot

${PROG}: ${PROG}.o
.if ${OBJFORMAT} == aout
	${LD} -nostdlib -N -s -T ${ORG} -o ${PROG}.out ${PROG}.o
	dd if=${PROG}.out of=${.TARGET} ibs=32 skip=1
.else
	${LD} -N -e start -Ttext ${ORG} -o ${PROG}.out ${PROG}.o
	objcopy -S -O binary ${PROG}.out ${.TARGET}
.endif

CLEANFILES+=	${PROG}.o ${PROG}.out

.include <bsd.prog.mk>
