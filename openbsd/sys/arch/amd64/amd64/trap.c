/*	$OpenBSD: trap.c,v 1.3 2004/02/23 08:32:36 mickey Exp $	*/
/*	$NetBSD: trap.c,v 1.2 2003/05/04 23:51:56 fvdl Exp $	*/

/*-
 * Copyright (c) 1998, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the University of Utah, and William Jolitz.
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
 *	@(#)trap.c	7.4 (Berkeley) 5/13/91
 */

/*
 * amd64 Trap and System call handling
 */
#undef	TRAP_SIGDEBUG

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/acct.h>
#include <sys/kernel.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <sys/reboot.h>
#include <sys/pool.h>

#include <uvm/uvm_extern.h>

#include <machine/cpu.h>
#include <machine/cpufunc.h>
#include <machine/fpu.h>
#include <machine/psl.h>
#include <machine/reg.h>
#include <machine/trap.h>
#include <machine/userret.h>
#ifdef DDB
#include <machine/db_machdep.h>
#endif

#include "isa.h"

#ifdef KGDB
#include <sys/kgdb.h>
#endif

void trap(struct trapframe);
#if defined(I386_CPU)
int trapwrite(unsigned);
#endif

const char *trap_type[] = {
	"privileged instruction fault",		/*  0 T_PRIVINFLT */
	"breakpoint trap",			/*  1 T_BPTFLT */
	"arithmetic trap",			/*  2 T_ARITHTRAP */
	"asynchronous system trap",		/*  3 T_ASTFLT */
	"protection fault",			/*  4 T_PROTFLT */
	"trace trap",				/*  5 T_TRCTRAP */
	"page fault",				/*  6 T_PAGEFLT */
	"alignment fault",			/*  7 T_ALIGNFLT */
	"integer divide fault",			/*  8 T_DIVIDE */
	"non-maskable interrupt",		/*  9 T_NMI */
	"overflow trap",			/* 10 T_OFLOW */
	"bounds check fault",			/* 11 T_BOUND */
	"FPU not available fault",		/* 12 T_DNA */
	"double fault",				/* 13 T_DOUBLEFLT */
	"FPU operand fetch fault",		/* 14 T_FPOPFLT */
	"invalid TSS fault",			/* 15 T_TSSFLT */
	"segment not present fault",		/* 16 T_SEGNPFLT */
	"stack fault",				/* 17 T_STKFLT */
	"machine check",			/* 18 T_MCA */
	"SSE FP exception",			/* 19 T_XMM */
	"reserved trap",			/* 20 T_RESERVED */
};
int	trap_types = sizeof trap_type / sizeof trap_type[0];

#ifdef DEBUG
int	trapdebug = 0;
#endif

#define	IDTVEC(name)	__CONCAT(X, name)

#ifdef TRAP_SIGDEBUG
static void frame_dump(struct trapframe *);
#endif

/*
 * trap(frame):
 *	Exception, fault, and trap interface to BSD kernel. This
 * common code is called from assembly language IDT gate entry
 * routines that prepare a suitable stack frame, and restore this
 * frame after the exception has been processed. Note that the
 * effect is as if the arguments were passed call by reference.
 */
/*ARGSUSED*/
void
trap(frame)
	struct trapframe frame;
{
	struct proc *p = curproc;
	int type = (int)frame.tf_trapno;
	struct pcb *pcb;
	extern char resume_iret[], IDTVEC(oosyscall)[];
#if 0
	extern char resume_pop_ds[], resume_pop_es[];
#endif
	struct trapframe *vframe;
	void *resume;
	caddr_t onfault;
	int error;
	uint64_t cr2;
	union sigval sv;

	uvmexp.traps++;

	pcb = (p != NULL && p->p_addr != NULL) ? &p->p_addr->u_pcb : NULL;

#ifdef DEBUG
	if (trapdebug) {
		printf("trap %d code %lx eip %lx cs %lx rflags %lx cr2 %lx "
		       "cpl %x\n",
		    type, frame.tf_err, frame.tf_rip, frame.tf_cs,
		    frame.tf_rflags, rcr2(), curcpu()->ci_ilevel);
		printf("curproc %p\n", curproc);
		if (curproc)
			printf("pid %d\n", p->p_pid);
	}
#endif

	if (!KERNELMODE(frame.tf_cs, frame.tf_rflags)) {
		type |= T_USER;
		p->p_md.md_regs = &frame;
	}

