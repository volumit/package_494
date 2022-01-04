/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

#include <stdio.h>

/* Use "naked" attribute to suppress C prologue/epilogue */

int __attribute__ ((naked)) read(int desc, void *buf, size_t len)
{
#if defined(__thumb__) && !defined(__thumb2__)
	(void)desc;
	(void)buf;
	(void)len;
	while (1)
		;
#else
	register int rdesc __asm__ ("r0") = desc;
	register void *rbuf __asm__ ("r1") = buf;
	register size_t rlen __asm__ ("r2") = len;
	register int rsys_op __asm__ ("r12") = 4; /*SYS_READ*/

	__asm__ volatile (
		"b	___virtio"
		: "+r" (rdesc)
		: "r" (rbuf), "r" (rlen), "r" (rsys_op));

	return rdesc;
#endif /* __thumb__ && !__thumb2__ */
}
