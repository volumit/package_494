#  Copyright (C) 1998-2006 HighTec EDV-Systeme GmbH.
#
# This file just make a stack frame for the contents of the .fini and
# .init sections.  Users may put any desired instructions in those
# sections.

# On the tricore the prolog is empty just define the symbols _init and _fini

	.file		"crti.s"

	.section	".init"
	.align 2
	.global	__init
__init:
   add  -4, sp
   st.w lp, 0[sp]


	.section	".fini"
	.align	2
	.global	__fini
__fini:
   add  -4, sp
   st.w lp, 0[sp]


# end of crti.s
