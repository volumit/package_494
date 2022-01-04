/* Instruction opcode table for mcs.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2009 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#include "sysdep.h"
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "mcs-desc.h"
#include "mcs-opc.h"
#include "libiberty.h"

/* The hash functions are recorded here to help keep assembler code out of
   the disassembler and vice versa.  */

static int asm_hash_insn_p        (const CGEN_INSN *);
static unsigned int asm_hash_insn (const char *);
static int dis_hash_insn_p        (const CGEN_INSN *);
static unsigned int dis_hash_insn (const char *, CGEN_INSN_INT);

/* Instruction formats.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define F(f) & mcs_cgen_ifld_table[MCS_##f]
#else
#define F(f) & mcs_cgen_ifld_table[MCS_/**/f]
#endif
static const CGEN_IFMT ifmt_empty ATTRIBUTE_UNUSED = {
  0, 0, 0x0, { { 0 } }
};

static const CGEN_IFMT ifmt_movl ATTRIBUTE_UNUSED = {
  32, 32, 0xf0000000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_C24) }, { 0 } }
};

static const CGEN_IFMT ifmt_mov ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fffff, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15_16) }, { 0 } }
};

static const CGEN_IFMT ifmt_mov_v3 ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fafff, { { F (F_OPC0) }, { F (F_OPC1) }, { F (F_15) }, { F (F_OPXA) }, { F (F_13) }, { F (F_OPXB) }, { F (F_11_12) }, { 0 } }
};

static const CGEN_IFMT ifmt_mrd ATTRIBUTE_UNUSED = {
  32, 32, 0xf0ffc000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB) }, { F (F_OPC1) }, { F (F_15_2) }, { F (F_C14) }, { 0 } }
};

static const CGEN_IFMT ifmt_mrd_V2 ATTRIBUTE_UNUSED = {
  32, 32, 0xf0fff000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB) }, { F (F_OPC1) }, { F (F_15_4) }, { F (F_C12) }, { 0 } }
};

static const CGEN_IFMT ifmt_mrd_V3 ATTRIBUTE_UNUSED = {
  32, 32, 0xf0ff8000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB) }, { F (F_OPC1) }, { F (F_15) }, { F (F_C15) }, { 0 } }
};

static const CGEN_IFMT ifmt_mwr ATTRIBUTE_UNUSED = {
  32, 32, 0xf0ffc000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB) }, { F (F_OPC1) }, { F (F_15_2) }, { F (F_C14) }, { 0 } }
};

static const CGEN_IFMT ifmt_mrdi_C ATTRIBUTE_UNUSED = {
  32, 32, 0xf00ff000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15_4) }, { F (F_SIMM12) }, { 0 } }
};

static const CGEN_IFMT ifmt_mrdi_V3_C ATTRIBUTE_UNUSED = {
  32, 32, 0xf00f8000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15) }, { F (F_SIMM15) }, { 0 } }
};

static const CGEN_IFMT ifmt_mwri ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fffff, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_C16) }, { 0 } }
};

static const CGEN_IFMT ifmt_pop ATTRIBUTE_UNUSED = {
  32, 32, 0xf0ffffff, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB) }, { F (F_OPC1) }, { F (F_C16) }, { 0 } }
};

static const CGEN_IFMT ifmt_ard ATTRIBUTE_UNUSED = {
  32, 32, 0xf00ffe00, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15_7) }, { F (F_C9) }, { 0 } }
};

static const CGEN_IFMT ifmt_ardi ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fffff, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_C16) }, { 0 } }
};

static const CGEN_IFMT ifmt_awr ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fffe0, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15_11) }, { F (F_C5) }, { 0 } }
};

static const CGEN_IFMT ifmt_brd ATTRIBUTE_UNUSED = {
  32, 32, 0xf0ff0000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB) }, { F (F_OPC1) }, { F (F_C16) }, { 0 } }
};

