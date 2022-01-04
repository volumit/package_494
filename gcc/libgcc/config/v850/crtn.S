#  Copyright (C) 1998-2006 HighTec EDV-Systeme GmbH.
#
# This file just makes sure that the .fini and .init sections do in
# fact return.  Users may put any desired instructions in those sections.
# This file is the last thing linked into any executable.

# On the v850 we have just to return

	.file		"crtn.S"

	.section	".init"
	;;
	jr  __return_r31

	.section	".fini"
	;;
	jr  __return_r31

# end of crtn.asm
