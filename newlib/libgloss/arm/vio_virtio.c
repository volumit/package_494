/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

/* Use "naked" attribute to suppress C prologue/epilogue */

void __attribute__ ((naked)) __virtio_hnd(void)
{
#if defined(__thumb__) && !defined(__thumb2__)
	while (1)
		;
#elif defined(__thumb2__)
	__asm__ volatile (
		".ascii \"_vio\"\n\t"
		".global ___virtio\n\t"
		".thumb_func\n"
"___virtio:\n\t");

	/* use "BKPT #17" to support ARMv7-M */
	__asm__ volatile (
		"bkpt  #17\n\t"
		"str   lr,[sp,#-4]!\n\t"
		"bl    __errno\n\t"
		"str   r12,[r0]\n\t"
		"mov   r0,r1\n\t"
		"ldr   pc,[sp],#4"
	);
#else
	__asm__ volatile (
		".ascii \"_vio\"\n\t"
		".global ___virtio\n\t"
		".type ___virtio,function\n"
"___virtio:\n\t");

#ifdef __XSCALE__
	/* use "BKPT #0" to support XScale's internal debug handler */
	__asm__ volatile (".long 0xE1200070");
#else
	__asm__ volatile (".long 0xE7FFDEFE");
#endif /* __XSCALE__ */

	__asm__ volatile (
		"str   lr,[sp,#-4]!\n\t"
		"bl    __errno\n\t"
		"str   r12,[r0]\n\t"
		"mov   r0,r1\n\t"
		"ldr   pc,[sp],#4"
	);
#endif /* __thumb__ && !__thumb2__ */
}
