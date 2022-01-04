/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

#include <stdio.h>

/* Use "naked" attribute to suppress C prologue/epilogue */

int __attribute__ ((naked)) close(int desc)
{
#if defined(__thumb__) && !defined(__thumb2__)
	(void)desc;
	while (1)
		;
#else
	register int rdesc __asm__ ("r0") = desc;
	register int rsys_op __asm__ ("r12") = 2; /*SYS_CLOSE*/

	__asm__ volatile (
		"b	___virtio"
		: "+r" (rdesc)
		: "r" (rsys_op));

	return rdesc;
#endif /* __thumb__ && !__thumb2__ */
}
