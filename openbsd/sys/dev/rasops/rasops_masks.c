/*	$OpenBSD: rasops_masks.c,v 1.2 2002/05/28 22:10:03 fgsch Exp $	*/
/*	$NetBSD: rasops_masks.c,v 1.5 2000/06/13 13:37:00 ad Exp $	*/

/*-
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Doran.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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

#include "rasops_masks.h"

/* `ragged edge' bitmasks */
const int32_t rasops_lmask[32+1] = {
	MBE(0x00000000), MBE(0x7fffffff), MBE(0x3fffffff), MBE(0x1fffffff),
	MBE(0x0fffffff), MBE(0x07ffffff), MBE(0x03ffffff), MBE(0x01ffffff),
	MBE(0x00ffffff), MBE(0x007fffff), MBE(0x003fffff), MBE(0x001fffff),
	MBE(0x000fffff), MBE(0x0007ffff), MBE(0x0003ffff), MBE(0x0001ffff),
	MBE(0x0000ffff), MBE(0x00007fff), MBE(0x00003fff), MBE(0x00001fff),
	MBE(0x00000fff), MBE(0x000007ff), MBE(0x000003ff), MBE(0x000001ff),
	MBE(0x000000ff), MBE(0x0000007f), MBE(0x0000003f), MBE(0x0000001f),
	MBE(0x0000000f), MBE(0x00000007), MBE(0x00000003), MBE(0x00000001),
	MBE(0x00000000)
};

const int32_t rasops_rmask[32+1] = {
	MBE(0x00000000), MBE(0x80000000), MBE(0xc0000000), MBE(0xe0000000),
	MBE(0xf0000000), MBE(0xf8000000), MBE(0xfc000000), MBE(0xfe000000),
	MBE(0xff000000), MBE(0xff800000), MBE(0xffc00000), MBE(0xffe00000),
	MBE(0xfff00000), MBE(0xfff80000), MBE(0xfffc0000), MBE(0xfffe0000),
	MBE(0xffff0000), MBE(0xffff8000), MBE(0xffffc000), MBE(0xffffe000),
	MBE(0xfffff000), MBE(0xfffff800), MBE(0xfffffc00), MBE(0xfffffe00),
	MBE(0xffffff00), MBE(0xffffff80), MBE(0xffffffc0), MBE(0xffffffe0),
	MBE(0xfffffff0), MBE(0xfffffff8), MBE(0xfffffffc), MBE(0xfffffffe),
	MBE(0xffffffff)
};