static const CGEN_IFMT ifmt_brdi ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fffff, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15_16) }, { 0 } }
};

static const CGEN_IFMT ifmt_shr ATTRIBUTE_UNUSED = {
  32, 32, 0xf0ffffe0, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB) }, { F (F_OPC1) }, { F (F_15_11) }, { F (F_C5) }, { 0 } }
};

static const CGEN_IFMT ifmt_shr_V3 ATTRIBUTE_UNUSED = {
  32, 32, 0xf0ffbfe0, { { F (F_OPC0) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15) }, { F (F_OPXA) }, { F (F_13_9) }, { F (F_C5) }, { 0 } }
};

static const CGEN_IFMT ifmt_mulu_C ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fafe0, { { F (F_OPC0) }, { F (F_OPC1) }, { F (F_15) }, { F (F_OPXA) }, { F (F_13) }, { F (F_OPXB) }, { F (F_11_7) }, { F (F_C5) }, { 0 } }
};

static const CGEN_IFMT ifmt_maxs ATTRIBUTE_UNUSED = {
  32, 34, 0xf00fffff, { { F (F_OPC0) }, { F (F_OPC1) }, { F (F_15_16) }, { F (F_OPXA) }, { F (F_OPXB) }, { 0 } }
};

static const CGEN_IFMT ifmt_jmp ATTRIBUTE_UNUSED = {
  32, 32, 0xffffc000, { { F (F_OPC0) }, { F (F_27_8) }, { F (F_OPC1) }, { F (F_15_2) }, { F (F_C14) }, { 0 } }
};

static const CGEN_IFMT ifmt_jmp_V2 ATTRIBUTE_UNUSED = {
  32, 32, 0xfffff000, { { F (F_OPC0) }, { F (F_27_8) }, { F (F_OPC1) }, { F (F_15_4) }, { F (F_C12) }, { 0 } }
};

static const CGEN_IFMT ifmt_jmp_V3 ATTRIBUTE_UNUSED = {
  32, 32, 0xffff8000, { { F (F_OPC0) }, { F (F_27_8) }, { F (F_OPC1) }, { F (F_15) }, { F (F_C15) }, { 0 } }
};

static const CGEN_IFMT ifmt_jbs ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fc000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15_2) }, { F (F_C14) }, { 0 } }
};

static const CGEN_IFMT ifmt_jbs_V2 ATTRIBUTE_UNUSED = {
  32, 32, 0xf00ff000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15_4) }, { F (F_C12) }, { 0 } }
};

static const CGEN_IFMT ifmt_jbs_V3 ATTRIBUTE_UNUSED = {
  32, 32, 0xf00f8000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_15) }, { F (F_C15) }, { 0 } }
};

static const CGEN_IFMT ifmt_ret ATTRIBUTE_UNUSED = {
  32, 32, 0xffffffff, { { F (F_OPC0) }, { F (F_27_8) }, { F (F_OPC1) }, { F (F_C16) }, { 0 } }
};

static const CGEN_IFMT ifmt_jmpi ATTRIBUTE_UNUSED = {
  32, 32, 0xffffffff, { { F (F_OPC0) }, { F (F_27_8) }, { F (F_OPC1) }, { F (F_15_16) }, { 0 } }
};

static const CGEN_IFMT ifmt_jbsi ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fafff, { { F (F_OPC0) }, { F (F_OPC1) }, { F (F_15) }, { F (F_OPXA) }, { F (F_13) }, { F (F_OPXB) }, { F (F_11_12) }, { 0 } }
};

static const CGEN_IFMT ifmt_wurm ATTRIBUTE_UNUSED = {
  32, 32, 0xf00f0000, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPB_4) }, { F (F_OPC1) }, { F (F_C16) }, { 0 } }
};

static const CGEN_IFMT ifmt_wurmx ATTRIBUTE_UNUSED = {
  32, 32, 0xf00fefff, { { F (F_OPC0) }, { F (F_OPA_4) }, { F (F_OPC1) }, { F (F_15_3) }, { F (F_OPXB) }, { F (F_11_12) }, { 0 } }
};