	switch (type) {

	default:
	we_re_toast:
#ifdef KGDB
		if (kgdb_trap(type, &frame))
			return;
		else {
			/*
			 * If this is a breakpoint, don't panic
			 * if we're not connected.
			 */
			if (type == T_BPTFLT) {
				printf("kgdb: ignored %s\n", trap_type[type]);
				return;
			}
		}
#endif
#ifdef DDB
		if (kdb_trap(type, 0, &frame))
			return;
#endif
		if (frame.tf_trapno < trap_types)
			printf("fatal %s", trap_type[frame.tf_trapno]);
		else
			printf("unknown trap %ld", (u_long)frame.tf_trapno);
		printf(" in %s mode\n", (type & T_USER) ? "user" : "supervisor");
		printf("trap type %d code %lx rip %lx cs %lx rflags %lx cr2 "
		       " %lx cpl %x rsp %lx\n",
		    type, frame.tf_err, (u_long)frame.tf_rip, frame.tf_cs,
		    frame.tf_rflags, rcr2(), curcpu()->ci_ilevel, frame.tf_rsp);

		/* panic("trap"); */
		boot(RB_HALT);
		/*NOTREACHED*/

	case T_PROTFLT:
	case T_SEGNPFLT:
	case T_ALIGNFLT:
	case T_TSSFLT:
		if (p == NULL)
			goto we_re_toast;
		/* Check for copyin/copyout fault. */
		if (pcb->pcb_onfault != 0) {
			error = EFAULT;
copyfault:
			frame.tf_rip = (u_int64_t)pcb->pcb_onfault;
			frame.tf_rax = error;
			return;
		}

		/*
		 * Check for failure during return to user mode.
		 *
		 * XXXfvdl check for rex prefix?
		 *
		 * We do this by looking at the instruction we faulted on.  The
		 * specific instructions we recognize only happen when
		 * returning from a trap, syscall, or interrupt.
		 *
		 * XXX
		 * The heuristic used here will currently fail for the case of
		 * one of the 2 pop instructions faulting when returning from a
		 * a fast interrupt.  This should not be possible.  It can be
		 * fixed by rearranging the trap frame so that the stack format
		 * at this point is the same as on exit from a `slow'
		 * interrupt.
		 */
		switch (*(u_char *)frame.tf_rip) {
		case 0xcf:	/* iret */
			vframe = (void *)((u_int64_t)&frame.tf_rsp - 44);
			resume = resume_iret;
			break;
/*
 * XXXfvdl these are illegal in long mode (not in compat mode, though)
 * and we do not take back the descriptors from the signal context anyway,
 * but may do so later for USER_LDT, in which case we need to intercept
 * other instructions (movl %eax, %Xs).
 */
#if 0
		case 0x1f:	/* popl %ds */
			vframe = (void *)((u_int64_t)&frame.tf_rsp - 4);
			resume = resume_pop_ds;
			break;
		case 0x07:	/* popl %es */
			vframe = (void *)((u_int64_t)&frame.tf_rsp - 0);
			resume = resume_pop_es;
			break;
#endif
		default:
			goto we_re_toast;
		}
		if (KERNELMODE(vframe->tf_cs, vframe->tf_rflags))
			goto we_re_toast;

		frame.tf_rip = (u_int64_t)resume;
		return;

	case T_PROTFLT|T_USER:		/* protection fault */
	case T_TSSFLT|T_USER:
	case T_SEGNPFLT|T_USER:
	case T_STKFLT|T_USER:
	case T_NMI|T_USER:
#ifdef TRAP_SIGDEBUG
		printf("pid %d (%s): BUS at rip %lx addr %lx\n",
		    p->p_pid, p->p_comm, frame.tf_rip, rcr2());
		frame_dump(&frame);
#endif
		sv.sival_ptr = (void *)frame.tf_rip;
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGBUS, type & ~T_USER, BUS_OBJERR, sv);
		KERNEL_PROC_UNLOCK(p);
		goto out;
	case T_ALIGNFLT|T_USER:
		sv.sival_ptr = (void *)frame.tf_rip;
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGBUS, type & ~T_USER, BUS_ADRALN, sv);
		KERNEL_PROC_UNLOCK(p);
		goto out;

	case T_PRIVINFLT|T_USER:	/* privileged instruction fault */
		sv.sival_ptr = (void *)frame.tf_rip;
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGILL, type & ~T_USER, ILL_PRVOPC, sv);
		KERNEL_PROC_UNLOCK(p);
		goto out;
	case T_FPOPFLT|T_USER:		/* coprocessor operand fault */
#ifdef TRAP_SIGDEBUG
		printf("pid %d (%s): ILL at rip %lx addr %lx\n",
		    p->p_pid, p->p_comm, frame.tf_rip, rcr2());
		frame_dump(&frame);
