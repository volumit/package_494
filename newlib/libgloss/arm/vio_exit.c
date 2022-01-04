/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

#include <stddef.h>
#include <errno.h>

/* Use "naked" attribute to suppress C prologue/epilogue */

void __attribute__ ((naked)) _exit(int status)
{
#ifdef __XSCALE__
	/* use "BKPT #0" to support XScale's internal debug handler */
	 __asm__ volatile (".long 0xE1200070");
#elif defined(__thumb2__)
	/* use "BKPT #17" to support ARMv7-M */
	__asm__ volatile (".long 0xBE11BE11");
#else
	__asm__ volatile (".long 0xE7FFDEFE");
#endif /* __XSCALE__ */
	while (1)
		;
}

void __attribute__ ((naked)) abort(void)
{
#ifdef __XSCALE__
	/* use "BKPT #0" to support XScale's internal debug handler */
	__asm__ volatile (".long 0xE1200070");
#elif defined(__thumb2__)
	/* use "BKPT #17" to support ARMv7-M */
	__asm__ volatile (".long 0xBE11BE11");
#else
	__asm__ volatile (".long 0xE7FFDEFE");
#endif /* __XSCALE__ */
	while (1)
		;
}
