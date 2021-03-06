/*-
 * Copyright (c) 1982, 1986 The Regents of the University of California.
 * Copyright (c) 1989, 1990 William Jolitz
 * Copyright (c) 1994 John Dyson
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department, and William Jolitz.
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
 *	from: @(#)vm_machdep.c	7.3 (Berkeley) 5/13/91
 *	Utah $Hdr: vm_machdep.c 1.16.1.1 89/06/23$
 * $FreeBSD: src/sys/powerpc/powerpc/vm_machdep.c,v 1.79 2002/12/10 02:33:44 julian Exp $
 */
/*
 * Copyright (c) 1994, 1995, 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <sys/bio.h>
#include <sys/buf.h>
#include <sys/ktr.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/vnode.h>
#include <sys/vmmeter.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <sys/unistd.h>

#include <machine/clock.h>
#include <machine/cpu.h>
#include <machine/fpu.h>
#include <machine/frame.h>
#include <machine/md_var.h>

#include <dev/ofw/openfirm.h>

#include <vm/vm.h>
#include <vm/vm_param.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <vm/vm_map.h>
#include <vm/vm_extern.h>

#include <sys/user.h>

/*
 * quick version of vm_fault
 */
int
vm_fault_quick(v, prot)
	caddr_t v;
	int prot;
{
	int r;
	if (prot & VM_PROT_WRITE)
		r = subyte(v, fubyte(v));
	else
		r = fubyte(v);
	return(r);
}

/*
 * Finish a fork operation, with process p2 nearly set up.
 * Copy and update the pcb, set up the stack so that the child
 * ready to run and return to user mode.
 */
void
cpu_fork(struct thread *td1, struct proc *p2, struct thread *td2, int flags)
{
	struct	proc *p1;
	struct	trapframe *tf;
	struct	callframe *cf;
	struct	pcb *pcb;

	KASSERT(td1 == curthread || td1 == &thread0,
	    ("cpu_fork: p1 not curproc and not proc0"));
	CTR3(KTR_PROC, "cpu_fork: called td1=%08x p2=%08x flags=%x", (u_int)td1, (u_int)p2, flags);

	if ((flags & RFPROC) == 0)
		return;

	p1 = td1->td_proc;

	pcb = (struct pcb *)((td2->td_kstack + KSTACK_PAGES * PAGE_SIZE -
	    sizeof(struct pcb)) & ~0x2fU);
	td2->td_pcb = pcb;

	/* Copy the pcb */
	bcopy(td1->td_pcb, pcb, sizeof(struct pcb));

	/*
	 * Create a fresh stack for the new process.
	 * Copy the trap frame for the return to user mode as if from a
	 * syscall.  This copies most of the user mode register values.
	 */
	tf = (struct trapframe *)pcb - 1;
	bcopy(td1->td_frame, tf, sizeof(*tf));

	/* Set up trap frame. */
	tf->fixreg[FIRSTARG] = 0;
	tf->fixreg[FIRSTARG + 1] = 0;
	tf->cr &= ~0x10000000;

	td2->td_frame = tf;

	cf = (struct callframe *)tf - 1;
	cf->cf_func = (register_t)fork_return;
	cf->cf_arg0 = (register_t)td2;
	cf->cf_arg1 = (register_t)tf;

	pcb->pcb_sp = (register_t)cf;
	pcb->pcb_lr = (register_t)fork_trampoline;
	pcb->pcb_usr = kernel_pmap->pm_sr[USER_SR];

	/*
 	 * Now cpu_switch() can schedule the new process.
	 */
}

/*
 * Intercept the return address from a freshly forked process that has NOT
 * been scheduled yet.
 *
 * This is needed to make kernel threads stay in kernel mode.
 */
void
cpu_set_fork_handler(td, func, arg)
	struct thread *td;
	void (*func)(void *);
	void *arg;
{
	struct	callframe *cf;

	CTR3(KTR_PROC, "cpu_set_fork_handler: called with td=%08x func=%08x arg=%08x",
	    (u_int)td, (u_int)func, (u_int)arg);

	cf = (struct callframe *)td->td_pcb->pcb_sp;

	cf->cf_func = (register_t)func;
	cf->cf_arg0 = (register_t)arg;
}

