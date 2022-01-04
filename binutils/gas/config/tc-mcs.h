/* tc-mcs.h -- Header file for tc-mcs.c
   Copyright 2011   Free Software Foundation, Inc.
   Contributed by Horst Lehser <Horst.Lehser@hightec-rt.com>

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with GAS; see the file COPYING.  If not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef TC_MCS_H
#define TC_MCS_H

#define TARGET_FORMAT "elf32-mcs"
#define TARGET_ARCH bfd_arch_mcs
#define TARGET_MACH bfd_mach_mcs

#define TARGET_BYTES_BIG_ENDIAN 0

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB 1

#define WORKING_DOT_WORD

/* Values passed to md_apply_fix3 don't include the symbol value.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

#define md_operand(X)
#define tc_gen_reloc gas_cgen_tc_gen_reloc

extern void mcs_cons_align(int);
#define md_cons_align(nbytes)   mcs_cons_align(nbytes)

/* Tell users that this version of GAS is supported by HighTec.  */
extern void htc_check_gas_version_flags PARAMS ((int *, char ***));
extern void htc_print_version_id PARAMS ((int));

#define HOST_SPECIAL_INIT(argc, argv) \
  htc_check_gas_version_flags (&argc, &argv)
#define HTC_GAS_VERSION VERSION

#define TARGET_USE_CFIPOP 1

#define DWARF2_LINE_MIN_INSN_LENGTH     4
#define DWARF2_DEFAULT_RETURN_COLUMN    30
#define DWARF2_CIE_DATA_ALIGNMENT       4

#endif /* TC_MCS_H */

