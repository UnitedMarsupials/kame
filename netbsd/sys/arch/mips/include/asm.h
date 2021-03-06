/*	$NetBSD: asm.h,v 1.21 1999/04/01 09:02:53 soda Exp $	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell.
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
 *	@(#)machAsmDefs.h	8.1 (Berkeley) 6/10/93
 */

/*
 * machAsmDefs.h --
 *
 *	Macros used when writing assembler programs.
 *
 *	Copyright (C) 1989 Digital Equipment Corporation.
 *	Permission to use, copy, modify, and distribute this software and
 *	its documentation for any purpose and without fee is hereby granted,
 *	provided that the above copyright notice appears in all copies.
 *	Digital Equipment Corporation makes no representations about the
 *	suitability of this software for any purpose.  It is provided "as is"
 *	without express or implied warranty.
 *
 * from: Header: /sprite/src/kernel/mach/ds3100.md/RCS/machAsmDefs.h,
 *	v 1.2 89/08/15 18:28:24 rab Exp  SPRITE (DECWRL)
 */

#ifndef _MIPS_ASM_H
#define _MIPS_ASM_H

#include <mips/regdef.h>

/*
 * Define -pg profile entry code.
 * XXX assume .set noreorder for kernel, .set reorder for user code.
 */
#define _KERN_MCOUNT		\
	.set	noat;		\
	move	$1,$31;		\
	jal	_mcount;	\
	subu	sp,sp,8;	\
	.set at

#ifdef GPROF
# if defined(_KERNEL) || defined(_LOCORE)
#  define MCOUNT _KERN_MCOUNT
# else
#  define MCOUNT .set noreorder; _KERN_MCOUNT ;  .set reorder;
# endif
#else
#define	MCOUNT
#endif

#ifdef __NO_LEADING_UNDERSCORES__
# define _C_LABEL(x)	x
#else
# ifdef __STDC__
#  define _C_LABEL(x)	_ ## x
# else
#  define _C_LABEL(x)	_/**/x
# endif
#endif

#ifdef USE_AENT
#define AENT(x)				\
	.aent	x, 0
#else
#define AENT(x)
#endif

/*
 * WARN_REFERENCES: create a warning if the specified symbol is referenced
 * (ELF only, and thus, no leading underscores).
 */
#ifdef __ELF__
#ifdef __STDC__
#define	WARN_REFERENCES(_sym,_msg)				\
	.section .gnu.warning. ## _sym ; .ascii _msg ; .text
#else
#define	WARN_REFERENCES(_sym,_msg)				\
	.section .gnu.warning./**/_sym ; .ascii _msg ; .text
#endif /* __STDC__ */
#endif /* __ELF__ */

/*
 * LEAF
 *	A leaf routine does
 *	- call no other function,
 *	- never use any register that callee-saved (S0-S8), and
 *	- not use any local stack storage.
 */
#define LEAF(x)				\
	.globl	_C_LABEL(x);		\
	.ent	_C_LABEL(x), 0;		\
_C_LABEL(x): ;				\
	.frame sp, 0, ra;		\
	MCOUNT

/*
 * LEAF_NOPROFILE
 *	No profilable leaf routine.
 */
#define LEAF_NOPROFILE(x)		\
	.globl	_C_LABEL(x);		\
	.ent	_C_LABEL(x), 0;		\
_C_LABEL(x): ;				\
	.frame	sp, 0, ra

/*
 * XLEAF
 *	declare alternate entry to leaf routine
 */
#define XLEAF(x)			\
	.globl	_C_LABEL(x);		\
	AENT (_C_LABEL(x));		\
_C_LABEL(x):

/*
 * NESTED
 *	A function calls other functions and needs
 *	therefore stack space to save/restore registers.
 */
#define NESTED(x, fsize, retpc)		\
	.globl	_C_LABEL(x);		\
	.ent	_C_LABEL(x), 0; 	\
_C_LABEL(x): ;				\
	.frame	sp, fsize, retpc;	\
	MCOUNT

/*
 * NESTED_NOPROFILE(x)
 *	No profilable nested routine.
 */
#define NESTED_NOPROFILE(x, fsize, retpc)	\
	.globl	_C_LABEL(x);		\
	.ent	_C_LABEL(x), 0;		\
_C_LABEL(x): ;				\
	.frame	sp, fsize, retpc

/*
 * XNESTED
 *	declare alternate entry point to nested routine.
 */
#define XNESTED(x)			\
	.globl	_C_LABEL(x);		\
	AENT (_C_LABEL(x));		\
_C_LABEL(x):

/*
 * END
 *	Mark end of a procedure.
 */
#define END(x) \
	.end _C_LABEL(x)

/*
 * IMPORT -- import external symbol
 */
#define IMPORT(sym, size)		\
	.extern _C_LABEL(sym),size

/*
 * EXPORT -- export definition of symbol
 */
#define EXPORT(x)			\
	.globl	_C_LABEL(x);		\
_C_LABEL(x):

/*
 * VECTOR
 *	exception vector entrypoint
 *	XXX: regmask should be used to generate .mask
 */
#define VECTOR(x, regmask)		\
	.ent	_C_LABEL(x),0;		\
	EXPORT(x);			\

#ifdef __STDC__
#define VECTOR_END(x)			\
	EXPORT(x ## End);		\
	END(x)
#else
#define VECTOR_END(x)			\
	EXPORT(x/**/End);		\
	END(x)
#endif

/*
 * Macros to panic and printf from assembly language.
 */
#define PANIC(msg)			\
	la	a0, 9f;			\
	jal	_C_LABEL(panic);	\
	MSG(msg)

#define	PRINTF(msg)			\
	la	a0, 9f;			\
	jal	_C_LABEL(printf);	\
	MSG(msg)

#define	MSG(msg)			\
	.rdata;				\
9:	.asciiz	msg;			\
	.text

#define ASMSTR(str)			\
	.asciiz str;			\
	.align	3

/*
 * XXX retain dialects XXX
 */
#define ALEAF(x)			XLEAF(x)
#define NLEAF(x)			LEAF_NOPROFILE(x)
#define NON_LEAF(x, fsize, retpc)	NESTED(x, fsize, retpc)
#define NNON_LEAF(x, fsize, retpc)	NESTED_NOPROFILE(x, fsize, retpc)

/* 
 *  standard callframe {
 *  	register_t cf_args[4];		arg0 - arg3
 *  	register_t cf_sp;		frame pointer
 *  	register_t cf_ra;		return address
 *  };
 */
#define	CALLFRAME_SIZ	(4 * (4 + 2))
#define	CALLFRAME_SP	(4 * 4)
#define	CALLFRAME_RA	(4 * 5)

/*
 * While it would be nice to be compatible with the SGI 
 * REG_L and REG_S macros, because they do not take parameters, it
 * is impossible to use them with the _MIPS_SIM_ABIX32 model.
 *
 * These macros hide the use of mips3 instructions from the 
 * assembler to prevent the assembler from generating 64-bit style
 * ABI calls.
 */

#if !defined(_MIPS_BSD_API) || _MIPS_BSD_API == _MIPS_BSD_API_LP32
#define	REG_L	lw
#define REG_S	sw
#define	REG_LI	li
#define	REG_PROLOGUE	.set push 
#define	REG_EPILOGUE	.set pop
#define SZREG	4
#else
#define	REG_L	ld
#define REG_S	sd
#define	REG_LI	dli
#define	REG_PROLOGUE	.set push ; .set mips3
#define	REG_EPILOGUE	.set pop
#define SZREG	8
#endif	/* _MIPS_BSD_API */

#endif /* _MIPS_ASM_H */
