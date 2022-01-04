/* Copyright (C) 2011-2014 Free Software Foundation, Inc.

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

////////////////////////////////////////////////////////////
.macro .DEFUN func
    .section ".text.libgcc", "ax",@progbits
    .align	1
    .global \func
    .type \func,@function
    \func:
.endm	

.macro .ENDF func
    .size	\func, .-\func
.endm	
////////////////////////////////////////////////////////////


#ifdef L_divmodsi4
// d2 = div (d4, d5)
// d3 = mod (d4, d5)
.DEFUN __divmodsi4
    dvinit	%e2, %d4, %d5
    dvstep	%e2, %e2, %d5
    dvstep	%e2, %e2, %d5
    dvstep	%e2, %e2, %d5
    dvstep	%e2, %e2, %d5
    dvadj	%e2, %e2, %d5
    ret
.ENDF __divmodsi4
#endif    

#ifdef L_udivmodsi4
// d2 = udiv (d4, d5)
// d3 = umod (d4, d5)
.DEFUN __udivmodsi4
    dvinit.u	%e2, %d4, %d5
    dvstep.u	%e2, %e2, %d5
    dvstep.u	%e2, %e2, %d5
    dvstep.u	%e2, %e2, %d5
    dvstep.u	%e2, %e2, %d5
    ret
.ENDF __udivmodsi4
#endif    

#ifdef L_rstcntxt
.DEFUN __tric_restore_context_queue
    
/* Implementation of the context queue restore for non local gotos and
   longjmps.
    
   We get the right context as a parameter in register %d4.
   We pop contexts from the queue until PCXI shows the desired context.

   We have beenn called with "call" so have to pop the saved
   context from the call.  */

    # First save the return address to %a3.
    mov.aa  %a3, %a11
    movh.a  %a11, HI:.Lnext_context
    lea     %a11, [%a11] LO:.Lnext_context
    ret

.Lnext_context:
    # Get PCXI and make sure caches are synced
    dsync

    #  get_context_queue	
    mfcr    %d1, $pcxi	

    # We have found the right context
    jeq     %d1, %d4, .Llast_context
	
    #  Extract CSA to A2
    sh      %d0, %d1, -16
    sh      %d0, %d0, 28
    insert  %d0, %d0, %d1, 6, 16
    mov.a   %a2, %d0
    dsync
    nop
    nop
#if defined (__TC13__) || defined (__TC131__) || defined (__TC16__)
    jz.t    %d1, 22, .Llower_context
#elif defined (__TC161__) || defined (__TC162__)
    jz.t    %d1, 20, .Llower_context
#else
#error adjust setjmp / longjmp
#endif

    st.d    [%a2] 16, %e8
    st.d    [%a2] 24, %e10
    st.da   [%a2] 32, %a12
    st.da   [%a2] 40, %a14
    st.d    [%a2] 48, %e12
    st.d    [%a2] 56, %e14
    dsync
    nop
    nop
    movh.a  %a11, HI:.Lnext_context
    lea     %a11, [%a11] LO:.Lnext_context
    ret

.Llower_context:
    st.a    [%a2] 12, %a3
    st.w    [%a2] 48, %d4
    dsync
    nop
    nop
    rslcx
    j       .Lnext_context

.Llast_context:
    # Return to the caller
    ji	%a3
.ENDF __tric_restore_context_queue
    
#endif /* L_rstcntxt */

#ifdef L_bswapsi2
    # Swap bytes in word
    # IN:  D4 = ABCD
    # OUT: D2 = DCBA, D4 = ?

.DEFUN __bswapsi2
    # D4 = ABCD
    # D2 = DABC
    dextr	%d2, %d4, %d4, 24
    # D4 = 000A
    sh		%d4, %d4, -24
    # D2 = DCBC
    insert	%d2, %d2, %d2, 16, 8
    # D2 = DCBA
    insert	%d2, %d2, %d4, 0, 8
    ret
    
.ENDF __bswapsi2

#endif /* L_bswapsi2 */

#ifdef L_fixsfsi
    # float D4
    # D2 = (int) D4
    # clobber: D0, D1
.DEFUN __fixsfsi
    extr.u %d1, %d4, 23, 8       # extract exponent from float -> 0..255
    lt     %d0, %d1, 127         # clear everything, if abs(value) is < 1
    rsub   %d1, %d1, 150         # 1.0: exp=127, 23 bits masked -> 23+127 = 150
    seln   %d1, %d0, %d1, 31     # clear everything but sign bit if |value| < 1
    max    %d1, %d1, 0           # limit size of mask to values >= 0
    mov    %d0, 0                # position is fix
    insert %d2, %d4, 0, %e0      # clear fract bits in float representation
    ftoi   %d2, %d2              # float to signed int: no rounding used
    ret
.ENDF __fixsfsi
#endif /* L_fixsfsi */
    
