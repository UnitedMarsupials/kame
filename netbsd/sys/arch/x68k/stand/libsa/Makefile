#	$NetBSD: Makefile,v 1.11 2002/05/05 11:46:30 isaki Exp $

S=		${.CURDIR}/../../../..
LIBSADIR=	$S/lib/libsa
LIBKERNDIR=	$S/lib/libkern
LIBZDIR=	$S/lib/libz

LIB=		sa

SRCS+=		alloc.c bcmp.c bcopy.c bzero.c errno.c getfile.c gets.c
SRCS+=		memcmp.c memcpy.c memset.c panic.c
SRCS+=		printf.c snprintf.c sprintf.c subr_prf.c twiddle.c vsprintf.c

SRCS+=		close.c closeall.c cread.c dev.c fstat.c ioctl.c lseek.c
SRCS+=		open.c read.c stat.c
SRCS+=		loadfile.c loadfile_aout.c loadfile_elf32.c
SRCS+=		cd9660.c lfs.c nfs.c ufs.c ufs_ls.c ustarfs.c
#SRCS+=		arp.c ether.c in_cksum.c net.c netif.c rpc.c udp.c
#SRCS+=		bootp.c

SRCS+=		ashldi3.c ashrdi3.c strcat.c strchr.c strcmp.c strncmp.c strcpy.c strerror.c strlen.c

SRCS+=		adler32.c crc32.c infblock.c infcodes.c inffast.c
SRCS+=		inflate.c inftrees.c infutil.c uncompr.c

SRCS+=		consio.c devopen.c parseutils.c sdcd.c fd.c fdsub.S chdsk.c
SRCS+=		putimage.S

NOPROFILE=	# defined
NOPIC=		# defined
NOLINT=		# defined

CPPFLAGS+=	-I${.OBJDIR} -I${LIBSADIR} -I$S -I${LIBZDIR} -D_STANDALONE
CPPFLAGS+=	-I${.CURDIR}/../libiocs -I${.CURDIR}/../common
CPPFLAGS+=	-DHEAP_VARIABLE -D__INTERNAL_LIBSA_CREAD
CPPFLAGS+=	-D_ZLIB_PRIVATE
CPPFLAGS+=	-DHAVE_CHANGEDISK_HOOK
#CPPFLAGS+=	-DDEBUG

.PATH:	${LIBSADIR} ${LIBKERNDIR} ${LIBZDIR}

.include "../Makefile.booters"

# only needed during build
libinstall::

.include <bsd.own.mk>
.undef DESTDIR
.include <bsd.lib.mk>
