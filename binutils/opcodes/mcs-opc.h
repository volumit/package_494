/* Instruction opcode header for mcs.

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

#ifndef MCS_OPC_H
#define MCS_OPC_H

/* -- opc.h */

/* Allows reason codes to be output when assembler errors occur.  */
#define CGEN_VERBOSE_ASSEMBLER_ERRORS

#define CGEN_DIS_HASH_SIZE 16
#define CGEN_DIS_HASH(buf,value) ((value >> 28) & 0xf) 

extern int mcs_cgen_insn_supported (CGEN_CPU_DESC, const CGEN_INSN *);
extern unsigned int mcs_extended_regs (unsigned int);
extern int mcs_no_use_extended_regs;


/* -- asm.c */
/* Enum declaration for mcs instruction types.  */
typedef enum cgen_insn_type {
  MCS_INSN_INVALID, MCS_INSN_MOVL, MCS_INSN_MOV, MCS_INSN_MOV_V3
 , MCS_INSN_SETB, MCS_INSN_CLRB, MCS_INSN_XCHB, MCS_INSN_MRD
 , MCS_INSN_MRD_V2, MCS_INSN_MRD_V3, MCS_INSN_MWR, MCS_INSN_MWR_V2
 , MCS_INSN_MWR_V3, MCS_INSN_MWRL_V1, MCS_INSN_MWRL, MCS_INSN_MWRL_V3
 , MCS_INSN_MWR24, MCS_INSN_MRDI, MCS_INSN_MRDI_K, MCS_INSN_MRDI_C
 , MCS_INSN_MRDI_CK, MCS_INSN_MRDI_V3_C, MCS_INSN_MRDI_V3_CK, MCS_INSN_MWRI
 , MCS_INSN_MWRI_K, MCS_INSN_MWRI_V2, MCS_INSN_MWRI_V2_K, MCS_INSN_MWRI_V3
 , MCS_INSN_MWRI_V3_K, MCS_INSN_MWRIL_1, MCS_INSN_MWRIL_1K, MCS_INSN_MWRIL
 , MCS_INSN_MWRIL_K, MCS_INSN_MWRI24, MCS_INSN_MWRI24_K, MCS_INSN_MRDIO
 , MCS_INSN_MWRIO, MCS_INSN_POP, MCS_INSN_PUSH, MCS_INSN_ARD
 , MCS_INSN_ARDI, MCS_INSN_AWR, MCS_INSN_AWRI, MCS_INSN_NARD
 , MCS_INSN_NARDI, MCS_INSN_BRD, MCS_INSN_BWR, MCS_INSN_BRDI
 , MCS_INSN_BRDI_K, MCS_INSN_BWRI, MCS_INSN_BWRI_K, MCS_INSN_ADDL
 , MCS_INSN_ADD, MCS_INSN_ADD_V3, MCS_INSN_ADDC, MCS_INSN_SUBL
 , MCS_INSN_SUB, MCS_INSN_SUB_V3, MCS_INSN_SUBC, MCS_INSN_NEG
 , MCS_INSN_NEG_V3, MCS_INSN_ANDL, MCS_INSN_AND, MCS_INSN_AND_V3
 , MCS_INSN_ORL, MCS_INSN_OR, MCS_INSN_OR_V3, MCS_INSN_XORL
 , MCS_INSN_XOR, MCS_INSN_XOR_V3, MCS_INSN_SHR, MCS_INSN_SHR_V3
 , MCS_INSN_SHL, MCS_INSN_SHL_V3, MCS_INSN_ASRU, MCS_INSN_ASRU_V3
 , MCS_INSN_ASRS, MCS_INSN_ASRS_V3, MCS_INSN_ASL, MCS_INSN_ASL_V3
 , MCS_INSN_MULU_C, MCS_INSN_MULU, MCS_INSN_MULS_C, MCS_INSN_MULS
 , MCS_INSN_DIVU_C, MCS_INSN_DIVU, MCS_INSN_DIVS_C, MCS_INSN_DIVS
 , MCS_INSN_MAXS, MCS_INSN_MAXS_V3, MCS_INSN_MAXU, MCS_INSN_MAXU_V3
 , MCS_INSN_MINS, MCS_INSN_MINS_V3, MCS_INSN_MINU, MCS_INSN_MINU_V3
 , MCS_INSN_ATUL, MCS_INSN_ATU, MCS_INSN_ATU_V3, MCS_INSN_ATSL
 , MCS_INSN_ATS, MCS_INSN_ATS_V3, MCS_INSN_BTL, MCS_INSN_BT
 , MCS_INSN_BT_V3, MCS_INSN_JMP, MCS_INSN_JMP_V2, MCS_INSN_JMP_V3
 , MCS_INSN_JBS, MCS_INSN_JBS_V2, MCS_INSN_JBS_V3, MCS_INSN_JBC
 , MCS_INSN_JBC_V2, MCS_INSN_JBC_V3, MCS_INSN_CALL, MCS_INSN_CALL_V2
 , MCS_INSN_CALL_V3, MCS_INSN_RET, MCS_INSN_JMPI, MCS_INSN_JBSI
 , MCS_INSN_JBCI, MCS_INSN_CALLI, MCS_INSN_WURM, MCS_INSN_WURMX
 , MCS_INSN_WURCX, MCS_INSN_WUCE, MCS_INSN_NOP
} CGEN_INSN_TYPE;

/* Index of `invalid' insn place holder.  */
#define CGEN_INSN_INVALID MCS_INSN_INVALID

/* Total number of insns in table.  */
#define MAX_INSNS ((int) MCS_INSN_NOP + 1)

/* This struct records data prior to insertion or after extraction.  */
struct cgen_fields
{
  int length;
  long f_nil;
  long f_anyof;
  long f_opc0;
  long f_opc1;
  long f_opa;
  long f_opa_4;
  long f_opa_1;
  long f_opb;
  long f_opb_4;
  long f_opb_1;
  long f_c24;
  long f_c16;
  long f_c15;
  long f_simm15;
  long f_c14;
  long f_simm12;
  long f_c12;
  long f_c9;
  long f_c5;
  long f_27_8;
  long f_15;
  long f_15_2;
  long f_15_3;
  long f_15_4;
  long f_15_7;
  long f_15_11;
  long f_15_16;
  long f_13;
  long f_13_9;
  long f_11_7;
  long f_11_12;
  long f_opxa;
  long f_opxb;
};

#define CGEN_INIT_PARSE(od) \
{\
}
#define CGEN_INIT_INSERT(od) \
{\
}
#define CGEN_INIT_EXTRACT(od) \
{\
}
#define CGEN_INIT_PRINT(od) \
{\
}


#endif /* MCS_OPC_H */
