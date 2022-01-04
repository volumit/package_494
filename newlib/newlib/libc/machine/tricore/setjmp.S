/*
 * setjmp.s -- setjmp() and longjmp() for the TriCore architecture.
 *
 * (C) Copyright 1998 HighTec EDV-Systeme GmbH
 * 12OCT98 (ms)
 *
 */

    .text

    .globl setjmp
    .type setjmp,@function
# %a4: pointer to jmp_buf variable
setjmp:
    dsync			# workaround HW bug of Rider-B/D
    nop
    nop
    st.a [%a4],%a11		# store return address (after setjmp())
    dsync			# make sure the caches are synced
    mfcr %d4,$pcxi		# get current pcxi value
    # csa = ((pcxi & 0xffff) << 6)
    #       | ((pcxi & 0xf0000) << 12)
    sha.h %d5, %d4, 12  # shift segment from 16-19 into bit 28-31
    insert %d5, %d5, %d4, 6,16   # move offset into bits 0-22
    mov.a %a5,%d5       # a5 = csa

    ld.d %e6,[%a5]0		# load/store saved pcxi and psw
    st.d [%a4]4,%e6
    ld.d %e6,[%a5]8		# load/store saved %a10 and %a11
    st.d [%a4]12,%e6
    ld.d %e6, [%a5]16    # load/store saved %d8, %d9
    st.d [%a4]20,%e6
    ld.d %e6, [%a5]24    # load/store saved %d10, %d11
    st.d [%a4]28,%e6
    ld.d %e6, [%a5]32    # load/store saved %a12, %a13
    st.d [%a4]36,%e6
    ld.d %e6, [%a5]40    # load/store saved %a14, %a15
    st.d [%a4]44,%e6
    ld.d %e6, [%a5]48    # load/store saved %d12, %d13
    st.d [%a4]52,%e6
    ld.d %e6, [%a5]56    # load/store saved %d14, %d15
    st.d [%a4]60,%e6

    mov %d2,0		# return 0
    ret


    .globl longjmp
    .type longjmp,@function
# %a4: pointer to jmp_buf variable
# %d4: return value for longjmp()
longjmp:
    dsync			# workaround HW bug of Rider-B/D
    nop
    nop
    ld.w %d6,[%a4]4		# load saved pcxi
    movh.a %a5,hi:loop	# load loop label as return address
    lea %a5,[%a5]lo:loop
loop:
    mfcr %d5,$pcxi		# get current pcxi
    jeq %d5,%d6,restore	# if same as saved one then restore registers
#if defined (__TC13__) || defined (__TC131__) || defined (__TC16__)
    jz.t %d5,22,lctx	# csa contains lower context?
#elif defined (__TC161__) || defined (__TC162__)
    jz.t %d5,20,lctx	# csa contains lower context?
#else
#error adjust setjmp / longjmp
#endif
    mov.aa %a11,%a5		# no, load loop label as return address
    nop
    ret			# return to loop
lctx:
    mov %d13,%d4		# save %d4, %d6, %a4 and %a5 in upper context
    mov %d15,%d6
    mov.aa %a12,%a4
    mov.aa %a13,%a5
    rslcx			# restore lower context
    mov %d4,%d13		# restore %d4, %d6, %a4 and %a5
    mov %d6,%d15
    mov.aa %a4,%a12
    mov.aa %a5,%a13
    j loop			# handle next csa
restore:
    ld.w %d6,[%a4]8		# load saved psw
    mtcr $psw,%d6		# restore psw
    dsync
    nop
    nop
#ifdef ERRATA_CPU81
    ld.a %a12,[%a4]12	# restore %a10 
    mov.aa %a10, %a12
    ld.a %a11,[%a4]16	# restore %a11
#else
    ld.da %a10,[%a4]12	# restore %a10 and %a11
#endif
    ld.d %e8,[%a4]20	# restore %d8 and %d9
    ld.d %e10,[%a4]28	# restore %d10 and %d11
    ld.da %a12,[%a4]36	# restore %a12 and %a13
    ld.da %a14,[%a4]44	# restore %a14 and %a15
    ld.d %e12,[%a4]52	# restore %d12 and %d13
    ld.d %e14,[%a4]60	# restore %d14 and %d15
    max.u %d2,%d4,1		# retval = val ? val : 1
    ld.a %a5,[%a4]		# return to the saved pc address
    nop
    ji %a5