/*
 * cpu_exit is called as the last action during exit.
 * We release the address space of the process, block interrupts,
 * and call switch_exit.  switch_exit switches to proc0's PCB and stack,
 * then jumps into the middle of cpu_switch, as if it were switching
 * from proc0.
 */
void
cpu_exit(td)
	register struct thread *td;
{
}

void
cpu_sched_exit(td)
	register struct thread *td;
{
}

void
cpu_wait(td)
	struct proc *td;
{
}

/* Temporary helper */
void
cpu_throw(void)
{

	cpu_switch();
	panic("cpu_throw() didn't");
}

/*
 * Map an IO request into kernel virtual address space.
 *
 * All requests are (re)mapped into kernel VA space.
 * Notice that we use b_bufsize for the size of the buffer
 * to be mapped.  b_bcount might be modified by the driver.
 */
void
vmapbuf(struct buf *bp)
{
	caddr_t addr, kva;
	vm_offset_t pa;
	int pidx;
	struct vm_page *m;
	pmap_t pmap;

	GIANT_REQUIRED;

	if ((bp->b_flags & B_PHYS) == 0)
		panic("vmapbuf");

        pmap = &curproc->p_vmspace->vm_pmap;
	for (addr = (caddr_t)trunc_page(bp->b_data), pidx = 0;
	    addr < bp->b_data + bp->b_bufsize;
	    addr += PAGE_SIZE, pidx++) {
		/*
		 * Do the vm_fault if needed; do the copy-on-write thing
		 * when reading stuff off device into memory.
		 */
		vm_fault_quick((addr >= bp->b_data) ? addr : bp->b_data,
			(bp->b_iocmd == BIO_READ)?(VM_PROT_READ|VM_PROT_WRITE):VM_PROT_READ);
		pa = trunc_page(pmap_extract(pmap, (vm_offset_t) addr));
		if (pa == 0)
			panic("vmapbuf: page not present");
		m = PHYS_TO_VM_PAGE(pa);
		vm_page_hold(m);
		bp->b_pages[pidx] = m;
	}
	if (pidx > btoc(MAXPHYS))
		panic("vmapbuf: mapped more than MAXPHYS");
	pmap_qenter((vm_offset_t)bp->b_saveaddr, bp->b_pages, pidx);

	kva = bp->b_saveaddr;
	bp->b_npages = pidx;
	bp->b_saveaddr = bp->b_data;
	bp->b_data = kva + (((vm_offset_t) bp->b_data) & PAGE_MASK);
}

/*
 * Free the io map PTEs associated with this IO operation.
 * We also invalidate the TLB entries and restore the original b_addr.
 */
void
vunmapbuf(struct buf *bp)
{
	int pidx;
	int npages;

	GIANT_REQUIRED;

	if ((bp->b_flags & B_PHYS) == 0)
		panic("vunmapbuf");

	npages = bp->b_npages;
	pmap_qremove(trunc_page((vm_offset_t)bp->b_data),
	    npages);
	for (pidx = 0; pidx < npages; pidx++)
		vm_page_unhold(bp->b_pages[pidx]);

	bp->b_data = bp->b_saveaddr;
}

/*
 * Reset back to firmware.
 */
void
cpu_reset()
{
	OF_exit();
}

/*
 * Software interrupt handler for queued VM system processing.
 */   
void  
swi_vm(void *dummy) 
{     
#if 0 /* XXX: Don't have busdma stuff yet */
	if (busdma_swi_pending != 0)
		busdma_swi();
#endif
}

/*
 * Tell whether this address is in some physical memory region.
 * Currently used by the kernel coredump code in order to avoid
 * dumping the ``ISA memory hole'' which could cause indefinite hangs,
 * or other unpredictable behaviour.
 */


int
is_physical_memory(addr)
	vm_offset_t addr;
{
	/*
	 * stuff other tests for known memory-mapped devices (PCI?)
	 * here
	 */

	return 1;
}

/*
 * KSE functions
 */
void
cpu_thread_exit(struct thread *td)     
{

	return;
}

void
cpu_thread_clean(struct thread *td)     
{
}

void
cpu_thread_setup(struct thread *td)
{

	return;
}

void
cpu_set_upcall(struct thread *td, void *pcb)
{

	return;
}

void
cpu_set_upcall_kse(struct thread *td, struct kse *ke)
{

	return;
}