static const CGEN_IFMT ifmt_nop ATTRIBUTE_UNUSED = {
  32, 32, 0xffffffff, { { F (F_OPC0) }, { F (F_OPA) }, { F (F_OPB) }, { F (F_OPC1) }, { F (F_C16) }, { 0 } }
};

#undef F

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_INSN_##a)
#else
#define A(a) (1 << CGEN_INSN_/**/a)
#endif
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define OPERAND(op) MCS_OPERAND_##op
#else
#define OPERAND(op) MCS_OPERAND_/**/op
#endif
#define MNEM CGEN_SYNTAX_MNEMONIC /* syntax value for mnemonic */
#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))

/* The instruction table.  */

static const CGEN_OPCODE mcs_cgen_insn_opcode_table[MAX_INSNS] =
{
  /* Special null first entry.
     A `num' value of zero is thus invalid.
     Also, the special `invalid' insn resides here.  */
  { { 0, 0, 0, 0 }, {{0}}, 0, {0}},
/* movl $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x10000000 }
  },
/* mov $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xa0000000 }
  },
/* mov $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xa0000000 }
  },
/* setb $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xb0060000 }
  },
/* clrb $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xb0070000 }
  },
/* xchb $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xa00f0000 }
  },
/* mrd $roa,$uimm14 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM14), 0 } },
    & ifmt_mrd, { 0xa0010000 }
  },
/* mrd $roa,$disp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (DISP12), 0 } },
    & ifmt_mrd_V2, { 0xa0010000 }
  },
/* mrd $roa,$disp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (DISP15), 0 } },
    & ifmt_mrd_V3, { 0xa0010000 }
  },
/* mwr $roa,$disp14 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (DISP14), 0 } },
    & ifmt_mwr, { 0xa0020000 }
  },
/* mwr $roa,$disp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (DISP12), 0 } },
    & ifmt_mrd_V2, { 0xa0020000 }
  },
/* mwr $roa,$disp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (DISP15), 0 } },
    & ifmt_mrd_V3, { 0xa0020000 }
  },
/* mwrl $roa,$uimm14 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM14), 0 } },
    & ifmt_mrd, { 0xa0070000 }
  },
/* mwrl $roa,$disp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (DISP12), 0 } },
    & ifmt_mrd_V2, { 0xa0070000 }
  },
/* mwrl $roa,$disp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (DISP15), 0 } },
    & ifmt_mrd_V3, { 0xa0070000 }
  },
/* mwr24 $roa,$uimm14 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM14), 0 } },
    & ifmt_mrd, { 0xa0070000 }
  },
/* mrdi $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xa0030000 }
  },
/* mrdi $roa,($rob) */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', 0 } },
    & ifmt_mov, { 0xa0030000 }
  },
/* mrdi $roa,$rob,$sdisp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), ',', OP (SDISP12), 0 } },
    & ifmt_mrdi_C, { 0xa0030000 }
  },
/* mrdi $roa,($rob)$sdisp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', OP (SDISP12), 0 } },
    & ifmt_mrdi_C, { 0xa0030000 }
  },
/* mrdi $roa,$rob,$sdisp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), ',', OP (SDISP15), 0 } },
    & ifmt_mrdi_V3_C, { 0xa0030000 }
  },
/* mrdi $roa,($rob)$sdisp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', OP (SDISP15), 0 } },
    & ifmt_mrdi_V3_C, { 0xa0030000 }
  },
/* mwri $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xa0040000 }
  },
/* mwri $roa,($rob) */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', 0 } },
    & ifmt_mwri, { 0xa0040000 }
  },
