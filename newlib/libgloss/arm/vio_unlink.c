/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

#include <stdio.h>

/* Use "naked" attribute to suppress C prologue/epilogue */

int __attribute__ ((naked)) unlink(const char *name)
{
#if defined(__thumb__) && !defined(__thumb2__)
	(void)name;
	while (1)
		;
#else
	register const char *rname __asm__ ("r0") = name;
	register int rsys_op __asm__ ("r12") = 7; /*SYS_UNLINK*/

	__asm__ volatile (
		"b	___virtio"
		: "+r" (rname)
		: "r" (rsys_op));

	return (int)rname;
#endif /* __thumb__ && !__thumb2__ */
}
