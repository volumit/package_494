/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

#include <stdio.h>

/* Use "naked" attribute to suppress C prologue/epilogue */

int __attribute__ ((naked)) creat(char *path, int mode)
{
#if defined(__thumb__) && !defined(__thumb2__)
	(void)path;
	(void)mode;
	while (1)
		;
#else
	register char *rpath __asm__ ("r0") = path;
	register int rmode __asm__ ("r1") = mode;
	register int rsys_op __asm__ ("r12") = 6; /*SYS_CREAT*/

	__asm__ volatile (
		"b	___virtio"
		: "+r" (rpath)
		: "r" (rmode), "r" (rsys_op));

	return (int)rpath;
#endif /* __thumb__ && !__thumb2__ */
}