/* mwri $roa,$rob,$sdisp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), ',', OP (SDISP12), 0 } },
    & ifmt_mrdi_C, { 0xa0040000 }
  },
/* mwri $roa,($rob)$sdisp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', OP (SDISP12), 0 } },
    & ifmt_mrdi_C, { 0xa0040000 }
  },
/* mwri $roa,$rob,$sdisp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), ',', OP (SDISP15), 0 } },
    & ifmt_mrdi_V3_C, { 0xa0040000 }
  },
/* mwri $roa,($rob)$sdisp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', OP (SDISP15), 0 } },
    & ifmt_mrdi_V3_C, { 0xa0040000 }
  },
/* mwril $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xa0080000 }
  },
/* mwril $roa,($rob) */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', 0 } },
    & ifmt_mwri, { 0xa0080000 }
  },
/* mwril $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xa0080000 }
  },
/* mwril $roa,($rob) */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', 0 } },
    & ifmt_mwri, { 0xa0080000 }
  },
/* mwri24 $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xa0080000 }
  },
/* mwri24 $roa,($rob) */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', '(', OP (ROB), ')', 0 } },
    & ifmt_mwri, { 0xa0080000 }
  },
/* mrdio $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xa00d0000 }
  },
/* mwrio $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xa00e0000 }
  },
/* pop $roa */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), 0 } },
    & ifmt_pop, { 0xa0050000 }
  },
/* push $roa */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), 0 } },
    & ifmt_pop, { 0xa0060000 }
  },
/* ard $raa,$rab,$uimm9 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RAA), ',', OP (RAB), ',', OP (UIMM9), 0 } },
    & ifmt_ard, { 0xb0000000 }
  },
/* ardi $raa,$rab */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RAA), ',', OP (RAB), 0 } },
    & ifmt_ardi, { 0xb0040000 }
  },
/* awr $roa,$rob,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), ',', OP (UIMM5), 0 } },
    & ifmt_awr, { 0xb0010000 }
  },
/* awri $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xb0050000 }
  },
/* nard $raa,$rab,$uimm9 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RAA), ',', OP (RAB), ',', OP (UIMM9), 0 } },
    & ifmt_ard, { 0xb0020000 }
  },
/* nardi $raa,$rab */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RAA), ',', OP (RAB), 0 } },
    & ifmt_ardi, { 0xb0030000 }
  },
/* brd $rga,$disp16 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RGA), ',', OP (DISP16), 0 } },
    & ifmt_brd, { 0xa0090000 }
  },
/* bwr $rga,$disp16 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RGA), ',', OP (DISP16), 0 } },
    & ifmt_brd, { 0xa00a0000 }
  },
/* brdi $rga,$rgb */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RGA), ',', OP (RGB), 0 } },
    & ifmt_brdi, { 0xa00b0000 }
  },
/* brdi $rga,($rgb) */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RGA), ',', '(', OP (RGB), ')', 0 } },
    & ifmt_brdi, { 0xa00b0000 }
  },
/* bwri $rga,$rgb */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RGA), ',', OP (RGB), 0 } },
    & ifmt_brdi, { 0xa00c0000 }
  },
/* bwri $rga,($rgb) */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RGA), ',', '(', OP (RGB), ')', 0 } },
    & ifmt_brdi, { 0xa00c0000 }
  },
