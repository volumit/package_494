/*	Copyright (C) 2011 HighTec EDV-Systeme GmbH.
	
	This file is part of GCC.

	GCC is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3, or (at your option)
	any later version.

	GCC is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	Under Section 7 of GPL version 3, you are granted additional
	permissions described in the GCC Runtime Library Exception, version
	3.1, as published by the Free Software Foundation.

	You should have received a copy of the GNU General Public License and
	a copy of the GCC Runtime Library Exception along with this program;
	see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
	<http://www.gnu.org/licenses/>.  */

/*
 *	This is the default trap vector table, which consists of eight
 *	entries, each consisting of eight words (32 bytes).  The table
 *	must be put into the section ".traptab", must be aligned to a 
 *	256-byte boundary, and the entry symbol (trap class 0 -- reset)
 *	must be called "TriCore_trap_table", as it is referenced below 
 *	in order to program the BTV register.  Trap class #6 (system 
 *	call) is special in that it might get used to invoke services 		
 * 	of an underlying operating system.  It is therefore defined in
 *	a separate file (trap6.o) in libos.a, so that it can easily be 
 * 	redefined by a customized version of this function.
 * */
 

asm ("						\n\
	.section .traptab, \"ax\", @progbits	\n\
	.align 8				\n\
	.globl TriCore_trap_table		\n\
TriCore_trap_table:				\n\
");
#if defined(ERRATA_CPU13) || defined(ERRATA_DMI12)
# if defined(ERRATA_CPU13)
#  define DSYNC asm ("dsync");
# else
#  define DSYNC
# endif
# ifdef ERRATA_CPU9
#   undef DSYNC
#   define DSYNC asm ("\tdsync\n\tnop\n\tnop");
# endif
# define DEFINE_TRAP(i)			\
    asm (".globl __trap_" #i);		\
    asm ("__trap_" #i ":");		\
    DSYNC				\
    asm ("svlcx");			\
    asm ("movh.a %a15,hi:Tdisptab");	\
    asm ("lea %a15,[%a15]lo:Tdisptab");	\
    asm ("ld.a %a15,[%a15]4*" #i);	\
    asm ("j __workaround_for_cpu13_9");	\
    asm (".align 5")
#else
# define DSYNC
# define DEFINE_TRAP(i)			\
    asm (".globl __trap_" #i);		\
    asm ("__trap_" #i ":");		\
    asm ("svlcx");			\
    asm ("movh.a %a15,hi:Tdisptab+(4*" #i ")");	\
    asm ("ld.w %d4,[%a15]lo:Tdisptab+(4*" #i ")");	\
    asm ("mov.a %a15, %d4");	\
    asm ("mov %d4,%d15");		\
    asm ("calli %a15");			\
    asm ("rslcx");			\
    asm ("rfe");			\
    asm (".align 5")
#endif


DEFINE_TRAP(0);		/* trap class 0 (Reset) */
DEFINE_TRAP(1);		/* trap class 1 (Internal Protection Traps) */
DEFINE_TRAP(2);		/* trap class 2 (Instruction Errors) */
DEFINE_TRAP(3);		/* trap class 3 (Context Management) */
DEFINE_TRAP(4);		/* trap class 4 (System Bus and Peripheral Errors) */
DEFINE_TRAP(5);		/* trap class 5 (Assertion Traps) */
asm (".globl ___trap_6");
asm ("___trap_6:");
DSYNC
asm ("j __trap_6");	/* trap class 6 (System Call) */
asm (".align 5");
DEFINE_TRAP(7);		/* trap class 7 (Non-Maskable Interrupt) */

#if defined(ERRATA_CPU13) || defined(ERRATA_DMI12)
  asm ("__workaround_for_cpu13_9:");
  asm ("mov %d4,%d15");
  asm ("calli %a15");
  asm ("rslcx");
  asm ("rfe");
#endif
asm (".text");

