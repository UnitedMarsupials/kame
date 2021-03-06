/*
 *	Slot structures for PC-CARD interface.
 *	Each slot has a controller specific structure
 *	attached to it. A slot number allows
 *	mapping from the character device to the
 *	slot structure. This is separate to the
 *	controller slot number to allow multiple controllers
 *	to be accessed.
 *-------------------------------------------------------------------------
 *
 * Copyright (c) 1995 Andrew McRae.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *	Controller data - Specific to each slot controller.
 */
struct slot;
struct slot_ctrl {
	int	(*mapmem) __P((struct slot *, int));
				/* Map memory */
	int	(*mapio) __P((struct slot *, int));
				/* Map io */
	void	(*reset) __P((void *));
				/* init */
	void	(*disable) __P((struct slot *));
				/* Disable slot */
	int	(*power) __P((struct slot *));
				/* Set power values */
	int	(*ioctl) __P((struct slot *, int, caddr_t));
				/* ioctl to lower level */
	void	(*mapirq) __P((struct slot *, int));
				/* Map interrupt number */
	void	(*resume) __P((struct slot *));
				/* suspend/resume support */
	int	extra;		/* Controller specific size */
	int	maxmem;		/* Number of allowed memory windows */
	int	maxio;		/* Number of allowed I/O windows */
	int	irqs;		/* IRQ's that are allowed */
	u_int	*imask;		/* IRQ mask for the PCIC controller */
	char	*name;		/* controller name */

	/*
	 *	The rest is maintained by the mainline PC-CARD code.
	 */
	struct slot_ctrl *next;	/* Allows linked list of controllers */
	int	slots;		/* Slots available */
};

/*
 *	Driver structure - each driver registers itself
 *	with the mainline PC-CARD code. These drivers are
 *	then available for linking to the devices.
 */
struct pccard_devinfo;

struct pccard_device {
	char	*name;					/* Driver name */
	int (*enable)(struct pccard_devinfo *);		/* init/enable driver */
	void (*disable)(struct pccard_devinfo *);	/* disable driver */
	int (*handler)(struct pccard_devinfo *);	/* interrupt handler */
	int	attr;					/* driver attributes */
	unsigned int *imask;				/* Interrupt mask ptr */

	struct pccard_device *next;
};

/*
 *	Device structure for cards. Each card may have one
 *	or more pccard drivers attached to it; each driver is assumed
 *	to require at most one interrupt handler, one I/O block
 *	and one memory block. This structure is used to link the different
 *	devices together.
 */
struct pccard_devinfo {
	struct pccard_device *drv;
	struct isa_device isahd;	/* Device details */
#if 0
	void *arg;			/* Device argument */
#endif
	int running;			/* Current state of driver */
	u_char	misc[128];		/* For any random info */
	struct slot *slt;		/* Back pointer to slot */

	struct pccard_devinfo *next;	/* List of drivers */
};

/*
 *	Per-slot structure.
 */
struct slot {
	int slotnum;			/* Slot number */
	int flags;			/* Slot flags (see below) */
	int rwmem;			/* Read/write flags */
	int irq;			/* IRQ allocated (0 = none) */
	int irqref;			/* Reference count of driver IRQs */
	struct pccard_devinfo *devices;	/* List of drivers attached */
	/*
	 *	flags.
	 */
	unsigned int 	insert_seq;	/* Firing up under the card */

	enum cardstate 	state, laststate; /* Current/last card states */
	struct selinfo	selp;		/* Info for select */
	struct mem_desc	mem[NUM_MEM_WINDOWS];	/* Memory windows */
	struct io_desc	io[NUM_IO_WINDOWS];	/* I/O windows */
	struct power	pwr;		/* Power values */
	struct slot_ctrl *ctrl;		/* Per-controller data */
	void		*cdata;		/* Controller specific data */
	int		pwr_off_pending;/* Power status of slot */
#ifdef DEVFS
	void		*devfs_token;
#endif /* DEVFS*/

	struct slot *next;		/* Master list */
};

enum card_event { card_removed, card_inserted };

struct slot	*pccard_alloc_slot(struct slot_ctrl *);
void		 pccard_event(struct slot *, enum card_event);
void		 pccard_remove_controller(struct slot_ctrl *);
