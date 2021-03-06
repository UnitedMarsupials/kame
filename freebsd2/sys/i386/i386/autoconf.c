/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
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
 *	from: @(#)autoconf.c	7.1 (Berkeley) 5/9/91
 *	$Id: autoconf.c,v 1.56.2.15 1998/05/20 19:32:10 gibbs Exp $
 */

/*
 * Setup the system to run on the current machine.
 *
 * Configure() is called at boot time and initializes the vba
 * device tables and the memory controller monitoring.  Available
 * devices are determined (from possibilities mentioned in ioconf.c),
 * and the drivers are initialized.
 */
#include "opt_cd9660.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/disklabel.h>
#include <sys/diskslice.h> /* for BASE_SLICE, MAX_SLICES */
#include <sys/dmap.h>
#include <sys/reboot.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/sysctl.h>

#include <machine/bootinfo.h>
#include <machine/cons.h>
#include <machine/md_var.h>
#include <i386/isa/icu.h> /* For interrupts */

#include "isa.h"
#if NISA > 0
#include <i386/isa/isa_device.h>
#endif

#include "pnp.h"
#if NPNP > 0
#include <i386/isa/pnp.h>
#endif
 
#include "eisa.h"
#if NEISA > 0
#include <i386/eisa/eisaconf.h>
#endif

#include "pci.h"
#if NPCI > 0
#include <pci/pcivar.h>
#endif

#include "card.h"
#if NCARD > 0
#include <pccard/driver.h>
#endif

#include "scbus.h"
#if NSCBUS > 0
#include <scsi/scsiconf.h>
#endif

static void	configure __P((void *));
SYSINIT(configure, SI_SUB_CONFIGURE, SI_ORDER_FIRST, configure, NULL)

#ifdef MFS_ROOT
extern struct vfsops	mfs_vfsops;
#endif
#ifdef FFS
extern struct vfsops	ufs_vfsops;
#endif
#ifdef LFS
extern struct vfsops	lfs_vfsops;
#endif
#ifdef NFS
extern int	nfs_mountroot __P((void *));
#endif
#ifdef MSDOSFS
extern int	msdosfs_mountroot __P((void *));
#endif

static void	configure_finish __P((void));
static void	configure_start __P((void));
static int	setdumpdev __P((dev_t dev));
static void	setroot __P((void));

#ifdef CD9660

#include <sys/fcntl.h>
#include <sys/proc.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <machine/clock.h>
#include <isofs/cd9660/iso.h>

/*
 * XXX All this CD-ROM root stuff is fairly messy.  Ick.
 *
 * We need to try out all our potential CDROM drives, so we need a table.
 */
static struct {
	char *name;
	int major;
} try_cdrom[] = {
	{ "cd", 6 },
	{ "mcd", 7 },
	{ "scd", 16 },
	{ "matcd", 17 },
	{ "wcd", 19 },
	{ 0, 0}
};

static int	find_cdrom_root __P((void));

static int
find_cdrom_root()
{
	int i, j, error;
	struct bdevsw *bd;
	dev_t orootdev;

#if CD9660_ROOTDELAY > 0
	DELAY(CD9660_ROOTDELAY * 1000000);
#endif
	orootdev = rootdev;
	for (i = 0 ; i < 2; i++)
		for (j = 0 ; try_cdrom[j].name ; j++) {
			if (try_cdrom[j].major >= nblkdev)
				continue;
			rootdev = makedev(try_cdrom[j].major, i * 8);
			bd = bdevsw[major(rootdev)];
			if (bd == NULL || bd->d_open == NULL)
				continue;
			if (bootverbose)
				printf("trying %s%d as rootdev (0x%x)\n",
				       try_cdrom[j].name, i, rootdev);
			error = (bd->d_open)(rootdev, FREAD, S_IFBLK, curproc);
			if (error == 0) {
				if (bd->d_close != NULL)
					(bd->d_close)(rootdev, FREAD, S_IFBLK,
						      curproc);
				return 0;
			}
		}

	rootdev = orootdev;
	return EINVAL;
}
#endif /* CD9660 */

static void
configure_start()
{
#if NSCBUS > 0
	scsi_configure_start();
#endif
}

static void
configure_finish()
{
#if NSCBUS > 0
	scsi_configure_finish();
#endif
}

/*
 * Determine i/o configuration for a machine.
 */
static void
configure(dummy)
	void *dummy;
{
	int i;

	configure_start();
	/* Allow all routines to decide for themselves if they want intrs */
	/*
	 * XXX Since this cannot be achieved on all architectures, we should
	 * XXX go back to disabling all interrupts until configuration is
	 * XXX completed and switch any devices that rely on the current
	 * XXX behavior to no longer rely on interrupts or to register an
	 * XXX interrupt_driven_config_hook for the task.
	 */
	/*
	 * XXX The above is wrong, because we're implicitly at splhigh(),
	 * XXX and should stay there, so enabling interrupts in the CPU
	 * XXX and the ICU at most gives pending interrupts which just get
	 * XXX in the way.
	 */
        enable_intr();
        INTREN(IRQ_SLAVE);

#if NEISA > 0
	eisa_configure();
#endif

#if NPCI > 0
	pci_configure();
#endif

#if NPNP > 0
	pnp_configure();
#endif

#if NISA > 0
	isa_configure();
#endif

#if NCARD > 0
	/* After everyone else has a chance at grabbing resources */
	pccard_configure();
#endif

	configure_finish();

	cninit_finish();

	if (bootverbose) {
		/*
		 * Print out the BIOS's idea of the disk geometries.
		 */
		printf("BIOS Geometries:\n");
		for (i = 0; i < N_BIOS_GEOM; i++) {
			unsigned long bios_geom;
			int max_cylinder, max_head, max_sector;

			bios_geom = bootinfo.bi_bios_geom[i];

			/*
			 * XXX the bootstrap punts a 1200K floppy geometry
			 * when the get-disk-geometry interrupt fails.  Skip
			 * drives that have this geometry.
			 */
			if (bios_geom == 0x4f010f)
				continue;

			printf(" %x:%08lx ", i, bios_geom);
			max_cylinder = bios_geom >> 16;
			max_head = (bios_geom >> 8) & 0xff;
			max_sector = bios_geom & 0xff;
			printf(
		"0..%d=%d cylinders, 0..%d=%d heads, 1..%d=%d sectors\n",
			       max_cylinder, max_cylinder + 1,
			       max_head, max_head + 1,
			       max_sector, max_sector);
		}
		printf(" %d accounted for\n", bootinfo.bi_n_bios_used);

		printf("Device configuration finished.\n");
	}
	cold = 0;
}

void
cpu_rootconf()
{
	/*
	 * XXX NetBSD has a much cleaner approach to finding root.
	 * XXX We should adopt their code.
	 */
#ifdef CD9660
	if ((boothowto & RB_CDROM) != 0) {
		if (bootverbose)
			printf("Considering CD-ROM root f/s.\n");
		/* NB: find_cdrom_root() sets rootdev if successful. */
		if (find_cdrom_root() == 0)
			mountroot = cd9660_mountroot;	/* XXX goes away*/
		else if (bootverbose)
			printf("No CD-ROM available as root f/s.\n");
	}
#endif

#ifdef MFS_ROOT
	if (!mountroot) {
		if (bootverbose)
			printf("Considering MFS root f/s.\n");
		mountroot = vfs_mountroot;	/* XXX goes away*/
		mountrootvfsops = &mfs_vfsops;
		/*
		 * Ignore the -a flag if this kernel isn't compiled
		 * with a generic root/swap configuration: if we skip
		 * setroot() and we aren't a generic kernel, chaos
		 * will ensue because setconf() will be a no-op.
		 * (rootdev is always initialized to NODEV in a
		 * generic configuration, so we test for that.)
		 */
		if ((boothowto & RB_ASKNAME) == 0 || rootdev != NODEV)
			setroot();
	}
#endif

#ifdef BOOTP_NFSROOT
	if (!mountroot && !nfs_diskless_valid) {
		if (bootverbose)
			printf("Considering BOOTP NFS root f/s.\n");
		mountroot = nfs_mountroot;
	}
#endif /* BOOTP_NFSROOT */

#ifdef NFS
	if (!mountroot && nfs_diskless_valid) {
		if (bootverbose)
			printf("Considering NFS root f/s.\n");
		mountroot = nfs_mountroot;
	}
#endif /* NFS */

#ifdef FFS
	if (!mountroot) {
		if (bootverbose)
			printf("Considering FFS root f/s.\n");
		mountroot = vfs_mountroot;	/* XXX goes away*/
		mountrootvfsops = &ufs_vfsops;
		/*
		 * Ignore the -a flag if this kernel isn't compiled
		 * with a generic root/swap configuration: if we skip
		 * setroot() and we aren't a generic kernel, chaos
		 * will ensue because setconf() will be a no-op.
		 * (rootdev is always initialized to NODEV in a
		 * generic configuration, so we test for that.)
		 */
		if ((boothowto & RB_ASKNAME) == 0 || rootdev != NODEV)
			setroot();
	}
#endif
#ifdef LFS
	if (!mountroot) {
		if (bootverbose)
			printf("Considering LFS root f/s.\n");
		mountroot = vfs_mountroot;	/* XXX goes away*/
		mountrootvfsops = &lfs_vfsops;
		/*
		 * Ignore the -a flag if this kernel isn't compiled
		 * with a generic root/swap configuration: if we skip
		 * setroot() and we aren't a generic kernel, chaos
		 * will ensue because setconf() will be a no-op.
		 * (rootdev is always initialized to NODEV in a
		 * generic configuration, so we test for that.)
		 */
		if ((boothowto & RB_ASKNAME) == 0 || rootdev != NODEV)
			setroot();
	}
#endif

	if (!mountroot) {
		panic("Nobody wants to mount my root for me");
	}
	setconf();
}

void
cpu_dumpconf()
{
	if (setdumpdev(dumpdev) != 0)
		dumpdev = NODEV;
}

static int
setdumpdev(dev)
	dev_t dev;
{
	int maj, psize;
	long newdumplo;

	if (dev == NODEV) {
		dumpdev = dev;
		return (0);
	}
	maj = major(dev);
	if (maj >= nblkdev)
		return (ENXIO);
	if (bdevsw[maj] == NULL)
		return (ENXIO);		/* XXX is this right? */
	if (bdevsw[maj]->d_psize == NULL)
		return (ENXIO);		/* XXX should be ENODEV ? */
	psize = bdevsw[maj]->d_psize(dev);
	if (psize == -1)
		return (ENXIO);		/* XXX should be ENODEV ? */
	/*
	 * XXX should clean up checking in dumpsys() to be more like this,
	 * and nuke dodump sysctl (too many knobs), and move this to
	 * kern_shutdown.c...
	 */
	if (dkpart(dev) != SWAP_PART)
		return (ENODEV);
	newdumplo = psize - Maxmem * PAGE_SIZE / DEV_BSIZE;
	if (newdumplo < 0)
		return (ENOSPC);
	dumpdev = dev;
	dumplo = newdumplo;
	return (0);
}

u_long	bootdev = 0;		/* not a dev_t - encoding is different */

/* Name lookup for bootable majors */
static char *devname[] = {
	"wd",
	"wfd",
#define FDMAJOR 2
	"fd",
	"wt",
	"sd",
};

#define	PARTITIONMASK	0x7
#define	PARTITIONSHIFT	3
#define FDUNITSHIFT     6
#define RAW_PART        2

/*
 * Attempt to find the device from which we were booted.
 * If we can do so, and not instructed not to do so,
 * change rootdev to correspond to the load device.
 */
static void
setroot()
{
	int  majdev, mindev, unit, part, adaptor, slice;
	dev_t orootdev;
	char *sname, partname[2];

/*printf("howto %x bootdev %x ", boothowto, bootdev);*/
	if (boothowto & RB_DFLTROOT ||
	    (bootdev & B_MAGICMASK) != (u_long)B_DEVMAGIC)
		return;
	majdev  = B_TYPE(bootdev);
	adaptor = B_ADAPTOR(bootdev);
	unit    = B_UNIT(bootdev);
	slice   = B_SLICE(bootdev);
	if ((slice < BASE_SLICE) || (slice >= MAX_SLICES))
		slice = COMPATIBILITY_SLICE;
	if (majdev > sizeof(devname) / sizeof(devname[0]))
		return;
	if (majdev == FDMAJOR) {
		part = RAW_PART;
		mindev = unit << FDUNITSHIFT;
	}
	else {
		part = (bootdev >> B_PARTITIONSHIFT) & B_PARTITIONMASK;
		mindev = dkmakeminor(unit, slice, part);
	}
	orootdev = rootdev;
	rootdev = makedev(majdev, mindev);
	/*
	 * If the original rootdev is the same as the one
	 * just calculated modulo the slice number, don't print an otherwise
	 * confusing diagnostic.
	 */
	if ((rootdev & ~0xff0000) == (orootdev & ~0xff0000))
		return;
	sname = dsname(devname[majdev], unit, slice, part, partname);
	printf("changing root device to %s%s\n", sname, partname);
}

static int
sysctl_kern_dumpdev SYSCTL_HANDLER_ARGS
{
	int error;
	dev_t ndumpdev;

	ndumpdev = dumpdev;
	error = sysctl_handle_opaque(oidp, &ndumpdev, sizeof ndumpdev, req);
	if (error == 0 && req->newptr != NULL)
		error = setdumpdev(ndumpdev);
	return (error);
}

SYSCTL_PROC(_kern, KERN_DUMPDEV, dumpdev, CTLTYPE_OPAQUE|CTLFLAG_RW,
	0, sizeof dumpdev, sysctl_kern_dumpdev, "T,dev_t", "");
