/*
 * int1.c -- The standard implementation for interrupt #1.  It does the
 * same as the other interrupt handlers in that it calls the function
 * stored in Cdisptab[] with the user-defined argument.
 *
 * Copyright (C) 1998 HighTec EDV-Systeme GmbH.
 *
 */

asm ("					\n\
	.text				\n\
	.globl __interrupt_1		\n\
__interrupt_1:				\n\
	dsync				\n\
	bisr 1				\n\
	movh.a %a15,hi:Cdisptab		\n\
	lea %a15,[%a15]lo:Cdisptab	\n\
	ld.a %a14,[%a15]1*8		\n\
	ld.w %d4,[%a15]1*8+4		\n\
	calli %a14			\n\
	rslcx				\n\
	rfe				\n\
");
