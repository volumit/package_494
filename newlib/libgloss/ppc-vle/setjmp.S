/* This is a simple version of setjmp and longjmp for the PowerPC.
   Ian Lance Taylor, Cygnus Support, 9 Feb 1994.  
   Modified by Jeff Johnston, Red Hat Inc. 2 Oct 2001.
   Modified by Horst Lehser, HighTec EDV Systeme 14 Sep 2011 for use with ZEN architecture */

#include "ppc-asm.h"

FUNC_START(setjmp)

	stw	1,0(3)		# offset 0
	stwu	2,4(3)		# offset 4
	stwu	13,4(3)		# offset 8
	stwu	14,4(3)		# offset 12
	stwu	15,4(3)		# offset 16
	stwu	16,4(3)		# offset 20
	stwu	17,4(3)		# offset 24
	stwu	18,4(3)		# offset 28
	stwu	19,4(3)		# offset 32
	stwu	20,4(3)		# offset 36
	stwu	21,4(3)		# offset 40
	stwu	22,4(3)		# offset 44
	stwu	23,4(3)		# offset 48
	stwu	24,4(3)		# offset 52
	stwu	25,4(3)		# offset 56
	stwu	26,4(3)		# offset 60
	stwu	27,4(3)		# offset 64
	stwu	28,4(3)		# offset 68
	stwu	29,4(3)		# offset 72
	stwu	30,4(3)		# offset 76
	stwu	31,4(3)		# offset 80

	mflr	4
	stwu	4,4(3)		# offset 84
	mfcr	4
	stwu	4,4(3)		# offset 88

	li	3,0
	blr
FUNC_END(setjmp)


FUNC_START(longjmp)
	lwz	1,0(3)		# offset 0 
	lwzu	2,4(3)		# offset 4 
	lwzu	13,4(3)		# offset 8 
	lwzu	14,4(3)		# offset 12
	lwzu	15,4(3)		# offset 16
	lwzu	16,4(3)		# offset 20
	lwzu	17,4(3)		# offset 24
	lwzu	18,4(3)		# offset 28
	lwzu	19,4(3)		# offset 32
	lwzu	20,4(3)		# offset 36
	lwzu	21,4(3)		# offset 40
	lwzu	22,4(3)		# offset 44
	lwzu	23,4(3)		# offset 48
	lwzu	24,4(3)		# offset 52
	lwzu	25,4(3)		# offset 56
	lwzu	26,4(3)		# offset 60
	lwzu	27,4(3)		# offset 64
	lwzu	28,4(3)		# offset 68
	lwzu	29,4(3)		# offset 72
	lwzu	30,4(3)		# offset 76
	lwzu	31,4(3)		# offset 80

	lwzu	5,4(3)		# offset 84
	mtlr	5
	lwzu	5,4(3)		# offset 88
	mtcrf	255,5
				# one word pad to get floating point aligned on 8 byte boundary


	mr.	3,4
#ifdef __PPC_VLE__
    bc      4,2,0f
	li	3,1
    0:
#else
	bclr+	4,2
	li	3,1
#endif
	blr
FUNC_END(longjmp)
