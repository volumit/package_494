/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

/* Use "naked" attribute to suppress C prologue/epilogue */

int __attribute__ ((naked)) open(char *path, int flags, int mode)
{
#if defined(__thumb__) && !defined(__thumb2__)
	(void)path;
	(void)flags;
	(void)mode;
	while (1)
		;
#else
	__asm__ volatile (
		"mov r12,#1\n\t"
		"b   ___virtio"
	);
	register char *rpath __asm__ ("r0") = path;
	register int rflags __asm__ ("r1") = flags;
	register int rmode __asm__ ("r2") = mode;
	register int rsys_op __asm__ ("r12") = 1; /*SYS_OPEN*/

	__asm__ volatile (
		"b	___virtio"
		: "+r" (rpath)
		: "r" (rflags), "r" (rmode), "r" (rsys_op));

	return (int)rpath;
#endif /* __thumb__ && !__thumb2__ */
}