/* addl $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x20000000 }
  },
/* add $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xc0000000 }
  },
/* add $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc0000000 }
  },
/* addc $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xd0060000 }
  },
/* subl $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x30000000 }
  },
/* sub $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xc0010000 }
  },
/* sub $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc0010000 }
  },
/* subc $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xd0070000 }
  },
/* neg $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xc0020000 }
  },
/* neg $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc0020000 }
  },
/* andl $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x40000000 }
  },
/* and $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xc0030000 }
  },
/* and $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc0030000 }
  },
/* orl $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x50000000 }
  },
/* or $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mwri, { 0xc0040000 }
  },
/* or $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc0040000 }
  },
/* xorl $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x60000000 }
  },
/* xor $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xc0050000 }
  },
/* xor $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc0050000 }
  },
/* shr $roa,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM5), 0 } },
    & ifmt_shr, { 0xc0060000 }
  },
/* shr $rxoa,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (UIMM5), 0 } },
    & ifmt_shr_V3, { 0xc0060000 }
  },
/* shl $roa,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM5), 0 } },
    & ifmt_shr, { 0xc0070000 }
  },
/* shl $rxoa,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (UIMM5), 0 } },
    & ifmt_shr_V3, { 0xc0070000 }
  },
/* asru $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xd0040000 }
  },
/* asru $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xd0040000 }
  },
/* asrs $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xd0050000 }
  },
/* asrs $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xd0050000 }
  },
/* asl $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xd0030000 }
  },
/* asl $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xd0030000 }
  },
/* mulu $rxoa,$rxob,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), ',', OP (UIMM5), 0 } },
    & ifmt_mulu_C, { 0xc0080000 }
  },
/* mulu $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc0080018 }
  },
/* muls $rxoa,$rxob,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), ',', OP (UIMM5), 0 } },
    & ifmt_mulu_C, { 0xc0090000 }
  },
/* muls $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc0090018 }
  },
/* divu $rxoa,$rxob,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), ',', OP (UIMM5), 0 } },
    & ifmt_mulu_C, { 0xc00a0000 }
  },
/* divu $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc00a0018 }
  },
/* divs $rxoa,$rxob,$uimm5 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), ',', OP (UIMM5), 0 } },
    & ifmt_mulu_C, { 0xc00b0000 }
  },
/* divs $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc00b0018 }
  },
/* maxs $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_maxs, { 0xc00f0000 }
  },
/* maxs $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc00f0000 }
  },
/* maxu $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_maxs, { 0xc00e0000 }
  },
/* maxu $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc00e0000 }
  },
/* mins $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_maxs, { 0xc00d0000 }
  },
/* mins $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc00d0000 }
  },
/* minu $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_maxs, { 0xc00c0000 }
  },
/* minu $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xc00c0000 }
  },
/* atul $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x70000000 }
  },
/* atu $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xd0000000 }
  },
/* atu $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xd0000000 }
  },
/* atsl $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x80000000 }
  },
/* ats $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xd0010000 }
  },
/* ats $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xd0010000 }
  },
/* btl $roa,$uimm24 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (UIMM24), 0 } },
    & ifmt_movl, { 0x90000000 }
  },
/* bt $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xd0020000 }
  },
/* bt $rxoa,$rxob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (RXOB), 0 } },
    & ifmt_mov_v3, { 0xd0020000 }
  },
/* jmp $disp14 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (DISP14), 0 } },
    & ifmt_jmp, { 0xe0000000 }
  },
/* jmp $disp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (DISP12), 0 } },
    & ifmt_jmp_V2, { 0xe0000000 }
  },
/* jmp $disp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (DISP15), 0 } },
    & ifmt_jmp_V3, { 0xe0000000 }
  },
/* jbs $roa,$lb,$disp14 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (LB), ',', OP (DISP14), 0 } },
    & ifmt_jbs, { 0xe0010000 }
  },
/* jbs $roa,$lb,$disp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (LB), ',', OP (DISP12), 0 } },
    & ifmt_jbs_V2, { 0xe0010000 }
  },
/* jbs $roa,$lb,$disp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (LB), ',', OP (DISP15), 0 } },
    & ifmt_jbs_V3, { 0xe0010000 }
  },
/* jbc $roa,$lb,$disp14 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (LB), ',', OP (DISP14), 0 } },
    & ifmt_jbs, { 0xe0020000 }
  },
/* jbc $roa,$lb,$disp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (LB), ',', OP (DISP12), 0 } },
    & ifmt_jbs_V2, { 0xe0020000 }
  },
/* jbc $roa,$lb,$disp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (LB), ',', OP (DISP15), 0 } },
    & ifmt_jbs_V3, { 0xe0020000 }
  },
/* call $disp14 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (DISP14), 0 } },
    & ifmt_jmp, { 0xe0030000 }
  },
/* call $disp12 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (DISP12), 0 } },
    & ifmt_jmp_V2, { 0xe0030000 }
  },
/* call $disp15 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (DISP15), 0 } },
    & ifmt_jmp_V3, { 0xe0030000 }
  },
/* ret */
  {
    { 0, 0, 0, 0 },
    { { MNEM, 0 } },
    & ifmt_ret, { 0xe0040000 }
  },
