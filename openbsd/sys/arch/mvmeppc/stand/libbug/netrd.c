/*	$OpenBSD: netrd.c,v 1.2 2001/07/04 08:31:36 niklas Exp $	*/

/*
 * bug routines -- assumes that the necessary sections of memory
 * are preserved.
 */
#include <sys/types.h>
#include <machine/prom.h>

/* returns 0: success, nonzero: error */
int
mvmeprom_netrd(arg)
	struct mvmeprom_netio *arg;
{
	asm volatile ("mr 3, %0": : "r" (arg));
	MVMEPROM_CALL(MVMEPROM_NETRD);
	return (arg->status);
}
