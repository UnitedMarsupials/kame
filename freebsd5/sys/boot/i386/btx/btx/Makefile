# $FreeBSD: src/sys/boot/i386/btx/btx/Makefile,v 1.13 2002/09/17 01:48:54 peter Exp $

M4?=	m4

.if defined(PAGING)
M4FLAGS+=	-DPAGING
.endif

.if defined(BOOT_BTX_NOHANG)
BOOT_BTX_FLAGS=0x1
.else
BOOT_BTX_FLAGS=0x0
.endif

AFLAGS+=	--defsym BTX_FLAGS=${BOOT_BTX_FLAGS}

.if defined(BTX_SERIAL)
BOOT_COMCONSOLE_PORT?= 0x3f8
BOOT_COMCONSOLE_SPEED?= 9600
B2SIOFMT?=	0x3

M4FLAGS+=	-DBTX_SERIAL -DSIOPRT=${BOOT_COMCONSOLE_PORT} \
		-DSIOFMT=${B2SIOFMT} -DSIOSPD=${BOOT_COMCONSOLE_SPEED}
.endif

ORG=	0x9000

all: btx

btx: btx.o
	${LD} -N -e start -Ttext ${ORG} -o btx.out btx.o
	objcopy -S -O binary btx.out ${.TARGET}

btx.o: btx.s
	(cd ${.CURDIR}; ${M4} ${M4FLAGS} btx.s) | \
	   ${AS} ${AFLAGS} -o ${.TARGET}

CLEANFILES+= btx btx.out btx.o

.include <bsd.prog.mk>