/* jmpi */
  {
    { 0, 0, 0, 0 },
    { { MNEM, 0 } },
    & ifmt_jmpi, { 0xe0050000 }
  },
/* jbsi $rxoa,$wxlb */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (WXLB), 0 } },
    & ifmt_jbsi, { 0xe0060000 }
  },
/* jbci $rxoa,$wxlb */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (RXOA), ',', OP (WXLB), 0 } },
    & ifmt_jbsi, { 0xe0070000 }
  },
/* calli */
  {
    { 0, 0, 0, 0 },
    { { MNEM, 0 } },
    & ifmt_jmpi, { 0xe0080000 }
  },
/* wurm $roa,$rob,$uimm16 */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), ',', OP (UIMM16), 0 } },
    & ifmt_wurm, { 0xf0000000 }
  },
/* wurmx $roa,$rwxb */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (RWXB), 0 } },
    & ifmt_wurmx, { 0xf0010000 }
  },
/* wurcx $roa,$rwxb */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (RWXB), 0 } },
    & ifmt_wurmx, { 0xf0020000 }
  },
/* wuce $roa,$rob */
  {
    { 0, 0, 0, 0 },
    { { MNEM, ' ', OP (ROA), ',', OP (ROB), 0 } },
    & ifmt_mov, { 0xf0030000 }
  },
/* nop */
  {
    { 0, 0, 0, 0 },
    { { MNEM, 0 } },
    & ifmt_nop, { 0x0 }
  },
};

#undef A
#undef OPERAND
#undef MNEM
#undef OP

/* Formats for ALIAS macro-insns.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define F(f) & mcs_cgen_ifld_table[MCS_##f]
#else
#define F(f) & mcs_cgen_ifld_table[MCS_/**/f]
#endif
#undef F

/* Each non-simple macro entry points to an array of expansion possibilities.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_INSN_##a)
#else
#define A(a) (1 << CGEN_INSN_/**/a)
#endif
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define OPERAND(op) MCS_OPERAND_##op
#else
#define OPERAND(op) MCS_OPERAND_/**/op
#endif
#define MNEM CGEN_SYNTAX_MNEMONIC /* syntax value for mnemonic */
#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))

/* The macro instruction table.  */

static const CGEN_IBASE mcs_cgen_macro_insn_table[] =
{
};

/* The macro instruction opcode table.  */

static const CGEN_OPCODE mcs_cgen_macro_insn_opcode_table[] =
{
};

#undef A
#undef OPERAND
#undef MNEM
#undef OP

#ifndef CGEN_ASM_HASH_P
#define CGEN_ASM_HASH_P(insn) 1
#endif

#ifndef CGEN_DIS_HASH_P
#define CGEN_DIS_HASH_P(insn) 1
#endif

/* Return non-zero if INSN is to be added to the hash table.
   Targets are free to override CGEN_{ASM,DIS}_HASH_P in the .opc file.  */

static int
asm_hash_insn_p (insn)
     const CGEN_INSN *insn ATTRIBUTE_UNUSED;
{
  return CGEN_ASM_HASH_P (insn);
}

static int
dis_hash_insn_p (insn)
     const CGEN_INSN *insn;
{
  /* If building the hash table and the NO-DIS attribute is present,
     ignore.  */
  if (CGEN_INSN_ATTR_VALUE (insn, CGEN_INSN_NO_DIS))
    return 0;
  return CGEN_DIS_HASH_P (insn);
}

