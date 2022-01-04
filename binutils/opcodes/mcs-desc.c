/* CPU data for mcs.

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
#include <stdio.h>
#include <stdarg.h>
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "mcs-desc.h"
#include "mcs-opc.h"
#include "opintl.h"
#include "libiberty.h"
#include "xregex.h"

/* Attributes.  */

static const CGEN_ATTR_ENTRY bool_attr[] =
{
  { "#f", 0 },
  { "#t", 1 },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY MACH_attr[] ATTRIBUTE_UNUSED =
{
  { "base", MACH_BASE },
  { "gtmmcsV1", MACH_GTMMCSV1 },
  { "gtmmcsV2", MACH_GTMMCSV2 },
  { "gtmmcsV3", MACH_GTMMCSV3 },
  { "gtmmcsV31", MACH_GTMMCSV31 },
  { "max", MACH_MAX },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY ISA_attr[] ATTRIBUTE_UNUSED =
{
  { "gtmmcs", ISA_GTMMCS },
  { "max", ISA_MAX },
  { 0, 0 }
};

const CGEN_ATTR_TABLE mcs_cgen_ifield_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "RESERVED", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE mcs_cgen_hardware_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "CACHE-ADDR", &bool_attr[0], &bool_attr[0] },
  { "PC", &bool_attr[0], &bool_attr[0] },
  { "PROFILE", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE mcs_cgen_operand_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { "NEGATIVE", &bool_attr[0], &bool_attr[0] },
  { "RELAX", &bool_attr[0], &bool_attr[0] },
  { "SEM-ONLY", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE mcs_cgen_insn_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "ALIAS", &bool_attr[0], &bool_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "UNCOND-CTI", &bool_attr[0], &bool_attr[0] },
  { "COND-CTI", &bool_attr[0], &bool_attr[0] },
  { "SKIP-CTI", &bool_attr[0], &bool_attr[0] },
  { "DELAY-SLOT", &bool_attr[0], &bool_attr[0] },
  { "RELAXABLE", &bool_attr[0], &bool_attr[0] },
  { "RELAXED", &bool_attr[0], &bool_attr[0] },
  { "NO-DIS", &bool_attr[0], &bool_attr[0] },
  { "PBB", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

/* Instruction set variants.  */

static const CGEN_ISA mcs_cgen_isa_table[] = {
  { "gtmmcs", 32, 32, 32, 34 },
  { 0, 0, 0, 0, 0 }
};

/* Machine variants.  */

static const CGEN_MACH mcs_cgen_mach_table[] = {
  { "gtmmcsV1", "GTM-MCS", MACH_GTMMCSV1, 0 },
  { "gtmmcsV2", "GTM-MCS", MACH_GTMMCSV2, 0 },
  { "gtmmcsV3", "GTM-MCS", MACH_GTMMCSV3, 0 },
  { "gtmmcsV31", "GTM-MCS", MACH_GTMMCSV31, 0 },
  { 0, 0, 0, 0 }
};

static CGEN_KEYWORD_ENTRY mcs_cgen_opval_h_oreg_entries[] =
{
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r7", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "STA", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "ACB", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "CTRG", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "STRG", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS0", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS1", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS2", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "MHB", 15, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD mcs_cgen_opval_h_oreg =
{
  & mcs_cgen_opval_h_oreg_entries[0],
  16,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY mcs_cgen_opval_h_xoreg_entries[] =
{
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r7", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "STA", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "ACB", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "CTRG", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "STRG", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS0", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS1", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS2", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "MHB", 15, {0, {{{0, 0}}}}, 0, 0 },
  { "rs0", 16, {0, {{{0, 0}}}}, 0, 0 },
  { "rs1", 17, {0, {{{0, 0}}}}, 0, 0 },
  { "rs2", 18, {0, {{{0, 0}}}}, 0, 0 },
  { "rs3", 19, {0, {{{0, 0}}}}, 0, 0 },
  { "rs4", 20, {0, {{{0, 0}}}}, 0, 0 },
  { "rs5", 21, {0, {{{0, 0}}}}, 0, 0 },
  { "rs6", 22, {0, {{{0, 0}}}}, 0, 0 },
  { "rs7", 23, {0, {{{0, 0}}}}, 0, 0 },
  { "GMI0", 24, {0, {{{0, 0}}}}, 0, 0 },
  { "GMI1", 25, {0, {{{0, 0}}}}, 0, 0 },
  { "DSTA", 26, {0, {{{0, 0}}}}, 0, 0 },
  { "DSTAX", 27, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD mcs_cgen_opval_h_xoreg =
{
  & mcs_cgen_opval_h_xoreg_entries[0],
  28,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY mcs_cgen_opval_h_wxreg_entries[] =
{
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r7", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "STA", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "ACB", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "CTRG", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "STRG", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS0", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS1", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS2", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "MHB", 15, {0, {{{0, 0}}}}, 0, 0 },
  { "GMI0", 24, {0, {{{0, 0}}}}, 0, 0 },
  { "GMI1", 25, {0, {{{0, 0}}}}, 0, 0 },
  { "DSTA", 26, {0, {{{0, 0}}}}, 0, 0 },
  { "DSTAX", 27, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD mcs_cgen_opval_h_wxreg =
{
  & mcs_cgen_opval_h_wxreg_entries[0],
  20,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY mcs_cgen_opval_h_areg_entries[] =
{
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r7", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "zero", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "zero", 8, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD mcs_cgen_opval_h_areg =
{
  & mcs_cgen_opval_h_areg_entries[0],
  10,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY mcs_cgen_opval_h_greg_entries[] =
{
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r7", 7, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD mcs_cgen_opval_h_greg =
{
  & mcs_cgen_opval_h_greg_entries[0],
  8,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY mcs_cgen_opval_h_bareg_entries[] =
{
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r7", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "STA", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "ACB", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "CTRG", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "STRG", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS0", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS1", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "TBU_TS2", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "MHB", 15, {0, {{{0, 0}}}}, 0, 0 },
  { "rs0", 16, {0, {{{0, 0}}}}, 0, 0 },
  { "rs1", 17, {0, {{{0, 0}}}}, 0, 0 },
  { "rs2", 18, {0, {{{0, 0}}}}, 0, 0 },
  { "rs3", 19, {0, {{{0, 0}}}}, 0, 0 },
  { "rs4", 20, {0, {{{0, 0}}}}, 0, 0 },
  { "rs5", 21, {0, {{{0, 0}}}}, 0, 0 },
  { "rs6", 22, {0, {{{0, 0}}}}, 0, 0 },
  { "rs7", 23, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD mcs_cgen_opval_h_bareg =
{
  & mcs_cgen_opval_h_bareg_entries[0],
  24,
  0, 0, 0, 0, ""
};


/* The hardware table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_HW_##a)
#else
#define A(a) (1 << CGEN_HW_/**/a)
#endif

const CGEN_HW_ENTRY mcs_cgen_hw_table[] =
{
  { "h-memory", HW_H_MEMORY, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-sint", HW_H_SINT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-uint", HW_H_UINT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-addr", HW_H_ADDR, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-iaddr", HW_H_IADDR, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-pc", HW_H_PC, CGEN_ASM_NONE, 0, { 0|A(PC), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-oreg", HW_H_OREG, CGEN_ASM_KEYWORD, (PTR) & mcs_cgen_opval_h_oreg, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-xoreg", HW_H_XOREG, CGEN_ASM_KEYWORD, (PTR) & mcs_cgen_opval_h_xoreg, { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } } },
  { "h-wxreg", HW_H_WXREG, CGEN_ASM_KEYWORD, (PTR) & mcs_cgen_opval_h_wxreg, { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } } },
  { "h-areg", HW_H_AREG, CGEN_ASM_KEYWORD, (PTR) & mcs_cgen_opval_h_areg, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-greg", HW_H_GREG, CGEN_ASM_KEYWORD, (PTR) & mcs_cgen_opval_h_greg, { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } } },
  { "h-bareg", HW_H_BAREG, CGEN_ASM_KEYWORD, (PTR) & mcs_cgen_opval_h_bareg, { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } } },
  { 0, 0, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A


/* The instruction field table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_IFLD_##a)
#else
#define A(a) (1 << CGEN_IFLD_/**/a)
#endif

const CGEN_IFLD mcs_cgen_ifld_table[] =
{
  { MCS_F_NIL, "f-nil", 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_ANYOF, "f-anyof", 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPC0, "f-opc0", 0, 32, 31, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPC1, "f-opc1", 0, 32, 19, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPA, "f-opa", 0, 32, 27, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPA_4, "f-opa-4", 0, 32, 27, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPA_1, "f-opa-1", 0, 32, 14, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPB, "f-opb", 0, 32, 23, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPB_4, "f-opb-4", 0, 32, 23, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPB_1, "f-opb-1", 0, 32, 12, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_C24, "f-c24", 0, 32, 23, 24, { 0|A(SIGN_OPT), { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_C16, "f-c16", 0, 32, 15, 16, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_C15, "f-c15", 0, 32, 14, 15, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_SIMM15, "f-simm15", 0, 32, 14, 15, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_C14, "f-c14", 0, 32, 13, 14, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_SIMM12, "f-simm12", 0, 32, 11, 12, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_C12, "f-c12", 0, 32, 11, 12, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_C9, "f-c9", 0, 32, 8, 9, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_C5, "f-c5", 0, 32, 4, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_27_8, "f-27-8", 0, 32, 27, 8, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_15, "f-15", 0, 32, 15, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_15_2, "f-15-2", 0, 32, 15, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_15_3, "f-15-3", 0, 32, 15, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_15_4, "f-15-4", 0, 32, 15, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_15_7, "f-15-7", 0, 32, 15, 7, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_15_11, "f-15-11", 0, 32, 15, 11, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_15_16, "f-15-16", 0, 32, 15, 16, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_13, "f-13", 0, 32, 13, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_13_9, "f-13-9", 0, 32, 13, 9, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_11_7, "f-11-7", 0, 32, 11, 7, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_11_12, "f-11-12", 0, 32, 11, 12, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { MCS_F_OPXA, "f-opxa", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }  },
  { MCS_F_OPXB, "f-opxb", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }  },
  { 0, 0, 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A



/* multi ifield declarations */

const CGEN_MAYBE_MULTI_IFLD MCS_F_OPXA_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD MCS_F_OPXB_MULTI_IFIELD [];


/* multi ifield definitions */

const CGEN_MAYBE_MULTI_IFLD MCS_F_OPXA_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPA_4] } },
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPA_1] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD MCS_F_OPXB_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPB_4] } },
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPB_1] } },
    { 0, { (const PTR) 0 } }
};

/* The operand table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_OPERAND_##a)
#else
#define A(a) (1 << CGEN_OPERAND_/**/a)
#endif
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define OPERAND(op) MCS_OPERAND_##op
#else
#define OPERAND(op) MCS_OPERAND_/**/op
#endif

const CGEN_OPERAND mcs_cgen_operand_table[] =
{
/* pc: program counter */
  { "pc", MCS_OPERAND_PC, HW_H_PC, 0, 0,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_NIL] } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* roa: register operand a */
  { "roa", MCS_OPERAND_ROA, HW_H_OREG, 27, 4,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPA_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* rob: register operand b */
  { "rob", MCS_OPERAND_ROB, HW_H_OREG, 23, 4,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPB_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* raa: register operand a */
  { "raa", MCS_OPERAND_RAA, HW_H_AREG, 27, 4,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPA_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* rab: register operand b */
  { "rab", MCS_OPERAND_RAB, HW_H_AREG, 23, 4,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPB_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* rga: register operand a */
  { "rga", MCS_OPERAND_RGA, HW_H_GREG, 27, 4,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPA_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* rgb: register operand b */
  { "rgb", MCS_OPERAND_RGB, HW_H_GREG, 23, 4,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPB_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* rwxb: register operand b */
  { "rwxb", MCS_OPERAND_RWXB, HW_H_WXREG, 12, 5,
    { 2, { (const PTR) &MCS_F_OPXB_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }  },
/* lb: literal operand b */
  { "lb", MCS_OPERAND_LB, HW_H_UINT, 23, 4,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_OPB_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uimm5: unsigned immediate  5-bit */
  { "uimm5", MCS_OPERAND_UIMM5, HW_H_UINT, 4, 5,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C5] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uimm9: unsigned immediate  9-bit */
  { "uimm9", MCS_OPERAND_UIMM9, HW_H_UINT, 8, 9,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C9] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uimm14: unsigned immediate 14-bit */
  { "uimm14", MCS_OPERAND_UIMM14, HW_H_UINT, 13, 14,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C14] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uimm15: unsigned immediate 15-bit */
  { "uimm15", MCS_OPERAND_UIMM15, HW_H_UINT, 14, 15,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C15] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uimm16: unsigned immediate 16-bit */
  { "uimm16", MCS_OPERAND_UIMM16, HW_H_UINT, 15, 16,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C16] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uimm24: unsigned immediate 24-bit */
  { "uimm24", MCS_OPERAND_UIMM24, HW_H_UINT, 23, 24,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C24] } }, 
    { 0|A(SIGN_OPT), { { { (1<<MACH_BASE), 0 } } } }  },
/* lit24: 24-bit immediate */
  { "lit24", MCS_OPERAND_LIT24, HW_H_UINT, 23, 24,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C24] } }, 
    { 0|A(SIGN_OPT), { { { (1<<MACH_BASE), 0 } } } }  },
/* lit16: 16-bit immediate */
  { "lit16", MCS_OPERAND_LIT16, HW_H_UINT, 15, 16,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C16] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* lit14: 14-bit immediate */
  { "lit14", MCS_OPERAND_LIT14, HW_H_UINT, 13, 14,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C14] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* lit12: 12-bit immediate */
  { "lit12", MCS_OPERAND_LIT12, HW_H_UINT, 11, 12,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C12] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* rxoa: extended register operand a */
  { "rxoa", MCS_OPERAND_RXOA, HW_H_XOREG, 14, 5,
    { 2, { (const PTR) &MCS_F_OPXA_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }  },
/* rxob: extended register operand b */
  { "rxob", MCS_OPERAND_RXOB, HW_H_XOREG, 12, 5,
    { 2, { (const PTR) &MCS_F_OPXB_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }  },
/* wxlb: literal operand b 0..24 */
  { "wxlb", MCS_OPERAND_WXLB, HW_H_UINT, 12, 5,
    { 2, { (const PTR) &MCS_F_OPXB_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }  },
/* disp24: 24-bit displacement */
  { "disp24", MCS_OPERAND_DISP24, HW_H_UINT, 23, 24,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C24] } }, 
    { 0|A(SIGN_OPT), { { { (1<<MACH_BASE), 0 } } } }  },
/* disp16: 16-bit displacement */
  { "disp16", MCS_OPERAND_DISP16, HW_H_UINT, 15, 16,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C16] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* disp15: 15-bit displacement */
  { "disp15", MCS_OPERAND_DISP15, HW_H_UINT, 14, 15,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C15] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* disp14: 14-bit displacement */
  { "disp14", MCS_OPERAND_DISP14, HW_H_UINT, 13, 14,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C14] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* disp12: 12-bit displacement */
  { "disp12", MCS_OPERAND_DISP12, HW_H_UINT, 11, 12,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_C12] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sdisp15: 15-bit displacement */
  { "sdisp15", MCS_OPERAND_SDISP15, HW_H_SINT, 14, 15,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_SIMM15] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sdisp12: 12-bit displacement */
  { "sdisp12", MCS_OPERAND_SDISP12, HW_H_SINT, 11, 12,
    { 0, { (const PTR) &mcs_cgen_ifld_table[MCS_F_SIMM12] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sentinel */
  { 0, 0, 0, 0, 0,
    { 0, { (const PTR) 0 } },
    { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A


/* The instruction table.  */

#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_INSN_##a)
#else
#define A(a) (1 << CGEN_INSN_/**/a)
#endif

static const CGEN_IBASE mcs_cgen_insn_table[MAX_INSNS] =
{
  /* Special null first entry.
     A `num' value of zero is thus invalid.
     Also, the special `invalid' insn resides here.  */
  { 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
/* movl $roa,$uimm24 */
  {
    MCS_INSN_MOVL, "movl", "movl", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* mov $roa,$rob */
  {
    MCS_INSN_MOV, "mov", "mov", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mov $rxoa,$rxob */
  {
    MCS_INSN_MOV_V3, "mov-v3", "mov", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* setb $rxoa,$rxob */
  {
    MCS_INSN_SETB, "setb", "setb", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* clrb $rxoa,$rxob */
  {
    MCS_INSN_CLRB, "clrb", "clrb", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* xchb $rxoa,$rxob */
  {
    MCS_INSN_XCHB, "xchb", "xchb", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mrd $roa,$uimm14 */
  {
    MCS_INSN_MRD, "mrd", "mrd", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* mrd $roa,$disp12 */
  {
    MCS_INSN_MRD_V2, "mrd-V2", "mrd", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mrd $roa,$disp15 */
  {
    MCS_INSN_MRD_V3, "mrd-V3", "mrd", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwr $roa,$disp14 */
  {
    MCS_INSN_MWR, "mwr", "mwr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* mwr $roa,$disp12 */
  {
    MCS_INSN_MWR_V2, "mwr-V2", "mwr", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mwr $roa,$disp15 */
  {
    MCS_INSN_MWR_V3, "mwr-V3", "mwr", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwrl $roa,$uimm14 */
  {
    MCS_INSN_MWRL_V1, "mwrl-V1", "mwrl", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* mwrl $roa,$disp12 */
  {
    MCS_INSN_MWRL, "mwrl", "mwrl", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mwrl $roa,$disp15 */
  {
    MCS_INSN_MWRL_V3, "mwrl-V3", "mwrl", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwr24 $roa,$uimm14 */
  {
    MCS_INSN_MWR24, "mwr24", "mwr24", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* mrdi $roa,$rob */
  {
    MCS_INSN_MRDI, "mrdi", "mrdi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* mrdi $roa,($rob) */
  {
    MCS_INSN_MRDI_K, "mrdi-K", "mrdi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* mrdi $roa,$rob,$sdisp12 */
  {
    MCS_INSN_MRDI_C, "mrdi-C", "mrdi", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mrdi $roa,($rob)$sdisp12 */
  {
    MCS_INSN_MRDI_CK, "mrdi-CK", "mrdi", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mrdi $roa,$rob,$sdisp15 */
  {
    MCS_INSN_MRDI_V3_C, "mrdi-V3-C", "mrdi", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mrdi $roa,($rob)$sdisp15 */
  {
    MCS_INSN_MRDI_V3_CK, "mrdi-V3-CK", "mrdi", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwri $roa,$rob */
  {
    MCS_INSN_MWRI, "mwri", "mwri", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* mwri $roa,($rob) */
  {
    MCS_INSN_MWRI_K, "mwri_K", "mwri", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* mwri $roa,$rob,$sdisp12 */
  {
    MCS_INSN_MWRI_V2, "mwri-V2", "mwri", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mwri $roa,($rob)$sdisp12 */
  {
    MCS_INSN_MWRI_V2_K, "mwri-V2-K", "mwri", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mwri $roa,$rob,$sdisp15 */
  {
    MCS_INSN_MWRI_V3, "mwri-V3", "mwri", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwri $roa,($rob)$sdisp15 */
  {
    MCS_INSN_MWRI_V3_K, "mwri-V3-K", "mwri", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwril $roa,$rob */
  {
    MCS_INSN_MWRIL_1, "mwril-1", "mwril", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* mwril $roa,($rob) */
  {
    MCS_INSN_MWRIL_1K, "mwril-1K", "mwril", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* mwril $roa,$rob */
  {
    MCS_INSN_MWRIL, "mwril", "mwril", 32,
    { 0, { { { (1<<MACH_GTMMCSV2)|(1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwril $roa,($rob) */
  {
    MCS_INSN_MWRIL_K, "mwril_K", "mwril", 32,
    { 0, { { { (1<<MACH_GTMMCSV2)|(1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwri24 $roa,$rob */
  {
    MCS_INSN_MWRI24, "mwri24", "mwri24", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* mwri24 $roa,($rob) */
  {
    MCS_INSN_MWRI24_K, "mwri24_K", "mwri24", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* mrdio $rxoa,$rxob */
  {
    MCS_INSN_MRDIO, "mrdio", "mrdio", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mwrio $rxoa,$rxob */
  {
    MCS_INSN_MWRIO, "mwrio", "mwrio", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* pop $roa */
  {
    MCS_INSN_POP, "pop", "pop", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* push $roa */
  {
    MCS_INSN_PUSH, "push", "push", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ard $raa,$rab,$uimm9 */
  {
    MCS_INSN_ARD, "ard", "ard", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ardi $raa,$rab */
  {
    MCS_INSN_ARDI, "ardi", "ardi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* awr $roa,$rob,$uimm5 */
  {
    MCS_INSN_AWR, "awr", "awr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* awri $roa,$rob */
  {
    MCS_INSN_AWRI, "awri", "awri", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* nard $raa,$rab,$uimm9 */
  {
    MCS_INSN_NARD, "nard", "nard", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* nardi $raa,$rab */
  {
    MCS_INSN_NARDI, "nardi", "nardi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* brd $rga,$disp16 */
  {
    MCS_INSN_BRD, "brd", "brd", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* bwr $rga,$disp16 */
  {
    MCS_INSN_BWR, "bwr", "bwr", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* brdi $rga,$rgb */
  {
    MCS_INSN_BRDI, "brdi", "brdi", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* brdi $rga,($rgb) */
  {
    MCS_INSN_BRDI_K, "brdi_K", "brdi", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* bwri $rga,$rgb */
  {
    MCS_INSN_BWRI, "bwri", "bwri", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* bwri $rga,($rgb) */
  {
    MCS_INSN_BWRI_K, "bwri_K", "bwri", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* addl $roa,$uimm24 */
  {
    MCS_INSN_ADDL, "addl", "addl", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add $roa,$rob */
  {
    MCS_INSN_ADD, "add", "add", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* add $rxoa,$rxob */
  {
    MCS_INSN_ADD_V3, "add-V3", "add", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* addc $rxoa,$rxob */
  {
    MCS_INSN_ADDC, "addc", "addc", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* subl $roa,$uimm24 */
  {
    MCS_INSN_SUBL, "subl", "subl", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub $roa,$rob */
  {
    MCS_INSN_SUB, "sub", "sub", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* sub $rxoa,$rxob */
  {
    MCS_INSN_SUB_V3, "sub-V3", "sub", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* subc $rxoa,$rxob */
  {
    MCS_INSN_SUBC, "subc", "subc", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* neg $roa,$rob */
  {
    MCS_INSN_NEG, "neg", "neg", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* neg $rxoa,$rxob */
  {
    MCS_INSN_NEG_V3, "neg-V3", "neg", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* andl $roa,$uimm24 */
  {
    MCS_INSN_ANDL, "andl", "andl", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and $roa,$rob */
  {
    MCS_INSN_AND, "and", "and", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* and $rxoa,$rxob */
  {
    MCS_INSN_AND_V3, "and-V3", "and", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* orl $roa,$uimm24 */
  {
    MCS_INSN_ORL, "orl", "orl", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or $roa,$rob */
  {
    MCS_INSN_OR, "or", "or", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* or $rxoa,$rxob */
  {
    MCS_INSN_OR_V3, "or-V3", "or", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* xorl $roa,$uimm24 */
  {
    MCS_INSN_XORL, "xorl", "xorl", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor $roa,$rob */
  {
    MCS_INSN_XOR, "xor", "xor", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* xor $rxoa,$rxob */
  {
    MCS_INSN_XOR_V3, "xor-V3", "xor", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* shr $roa,$uimm5 */
  {
    MCS_INSN_SHR, "shr", "shr", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* shr $rxoa,$uimm5 */
  {
    MCS_INSN_SHR_V3, "shr-V3", "shr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shl $roa,$uimm5 */
  {
    MCS_INSN_SHL, "shl", "shl", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* shl $rxoa,$uimm5 */
  {
    MCS_INSN_SHL_V3, "shl-V3", "shl", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* asru $roa,$rob */
  {
    MCS_INSN_ASRU, "asru", "asru", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* asru $rxoa,$rxob */
  {
    MCS_INSN_ASRU_V3, "asru-V3", "asru", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* asrs $roa,$rob */
  {
    MCS_INSN_ASRS, "asrs", "asrs", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* asrs $rxoa,$rxob */
  {
    MCS_INSN_ASRS_V3, "asrs-V3", "asrs", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* asl $roa,$rob */
  {
    MCS_INSN_ASL, "asl", "asl", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* asl $rxoa,$rxob */
  {
    MCS_INSN_ASL_V3, "asl-V3", "asl", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mulu $rxoa,$rxob,$uimm5 */
  {
    MCS_INSN_MULU_C, "mulu-C", "mulu", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mulu $rxoa,$rxob */
  {
    MCS_INSN_MULU, "mulu", "mulu", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* muls $rxoa,$rxob,$uimm5 */
  {
    MCS_INSN_MULS_C, "muls-C", "muls", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* muls $rxoa,$rxob */
  {
    MCS_INSN_MULS, "muls", "muls", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* divu $rxoa,$rxob,$uimm5 */
  {
    MCS_INSN_DIVU_C, "divu-C", "divu", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* divu $rxoa,$rxob */
  {
    MCS_INSN_DIVU, "divu", "divu", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* divs $rxoa,$rxob,$uimm5 */
  {
    MCS_INSN_DIVS_C, "divs-C", "divs", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* divs $rxoa,$rxob */
  {
    MCS_INSN_DIVS, "divs", "divs", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* maxs $roa,$rob */
  {
    MCS_INSN_MAXS, "maxs", "maxs", 34,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* maxs $rxoa,$rxob */
  {
    MCS_INSN_MAXS_V3, "maxs-V3", "maxs", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* maxu $roa,$rob */
  {
    MCS_INSN_MAXU, "maxu", "maxu", 34,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* maxu $rxoa,$rxob */
  {
    MCS_INSN_MAXU_V3, "maxu-V3", "maxu", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* mins $roa,$rob */
  {
    MCS_INSN_MINS, "mins", "mins", 34,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* mins $rxoa,$rxob */
  {
    MCS_INSN_MINS_V3, "mins-V3", "mins", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* minu $roa,$rob */
  {
    MCS_INSN_MINU, "minu", "minu", 34,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* minu $rxoa,$rxob */
  {
    MCS_INSN_MINU_V3, "minu-V3", "minu", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* atul $roa,$uimm24 */
  {
    MCS_INSN_ATUL, "atul", "atul", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* atu $roa,$rob */
  {
    MCS_INSN_ATU, "atu", "atu", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* atu $rxoa,$rxob */
  {
    MCS_INSN_ATU_V3, "atu-V3", "atu", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* atsl $roa,$uimm24 */
  {
    MCS_INSN_ATSL, "atsl", "atsl", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ats $roa,$rob */
  {
    MCS_INSN_ATS, "ats", "ats", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* ats $rxoa,$rxob */
  {
    MCS_INSN_ATS_V3, "ats-V3", "ats", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* btl $roa,$uimm24 */
  {
    MCS_INSN_BTL, "btl", "btl", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bt $roa,$rob */
  {
    MCS_INSN_BT, "bt", "bt", 32,
    { 0, { { { (1<<MACH_GTMMCSV1)|(1<<MACH_GTMMCSV2), 0 } } } }
  },
/* bt $rxoa,$rxob */
  {
    MCS_INSN_BT_V3, "bt-V3", "bt", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* jmp $disp14 */
  {
    MCS_INSN_JMP, "jmp", "jmp", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* jmp $disp12 */
  {
    MCS_INSN_JMP_V2, "jmp-V2", "jmp", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* jmp $disp15 */
  {
    MCS_INSN_JMP_V3, "jmp-V3", "jmp", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* jbs $roa,$lb,$disp14 */
  {
    MCS_INSN_JBS, "jbs", "jbs", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* jbs $roa,$lb,$disp12 */
  {
    MCS_INSN_JBS_V2, "jbs-V2", "jbs", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* jbs $roa,$lb,$disp15 */
  {
    MCS_INSN_JBS_V3, "jbs-V3", "jbs", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* jbc $roa,$lb,$disp14 */
  {
    MCS_INSN_JBC, "jbc", "jbc", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* jbc $roa,$lb,$disp12 */
  {
    MCS_INSN_JBC_V2, "jbc-V2", "jbc", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* jbc $roa,$lb,$disp15 */
  {
    MCS_INSN_JBC_V3, "jbc-V3", "jbc", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* call $disp14 */
  {
    MCS_INSN_CALL, "call", "call", 32,
    { 0, { { { (1<<MACH_GTMMCSV1), 0 } } } }
  },
/* call $disp12 */
  {
    MCS_INSN_CALL_V2, "call-V2", "call", 32,
    { 0, { { { (1<<MACH_GTMMCSV2), 0 } } } }
  },
/* call $disp15 */
  {
    MCS_INSN_CALL_V3, "call-V3", "call", 32,
    { 0, { { { (1<<MACH_GTMMCSV3)|(1<<MACH_GTMMCSV31), 0 } } } }
  },
/* ret */
  {
    MCS_INSN_RET, "ret", "ret", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* jmpi */
  {
    MCS_INSN_JMPI, "jmpi", "jmpi", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* jbsi $rxoa,$wxlb */
  {
    MCS_INSN_JBSI, "jbsi", "jbsi", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* jbci $rxoa,$wxlb */
  {
    MCS_INSN_JBCI, "jbci", "jbci", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* calli */
  {
    MCS_INSN_CALLI, "calli", "calli", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* wurm $roa,$rob,$uimm16 */
  {
    MCS_INSN_WURM, "wurm", "wurm", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* wurmx $roa,$rwxb */
  {
    MCS_INSN_WURMX, "wurmx", "wurmx", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* wurcx $roa,$rwxb */
  {
    MCS_INSN_WURCX, "wurcx", "wurcx", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* wuce $roa,$rob */
  {
    MCS_INSN_WUCE, "wuce", "wuce", 32,
    { 0, { { { (1<<MACH_GTMMCSV31), 0 } } } }
  },
/* nop */
  {
    MCS_INSN_NOP, "nop", "nop", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
};

#undef OP
#undef A

/* Initialize anything needed to be done once, before any cpu_open call.  */

static void
init_tables (void)
{
}

static const CGEN_MACH * lookup_mach_via_bfd_name (const CGEN_MACH *, const char *);
static void build_hw_table      (CGEN_CPU_TABLE *);
static void build_ifield_table  (CGEN_CPU_TABLE *);
static void build_operand_table (CGEN_CPU_TABLE *);
static void build_insn_table    (CGEN_CPU_TABLE *);
static void mcs_cgen_rebuild_tables (CGEN_CPU_TABLE *);

/* Subroutine of mcs_cgen_cpu_open to look up a mach via its bfd name.  */

static const CGEN_MACH *
lookup_mach_via_bfd_name (const CGEN_MACH *table, const char *name)
{
  while (table->name)
    {
      if (strcmp (name, table->bfd_name) == 0)
	return table;
      ++table;
    }
  abort ();
}

/* Subroutine of mcs_cgen_cpu_open to build the hardware table.  */

static void
build_hw_table (CGEN_CPU_TABLE *cd)
{
  int i;
  int machs = cd->machs;
  const CGEN_HW_ENTRY *init = & mcs_cgen_hw_table[0];
  /* MAX_HW is only an upper bound on the number of selected entries.
     However each entry is indexed by it's enum so there can be holes in
     the table.  */
  const CGEN_HW_ENTRY **selected =
    (const CGEN_HW_ENTRY **) xmalloc (MAX_HW * sizeof (CGEN_HW_ENTRY *));

  cd->hw_table.init_entries = init;
  cd->hw_table.entry_size = sizeof (CGEN_HW_ENTRY);
  memset (selected, 0, MAX_HW * sizeof (CGEN_HW_ENTRY *));
  /* ??? For now we just use machs to determine which ones we want.  */
  for (i = 0; init[i].name != NULL; ++i)
    if (CGEN_HW_ATTR_VALUE (&init[i], CGEN_HW_MACH)
	& machs)
      selected[init[i].type] = &init[i];
  cd->hw_table.entries = selected;
  cd->hw_table.num_entries = MAX_HW;
}

/* Subroutine of mcs_cgen_cpu_open to build the hardware table.  */

static void
build_ifield_table (CGEN_CPU_TABLE *cd)
{
  cd->ifld_table = & mcs_cgen_ifld_table[0];
}

/* Subroutine of mcs_cgen_cpu_open to build the hardware table.  */

static void
build_operand_table (CGEN_CPU_TABLE *cd)
{
  int i;
  int machs = cd->machs;
  const CGEN_OPERAND *init = & mcs_cgen_operand_table[0];
  /* MAX_OPERANDS is only an upper bound on the number of selected entries.
     However each entry is indexed by it's enum so there can be holes in
     the table.  */
  const CGEN_OPERAND **selected = xmalloc (MAX_OPERANDS * sizeof (* selected));

  cd->operand_table.init_entries = init;
  cd->operand_table.entry_size = sizeof (CGEN_OPERAND);
  memset (selected, 0, MAX_OPERANDS * sizeof (CGEN_OPERAND *));
  /* ??? For now we just use mach to determine which ones we want.  */
  for (i = 0; init[i].name != NULL; ++i)
    if (CGEN_OPERAND_ATTR_VALUE (&init[i], CGEN_OPERAND_MACH)
	& machs)
      selected[init[i].type] = &init[i];
  cd->operand_table.entries = selected;
  cd->operand_table.num_entries = MAX_OPERANDS;
}

/* Subroutine of mcs_cgen_cpu_open to build the hardware table.
   ??? This could leave out insns not supported by the specified mach/isa,
   but that would cause errors like "foo only supported by bar" to become
   "unknown insn", so for now we include all insns and require the app to
   do the checking later.
   ??? On the other hand, parsing of such insns may require their hardware or
   operand elements to be in the table [which they mightn't be].  */

static void
build_insn_table (CGEN_CPU_TABLE *cd)
{
  int i;
  const CGEN_IBASE *ib = & mcs_cgen_insn_table[0];
  CGEN_INSN *insns = xmalloc (MAX_INSNS * sizeof (CGEN_INSN));

  memset (insns, 0, MAX_INSNS * sizeof (CGEN_INSN));
  for (i = 0; i < MAX_INSNS; ++i)
    insns[i].base = &ib[i];
  cd->insn_table.init_entries = insns;
  cd->insn_table.entry_size = sizeof (CGEN_IBASE);
  cd->insn_table.num_init_entries = MAX_INSNS;
}

/* Subroutine of mcs_cgen_cpu_open to rebuild the tables.  */

static void
mcs_cgen_rebuild_tables (CGEN_CPU_TABLE *cd)
{
  int i;
  CGEN_BITSET *isas = cd->isas;
  unsigned int machs = cd->machs;

  cd->int_insn_p = CGEN_INT_INSN_P;

  /* Data derived from the isa spec.  */
#define UNSET (CGEN_SIZE_UNKNOWN + 1)
  cd->default_insn_bitsize = UNSET;
  cd->base_insn_bitsize = UNSET;
  cd->min_insn_bitsize = 65535; /* Some ridiculously big number.  */
  cd->max_insn_bitsize = 0;
  for (i = 0; i < MAX_ISAS; ++i)
    if (cgen_bitset_contains (isas, i))
      {
	const CGEN_ISA *isa = & mcs_cgen_isa_table[i];

	/* Default insn sizes of all selected isas must be
	   equal or we set the result to 0, meaning "unknown".  */
	if (cd->default_insn_bitsize == UNSET)
	  cd->default_insn_bitsize = isa->default_insn_bitsize;
	else if (isa->default_insn_bitsize == cd->default_insn_bitsize)
	  ; /* This is ok.  */
	else
	  cd->default_insn_bitsize = CGEN_SIZE_UNKNOWN;

	/* Base insn sizes of all selected isas must be equal
	   or we set the result to 0, meaning "unknown".  */
	if (cd->base_insn_bitsize == UNSET)
	  cd->base_insn_bitsize = isa->base_insn_bitsize;
	else if (isa->base_insn_bitsize == cd->base_insn_bitsize)
	  ; /* This is ok.  */
	else
	  cd->base_insn_bitsize = CGEN_SIZE_UNKNOWN;

	/* Set min,max insn sizes.  */
	if (isa->min_insn_bitsize < cd->min_insn_bitsize)
	  cd->min_insn_bitsize = isa->min_insn_bitsize;
	if (isa->max_insn_bitsize > cd->max_insn_bitsize)
	  cd->max_insn_bitsize = isa->max_insn_bitsize;
      }

  /* Data derived from the mach spec.  */
  for (i = 0; i < MAX_MACHS; ++i)
    if (((1 << i) & machs) != 0)
      {
	const CGEN_MACH *mach = & mcs_cgen_mach_table[i];

	if (mach->insn_chunk_bitsize != 0)
	{
	  if (cd->insn_chunk_bitsize != 0 && cd->insn_chunk_bitsize != mach->insn_chunk_bitsize)
	    {
	      fprintf (stderr, "mcs_cgen_rebuild_tables: conflicting insn-chunk-bitsize values: `%d' vs. `%d'\n",
		       cd->insn_chunk_bitsize, mach->insn_chunk_bitsize);
	      abort ();
	    }

 	  cd->insn_chunk_bitsize = mach->insn_chunk_bitsize;
	}
      }

  /* Determine which hw elements are used by MACH.  */
  build_hw_table (cd);

  /* Build the ifield table.  */
  build_ifield_table (cd);

  /* Determine which operands are used by MACH/ISA.  */
  build_operand_table (cd);

  /* Build the instruction table.  */
  build_insn_table (cd);
}

/* Initialize a cpu table and return a descriptor.
   It's much like opening a file, and must be the first function called.
   The arguments are a set of (type/value) pairs, terminated with
   CGEN_CPU_OPEN_END.

   Currently supported values:
   CGEN_CPU_OPEN_ISAS:    bitmap of values in enum isa_attr
   CGEN_CPU_OPEN_MACHS:   bitmap of values in enum mach_attr
   CGEN_CPU_OPEN_BFDMACH: specify 1 mach using bfd name
   CGEN_CPU_OPEN_ENDIAN:  specify endian choice
   CGEN_CPU_OPEN_END:     terminates arguments

   ??? Simultaneous multiple isas might not make sense, but it's not (yet)
   precluded.

   ??? We only support ISO C stdargs here, not K&R.
   Laziness, plus experiment to see if anything requires K&R - eventually
   K&R will no longer be supported - e.g. GDB is currently trying this.  */

CGEN_CPU_DESC
mcs_cgen_cpu_open (enum cgen_cpu_open_arg arg_type, ...)
{
  CGEN_CPU_TABLE *cd = (CGEN_CPU_TABLE *) xmalloc (sizeof (CGEN_CPU_TABLE));
  static int init_p;
  CGEN_BITSET *isas = 0;  /* 0 = "unspecified" */
  unsigned int machs = 0; /* 0 = "unspecified" */
  enum cgen_endian endian = CGEN_ENDIAN_UNKNOWN;
  va_list ap;

  if (! init_p)
    {
      init_tables ();
      init_p = 1;
    }

  memset (cd, 0, sizeof (*cd));

  va_start (ap, arg_type);
  while (arg_type != CGEN_CPU_OPEN_END)
    {
      switch (arg_type)
	{
	case CGEN_CPU_OPEN_ISAS :
	  isas = va_arg (ap, CGEN_BITSET *);
	  break;
	case CGEN_CPU_OPEN_MACHS :
	  machs = va_arg (ap, unsigned int);
	  break;
	case CGEN_CPU_OPEN_BFDMACH :
	  {
	    const char *name = va_arg (ap, const char *);
	    const CGEN_MACH *mach =
	      lookup_mach_via_bfd_name (mcs_cgen_mach_table, name);

	    machs |= 1 << mach->num;
	    break;
	  }
	case CGEN_CPU_OPEN_ENDIAN :
	  endian = va_arg (ap, enum cgen_endian);
	  break;
	default :
	  fprintf (stderr, "mcs_cgen_cpu_open: unsupported argument `%d'\n",
		   arg_type);
	  abort (); /* ??? return NULL? */
	}
      arg_type = va_arg (ap, enum cgen_cpu_open_arg);
    }
  va_end (ap);

  /* Mach unspecified means "all".  */
  if (machs == 0)
    machs = (1 << MAX_MACHS) - 1;
  /* Base mach is always selected.  */
  machs |= 1;
  if (endian == CGEN_ENDIAN_UNKNOWN)
    {
      /* ??? If target has only one, could have a default.  */
      fprintf (stderr, "mcs_cgen_cpu_open: no endianness specified\n");
      abort ();
    }

  cd->isas = cgen_bitset_copy (isas);
  cd->machs = machs;
  cd->endian = endian;
  /* FIXME: for the sparc case we can determine insn-endianness statically.
     The worry here is where both data and insn endian can be independently
     chosen, in which case this function will need another argument.
     Actually, will want to allow for more arguments in the future anyway.  */
  cd->insn_endian = endian;

  /* Table (re)builder.  */
  cd->rebuild_tables = mcs_cgen_rebuild_tables;
  mcs_cgen_rebuild_tables (cd);

  /* Default to not allowing signed overflow.  */
  cd->signed_overflow_ok_p = 0;
  
  return (CGEN_CPU_DESC) cd;
}

/* Cover fn to mcs_cgen_cpu_open to handle the simple case of 1 isa, 1 mach.
   MACH_NAME is the bfd name of the mach.  */

CGEN_CPU_DESC
mcs_cgen_cpu_open_1 (const char *mach_name, enum cgen_endian endian)
{
  return mcs_cgen_cpu_open (CGEN_CPU_OPEN_BFDMACH, mach_name,
			       CGEN_CPU_OPEN_ENDIAN, endian,
			       CGEN_CPU_OPEN_END);
}

/* Close a cpu table.
   ??? This can live in a machine independent file, but there's currently
   no place to put this file (there's no libcgen).  libopcodes is the wrong
   place as some simulator ports use this but they don't use libopcodes.  */

void
mcs_cgen_cpu_close (CGEN_CPU_DESC cd)
{
  unsigned int i;
  const CGEN_INSN *insns;

  if (cd->macro_insn_table.init_entries)
    {
      insns = cd->macro_insn_table.init_entries;
      for (i = 0; i < cd->macro_insn_table.num_init_entries; ++i, ++insns)
	if (CGEN_INSN_RX ((insns)))
	  regfree (CGEN_INSN_RX (insns));
    }

  if (cd->insn_table.init_entries)
    {
      insns = cd->insn_table.init_entries;
      for (i = 0; i < cd->insn_table.num_init_entries; ++i, ++insns)
	if (CGEN_INSN_RX (insns))
	  regfree (CGEN_INSN_RX (insns));
    }  

  if (cd->macro_insn_table.init_entries)
    free ((CGEN_INSN *) cd->macro_insn_table.init_entries);

  if (cd->insn_table.init_entries)
    free ((CGEN_INSN *) cd->insn_table.init_entries);

  if (cd->hw_table.entries)
    free ((CGEN_HW_ENTRY *) cd->hw_table.entries);

  if (cd->operand_table.entries)
    free ((CGEN_HW_ENTRY *) cd->operand_table.entries);

  free (cd);
}

