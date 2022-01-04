/*
 * trap6.c -- The standard implementation for trap #6.  It does the
 * same as the other trap handlers in that it calls the registered
 * trap handler with the TIN as argument.  In addition, it re-enables
 * interrupts if they weren't disabled before the syscall was issued.
 *
 * Copyright (C) 1998 HighTec EDV-Systeme GmbH.
 *
 */

asm ("						\n\
	.text					\n\
	.globl __trap_6				\n\
__trap_6:					\n\
	dsync					\n\
	mfcr 	%d8,$pcxi				\n\
	jz.t 	%d8,23,0f				\n\
	enable					\n\
0:	svlcx					\n\
	movh.a 	%a15,hi:Tdisptab+4*6			\n\
	ld.w 	%d4,[%a15]lo:Tdisptab+4*6		\n\
	mov.a	%a15, %d4			\n\
	mov 	%d4,%d15				\n\
	calli 	%a15				\n\
	rslcx					\n\
	rfe					\n\
");