#ifndef CGEN_ASM_HASH
#define CGEN_ASM_HASH_SIZE 127
#ifdef CGEN_MNEMONIC_OPERANDS
#define CGEN_ASM_HASH(mnem) (*(unsigned char *) (mnem) % CGEN_ASM_HASH_SIZE)
#else
#define CGEN_ASM_HASH(mnem) (*(unsigned char *) (mnem) % CGEN_ASM_HASH_SIZE) /*FIXME*/
#endif
#endif

/* It doesn't make much sense to provide a default here,
   but while this is under development we do.
   BUFFER is a pointer to the bytes of the insn, target order.
   VALUE is the first base_insn_bitsize bits as an int in host order.  */

#ifndef CGEN_DIS_HASH
#define CGEN_DIS_HASH_SIZE 256
#define CGEN_DIS_HASH(buf, value) (*(unsigned char *) (buf))
#endif

/* The result is the hash value of the insn.
   Targets are free to override CGEN_{ASM,DIS}_HASH in the .opc file.  */

static unsigned int
asm_hash_insn (mnem)
     const char * mnem;
{
  return CGEN_ASM_HASH (mnem);
}

/* BUF is a pointer to the bytes of the insn, target order.
   VALUE is the first base_insn_bitsize bits as an int in host order.  */

static unsigned int
dis_hash_insn (buf, value)
     const char * buf ATTRIBUTE_UNUSED;
     CGEN_INSN_INT value ATTRIBUTE_UNUSED;
{
  return CGEN_DIS_HASH (buf, value);
}

/* Set the recorded length of the insn in the CGEN_FIELDS struct.  */

static void
set_fields_bitsize (CGEN_FIELDS *fields, int size)
{
  CGEN_FIELDS_BITSIZE (fields) = size;
}

/* Function to call before using the operand instance table.
   This plugs the opcode entries and macro instructions into the cpu table.  */

void
mcs_cgen_init_opcode_table (CGEN_CPU_DESC cd)
{
  int i;
  int num_macros = (sizeof (mcs_cgen_macro_insn_table) /
		    sizeof (mcs_cgen_macro_insn_table[0]));
  const CGEN_IBASE *ib = & mcs_cgen_macro_insn_table[0];
  const CGEN_OPCODE *oc = & mcs_cgen_macro_insn_opcode_table[0];
  CGEN_INSN *insns = xmalloc (num_macros * sizeof (CGEN_INSN));

  /* This test has been added to avoid a warning generated
     if memset is called with a third argument of value zero.  */
  if (num_macros >= 1)
    memset (insns, 0, num_macros * sizeof (CGEN_INSN));
  for (i = 0; i < num_macros; ++i)
    {
      insns[i].base = &ib[i];
      insns[i].opcode = &oc[i];
      mcs_cgen_build_insn_regex (& insns[i]);
    }
  cd->macro_insn_table.init_entries = insns;
  cd->macro_insn_table.entry_size = sizeof (CGEN_IBASE);
  cd->macro_insn_table.num_init_entries = num_macros;

  oc = & mcs_cgen_insn_opcode_table[0];
  insns = (CGEN_INSN *) cd->insn_table.init_entries;
  for (i = 0; i < MAX_INSNS; ++i)
    {
      insns[i].opcode = &oc[i];
      mcs_cgen_build_insn_regex (& insns[i]);
    }

  cd->sizeof_fields = sizeof (CGEN_FIELDS);
  cd->set_fields_bitsize = set_fields_bitsize;

  cd->asm_hash_p = asm_hash_insn_p;
  cd->asm_hash = asm_hash_insn;
  cd->asm_hash_size = CGEN_ASM_HASH_SIZE;

  cd->dis_hash_p = dis_hash_insn_p;
  cd->dis_hash = dis_hash_insn;
  cd->dis_hash_size = CGEN_DIS_HASH_SIZE;
}
