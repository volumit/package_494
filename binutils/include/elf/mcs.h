/* RB GTM MCS ELF support for BFD.
   Copyright 2011   Free Software Foundation, Inc.
   Contributed by Horst Lehser <Horst.Lehser@hightec-rt.com>

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _ELF_MCS_H
#define _ELF_MCS_H

#include "elf/reloc-macros.h"

/* Relocations.  */
START_RELOC_NUMBERS (elf_mcs_reloc_type)
     RELOC_NUMBER (R_MCS_NONE,                      0)
     RELOC_NUMBER (R_MCS_4,                         1)
     RELOC_NUMBER (R_MCS_5,                         2)
     RELOC_NUMBER (R_MCS_8,                         3)
     RELOC_NUMBER (R_MCS_9,                         4)
     RELOC_NUMBER (R_MCS_14,                        5)
     RELOC_NUMBER (R_MCS_16,                        6)
     RELOC_NUMBER (R_MCS_24,                        7)
     RELOC_NUMBER (R_MCS_32,                        8)
     RELOC_NUMBER (R_MCS_WXLB,                      9)
END_RELOC_NUMBERS (R_MCS_max)

/* Processor specific flags for the ELF header e_flags field.  */

#define EF_MCS_MACH                 0x00000001

/* Various CPU types.  */

#define E_MCS_MACH                  0x1
#define E_MCS_MACH_V2               0x2
#define E_MCS_MACH_V3               0x4
#define E_MCS_MACH_V31              0x8

#endif /* _ELF_MCS_H */
