TARGET?=	bogus
ARCH?=		i386
.if ${TARGET} == "freebsd4" || ${TARGET} == "openbsd" || ${TARGET} == "netbsd"
KERNCONF?=	GENERIC.KAME
.else
KERNCONF?=	GENERIC.v6
.endif

DEVELOPER?=	NO
.if ${DEVELOPER} == "YES"
CVSHOST=	cvs.kame.net
.else
CVSHOST=	anoncvs.kame.net
.endif

all:
	@(echo '**** WARNING: Read through INSTALL and platform/INSTALL, '; \
	echo '    and follow the steps documented'.; \
	exit 1)

prepare::
	(cd ${.CURDIR}; perl prepare.pl kame ${TARGET})

clean::
	(cd ${.CURDIR}; find ${TARGET} -type l -print | perl -nle unlink)

# only for developers
bsdi3:
	(cd ${.CURDIR}; set CVSROOT=cvs.kame.net:/cvsroot/kame-local; export CVSROOT; cvs -d cvs.kame.net:/cvsroot/kame-local co -d bsdi3 -P kame/bsdi3)

bsdi4:
	(cd ${.CURDIR}; set CVSROOT=cvs.kame.net:/cvsroot/kame-local; export CVSROOT; cvs -d cvs.kame.net:/cvsroot/kame-local co -d bsdi4 -P kame/bsdi4)

PLAT=	freebsd2 freebsd3 freebsd4 kame netbsd openbsd bsdi3 bsdi4
# DOCS is defined in Makefile.inc
TOOLS=	Makefile Makefile.inc prepare.pl

update: update-doc update-plat
update-doc:
	(cd ${.CURDIR}; cvs update -d -P ${DOCS} ${TOOLS})
update-plat:
	(cd ${.CURDIR}; \
	for i in kame ${TARGET}; do \
		if test -d $$i; then \
			(cd $$i; cvs update -d -P); \
		fi \
	done)
update-all: update-doc
	(cd ${.CURDIR}; \
	for i in ${PLAT}; do \
		if test -d $$i; then \
			(cd $$i; cvs update -d -P); \
		fi \
	done)

# % cvs co kame/Makefile kame/Makefile.inc
# % cd kame
# % make TARGET=foo tree
tree:
	(cd ${.CURDIR}; \
	$(MAKE) update-doc; \
	if test $(TARGET) = bsdi3  -o $(TARGET) = bsdi4; then \
		$(MAKE) $(TARGET); \
	else \
		cvs update -d -P $(TARGET); \
	fi; \
	cvs update -d -P kame)

# use it with caution - must be root for "make includes"
autobuild:
	(cd ${.CURDIR}; ${MAKE} clean update prepare)
	(cd ${.CURDIR}/${TARGET}; ${MAKE} clean)
	case ${TARGET} in \
	bsdi*|freebsd*) \
		(cd ${.CURDIR}/${TARGET}; ${MAKE} includes); \
		(cd ${.CURDIR}/${TARGET}; ${MAKE} install-includes); \
		;; \
	netbsd*|openbsd*) \
		(cd ${.CURDIR}/${TARGET}; ${MAKE} includes); \
		;; \
	esac
	(cd ${.CURDIR}/${TARGET}; ${MAKE})
	case ${TARGET} in \
	bsdi*|freebsd*) \
		(cd ${.CURDIR}/${TARGET}/sys/compile; /bin/rm -fr ${KERNCONF}); \
		(cd ${.CURDIR}/${TARGET}/sys/${ARCH}/conf; config ${KERNCONF}); \
		(cd ${.CURDIR}/${TARGET}/sys/compile/${KERNCONF}; ${MAKE} depend; ${MAKE}); \
		;; \
	netbsd*|openbsd*) \
		(cd ${.CURDIR}/${TARGET}/sys/arch/${ARCH}/compile; /bin/rm -fr ${KERNCONF}); \
		(cd ${.CURDIR}/${TARGET}/sys/arch/${ARCH}/conf; config ${KERNCONF}); \
		(cd ${.CURDIR}/${TARGET}/sys/arch/${ARCH}/compile/${KERNCONF}; ${MAKE} depend; ${MAKE}); \
		;; \
	esac

.include "Makefile.inc"