/* Part bitmasks */
const int32_t rasops_pmask[32][32] = {
      { MBE(0xffffffff), MBE(0x80000000), MBE(0xc0000000), MBE(0xe0000000),
	MBE(0xf0000000), MBE(0xf8000000), MBE(0xfc000000), MBE(0xfe000000),
	MBE(0xff000000), MBE(0xff800000), MBE(0xffc00000), MBE(0xffe00000),
	MBE(0xfff00000), MBE(0xfff80000), MBE(0xfffc0000), MBE(0xfffe0000),
	MBE(0xffff0000), MBE(0xffff8000), MBE(0xffffc000), MBE(0xffffe000),
	MBE(0xfffff000), MBE(0xfffff800), MBE(0xfffffc00), MBE(0xfffffe00),
	MBE(0xffffff00), MBE(0xffffff80), MBE(0xffffffc0), MBE(0xffffffe0),
	MBE(0xfffffff0), MBE(0xfffffff8), MBE(0xfffffffc), MBE(0xfffffffe), },

      { MBE(0x00000000), MBE(0x40000000), MBE(0x60000000), MBE(0x70000000),
	MBE(0x78000000), MBE(0x7c000000), MBE(0x7e000000), MBE(0x7f000000),
	MBE(0x7f800000), MBE(0x7fc00000), MBE(0x7fe00000), MBE(0x7ff00000),
	MBE(0x7ff80000), MBE(0x7ffc0000), MBE(0x7ffe0000), MBE(0x7fff0000),
	MBE(0x7fff8000), MBE(0x7fffc000), MBE(0x7fffe000), MBE(0x7ffff000),
	MBE(0x7ffff800), MBE(0x7ffffc00), MBE(0x7ffffe00), MBE(0x7fffff00),
	MBE(0x7fffff80), MBE(0x7fffffc0), MBE(0x7fffffe0), MBE(0x7ffffff0),
	MBE(0x7ffffff8), MBE(0x7ffffffc), MBE(0x7ffffffe), MBE(0x7fffffff), },

      { MBE(0x00000000), MBE(0x20000000), MBE(0x30000000), MBE(0x38000000),
	MBE(0x3c000000), MBE(0x3e000000), MBE(0x3f000000), MBE(0x3f800000),
	MBE(0x3fc00000), MBE(0x3fe00000), MBE(0x3ff00000), MBE(0x3ff80000),
	MBE(0x3ffc0000), MBE(0x3ffe0000), MBE(0x3fff0000), MBE(0x3fff8000),
	MBE(0x3fffc000), MBE(0x3fffe000), MBE(0x3ffff000), MBE(0x3ffff800),
	MBE(0x3ffffc00), MBE(0x3ffffe00), MBE(0x3fffff00), MBE(0x3fffff80),
	MBE(0x3fffffc0), MBE(0x3fffffe0), MBE(0x3ffffff0), MBE(0x3ffffff8),
	MBE(0x3ffffffc), MBE(0x3ffffffe), MBE(0x3fffffff), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x10000000), MBE(0x18000000), MBE(0x1c000000),
	MBE(0x1e000000), MBE(0x1f000000), MBE(0x1f800000), MBE(0x1fc00000),
	MBE(0x1fe00000), MBE(0x1ff00000), MBE(0x1ff80000), MBE(0x1ffc0000),
	MBE(0x1ffe0000), MBE(0x1fff0000), MBE(0x1fff8000), MBE(0x1fffc000),
	MBE(0x1fffe000), MBE(0x1ffff000), MBE(0x1ffff800), MBE(0x1ffffc00),
	MBE(0x1ffffe00), MBE(0x1fffff00), MBE(0x1fffff80), MBE(0x1fffffc0),
	MBE(0x1fffffe0), MBE(0x1ffffff0), MBE(0x1ffffff8), MBE(0x1ffffffc),
	MBE(0x1ffffffe), MBE(0x1fffffff), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x08000000), MBE(0x0c000000), MBE(0x0e000000),
	MBE(0x0f000000), MBE(0x0f800000), MBE(0x0fc00000), MBE(0x0fe00000),
	MBE(0x0ff00000), MBE(0x0ff80000), MBE(0x0ffc0000), MBE(0x0ffe0000),
	MBE(0x0fff0000), MBE(0x0fff8000), MBE(0x0fffc000), MBE(0x0fffe000),
	MBE(0x0ffff000), MBE(0x0ffff800), MBE(0x0ffffc00), MBE(0x0ffffe00),
	MBE(0x0fffff00), MBE(0x0fffff80), MBE(0x0fffffc0), MBE(0x0fffffe0),
	MBE(0x0ffffff0), MBE(0x0ffffff8), MBE(0x0ffffffc), MBE(0x0ffffffe),
	MBE(0x0fffffff), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x04000000), MBE(0x06000000), MBE(0x07000000),
	MBE(0x07800000), MBE(0x07c00000), MBE(0x07e00000), MBE(0x07f00000),
	MBE(0x07f80000), MBE(0x07fc0000), MBE(0x07fe0000), MBE(0x07ff0000),
	MBE(0x07ff8000), MBE(0x07ffc000), MBE(0x07ffe000), MBE(0x07fff000),
	MBE(0x07fff800), MBE(0x07fffc00), MBE(0x07fffe00), MBE(0x07ffff00),
	MBE(0x07ffff80), MBE(0x07ffffc0), MBE(0x07ffffe0), MBE(0x07fffff0),
	MBE(0x07fffff8), MBE(0x07fffffc), MBE(0x07fffffe), MBE(0x07ffffff),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x02000000), MBE(0x03000000), MBE(0x03800000),
	MBE(0x03c00000), MBE(0x03e00000), MBE(0x03f00000), MBE(0x03f80000),
	MBE(0x03fc0000), MBE(0x03fe0000), MBE(0x03ff0000), MBE(0x03ff8000),
	MBE(0x03ffc000), MBE(0x03ffe000), MBE(0x03fff000), MBE(0x03fff800),
	MBE(0x03fffc00), MBE(0x03fffe00), MBE(0x03ffff00), MBE(0x03ffff80),
	MBE(0x03ffffc0), MBE(0x03ffffe0), MBE(0x03fffff0), MBE(0x03fffff8),
	MBE(0x03fffffc), MBE(0x03fffffe), MBE(0x03ffffff), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x01000000), MBE(0x01800000), MBE(0x01c00000),
	MBE(0x01e00000), MBE(0x01f00000), MBE(0x01f80000), MBE(0x01fc0000),
	MBE(0x01fe0000), MBE(0x01ff0000), MBE(0x01ff8000), MBE(0x01ffc000),
	MBE(0x01ffe000), MBE(0x01fff000), MBE(0x01fff800), MBE(0x01fffc00),
	MBE(0x01fffe00), MBE(0x01ffff00), MBE(0x01ffff80), MBE(0x01ffffc0),
	MBE(0x01ffffe0), MBE(0x01fffff0), MBE(0x01fffff8), MBE(0x01fffffc),
	MBE(0x01fffffe), MBE(0x01ffffff), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00800000), MBE(0x00c00000), MBE(0x00e00000),
	MBE(0x00f00000), MBE(0x00f80000), MBE(0x00fc0000), MBE(0x00fe0000),
	MBE(0x00ff0000), MBE(0x00ff8000), MBE(0x00ffc000), MBE(0x00ffe000),
	MBE(0x00fff000), MBE(0x00fff800), MBE(0x00fffc00), MBE(0x00fffe00),
	MBE(0x00ffff00), MBE(0x00ffff80), MBE(0x00ffffc0), MBE(0x00ffffe0),
	MBE(0x00fffff0), MBE(0x00fffff8), MBE(0x00fffffc), MBE(0x00fffffe),
	MBE(0x00ffffff), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00400000), MBE(0x00600000), MBE(0x00700000),
	MBE(0x00780000), MBE(0x007c0000), MBE(0x007e0000), MBE(0x007f0000),
	MBE(0x007f8000), MBE(0x007fc000), MBE(0x007fe000), MBE(0x007ff000),
	MBE(0x007ff800), MBE(0x007ffc00), MBE(0x007ffe00), MBE(0x007fff00),
	MBE(0x007fff80), MBE(0x007fffc0), MBE(0x007fffe0), MBE(0x007ffff0),
	MBE(0x007ffff8), MBE(0x007ffffc), MBE(0x007ffffe), MBE(0x007fffff),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00200000), MBE(0x00300000), MBE(0x00380000),
	MBE(0x003c0000), MBE(0x003e0000), MBE(0x003f0000), MBE(0x003f8000),
	MBE(0x003fc000), MBE(0x003fe000), MBE(0x003ff000), MBE(0x003ff800),
	MBE(0x003ffc00), MBE(0x003ffe00), MBE(0x003fff00), MBE(0x003fff80),
	MBE(0x003fffc0), MBE(0x003fffe0), MBE(0x003ffff0), MBE(0x003ffff8),
	MBE(0x003ffffc), MBE(0x003ffffe), MBE(0x003fffff), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00100000), MBE(0x00180000), MBE(0x001c0000),
	MBE(0x001e0000), MBE(0x001f0000), MBE(0x001f8000), MBE(0x001fc000),
	MBE(0x001fe000), MBE(0x001ff000), MBE(0x001ff800), MBE(0x001ffc00),
	MBE(0x001ffe00), MBE(0x001fff00), MBE(0x001fff80), MBE(0x001fffc0),
	MBE(0x001fffe0), MBE(0x001ffff0), MBE(0x001ffff8), MBE(0x001ffffc),
	MBE(0x001ffffe), MBE(0x001fffff), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00080000), MBE(0x000c0000), MBE(0x000e0000),
	MBE(0x000f0000), MBE(0x000f8000), MBE(0x000fc000), MBE(0x000fe000),
	MBE(0x000ff000), MBE(0x000ff800), MBE(0x000ffc00), MBE(0x000ffe00),
	MBE(0x000fff00), MBE(0x000fff80), MBE(0x000fffc0), MBE(0x000fffe0),
	MBE(0x000ffff0), MBE(0x000ffff8), MBE(0x000ffffc), MBE(0x000ffffe),
	MBE(0x000fffff), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00040000), MBE(0x00060000), MBE(0x00070000),
	MBE(0x00078000), MBE(0x0007c000), MBE(0x0007e000), MBE(0x0007f000),
	MBE(0x0007f800), MBE(0x0007fc00), MBE(0x0007fe00), MBE(0x0007ff00),
	MBE(0x0007ff80), MBE(0x0007ffc0), MBE(0x0007ffe0), MBE(0x0007fff0),
	MBE(0x0007fff8), MBE(0x0007fffc), MBE(0x0007fffe), MBE(0x0007ffff),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00020000), MBE(0x00030000), MBE(0x00038000),
	MBE(0x0003c000), MBE(0x0003e000), MBE(0x0003f000), MBE(0x0003f800),
	MBE(0x0003fc00), MBE(0x0003fe00), MBE(0x0003ff00), MBE(0x0003ff80),
	MBE(0x0003ffc0), MBE(0x0003ffe0), MBE(0x0003fff0), MBE(0x0003fff8),
	MBE(0x0003fffc), MBE(0x0003fffe), MBE(0x0003ffff), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00010000), MBE(0x00018000), MBE(0x0001c000),
	MBE(0x0001e000), MBE(0x0001f000), MBE(0x0001f800), MBE(0x0001fc00),
	MBE(0x0001fe00), MBE(0x0001ff00), MBE(0x0001ff80), MBE(0x0001ffc0),
	MBE(0x0001ffe0), MBE(0x0001fff0), MBE(0x0001fff8), MBE(0x0001fffc),
	MBE(0x0001fffe), MBE(0x0001ffff), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00008000), MBE(0x0000c000), MBE(0x0000e000),
	MBE(0x0000f000), MBE(0x0000f800), MBE(0x0000fc00), MBE(0x0000fe00),
	MBE(0x0000ff00), MBE(0x0000ff80), MBE(0x0000ffc0), MBE(0x0000ffe0),
	MBE(0x0000fff0), MBE(0x0000fff8), MBE(0x0000fffc), MBE(0x0000fffe),
	MBE(0x0000ffff), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00004000), MBE(0x00006000), MBE(0x00007000),
	MBE(0x00007800), MBE(0x00007c00), MBE(0x00007e00), MBE(0x00007f00),
	MBE(0x00007f80), MBE(0x00007fc0), MBE(0x00007fe0), MBE(0x00007ff0),
	MBE(0x00007ff8), MBE(0x00007ffc), MBE(0x00007ffe), MBE(0x00007fff),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00002000), MBE(0x00003000), MBE(0x00003800),
	MBE(0x00003c00), MBE(0x00003e00), MBE(0x00003f00), MBE(0x00003f80),
	MBE(0x00003fc0), MBE(0x00003fe0), MBE(0x00003ff0), MBE(0x00003ff8),
	MBE(0x00003ffc), MBE(0x00003ffe), MBE(0x00003fff), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00001000), MBE(0x00001800), MBE(0x00001c00),
	MBE(0x00001e00), MBE(0x00001f00), MBE(0x00001f80), MBE(0x00001fc0),
	MBE(0x00001fe0), MBE(0x00001ff0), MBE(0x00001ff8), MBE(0x00001ffc),
	MBE(0x00001ffe), MBE(0x00001fff), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000800), MBE(0x00000c00), MBE(0x00000e00),
	MBE(0x00000f00), MBE(0x00000f80), MBE(0x00000fc0), MBE(0x00000fe0),
	MBE(0x00000ff0), MBE(0x00000ff8), MBE(0x00000ffc), MBE(0x00000ffe),
	MBE(0x00000fff), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000400), MBE(0x00000600), MBE(0x00000700),
	MBE(0x00000780), MBE(0x000007c0), MBE(0x000007e0), MBE(0x000007f0),
	MBE(0x000007f8), MBE(0x000007fc), MBE(0x000007fe), MBE(0x000007ff),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000200), MBE(0x00000300), MBE(0x00000380),
	MBE(0x000003c0), MBE(0x000003e0), MBE(0x000003f0), MBE(0x000003f8),
	MBE(0x000003fc), MBE(0x000003fe), MBE(0x000003ff), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000100), MBE(0x00000180), MBE(0x000001c0),
	MBE(0x000001e0), MBE(0x000001f0), MBE(0x000001f8), MBE(0x000001fc),
	MBE(0x000001fe), MBE(0x000001ff), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000080), MBE(0x000000c0), MBE(0x000000e0),
	MBE(0x000000f0), MBE(0x000000f8), MBE(0x000000fc), MBE(0x000000fe),
	MBE(0x000000ff), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },
     { MBE(0x00000000), MBE(0x00000040), MBE(0x00000060), MBE(0x00000070),
	MBE(0x00000078), MBE(0x0000007c), MBE(0x0000007e), MBE(0x0000007f),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000020), MBE(0x00000030), MBE(0x00000038),
	MBE(0x0000003c), MBE(0x0000003e), MBE(0x0000003f), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000010), MBE(0x00000018), MBE(0x0000001c),
	MBE(0x0000001e), MBE(0x0000001f), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000008), MBE(0x0000000c), MBE(0x0000000e),
	MBE(0x0000000f), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000004), MBE(0x00000006), MBE(0x00000007),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000002), MBE(0x00000003), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },

      { MBE(0x00000000), MBE(0x00000001), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000),
	MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), MBE(0x00000000), },
};
