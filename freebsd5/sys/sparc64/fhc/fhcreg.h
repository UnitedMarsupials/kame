/*-
 * Copyright (c) 2003 Jake Burkholder.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/sparc64/fhc/fhcreg.h,v 1.2 2003/02/19 08:23:38 jake Exp $
 */

#ifndef _SPARC64_FHC_FHCREG_H_
#define	_SPARC64_FHC_FHCREG_H_

#define	FHC_NREG	(6)

#define	FHC_INTERNAL	(0)
#define	FHC_IGN		(1)
#define	FHC_FANFAIL	(2)
#define	FHC_SYSTEM	(3)
#define	FHC_UART	(4)
#define	FHC_TOD		(5)

#define	FHC_IMAP	0x0
#define	FHC_ICLR	0x10

#define	FHC_CTRL	(0x20)
#define	FHC_CTRL_SLINE	(0x00010000)
#define	FHC_CTRL_AOFF	(0x00001000)
#define	FHC_CTRL_BOFF	(0x00000800)
#define	FHC_CTRL_IXIST	(0x00000200)
#define	FHC_BSR		(0x30)

#endif
