PROG = fetch
SRCS = file.c ftp.c http.c main.c util.c uri.c

CFLAGS+=	-Wall -Wwrite-strings -Wmissing-prototypes -DINET6 -g

DPADD=  ${LIBFTPIO} ${LIBMD}
LDADD=  -L../../lib/libftpio -lftpio -lmd
LDADD+= -L${.OBJDIR}/../../lib/libinet6 \
        -L/usr/local/v6/lib -linet6
DPADD+= ${.OBJDIR}/../../lib/libinet6/libinet6.a \
        /usr/local/v6/lib/libinet6.a

.include <bsd.prog.mk>