#ifdef L_fixunssfsi
    # float D4
    # D2 = (unsigned int) D4
    # clobber: D0, D1
.DEFUN __fixunssfsi
    extr.u %d1, %d4, 23, 8       # extract exponent from float -> 0..255
    lt     %d0, %d1, 127         # clear everything, if abs(value) is < 1
    rsub   %d1, %d1, 150         # 1.0: exp=127, 23 bits masked -> 23+127 = 150
    seln   %d1, %d0, %d1, 31     # clear everything but sign bit if |value| < 1
    max    %d1, %d1, 0           # limit size of mask to values >= 0
    mov    %d0, 0                # position is fix
    insert %d2, %d4, 0, %e0      # clear fract bits in float representation
    ftou   %d2, %d2              # float to unsigned int: no rounding used
    ret
.ENDF __fixunssfsi
#endif /* L_fixunssfsi */

#ifdef L_paritysi2
    # D2 = parity (D4)
    # clobber: D3
.DEFUN __paritysi2
    parity %d2, %d4
    bsplit %e2, %d2
    bsplit %e2, %d2
    parity %d2, %d2
    ret
.ENDF __paritysi2
#endif /* L_paritysi2 */


#ifdef L_ashldi3
.DEFUN __ashldi3
    jnz.t %d6,5, 1f
    sh    %d2, %d4, %d6
    dextr %d3, %d5, %d4, %d6
    ret
1:  add   %d6, %d6, -32
    mov   %d2, 0
    sh    %d3, %d4, %d6
    ret
.ENDF __ashldi3
#endif /* L_ashldi3 */

#ifdef L_ashrdi3
.DEFUN __ashrdi3
    jnz.t %d6,5, 1f
    rsub  %d7, %d6, 32
    rsub  %d6
    dextr %d2, %d5, %d4, %d7
    sha   %d3, %d5, %d6
    sel   %d2, %d6, %d2, %d4
    ret
1:  
    add   %d6, %d6, 32
    rsub  %d6
    sha   %d2, %d5, %d6
    sha   %d3, %d5, -31
    ret
.ENDF __ashrdi3
#endif /* L_ashrdi3 */

#ifdef L_lshrdi3
.DEFUN __lshrdi3
    jnz.t %d6,5, 1f
    rsub  %d7, %d6, 32
    rsub  %d6
    dextr %d2, %d5, %d4, %d7
    sh    %d3, %d5, %d6
    sel   %d2, %d6, %d2, %d4
    ret
1:  
    add   %d6, %d6, 32
    rsub  %d6
    sh    %d2, %d5, %d6
    mov   %d3, 0
    ret
.ENDF __lshrdi3
#endif /* L_lshrdi3 */

#ifdef Lmemcpy
# implement memcpy as an assembler subroutine
# C-Interface void *memcpy(void *dst, const void *src, size_t n)
# register usage:
# Input %a4 = dst
#     %a5 = src
#     %d4 = n
#   return  %a2 = dst
#   clobbers
#     %a3 = as loop counter

.DEFUN memcpy
    mov.aa  %a2, %a4
    jz    %d4, 1f

    mov.a %a3, %d4
    add.a   %a3, -1
0:
    ld.bu %d4, [%a5+]1
    st.b  [%a4+]1, %d4
    loop  %a3, 0b
1:
    ret
.ENDF memcpy
#endif /* Lmemcpy */

#ifdef Lmemset
# implement memset as an assembler subroutine
# C-Interface void *memset(void *dst, int c, size_t n)
# register usage:
# Input %a4 = dst
#     %d4 = c
#     %d5 = n
#   return  %a2 = dst
#   clobbers
#     %a3 = as loop counter

.DEFUN memset
    mov.aa  %a2, %a4
    jz    %d5, 1f

    mov.a %a3, %d5
    add.a   %a3, -1
0:
    st.b  [%a4+]1, %d4
    loop  %a3, 0b
1:
    ret
.ENDF memset
#endif /* Lmemset */

#ifdef Lmemcmp
# implement memcmp as an assembler subroutine
# C-Interface int memcmp(void *dst, void *src, size_t n)
# register usage:
# Input %a4 = dst
#     %a5 = src
#     %d4 = n
#   return  %d2 = equal
#   clobbers
#     %a3 = as loop counter
#     %d3 
#     %d15 
.DEFUN memcmp
    jz      %d4, 2f
    add     %d4, -1
    mov.a   %a3, %d4
0:
    ld.bu   %d2, [%a4+]1
    ld.bu   %d15, [%a5+]1
    jeq     %d2, %d15, 1f
    ge.u    %d2, %d2, %d15
    mov     %d3, 1
    sel     %d2, %d2, %d3, -1
    ret
1:
    loop    %a3, 0b
2:
    mov     %d2, 0
    ret
.ENDF memcmp
#endif /* Lmemcmp */
