# New ports collection makefile for:    ruby
# Date created:         04 September 1997
# Whom:                 Eiji-usagi-MATSUmoto <usagi@ruby.club.or.jp>
#
# $FreeBSD: ports/lang/ruby/Makefile,v 1.16 2000/01/12 19:16:43 sumikawa Exp $
#

PORTNAME=	ruby
PORTVERSION=	1.4.5
CATEGORIES=	lang
MASTER_SITES=	ftp://ftp.netlab.co.jp/pub/lang/ruby/${BRANCH_NAME}/ \
		ftp://ftp.TokyoNet.AD.JP/pub/misc/ruby/${BRANCH_NAME}/ \
		ftp://ftp.iij.ad.jp/pub/lang/ruby/${BRANCH_NAME}/ \
		ftp://blade.nagaokaut.ac.jp/pub/lang/ruby/${BRANCH_NAME}/ \
		ftp://ftp.krnet.ne.jp/pub/ruby/${BRANCH_NAME}/ \
		ftp://mirror.nucba.ac.jp/mirror/ruby/${BRANCH_NAME}/

MAINTAINER=	yasuf@big.or.jp

GNU_CONFIGURE=	yes
CONFIGURE_ARGS+= --disable-shared
#.if defined(USE_INET6)
CONFIGURE_ARGS+= --enable-ipv6
#.endif
MAN1=		ruby.1
PLIST_SUB+=	RUBY_VER="${RUBY_VER}" RUBY_ARCH="${RUBY_ARCH}"

RUBY_VER=	1.4
RUBY_ARCH=	${ARCH}-freebsd${OSREL}

BRANCH_NAME=	1.4

DOCDIR=		${PREFIX}/share/doc/ruby
EXAMPLESDIR=	${PREFIX}/share/examples/ruby

post-patch:
	${RM} -f ${WRKSRC}/ext/tcltklib/MANIFEST
	${RM} -f ${WRKSRC}/ext/tk/MANIFEST
	${RM} -rf ${WRKSRC}/lib/irb ${WRKSRC}/sample/irb.rb

post-install:
	strip ${PREFIX}/bin/ruby
	${MKDIR} ${EXAMPLESDIR}
	${INSTALL_DATA} ${WRKSRC}/sample/* ${EXAMPLESDIR}
	${MKDIR} ${EXAMPLESDIR}/misc
	${INSTALL_DATA} ${WRKSRC}/misc/* ${EXAMPLESDIR}/misc
	${MKDIR} ${EXAMPLESDIR}/curses
	${INSTALL_DATA} ${WRKSRC}/ext/curses/hello.rb ${WRKSRC}/ext/curses/rain.rb ${WRKSRC}/ext/curses/view.rb ${EXAMPLESDIR}/curses
	${MKDIR} ${EXAMPLESDIR}/pty
	${INSTALL_DATA} ${WRKSRC}/ext/pty/expect_sample.rb ${WRKSRC}/ext/pty/script.rb ${WRKSRC}/ext/pty/shl.rb ${EXAMPLESDIR}/pty
.if !defined(NOPORTDOCS)
	${MKDIR} ${DOCDIR}
	${INSTALL_DATA} ${WRKSRC}/README* ${DOCDIR}
	${MKDIR} ${DOCDIR}/etc
	${INSTALL_DATA} ${WRKSRC}/ext/etc/etc.txt* ${DOCDIR}/etc
	${MKDIR} ${DOCDIR}/md5
	${INSTALL_DATA} ${WRKSRC}/ext/md5/md5.txt* ${DOCDIR}/md5
	${MKDIR} ${DOCDIR}/pty
	${INSTALL_DATA} ${WRKSRC}/ext/pty/README* ${DOCDIR}/pty
	${MKDIR} ${DOCDIR}/readline
	${INSTALL_DATA} ${WRKSRC}/ext/readline/README ${DOCDIR}/readline
.endif
	${SETENV} OBJFORMAT=${PORTOBJFORMAT} ${LDCONFIG} -m ${PREFIX}/lib
	@${CAT} ${PKGMESSAGE}

test:
	@(cd ${WRKSRC}; make test)

.include <bsd.port.mk>