#endif
		sv.sival_ptr = (void *)frame.tf_rip;
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGILL, type & ~T_USER, ILL_COPROC, sv);
		KERNEL_PROC_UNLOCK(p);
		goto out;

	case T_ASTFLT|T_USER:		/* Allow process switch */
		uvmexp.softs++;
		if (p->p_flag & P_OWEUPC) {
			p->p_flag &= ~P_OWEUPC;
			KERNEL_PROC_LOCK(p);
			ADDUPROF(p);
			KERNEL_PROC_UNLOCK(p);
		}
		/* Allow a forced task switch. */
		if (curcpu()->ci_want_resched)
			preempt(NULL);
		goto out;

	case T_DNA|T_USER: {
		printf("pid %d killed due to lack of floating point\n",
		    p->p_pid);
		sv.sival_ptr = (void *)frame.tf_rip;
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGKILL, type &~ T_USER, FPE_FLTINV, sv);
		KERNEL_PROC_UNLOCK(p);
		goto out;
	}

	case T_BOUND|T_USER:
		sv.sival_ptr = (void *)frame.tf_rip;
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGFPE, type &~ T_USER, FPE_FLTSUB, sv);
		KERNEL_PROC_UNLOCK(p);
		goto out;
	case T_OFLOW|T_USER:
		sv.sival_ptr = (void *)frame.tf_rip;
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGFPE, type &~ T_USER, FPE_INTOVF, sv);
		KERNEL_PROC_UNLOCK(p);
		goto out;
	case T_DIVIDE|T_USER:
		sv.sival_ptr = (void *)frame.tf_rip;
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGFPE, type &~ T_USER, FPE_INTDIV, sv);
		KERNEL_PROC_UNLOCK(p);
		goto out;

	case T_ARITHTRAP|T_USER:
	case T_XMM|T_USER:
		fputrap(&frame);
		goto out;

	case T_PAGEFLT:			/* allow page faults in kernel mode */
		if (p == NULL)
			goto we_re_toast;
#ifdef LOCKDEBUG
		if (simple_lock_held(&sched_lock))
			goto we_re_toast;
#endif
#ifdef MULTIPROCESSOR
		if ((p->p_flag & P_BIGLOCK) == 0)
			goto we_re_toast;
