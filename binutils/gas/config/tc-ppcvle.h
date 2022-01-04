/* tc-ppc.c -- translation table for PPC instruction to VLE
   Copyright 2010 Free Software Foundation, Inc.
   Written by Ingo Grenner/Horst Lehser , HighTec EDV Systeme GmbH

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */


int vle_macro(const char*,char*);


typedef enum
{
    TR_OT_IMM,
    TR_OT_REG,
    TR_OT_SYM
} TRANSL_OP_TYPE;


typedef enum
{
    TR_ST_OK,
    TR_ST_NOTHING_TODO,
    TR_ST_ERROR    
} TRANSL_STATE;

typedef struct 
{
  char* descriptor;
  long value;
  TRANSL_OP_TYPE type;
} ppcvle_operand;


typedef enum {
  OP_INVALID,
  OP_VLE_REG,
  OP_GPR_REG,
  OP_AGPR_REG,
  OP_GPR1_REG,
  OP_GPR0_REG,
  OP_CR_REG,
  OP_CR03_REG,
  OP_CR0_REG,
  OP_CR_BIT,
  OP_CR03_BIT,
  OP_CR0_BIT,
  OP_BO16,
  OP_BO32,
  OP_BO_UNCOND,
  OP_BD15,
  OP_BD24,
  OP_BD8,
  OP_SI16,
  OP_SI8,
  OP_SCI8,
  OP_LI20,
  OP_OIM5,
  OP_NEGOIM5,
  OP_SD4,
  OP_SD41,
  OP_SD42,
  OP_UI5,
  OP_UI7,
  OP_UI16,
  OP_SINGLEBIT,
  OP_ISVALID
} operand_constraint_t;

typedef enum {
  OPS_ISVALID,
  OPS_EQ,
  OPS_NEQ,
  OPS_OP1_LT_OP2,
  OPS_TRAIL1,
  OPS_EQ_SINGLEBIT,
  OPS_EQ_SINGLE_NOTMASK,
  OPS_INVALID  
} insn_constraint_t;


typedef struct {
  int   num_trans;              /* number of translated insns */
  const char *insns[3];         /* the translated insns */
  operand_constraint_t op_const[5];    /* constraints for the operands */
  insn_constraint_t insn_const;   /* additonal constraint between two operands
                                   */
  int  op0, op1, op2, op3, op4;              /* operand parameters for additonal constraint
                                    */
} ppc_vle_trans;


typedef struct {
  const char *name;             /* original instruction name */
  const int  numoftrans;        /* number of possible translations */
  const ppc_vle_trans trans[8]; /* the translations */
  TRANSL_STATE   state;         /* state if no translation is valid maybe
                                   TR_ST_OK, TR_ST_NOTHING_TODO - take the
                                   original insn, TR_ST_ERROR - no translation
                                   possible */
} ppc_vle_macros_t;

extern const ppc_vle_macros_t ppc_vle_macro_table[];
extern const int  ppc_vle_num_macros;
extern bfd_boolean ppcvle_is_op_valid(operand_constraint_t constraint, ppcvle_operand *op);
extern bfd_boolean ppcvle_is_op_list_valid(insn_constraint_t constraint,
                 ppcvle_operand *op0, ppcvle_operand *op1, ppcvle_operand *op2,
                 ppcvle_operand *op3, ppcvle_operand *op4);
