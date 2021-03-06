/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)subr_log.c	8.1 (Berkeley) 6/10/93
 * $Id: subr_log.c,v 1.18.2.1 1998/06/25 00:47:11 jkh Exp $
 */

/*
 * Error log buffer for kernel printf's.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/ioctl.h>
#include <sys/msgbuf.h>
#include <sys/file.h>
#include <sys/signalvar.h>
#include <sys/kernel.h>
#ifdef DEVFS
#include <sys/devfsext.h>
#endif /*DEVFS*/

#define LOG_RDPRI	(PZERO + 1)

#define LOG_ASYNC	0x04
#define LOG_RDWAIT	0x08

static	d_open_t	logopen;
static	d_close_t	logclose;
static	d_read_t	logread;
static	d_ioctl_t	logioctl;
static	d_select_t	logselect;

#define CDEV_MAJOR 7
static struct cdevsw log_cdevsw = 
	{ logopen,	logclose,	logread,	nowrite,	/*7*/
	  logioctl,	nostop,		nullreset,	nodevtotty,/* klog */
	  logselect,	nommap,		NULL,	"log",	NULL,	-1 };

static struct logsoftc {
	int	sc_state;		/* see above for possibilities */
	struct	selinfo sc_selp;	/* process waiting on select call */
	int	sc_pgid;		/* process/group for async I/O */
} logsoftc;

int	log_open;			/* also used in log() */

/*ARGSUSED*/
static	int
logopen(dev, flags, mode, p)
	dev_t dev;
	int flags, mode;
	struct proc *p;
{
	if (log_open)
		return (EBUSY);
	log_open = 1;
	logsoftc.sc_pgid = p->p_pid;		/* signal process only */
	return (0);
}

/*ARGSUSED*/
static	int
logclose(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{

	log_open = 0;
	logsoftc.sc_state = 0;
	return (0);
}

/*ARGSUSED*/
static	int
logread(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	register struct msgbuf *mbp = msgbufp;
	register long l;
	register int s;
	int error = 0;

	s = splhigh();
	while (mbp->msg_bufr == mbp->msg_bufx) {
		if (flag & IO_NDELAY) {
			splx(s);
			return (EWOULDBLOCK);
		}
		logsoftc.sc_state |= LOG_RDWAIT;
		if ((error = tsleep((caddr_t)mbp, LOG_RDPRI | PCATCH,
		    "klog", 0))) {
			splx(s);
			return (error);
		}
	}
	splx(s);
	logsoftc.sc_state &= ~LOG_RDWAIT;

	while (uio->uio_resid > 0) {
		l = mbp->msg_bufx - mbp->msg_bufr;
		if (l < 0)
			l = mbp->msg_size - mbp->msg_bufr;
		l = min(l, uio->uio_resid);
		if (l == 0)
			break;
		error = uiomove((caddr_t)msgbufp->msg_ptr + mbp->msg_bufr,
		    (int)l, uio);
		if (error)
			break;
		mbp->msg_bufr += l;
		if (mbp->msg_bufr >= mbp->msg_size)
			mbp->msg_bufr = 0;
	}
	return (error);
}

/*ARGSUSED*/
static	int
logselect(dev, rw, p)
	dev_t dev;
	int rw;
	struct proc *p;
{
	int s = splhigh();

	switch (rw) {

	case FREAD:
		if (msgbufp->msg_bufr != msgbufp->msg_bufx) {
			splx(s);
			return (1);
		}
		selrecord(p, &logsoftc.sc_selp);
		break;
	}
	splx(s);
	return (0);
}

void
logwakeup()
{
	struct proc *p;

	if (!log_open)
		return;
	selwakeup(&logsoftc.sc_selp);
	if (logsoftc.sc_state & LOG_ASYNC) {
		if (logsoftc.sc_pgid < 0)
			gsignal(-logsoftc.sc_pgid, SIGIO);
		else if ((p = pfind(logsoftc.sc_pgid)))
			psignal(p, SIGIO);
	}
	if (logsoftc.sc_state & LOG_RDWAIT) {
		wakeup((caddr_t)msgbufp);
		logsoftc.sc_state &= ~LOG_RDWAIT;
	}
}

/*ARGSUSED*/
static	int
logioctl(dev, com, data, flag, p)
	dev_t dev;
	int com;
	caddr_t data;
	int flag;
	struct proc *p;
{
	long l;
	int s;

	switch (com) {

	/* return number of characters immediately available */
	case FIONREAD:
		s = splhigh();
		l = msgbufp->msg_bufx - msgbufp->msg_bufr;
		splx(s);
		if (l < 0)
			l += msgbufp->msg_size;
		*(int *)data = l;
		break;

	case FIONBIO:
		break;

	case FIOASYNC:
		if (*(int *)data)
			logsoftc.sc_state |= LOG_ASYNC;
		else
			logsoftc.sc_state &= ~LOG_ASYNC;
		break;

	case TIOCSPGRP:
		logsoftc.sc_pgid = *(int *)data;
		break;

	case TIOCGPGRP:
		*(int *)data = logsoftc.sc_pgid;
		break;

	default:
		return (ENOTTY);
	}
	return (0);
}

static log_devsw_installed = 0;
#ifdef DEVFS
static	void	*log_devfs_token;
#endif

static void
log_drvinit(void *unused)
{
	dev_t dev;

	if( ! log_devsw_installed ) {
		dev = makedev(CDEV_MAJOR,0);
		cdevsw_add(&dev,&log_cdevsw,NULL);
		log_devsw_installed = 1;
#ifdef DEVFS
		log_devfs_token = devfs_add_devswf(&log_cdevsw, 0, DV_CHR,
						   UID_ROOT, GID_WHEEL, 0600,
						   "klog");
#endif
    	}
}

SYSINIT(logdev,SI_SUB_DRIVERS,SI_ORDER_MIDDLE+CDEV_MAJOR,log_drvinit,NULL)