#endif
		cr2 = rcr2();
		KERNEL_LOCK(LK_CANRECURSE|LK_EXCLUSIVE);
		goto faultcommon;

	case T_PAGEFLT|T_USER: {	/* page fault */
		vaddr_t va, fa;
		struct vmspace *vm;
		struct vm_map *map;
		vm_prot_t ftype;
		extern struct vm_map *kernel_map;
		unsigned long nss;

		cr2 = rcr2();
		KERNEL_PROC_LOCK(p);
faultcommon:
		vm = p->p_vmspace;
		if (vm == NULL)
			goto we_re_toast;
		fa = cr2;
		va = trunc_page((vaddr_t)cr2);
		/*
		 * It is only a kernel address space fault iff:
		 *	1. (type & T_USER) == 0  and
		 *	2. pcb_onfault not set or
		 *	3. pcb_onfault set but supervisor space fault
		 * The last can occur during an exec() copyin where the
		 * argument space is lazy-allocated.
		 */
		if (type == T_PAGEFLT && va >= VM_MIN_KERNEL_ADDRESS)
			map = kernel_map;
		else
			map = &vm->vm_map;
		if (frame.tf_err & PGEX_W)
			ftype = VM_PROT_WRITE;
		else if (frame.tf_err & PGEX_I)
			ftype = VM_PROT_EXECUTE;
		else
			ftype = VM_PROT_READ;

#ifdef DIAGNOSTIC
		if (map == kernel_map && va == 0) {
			printf("trap: bad kernel access at %lx\n", va);
			goto we_re_toast;
		}
#endif

		nss = 0;
		if ((caddr_t)va >= vm->vm_maxsaddr
		    && (caddr_t)va < (caddr_t)VM_MAXUSER_ADDRESS
		    && map != kernel_map) {
			nss = btoc(USRSTACK-(unsigned long)va);
			if (nss > (u_long)btoc(p->p_rlimit[RLIMIT_STACK].rlim_cur)) {
				/*
				 * We used to fail here. However, it may
				 * just have been an mmap()ed page low
				 * in the stack, which is legal. If it
				 * wasn't, uvm_fault() will fail below.
				 *
				 * Set nss to 0, since this case is not
				 * a "stack extension".
				 */
				nss = 0;
			}
		}

		/* Fault the original page in. */
		onfault = pcb->pcb_onfault;
		pcb->pcb_onfault = NULL;
		error = uvm_fault(map, va, frame.tf_err & PGEX_P?
		    VM_FAULT_PROTECT : VM_FAULT_INVALID, ftype);
		pcb->pcb_onfault = onfault;
		if (error == 0) {
			if (nss > (u_long)vm->vm_ssize)
				vm->vm_ssize = nss;

			if (type == T_PAGEFLT) {
				KERNEL_UNLOCK();
				return;
			}
			KERNEL_PROC_UNLOCK(p);
			goto out;
		}
		if (error == EACCES) {
			error = EFAULT;
		}

		if (type == T_PAGEFLT) {
			if (pcb->pcb_onfault != 0) {
				KERNEL_UNLOCK();
				goto copyfault;
			}
			printf("uvm_fault(%p, 0x%lx, 0, %d) -> %x\n",
			    map, va, ftype, error);
			goto we_re_toast;
		}
		if (error == ENOMEM) {
			printf("UVM: pid %d (%s), uid %d killed: out of swap\n",
			       p->p_pid, p->p_comm,
			       p->p_cred && p->p_ucred ?
			       (int)p->p_ucred->cr_uid : -1);
			sv.sival_ptr = (void *)fa;
			trapsignal(p, SIGKILL, T_PAGEFLT, SEGV_MAPERR, sv);
		} else {
#ifdef TRAP_SIGDEBUG
			printf("pid %d (%s): SEGV at rip %lx addr %lx\n",
			    p->p_pid, p->p_comm, frame.tf_rip, va);
			frame_dump(&frame);
#endif
			sv.sival_ptr = (void *)fa;
			trapsignal(p, SIGSEGV, T_PAGEFLT, SEGV_MAPERR, sv);
		}
		if (type == T_PAGEFLT)
			KERNEL_UNLOCK();
		else
			KERNEL_PROC_UNLOCK(p);
		break;
	}

	case T_TRCTRAP:
		/* Check whether they single-stepped into a lcall. */
		if (frame.tf_rip == (int)IDTVEC(oosyscall))
			return;
		if (frame.tf_rip == (int)IDTVEC(oosyscall) + 1) {
			frame.tf_rflags &= ~PSL_T;
			return;
		}
		goto we_re_toast;

	case T_BPTFLT|T_USER:		/* bpt instruction fault */
	case T_TRCTRAP|T_USER:		/* trace trap */
#ifdef MATH_EMULATE
	trace:
#endif
		KERNEL_PROC_LOCK(p);
		trapsignal(p, SIGTRAP, type &~ T_USER, TRAP_BRKPT, sv);
		KERNEL_PROC_UNLOCK(p);
		break;

#if	NISA > 0
	case T_NMI:
#if defined(KGDB) || defined(DDB)
		/* NMI can be hooked up to a pushbutton for debugging */
		printf ("NMI ... going to debugger\n");
#ifdef KGDB

		if (kgdb_trap(type, &frame))
			return;
#endif
#ifdef DDB
		if (kdb_trap(type, 0, &frame))
			return;
#endif
#endif /* KGDB || DDB */
		/* machine/parity/power fail/"kitchen sink" faults */

		if (x86_nmi() != 0)
			goto we_re_toast;
		else
			return;
#endif /* NISA > 0 */
	}

	if ((type & T_USER) == 0)
		return;
out:
	userret(p);
}

#ifdef TRAP_SIGDEBUG
static void
frame_dump(struct trapframe *tf)
{
	printf("rip %p  rsp %p  rfl %p\n",
	    (void *)tf->tf_rip, (void *)tf->tf_rsp, (void *)tf->tf_rflags);
	printf("rdi %p  rsi %p  rdx %p\n",
	    (void *)tf->tf_rdi, (void *)tf->tf_rsi, (void *)tf->tf_rdx);
	printf("rcx %p  r8  %p  r9  %p\n",
	    (void *)tf->tf_rcx, (void *)tf->tf_r8, (void *)tf->tf_r9);
	printf("r10 %p  r11 %p  r12 %p\n",
	    (void *)tf->tf_r10, (void *)tf->tf_r11, (void *)tf->tf_r12);
	printf("r13 %p  r14 %p  r15 %p\n",
	    (void *)tf->tf_r13, (void *)tf->tf_r14, (void *)tf->tf_r15);
	printf("rbp %p  rbx %p  rax %p\n",
	    (void *)tf->tf_rbp, (void *)tf->tf_rbx, (void *)tf->tf_rax);
}
#endif
