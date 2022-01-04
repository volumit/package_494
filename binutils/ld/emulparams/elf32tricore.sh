#
# Emulation parameters for embedded TriCore applications.
# Copyright (C) 1998-2003 Free Software Foundation, Inc.
# Contributed by Michael Schumacher (mike@hightec-rt.com).
#
SCRIPT_NAME=elftricore
OUTPUT_FORMAT=elf32-tricore
TEXT_START_ADDR=0xa0000000
DATA_ADDR=0xd0000000
DATA_ALIGNMENT="((. + 7) & -8)"
OTHER_SECTIONS=".version_info    0 : { *(.version_info) }
  .boffs           0 : { KEEP (*(.boffs)) }"
MAXPAGESIZE=0x0
NONPAGED_TEXT_START_ADDR=0xa0000000
ARCH=tricore
TEMPLATE_NAME=elf32
EXTRA_EM_FILE=tricoreelf
EMBEDDED=yes
