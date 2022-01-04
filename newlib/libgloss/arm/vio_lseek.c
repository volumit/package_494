/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

#include <unistd.h>

/* Use "naked" attribute to suppress C prologue/epilogue */

off_t __attribute__ ((naked)) lseek(int desc, off_t offset, int whence)
{
#if defined(__thumb__) && !defined(__thumb2__)
	(void)desc;
	(void)offset;
	(void)whence;
	while (1)
		;
#else
	register int rdesc __asm__ ("r0") = desc;
	register off_t roffset __asm__ ("r1") = offset;
	register int rwhence __asm__ ("r2") = whence;
	register int rsys_op __asm__ ("r12") = 3; /*SYS_LSEEK*/

	__asm__ volatile (
		"b	___virtio"
		: "+r" (rdesc)
		: "r" (roffset), "r" (rwhence), "r" (rsys_op));

	return (off_t)rdesc;
#endif /* __thumb__ && !__thumb2__ */
}
