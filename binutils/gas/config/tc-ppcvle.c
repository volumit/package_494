/* tc-ppcvle.c -- translation table for PPC instruction to VLE
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

#include "as.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include "dw2gencfi.h"
#include "opcode/ppc.h"
#include "config/tc-ppcvle.h"

#ifdef OBJ_ELF
#include "elf/ppc.h"
#include "dwarf2dbg.h"
#endif



/*********************************
 * helper functions and macros
 *********************************/

/*********************************
 * operand constraints
 *********************************/

static int is_val_sci8 (long val);
static int is_val_signed(long val, unsigned int mask);
static bfd_boolean is_trailing_1(ppcvle_operand *op);
static int is_single_bit(unsigned long val,bfd_boolean set);

extern bfd_boolean tf_set_mb_me(long* val1,long* val2,long value);

#define IS_VLE_REG(op)  (op < 8 || op > 23)     /* valid VLE GPR r0-r7,r24-r31 */
#define IS_AGPR_REG(op) (op >= 8 || op <= 23)     /* valid alternate VLE GPR r8-r23 */
#define IS_GPR_REG(op)  ((op >= 0) && (op < 32))  /* valid GP-register r0-r31 */ 
#define IS_GPR1_REG(op) ((op > 0) && (op < 32))   /* valid GP-register r1-r31 */ 
#define IS_GPR0_REG(op) (op == 0)                  /* valid GP-register r0 */ 

#define IS_CR_REG(op)   ((op >= 0) && (op <= 7))   /* valid CR-register cr0-cr7 */ 
#define IS_CR03_REG(op) ((op >= 0) && (op <= 3))   /* valid CR-register cr0-cr3 */
#define IS_CR0_REG(op)  (op == 0)                  /* valid CR-register cr0 */ 

#define IS_CR_BIT(op)   ((op >= 0)&& (op<32))       /* valid CR-register bit cr0-cr7 */ 
#define IS_CR03_BIT(op) ((op >= 0)&& (op<16))       /* valid CR-register bit cr0-cr3 */ 
#define IS_CR0_BIT(op)  ((op >= 0)&& (op<4))        /* valid CR-register bit cr0     */ 

#define IS_BD15(op)     is_val_signed(op,0xfffe)   /* valid BD15  signed 16-bit */
#define IS_BD24(op)     is_val_signed(op,0x1fffffe)/* valid BD24  signed 24-bit */
#define IS_BD8(op)      is_val_signed(op,0x1fe)    /* valid BD8  signed 9-bit */

#define IS_SI16(op)     is_val_signed(op,0xffff)   /* valid D/SI signed 16-bit */
#define IS_SI8(op)      is_val_signed(op,0xff)     /* valid D8 operand signed 8-bit */
#define IS_SCI8(op)     is_val_sci8(op)         /* valid SCI8 operan*/
#define IS_LI20(op)     is_val_signed(op,0xfffff)  /* valid LI20 signed 20-bit */
#define IS_OIM5(op)     ((op > 0) && (op <= 32))      /* valid OIM5 (1 - 32) */
#define IS_NEGOIM5(op)     ((op < 0) && (op >= -32))      /* valid NEGOIM5 ( (-1) - (-32)) */

#define IS_SD4(op)      ((op & ~0xf) == 0)         /* valid SD4 for byte access */
#define IS_SD41(op)     ((op & ~0x1e) == 0)        /* valid SD4 for half word access */
#define IS_SD42(op)      ((op & ~0x3c) == 0)         /* valid SD4 for word access */

#define IS_UI5(op)      ((op >= 0) && (op < (1<<5)))  /* valid UI5 unsigned 5-bit */
#define IS_UI7(op)      ((op >= 0) && (op < (1<<7)))  /* valid UI7 unsigned 7-bit */
#define IS_UI16(op)     ((op >= 0) && (op < (1<<16))) /* valid UI unsigned 16-bit */

#define IS_TRAIL1(op) is_trailing_1(op)
#define IS_SINGLEBIT(op) is_single_bit(op,TRUE)

const ppc_vle_macros_t ppc_vle_macro_table[] = {
    { "add" /* rD,rA,rB */,
      2,
      {
          { 1, {"se_add %0,%2",0,0}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1, -1, -1 },
          { 1, {"se_add %0,%1",0,0}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 2, -1, -1, -1 }
        },
        TR_ST_NOTHING_TODO,
    },

    { "addi" /* rD,rA,imm */,
      8,
      {
          { 1, {"se_li %0,%2"}, {OP_VLE_REG,OP_GPR0_REG,OP_UI7}, OPS_ISVALID,0,0,0, -1, -1 },
          { 1, {"se_bmaski %0,%2"}, {OP_VLE_REG,OP_GPR0_REG,OP_SI16}, OPS_TRAIL1, 2, -1, -1, -1, -1 },
          { 1, {"se_bgeni %0,%2"}, {OP_VLE_REG,OP_GPR0_REG,OP_ISVALID}, OPS_EQ_SINGLEBIT,0,1,2, -1, -1 },
          { 1, {"e_li %0,%2"}, {OP_GPR_REG,OP_GPR0_REG,OP_SI16}, OPS_ISVALID,0,0,0, -1, -1 },
          { 1, {"se_addi %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_OIM5}, OPS_EQ, 0, 1, -1, -1, -1 },
          { 1, {"se_subi %0,-%2"}, {OP_VLE_REG,OP_VLE_REG,OP_NEGOIM5}, OPS_EQ,0,1,-1, -1, -1 },
          { 1, {"e_addi %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID,0,0,0, -1, -1 },
          { 1, {"e_add16i %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_ISVALID,0,0,0, -1, -1 },
      },
      TR_ST_ERROR,
    },
    { "eaddi." /* rD,rA,imm */,
      6,
      {
        { 1, {"e_add2i. %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_EQ,0,1,-1, -1, -1 },
        { 1, {"e_addi. %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID,0,0,0, -1, -1 },
        { 2, {"se_mr %0,%1","e_add2i. %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"se_mtar %0,%1","e_add2i. %0,%2"}, {OP_AGPR_REG,OP_VLE_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"se_mfar %0,%1","e_add2i. %0,%2"}, {OP_VLE_REG,OP_AGPR_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"mr %0,%1","e_add2i. %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
      },
      TR_ST_ERROR,
    },

    
    { "eaddi" /* rD,rA,imm */, /* special insn to translate addi with r0 as
                                  real source */
      4,
      {
          { 1, {"se_addi %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_OIM5}, OPS_EQ, 0, 1, -1, -1, -1 },
          { 1, {"se_subi %0,-%2"}, {OP_VLE_REG,OP_VLE_REG,OP_NEGOIM5}, OPS_EQ,0,1,-1, -1, -1 },
          { 1, {"e_addi %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID,0,0,0, -1, -1 },
          { 1, {"e_add16i %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_ISVALID,0,0,0, -1, -1 },
      },
      TR_ST_ERROR,
    },

    { "addic" /* rD,rA,SI16 */,
      2,
      {
          { 1,{"e_addic %0,%1,%2"},{OP_GPR_REG,OP_GPR_REG,OP_SCI8},OPS_ISVALID,0,0,0,-1,-1},
          { 2,{"e_li %0, %2","addc %0,%1,%0"},{OP_GPR_REG,OP_GPR_REG,OP_SI16},OPS_NEQ,0,1,0,-1,-1},
      },
      TR_ST_ERROR,
    },

    { "addic." /* rD,rA,SI16 */,
      1,
      {
          { 1,{"e_addic. %0,%1,%2"},{OP_GPR_REG,OP_GPR_REG,OP_SCI8},OPS_ISVALID,0,0,0,-1,-1},
      },
      TR_ST_ERROR,
    },
    
    { "eaddis" /* rD,rA,uimm */,
      5,
      {
          { 1, {"e_add2is %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_EQ, 0, 1, -1 ,-1,-1},
          { 2, {"se_mr %0,%1","e_add2is %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
          { 2, {"se_mtar %0,%1","e_add2is %0,%2"}, {OP_AGPR_REG,OP_VLE_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
          { 2, {"se_mfar %0,%1","e_add2is %0,%2"}, {OP_VLE_REG,OP_AGPR_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
          { 2, {"mr %0,%1","e_add2is %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
      },
      TR_ST_ERROR,
    },
    { "addis" /* rD,rA,uimm */,
      7,
      {
          { 1, {"se_bgeni %0,%1-16"}, {OP_VLE_REG,OP_GPR0_REG,OP_ISVALID}, OPS_EQ_SINGLEBIT,0,0,1, -1, -1 },
          { 1, {"e_lis %0,%2"}, {OP_GPR_REG,OP_GPR0_REG,OP_SI16}, OPS_ISVALID,0,0,0 ,-1,-1},
          { 1, {"e_add2is %0,%2"}, {OP_GPR_REG,OP_GPR1_REG,OP_SI16}, OPS_EQ, 0, 1, -1 ,-1,-1},
          { 2, {"se_mr %0,%1","e_add2is %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
          { 2, {"se_mtar %0,%1","e_add2is %0,%2"}, {OP_AGPR_REG,OP_VLE_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
          { 2, {"se_mfar %0,%1","e_add2is %0,%2"}, {OP_VLE_REG,OP_AGPR_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
          { 2, {"mr %0,%1","e_add2is %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_NEQ,0,1,-1 ,-1,-1},
      },
      TR_ST_ERROR,
    },

    { "and" /* rD,rA,rB */,
      2,
        {
          { 1, {"se_and %0,%1",0,0}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 2, -1 ,-1,-1},
          { 1, {"se_and %0,%2",0,0}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1 ,-1,-1}
        },
        TR_ST_NOTHING_TODO,
    },
    
    { "and." /* rD,rA,rB */,
        2,
        {
          { 1, {"se_and. %0,%1",0,0}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 2, -1 ,-1,-1},
          { 1, {"se_and. %0,%2",0,0}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1 ,-1,-1}
        },
        TR_ST_NOTHING_TODO,
    },
    
    { "andc" /* rD,rA,rB */,
      1,
      {
      /*{ 1, {"se_andc %0,%1",0,0}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 2, -1 ,-1,-1},*/
        { 1, {"se_andc %0,%2",0,0}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1 ,-1,-1}

      },
      TR_ST_NOTHING_TODO,
    },


    { "andi." /* rA,rA,UIMM */,
      6,
      {
        { 1, {"e_and2i. %0,%2",0,0}, {OP_GPR_REG,OP_GPR_REG,OP_UI16}, OPS_EQ, 0, 1, -1 ,-1,-1},
        { 1, {"e_andi. %0,%1,%2",0,0}, {OP_GPR_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID, 0, 0, -1 ,-1,-1},
        { 2, {"se_mr %0,%1","e_and2i. %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_UI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"se_mtar %0,%1","e_and2i. %0,%2"}, {OP_AGPR_REG,OP_VLE_REG,OP_UI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"se_mfar %0,%1","e_and2i. %0,%2"}, {OP_VLE_REG,OP_AGPR_REG,OP_UI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"mr %0,%1","e_and2i. %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_UI16}, OPS_NEQ,0,1,-1 ,-1,-1},
      },
      TR_ST_ERROR,
    },

    { "andis." /* rA,rA,UIMM */,
      5,
      {
        { 1, {"e_and2is. %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_UI16}, OPS_EQ, 0, 1,-1 ,-1,-1},
        { 2, {"se_mr %0,%1","e_and2is. %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_UI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"se_mtar %0,%1","e_and2is. %0,%2"}, {OP_AGPR_REG,OP_VLE_REG,OP_UI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"se_mfar %0,%1","e_and2is. %0,%2"}, {OP_VLE_REG,OP_AGPR_REG,OP_UI16}, OPS_NEQ,0,1,-1 ,-1,-1},
        { 2, {"mr %0,%1","e_and2is. %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_UI16}, OPS_NEQ,0,1,-1 ,-1,-1},
      },
      TR_ST_ERROR,
    },

    
    { "b" /* b label */,
      2,
      {
        { 1, { "se_b %0"}, {OP_BD8}, OPS_ISVALID, 0, 0, 0 ,-1,-1},
        { 1, { "e_b %0"}, {OP_BD24}, OPS_ISVALID, 0, 0, 0 ,-1,-1}
      },
      TR_ST_ERROR,
    },

    { "ba" /* ba label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,-1,-1}}, TR_ST_ERROR},

    { "bca" /* bca BO,BI,label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,-1,-1}}, TR_ST_ERROR},

    {"bc", /* BO,BI,label*/
     4,
     {
       {1,{"se_bc %b0,%1,%2"}, {OP_BO16,OP_CR0_BIT,OP_BD8},OPS_ISVALID, 0, 0, 0,-1,-1},
       {1,{"se_b %2"}, {OP_BO_UNCOND,OP_ISVALID,OP_BD8},OPS_ISVALID, 0, 0, 0,-1,-1},
       {1,{"e_bc %B0,%1,%2"}, {OP_BO32,OP_CR03_BIT,OP_BD15},OPS_ISVALID, 0, 0, 0,-1,-1},
       {1,{"e_b %2"}, {OP_BO_UNCOND,OP_CR03_BIT,OP_BD15},OPS_ISVALID, 0, 0, 0,-1,-1},
     },
     TR_ST_ERROR
    },

    {"bcctr",
     0,
     {{0,{""}, {OP_INVALID},OPS_INVALID, 0, 0, 0,-1,-1}},
     TR_ST_ERROR
    },

    {"bcctrl",
     0,
     {{0,{""}, {OP_INVALID},OPS_INVALID, 0, 0, 0,-1,-1}},
     TR_ST_ERROR
    },

    { "bcl" /* bcl */,
      3,
      {
        {1,{"se_bl %2"}, {OP_BO_UNCOND,OP_ISVALID,OP_BD8},OPS_ISVALID, 0, 0, 0,-1,-1},
        {1,{"e_bcl %B0,%1,%2"}, {OP_BO32,OP_CR03_BIT,OP_BD15},OPS_ISVALID, 0, 0, 0,-1,-1},
        {1,{"e_bl %2"}, {OP_BO_UNCOND,OP_CR03_BIT,OP_BD15},OPS_ISVALID, 0, 0, 0,-1,-1},
      },
      TR_ST_ERROR,
    },

    { "bcla" /* bcla BO,BI,label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    
    { "bclr" /* bclr */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0 ,-1,-1}},
      TR_ST_ERROR,
    },

    { "bclrl" /* bclr */,
      0,
      {{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0 ,-1,-1}},
      TR_ST_ERROR,
    },

    { "bctr" /* bctr */,
      1,
      {{ 1, { "se_bctr"}, {OP_ISVALID}, OPS_ISVALID, 0, 0, 0 ,-1,-1}},
      TR_ST_ERROR,
    },
    { "bctrl" /* bctrl */,
      1,
      {{ 1, { "se_bctrl"}, {OP_ISVALID}, OPS_ISVALID, 0, 0, 0,-1,-1 }},
      TR_ST_ERROR,
    },

    { "bdnz" /* bdnz label */,
      1,
      {{ 1, { "e_bc 2,0, %0"}, {OP_ISVALID}, OPS_ISVALID, 0, 0, 0,-1,-1 }},
      TR_ST_ERROR,
    },

    { "bdnza" /* bclr */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0,-1,-1 }}, TR_ST_ERROR,
    },

    { "bdnzf" /* bclr */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0,-1,-1 }}, TR_ST_ERROR,
    },

    { "bdnzfa" /* bclr */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0,-1,-1 }}, TR_ST_ERROR,
    },

    { "bdnzfl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0,-1,-1 }}, TR_ST_ERROR,
    },

    { "bdnzfla" /* bclr */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnzflr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnzflrl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnzl" /* bdnz label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnzla" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnzlr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnzlrl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnzt" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnzta" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnztl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnztla" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnztlr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdnztlrl" /* bcl */,
      0,{{ 0, { ""}, {OP_BD24}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bdz" /* bcl */,
      1,
      {{ 1, { "e_bc 3,0,%0"}, {OP_BD15}, OPS_ISVALID, 0, 0, 0 ,0, 0}},
      TR_ST_ERROR,
    },
    
    { "bdza" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzf" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzfa" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzfl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bdzfla" /* bcl */,
      0, {{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzflr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzflrl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzl" /* bcl */,
      1,
      {{ 1, { "e_bcl 3,%0,%1"}, {OP_CR03_BIT,OP_BD15}, OPS_ISVALID, 0, 0, 0, 0, 0 }},
      TR_ST_ERROR,
    },
    
    { "bdzla" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzlr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzlrl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bdzt" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdzta" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bdztl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdztla" /* bcl */,
      0,
      {{ 0, {""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bdztlrl" /* bcl */,
      0,
      {{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "beq" /* bcl */,
      2,
      {{ 1, { "se_bc 1,eq,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID, 0, 0, 0, 0, 0},
       { 1, { "e_bc 1,4*%0+eq,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID, 0, 0, 0, 0, 0 }},
      TR_ST_ERROR,
    },
    
    { "beqa" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "beqctr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "beqctrl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "beql" /* bcl */,
      2,
      {{ 1, { "se_bcl 1,eq,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bcl 1,4*%0+eq,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "beqla" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "beqlr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "beqlrl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bf" /* bl label */,
      2,
      {{ 1, { "se_bc 0,%0,%1"}, {OP_CR0_BIT,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 0,%0,%1"}, {OP_CR03_BIT,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bfa" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bfctr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bfctrl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    
    { "bfl" /* bl label */,
      1,
      {{ 1, { "e_bcl 0 %0 %1"}, {OP_CR03_BIT,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bfla" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bflr" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bflrl" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bge" /* bl label */,
      2,
      {{ 1, { "se_bc 0,lt,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 0,4*%0+lt,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bgea" /* bcl */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgectr" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgectrl" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgel" /* bl label */,
      1,
      {{ 1, { "e_bcl 0,4*%0+lt,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bgela" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgelr" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgelrl" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgt" /* bl label */,
      2,
      {{ 1, { "se_bc 1,gt,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 1,4*%0+gt,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bgta" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgtctr" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgtctrl" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgtl" /* bl label */,
      1,
      {{ 1, { "e_bcl 1,4*%0+gt,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bgtla" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgtlr" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },

    { "bgtlrl" /* bl label */,
      0,{{ 0, { ""}, {OP_INVALID}, OPS_INVALID, 0, 0, 0, 0, 0 }}, TR_ST_ERROR,
    },
    
    { "bl" /* bl label */,
      2,
      {
          { 1, { "se_bl %0"}, {OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
          { 1, { "e_bl %0"}, {OP_BD24}, OPS_ISVALID,0,0,0,0,0 }
      },
      TR_ST_ERROR,
    },

    { "bla" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "ble" /* bl label */,
      2,
      {{ 1, { "se_bc 0,gt,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 0,4*%0+gt,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "blea" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "blectr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "blectrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "blel" /* bl label */,
      1,
      {{ 1, { "e_bcl 0,4*%0+gt,%"}, {OP_CR0_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "blela" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "blelr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "blelrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    
    { "blr" /* blr */,
      1,
      {{ 1, { "se_blr"}, {}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "blrl" /* blr */,
      1,
      {{ 1, { "se_blrl"}, {}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    
    { "blt" /* blr */,
      2,
      {{ 1, { "se_bc 1,lt,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 1,4*%0+lt,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },
    
    { "blta" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bltctr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bltctrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},


    { "bltl" /* blr */,
      1,
      {{ 1, { "e_bcl 1,4*%0+lt,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bltla" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bltlr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bltlrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    
    { "bne" /* blr */,
      2,
      {{ 1, { "se_bc 0,eq,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 0,4*%0+eq,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },


    { "bnea" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnectr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnectrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bnel" /* blr */,
      1,
      {{ 1, { "e_bcl 0,4*%0+eq,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bnela" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnelr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnelrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    

    { "bng" /* blr */,
      2,
      {{ 1, { "se_bc 0,gt,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 0,4*%0+gt,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bnga" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bngctr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bngctrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    

    { "bngl" /* blr */,
      1,
      {{ 1, { "e_bcl 0,4*%0+gt,%1"}, {OP_CR03_REG,OP_ISVALID}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bngla" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bnglr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnglrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bnl" /* blr */,
      2,
      {{ 1, { "se_bc 0,lt,%1"}, {OP_CR0_REG,OP_ISVALID}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 0,4*%0+lt,%1"}, {OP_CR03_REG,OP_ISVALID}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bnla" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnlctr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnlctrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bnll" /* blr */,
      1,
      {{ 1, { "e_bcl 0,4*%0+lt,%1"}, {OP_CR03_REG,OP_ISVALID}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bnlla" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnllr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnllrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    
    { "bns" /* bns [CR,] abel */,
      2,
      {{ 1, { "se_bc 0,so,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 0,4*%0+so,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bnsa" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnsctr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnsctrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},


    { "bnsl" /* bnsl [CR,] label */,
      1,
      {{ 1, { "e_bcl 0,4*%0+so,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bnsla" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnslr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnslrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},



    { "bnu" /* blr */,
      2,
      {{ 1, { "se_bc 0,so,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 0,4*%0+so,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bnua" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnuctr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnuctrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},


    { "bnul" /* blr */,
      1,
      {{ 1, { "e_bcl 0,4*%0+so,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bnula" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnulr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bnulrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bso" /* blr */,
      2,
      {{ 1, { "se_bc 1,so,%1"}, {OP_CR0_REG,OP_BD8}, OPS_ISVALID,0,0,0,0,0 },
       { 1, { "e_bc 1,4*%0+so,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bsoa" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bsoctr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bsoctrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    
    { "bsol" /* blr */,
      1,
      {{ 1, { "e_bcl 1,4*%0+so,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bsola" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bsolr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bsolrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},


    {"bt", /* BI,label*/
     2,
     {
       {1,{"se_bc 1,%0,%1"}, {OP_CR0_BIT,OP_BD8},OPS_ISVALID,0,0,0,0,0},
       {1,{"e_bc 1,%0,%1"}, {OP_CR03_BIT,OP_BD15},OPS_ISVALID,0,0,0,0,0},
     },
     TR_ST_ERROR,
    },

    { "bta" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "btctr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "btctrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    {"btl", /* BI,label*/
     1,
     {
         {1,{"e_bcl 1,%0,%1"}, {OP_CR03_BIT,OP_BD15},OPS_ISVALID,0,0,0,0,0},
     },
     TR_ST_ERROR,
    },

    { "btla" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "btlr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "btlrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},



    {"bun", /* BI,label*/
     2,
     {
       {1,{"se_bc 1,so,%1"}, {OP_CR0_REG,OP_BD8},OPS_ISVALID,0,0,0,0,0},
       {1,{"e_bc 1,4*%0+so,%1"}, {OP_CR03_REG,OP_BD15},OPS_ISVALID,0,0,0,0,0},
     },
     TR_ST_ERROR,
    },

    { "buna" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bunctr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bunctrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "bunl" /* blr */,
      1,
      {{ 1, { "e_bcl 1,4*%0+so,%1"}, {OP_CR03_REG,OP_BD15}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "bunla" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bunlr" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "bunlrl" /* bla label */, 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    
    { "cmp" /* crD, L=0, rA, rB */,
      1,
      { { 1, {"se_cmp %2,%3"}, {OP_CR0_REG,OP_GPR0_REG,OP_VLE_REG,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },},
      TR_ST_NOTHING_TODO,
    },
    { "cmpl" /* crD, L=0,rA, rB */,
      1,
      {
          { 1, {"se_cmpl %2,%3"}, {OP_CR0_REG,OP_GPR0_REG,OP_VLE_REG,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_NOTHING_TODO,
    },
    { "cmpw" /* crD, rA, rB */,
      1,
      {
          { 1, {"se_cmp %1,%2"}, {OP_CR0_REG,OP_VLE_REG,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_NOTHING_TODO,
    },
    /* BIN-54 define mapping for cmplw */
    { "cmplw" /* crD, rA, rB */, 
      1,
      {
          { 1, {"se_cmpl %1,%2"}, {OP_CR0_REG,OP_VLE_REG,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_NOTHING_TODO,
    },
    { "cmpi" /* crD,L=0,rA, simm16 */,
      3,
      {
          { 1, {"se_cmpi %2,%3"}, {OP_CR0_REG,OP_GPR0_REG,OP_VLE_REG,OP_UI5}, OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_cmpi %0,%2,%3"}, {OP_CR03_REG,OP_GPR0_REG,OP_GPR_REG,OP_SCI8},OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_cmp16i %2,%3"}, {OP_CR0_REG,OP_GPR0_REG,OP_GPR_REG,OP_SI16}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    { "cmpwi" /* crD, rA, simm16 */,
      3,
      {
          { 1, {"se_cmpi %1,%2"}, {OP_CR0_REG,OP_VLE_REG,OP_UI5}, OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_cmpi %0,%1,%2"}, {OP_CR03_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_cmp16i %1,%2"}, {OP_CR0_REG,OP_GPR_REG,OP_SI16}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    { "cmpli" /* crD,L=0,rA, uimm16 */,
      3,
      {
          { 1, {"se_cmpli %2,%3"}, {OP_CR0_REG,OP_GPR0_REG,OP_VLE_REG,OP_OIM5}, OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_cmpli %0,%2,%3"},{OP_CR03_REG,OP_GPR0_REG,OP_GPR_REG,OP_SCI8},OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_cmpl16i %2,%3"}, {OP_CR0_REG,OP_GPR0_REG,OP_GPR_REG,OP_UI16}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    { "cmplwi" /* crD, rA, uimm16 */,
      3,
      {
          { 1, {"se_cmpli %1,%2"}, {OP_CR0_REG,OP_VLE_REG,OP_OIM5}, OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_cmpli %0,%1,%2"}, {OP_CR03_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_cmpl16i %1,%2"}, {OP_CR0_REG,OP_GPR_REG,OP_UI16}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    { "crand" /* crD, crA, crB */,
      1,
      { { 1, {"e_crand %0,%1,%2"}, {OP_CR_BIT,OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crandc" /* crD, crA, crB */,
      1,
      { { 1, {"e_crandc %0,%1,%2"}, {OP_CR_BIT,OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crclr" /* crD */,
      1,
      { { 1, {"e_crxor %0,%0,%0"}, {OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "creqv" /* crD,crA,crB */,
      1,
      { { 1, {"e_creqv %0,%1,%2"}, {OP_CR_BIT,OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crnand" /* crD, crA, crB */,
      1,
      { { 1, {"e_crnand %0,%1,%2"}, {OP_CR_BIT,OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crmove" /* crD, crA */,
      1,
      { { 1, {"e_cror %0,%1,%1"}, {OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crnor" /* crD, crA, crB */,
      1,
      { { 1, {"e_crnor %0,%1,%2"}, {OP_CR_BIT,OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crnot" /* crD, crA */,
      1,
      { { 1, {"e_crnor %0,%1,%1"}, {OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "cror" /* crD,crA,crB */,
      1,
      { { 1, {"e_cror %0,%1,%2"}, {OP_CR_BIT,OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crorc" /* crD,crA,crB */,
      1,
      { { 1, {"e_crorc %0,%1,%2"}, {OP_CR_BIT,OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crset" /* crD */,
      1,
      { { 1, {"e_creqv %0,%0,%0"}, {OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "crxor" /* crD,crA,crB */,
      1,
      { { 1, {"e_crxor %0,%1,%2"}, {OP_CR_BIT,OP_CR_BIT,OP_CR_BIT}, OPS_ISVALID,0,0,0,0,0 }, },
      TR_ST_ERROR,
    },
    { "extsb" /* rD,rA */,
        1,
        {{ 1, {"se_extsb %0",0,0}, {OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1, -1, -1 }},
        TR_ST_NOTHING_TODO,
    },
    { "extsh" /* rD,rA */,
        1,
        {{ 1, {"se_extsh %0",0,0}, {OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1, -1, -1 }},
        TR_ST_NOTHING_TODO,
    },


    { "fabs", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fabs.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fadd", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fadd.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fadds", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fadds.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fcmpo", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fcmpu", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fctiw", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fctiw.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fctiwz", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fctiwz.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fdiv", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fdiv.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fdivs", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fdivs.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmadd", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmadd.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmadds", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmadds.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmr", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmr.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmsub", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmsub.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmsubs", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmsubs.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmul", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmul.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmuls", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fmuls.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnabs", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnabs.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fneg", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fneg.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnmadd", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnmadd.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnmadds", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnmadds.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnmsub", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnmsub.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnmsubs", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fnmsubs.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fres", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fres.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "frsp", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "frsp.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fsel", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fsel.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fsub", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fsub.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fsubs", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "fsubs.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    
    { "inseleq", // RD, RA, RB
      1,
      {{ 1, {"isel %0,%1,%2,2"},{OP_GPR_REG,OP_GPR_REG,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "inselgt", // RD, RA, RB
      1,
      {{ 1, {"isel %0,%1,%2,1"},{OP_GPR_REG,OP_GPR_REG,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "insellt", // RD, RA, RB
      1,
      {{ 1, {"isel %0,%1,%2,0"},{OP_GPR_REG,OP_GPR_REG,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    
    { "isync" /*  */,
        1,
      {{ 1, {"se_isync",0,0}, {}, OPS_ISVALID,0,0,0,0,0 }},
      TR_ST_ERROR,
    },

    { "la" /* rD,D(rA) */,
      4,
      {
        { 1, {"se_li %0,%1"},    {OP_VLE_REG,OP_UI7,OP_GPR0_REG}, OPS_ISVALID,0,0,0,0,0 },
        { 1, {"se_addi %0,%1"},  {OP_VLE_REG,OP_OIM5,OP_VLE_REG}, OPS_EQ, 0, 2, -1, -1, -1 },
        { 1, {"se_subi %0,-%1"}, {OP_VLE_REG,OP_NEGOIM5,OP_VLE_REG}, OPS_EQ,0,2,-1, -1, -1 },
        { 1, {"e_add16i %0,%2,%1"}, {OP_GPR_REG,OP_SI16,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "lbz" /* rD,D(rA) */,
      2,
      {
        { 1, {"se_lbz %0,%1(%2)"}, {OP_VLE_REG,OP_SD4,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
        { 1, {"e_lbz %0,%1(%2)"}, {OP_GPR_REG,OP_SI16,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    { "lbzu" /* rD,D(rA) */,
      1,
      {
        { 1, {"e_lbzu %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_NEQ, 0, 2, -1, -1, -1 },
      },
      TR_ST_ERROR,
    },

    { "lfd" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "lfdu" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "lfdux" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "lfdx" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "lfs" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "lfsu" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "lfu" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "lfsux" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "lfsx" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "lha" /* rD,D(rA) */,
      1,
      {
        { 1, {"e_lha %0,%1(%2)"}, {OP_GPR_REG,OP_SI16,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    { "lhau" /* rD,D(rA) */,
      1,
      {
        { 1, {"e_lhau %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "lhz" /* rD,D(rA) */,
      2,
      {
        { 1, {"se_lhz %0,%1(%2)"}, {OP_VLE_REG,OP_SD41,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
        { 1, {"e_lhz %0,%1(%2)"}, {OP_GPR_REG,OP_SI16,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    { "lhzu" /* rD,D(rA) */,
      1,
      {
          { 1, {"e_lhzu %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    
    {"li" /* rD,imm16 */, 
     4,
     {
       { 1, {"se_li %0,%1"}, {OP_VLE_REG,OP_UI7}, OPS_ISVALID,0,0,0,0,0 },
       { 1, {"se_bmaski %0,%1"}, {OP_VLE_REG,OP_SI16}, OPS_TRAIL1, 1, -1, -1, -1, -1 },
       { 1, {"se_bgeni %0,%1"}, {OP_VLE_REG,OP_ISVALID}, OPS_EQ_SINGLEBIT,0,0,1, -1, -1 },
       { 1, {"e_li %0,%1"}, {OP_GPR_REG,OP_LI20}, OPS_ISVALID,0,0,0,0,0 },
     },
     TR_ST_ERROR,
    },


    {"lis" /* rD,imm16 */, 
     2,
     {
       { 1, {"se_bgeni %0,%1-16"}, {OP_VLE_REG,OP_ISVALID}, OPS_EQ_SINGLEBIT,0,0,1, -1, -1 },
       { 1, {"e_lis %0,%1"}, {OP_GPR_REG,OP_SI16}, OPS_ISVALID,0,0,0,0,0 },
     },
     TR_ST_ERROR,
    },
    
    {"lmw" /* rD,D8(rA) */, 
      1,
      {
        { 1, {"e_lmw %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_OP1_LT_OP2, 2, 0, -1, -1, -1 },
      },
      TR_ST_ERROR,
    },
    
    { "lwz" /* rD,D(rA) */,
      2,
      {
          { 1, {"se_lwz %0,%1(%2)"}, {OP_VLE_REG,OP_SD42,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_lwz %0,%1(%2)"}, {OP_GPR_REG,OP_SI16,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    
    { "lwzu" /* rD,D(rA) */,
      1,
      {
          { 1, {"e_lwzu %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "mcrf" /* rD,D(rA) */,
      1,
      {
          { 1, {"e_mcrf %0,%1"}, {OP_ISVALID,OP_ISVALID}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "mcrfs" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    
    { "mfctr" /* rA */,
        1,
        {
          { 1, {"se_mfctr %0"}, {OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_NOTHING_TODO
    },


    { "mffs" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "mffs." , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    
    { "mflr" /* rA */,
      1,
      {
          { 1, {"se_mflr %0"}, {OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_NOTHING_TODO
    },
    
    { "mftb",
      1,
      {
          { 1, {"mfspr %0,268"}, {OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    
    { "mftbu",
      1,
      {
          { 1, {"mfspr %0,269"}, {OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    
    
    { "mr" /* rA ,rB */,
        1,
        {
          { 1, {"se_mr %0,%1"}, {OP_VLE_REG,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_NOTHING_TODO,
    },

    { "mtcr",
      1,
      {
        { 1, {"mtcrf 0xff,%0"}, {OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "mtctr" /* rA */,
      1,
      {
        { 1, {"se_mtctr %0"}, {OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_NOTHING_TODO,
    },


    { "mtfsb0" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "mtfsb0.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "mtfsb1" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "mtfsb1.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "mtfsbf" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "mtfsbf.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "mtfsbfi" , 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "mtfsbfi.", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},


    { "mtlr" /* rA */,
        1,
        {
          { 1, {"se_mtlr %0"}, {OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_NOTHING_TODO,
    },
        
    { "mulli" /*RD,RA,SIMM16*/ ,
        6,
        {
            { 1, {"e_mull2i %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_EQ, 0, 1, -1, -1, -1},
            { 1, {"e_mulli %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI8}, OPS_EQ, 0, 1, -1 , -1, -1},
            { 2, {"se_mr %0,%1","e_mull2i %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_SI16},OPS_NEQ, 0, 1, -1, -1, -1},
            { 2, {"se_mtar %0,%1","e_mull2i %0,%2"}, {OP_AGPR_REG,OP_VLE_REG,OP_SI16},OPS_NEQ, 0, 1, -1 , -1, -1},
            { 2, {"se_mfar %0,%1","e_mull2i %0,%2"}, {OP_VLE_REG,OP_AGPR_REG,OP_SI16},OPS_NEQ, 0, 1, -1 , -1, -1},
            { 2, {"mr %0,%1","e_mull2i %0,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16},OPS_NEQ, 0, 1, -1 , -1, -1},
      },
      TR_ST_ERROR,
    },

    { "mullw" /*RD,RA,RB*/ ,
        2,
        {
          { 1, {"se_mullw %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1, -1, -1},
          { 1, {"se_mullw %0,%1"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 2, -1, -1, -1},
        },
      TR_ST_NOTHING_TODO,
    },
    
    { "neg" /*RD,RA*/ ,
      1,
      {
        { 1, {"se_neg %0"}, {OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, 0, -1, -1},
      },
      TR_ST_NOTHING_TODO,
    },

    {"nop",
     1,{{1,{"se_or 0,0"},{},OPS_ISVALID,0,0,0,0,0}},TR_ST_ERROR},

    {"not", /* RA RS */
     2,{
        {1,{"se_not %0"},{OP_VLE_REG,OP_VLE_REG},OPS_EQ,0,1,-1, -1, -1},
        {1,{"nor %0,%1,%1"},{OP_GPR_REG,OP_GPR_REG,OP_GPR_REG},OPS_ISVALID,0,0,0,0,0}
      }
     ,TR_ST_ERROR
    },
 
    {"or", // RA, RS, RB
     2,
     {
       {1,{"se_or %0,%2"},{OP_VLE_REG,OP_VLE_REG,OP_VLE_REG},OPS_EQ,0,1,-1, -1, -1},
       {1,{"se_or %0,%1"},{OP_VLE_REG,OP_VLE_REG,OP_VLE_REG},OPS_EQ,0,2,-1, -1, -1}
     },
     TR_ST_NOTHING_TODO     
    },

    {"ori", // RA, RS, UIMM16
     7,
     {
       {1,{"se_bseti %0,%2"},{OP_VLE_REG,OP_VLE_REG,OP_ISVALID},OPS_EQ_SINGLEBIT,0,1,2, -1, -1},
       {1,{"e_ori %0,%1,%2"},{OP_GPR_REG,OP_GPR_REG,OP_SCI8},OPS_ISVALID,0,0,0,0,0},
       {1,{"e_or2i %0,%2"},{OP_GPR_REG,OP_GPR_REG,OP_ISVALID},OPS_EQ,0,1,-1, -1, -1},
       {2,{"se_mr %0,%1","e_or2i %0,%2"},{OP_VLE_REG,OP_VLE_REG,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
       {2,{"se_mfar %0,%1","e_or2i %0,%2"},{OP_VLE_REG,OP_AGPR_REG,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
       {2,{"se_mtar %0,%1","e_or2i %0,%2"},{OP_AGPR_REG,OP_VLE_REG,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
       {2,{"mr %0,%1","e_or2i %0,%2"},{OP_GPR_REG,OP_GPR_REG,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
     },
     TR_ST_ERROR
    },

    {"oris", // RA, RS, UIMM16
     6,
     {
       {1,{"se_bseti %0,%2-16"},{OP_VLE_REG,OP_VLE_REG,OP_ISVALID},OPS_EQ_SINGLEBIT,0,1,2, -1, -1},
       {1,{"e_or2is %0,%2"},{OP_GPR_REG,OP_GPR_REG,OP_ISVALID},OPS_EQ,0,1,-1, -1, -1},
       {2,{"se_mr %0,%1","e_or2is %0,%2"},{OP_VLE_REG,OP_VLE_REG,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
       {2,{"se_mfar %0,%1","e_or2is %0,%2"},{OP_VLE_REG,OP_AGPR_REG,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
       {2,{"se_mtar %0,%1","e_or2is %0,%2"},{OP_AGPR_REG,OP_VLE_REG,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
       {2,{"mr %0,%1","e_or2is %0,%2"},{OP_GPR_REG,OP_GPR_REG,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
     },
     TR_ST_ERROR
    },
    

    {"rfci",
     1,
     {
       {1,{"se_rfci"},{ },OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR
    },

    { "rfdi",
      1,
      {
        {1, {"se_rfdi"},{},OPS_ISVALID,0,0,0,0,0}},
      TR_ST_ERROR
    },

    {"rfi",
     1,
     {
       {1,{"se_rfi"},{ },OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR
    },

    {"rfmci",
     1,
     {
       {1,{"se_rfmci"},{ },OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR
    },



    
    {"rlwimi", // RA, RS, SH, MB, ME
     1,
     {
       {1,{"e_rlwimi %0,%1,%2,%3,%4"},{OP_ISVALID,OP_ISVALID,OP_ISVALID,OP_ISVALID,OP_ISVALID },OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR
    },

    { "rlwimi.", // RA, RS, SH, MB, ME
     2, 
     {
         {2, {"e_rlwimi %0,%1,%2,%3,%4","se_cmpi %0,0"},
             {OP_VLE_REG,OP_GPR_REG,OP_ISVALID,OP_ISVALID,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
         {2, {"e_rlwimi %0,%1,%2,%3,%4","e_cmp16i %0,0"},
             {OP_GPR_REG,OP_GPR_REG,OP_ISVALID,OP_ISVALID,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
     },
     TR_ST_ERROR },

    {"rlwinm", // RA, RS, SH, MB, ME
     2,
     {
       {1,{"se_bclri %0,%2"},{OP_VLE_REG,OP_VLE_REG,OP_ISVALID,OP_ISVALID,OP_ISVALID},OPS_EQ_SINGLE_NOTMASK,0,1,2,3,4},
       {1,{"e_rlwinm %0,%1,%2,%3,%4"},{OP_ISVALID,OP_ISVALID,OP_ISVALID,OP_ISVALID,OP_ISVALID },OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR
    },

    { "rlwinm.", // RA, RS, SH, MB, ME
     2, 
     {
         {2, {"e_rlwinm %0,%1,%2,%3,%4","se_cmpi %0,0"},
             {OP_VLE_REG,OP_GPR_REG,OP_ISVALID,OP_ISVALID,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
         {2, {"e_rlwinm %0,%1,%2,%3,%4","e_cmp16i %0,0"},
             {OP_GPR_REG,OP_GPR_REG,OP_ISVALID,OP_ISVALID,OP_ISVALID},OPS_ISVALID,0,0,0,0,0},
     },
     TR_ST_ERROR },


    {"rlwnm", // RA, RS, rsh, MB, ME
     1,
     {
       {2,{"e_rlw %0,%1,%2","e_rlwinm %0,%0,0,%3,%4"}
        ,{OP_GPR_REG,OP_GPR_REG,OP_GPR_REG,OP_UI5,OP_UI5 } ,OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR
    },

    { "rlwnm.", // RA, RS, rsh, MB, ME
     2, 
     {
       {3,{"e_rlw %0,%1,%2","e_rlwinm %0,%0,0,%3,%4","se_cmpi %0,0"}
        ,{OP_VLE_REG,OP_GPR_REG,OP_GPR_REG,OP_UI5,OP_UI5 } ,OPS_ISVALID,0,0,0,0,0},
       {3,{"e_rlw %0,%1,%2","e_rlwinm %0,%0,0,%3,%4","e_cmp16i %0,0"}
        ,{OP_GPR_REG,OP_GPR_REG,OP_GPR_REG,OP_UI5,OP_UI5 } ,OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR },

    {"rotlw", // RA, RS, SH,
    1,
     {
       {2,{"e_rlw %0,%1,%2","e_rlwinm %0,%0,0,0,31"}
        ,{OP_GPR_REG,OP_GPR_REG,OP_GPR_REG }
        ,OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR
    },
    {"rotlwi", // RA, RS, SH,
    1,
     {
       {1,{"e_rlwinm %0,%1,%2,0,31"} ,{OP_GPR_REG,OP_GPR_REG,OP_GPR_REG } ,OPS_ISVALID,0,0,0,0,0}
     },
     TR_ST_ERROR
    },

    { "sc" /* */,
      1,
      {
          { 1, {"se_sc"}, {OP_ISVALID}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    
    { "slw" /* rD,rA,rB */,
      1,
      {
        { 1, {"se_slw %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1, -1, -1 },
      },
      TR_ST_NOTHING_TODO,
    },
    { "slwi" /* rD,rA,imm5 */,
      2,
      {
          { 1, {"se_slwi %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_UI5}, OPS_EQ, 0, 1, -1, -1, -1 },
          { 1, {"e_slwi %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_UI5}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "slwi." /* rD,rA,imm5 */,
      1,
      {
          { 1, {"e_slwi. %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_UI5}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "sraw", // RA, RS, RB 
      1,
      {
        { 1, {"se_sraw %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1, -1, -1 },
      },
      TR_ST_NOTHING_TODO,
    },

    { "srawi", // RA, RS, UI5
      1,
      {
        { 1, {"se_srawi %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_UI5}, OPS_EQ, 0, 1, -1, -1, -1 },
      },
      TR_ST_NOTHING_TODO,
    },

    { "srw" /* rD,rA,rB */,
      1,
      {
        { 1, {"se_srw %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1, -1, -1 },
      },
      TR_ST_NOTHING_TODO,
    },
    
    { "srwi" /* rD,rA,imm5 */,
      2,
      {
        { 1, {"se_srwi %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_UI5}, OPS_EQ, 0, 1, -1, -1, -1 },
        { 1, {"e_srwi %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_UI5}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    
    { "srwi." /* rD,rA,imm5 */,
      1,
      {
        { 1, {"e_srwi. %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_UI5}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "stb" /* rD,D(rA) */,
      2,
      {
          { 1, {"se_stb %0,%1(%2)"}, {OP_VLE_REG,OP_SD4,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
          { 1, {"e_stb %0,%1(%2)"}, {OP_GPR_REG,OP_SI16,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "stbu" /* rD,D(rA) */,
      1,
      {
          { 1, {"e_stbu %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "stfd", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "stfdu", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "stfdux", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "stfdx", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "stfiwx", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "stfs", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "stfsu", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "stfsux", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "stfsx", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    { "sth" /* rD,D(rA) */,
      2,
      {
        { 1, {"se_sth %0,%1(%2)"}, {OP_VLE_REG,OP_SD41,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
        { 1, {"e_sth %0,%1(%2)"}, {OP_GPR_REG,OP_SI16,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "sthu" /* rD,D(rA) */,
      1,
      {
          { 1, {"e_sthu %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "stmw", // rS,D(rA)
      1,
      {
          { 1, {"e_stmw %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },


    { "stw" /* rD,D(rA) */,
      2,
      {
        { 1, {"se_stw %0,%1(%2)"}, {OP_VLE_REG,OP_SD42,OP_VLE_REG}, OPS_ISVALID,0,0,0,0,0 },
        { 1, {"e_stw %0,%1(%2)"}, {OP_GPR_REG,OP_SI16,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    
    { "stwu" /* rD,D(rA) */,
      1,
      {
        { 1, {"e_stwu %0,%1(%2)"}, {OP_GPR_REG,OP_SI8,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },


    { "sub", // RD, RA RB
      3,
      {
        { 1, {"se_sub %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1, -1, -1 },
        { 1, {"se_subf %0,%1"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 2, -1, -1, -1 },
        { 1, {"subf %0,%2,%1"}, {OP_GPR_REG,OP_GPR_REG,OP_GPR_REG}, OPS_ISVALID,0,0,0,0,0 },        
      },
      TR_ST_NOTHING_TODO,
    },

    { "subc", // RD, RA RB
      1,
      {
        { 1, {"subfc %0,%2,%1"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1 , -1, -1},
      },
      TR_ST_NOTHING_TODO,
    },
        
    { "subf", // RD, RA RB
      2,
      {
        { 1, {"se_subf %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 1, -1 , -1, -1},
        { 1, {"se_sub %0,%1"}, {OP_VLE_REG,OP_VLE_REG,OP_VLE_REG}, OPS_EQ, 0, 2, -1, -1, -1 },
      },
      TR_ST_NOTHING_TODO,
    },
    
    { "subfic" /* rD,rA,imm16 */,
      1,
      {
        { 1, {"e_subfic %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    
    { "subfic." /* rD,rA,imm16 */,
      1,
      {
          { 1, {"e_subfic. %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },
    
    { "subi", // RD, RA, imm
      4,
      {
        { 1, {"se_li %0,-%2"}, {OP_VLE_REG,OP_GPR0_REG,OP_OIM5}, OPS_ISVALID,0,0,0,0,0 },
        { 1, {"se_subi %0,%2"}, {OP_VLE_REG,OP_VLE_REG,OP_OIM5}, OPS_EQ, 0, 1, -1, -1, -1 },
        { 1, {"e_li %0,-%2"}, {OP_GPR_REG,OP_GPR0_REG,OP_SI16}, OPS_ISVALID,0,0,0,0,0 },
        { 1, {"e_add16i %0,%1,-%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_ISVALID,0,0,0,0,0 },
      },
      TR_ST_ERROR,
    },

    { "subic", // RD, RA, imm
      1,
      {
        { 1,{"e_addic %0,%1,-%2"},{OP_GPR_REG,OP_GPR_REG,OP_SCI8},OPS_ISVALID,0,0,0,0,0},
      },
      TR_ST_ERROR,
    },

    { "subic.", // RD, RA, imm
      1,
      {
        { 1,{"e_addic. %0,%1,-%2"},{OP_GPR_REG,OP_GPR_REG,OP_SCI8},OPS_ISVALID,0,0,0,0,0},
      },
      TR_ST_ERROR,
    },

    { "subis", // RD, RA, imm
      6,
      {
        { 1, {"e_lis %0,%2"}, {OP_GPR_REG,OP_GPR0_REG,OP_SI16}, OPS_ISVALID,0,0,0,0,0 },
        { 1, {"e_add2is %0,-%2"}, {OP_GPR_REG,OP_GPR1_REG,OP_SI16}, OPS_EQ, 0, 1, -1,-1,-1 },
        { 2, {"se_mr %0,%1","e_add2is %0,-%2"}, {OP_VLE_REG,OP_VLE_REG,OP_SI16}, OPS_NEQ,0,1,-1,-1,-1 },
        { 2, {"se_mtar %0,%1","e_add2is %0,-%2"}, {OP_AGPR_REG,OP_VLE_REG,OP_SI16}, OPS_NEQ,0,1,-1,-1,-1 },
        { 2, {"se_mfar %0,%1","e_add2is %0,-%2"}, {OP_VLE_REG,OP_AGPR_REG,OP_SI16}, OPS_NEQ,0,1,-1,-1,-1 },
        { 2, {"mr %0,%1","e_add2is %0,-%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SI16}, OPS_NEQ,0,1,-1,-1,-1 },
      },
      TR_ST_ERROR,
    },
    
    { "tweqi", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},    
    { "twgei", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twgti", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twi", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twlei", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twlti", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twlgei", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twlgti", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twllei", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twllti", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twlngi", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twlnli", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twnei", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twngi", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},
    { "twnli", 0, {{0, {""},{OP_INVALID},OPS_INVALID,0,0,0,0,0}}, TR_ST_ERROR},

    
    { "xori", // RA, RS, UIMM16
      1,
      {
        { 1, {"e_xori %0,%1,%2"}, {OP_GPR_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID, 0, 0, 0,0,0 },
      },
      TR_ST_ERROR,
    },
    { "xoris", // rA,rS,UIMM16
      2,
      {
        { 1, {"e_xori %0,%1,%2<<16"}, {OP_GPR_REG,OP_GPR_REG,OP_SCI8}, OPS_ISVALID, 0, 0, 0,0,0 },
        { 2, {"e_xori %0,%1, (%2 & 0xff)<<16", "e_xori %0,%0, (%2 & 0xff00)<<16"}, 
                {OP_GPR_REG,OP_GPR_REG,OP_UI16}, OPS_ISVALID, 0, 0, 0,0,0 },
      },
      TR_ST_ERROR,
    }

};

const int  ppc_vle_num_macros = 
  sizeof (ppc_vle_macro_table) / sizeof (ppc_vle_macro_table[0]);

/* validate an SCI8 operand 
   Format used by some 32-bit arithmetic, compare, and logical instructions. 
   The UI8 field is an 8-bit immediate value shifted left 0, 1, 2, or 3 byte 
   positions according to the value of the SCL field. The remaining bits in the 32-bit 
   word are filled with the value of the F field, and the resulting 32-bit value is used 
   as one operand of the instruction.
   More formally,
           if SCL=0 then imm_value ← 24F || UI8 else
           if SCL=1 then imm_value ← 16F || UI8 || 8F else
           if SCL=2 then imm_value ← 8F || UI8 || 16F
           else imm_value ← UI8 || 24F
*/
static int is_val_sci8 (long val)
{
  unsigned value = val & 0xffffffff;

  if (((value & ~0xff) == 0) || ((value & ~0xff) == 0xffffff00))
    {
      return 1;
    }
  else if (((value & ~0xff00) == 0) || ((value & ~0xff00) == 0xffff00ff))
    {
      return 1;
    }
  else if (((value & ~0xff0000) == 0) || ((value & ~0xff0000) == 0xff00ffff))
    {
      return 1;
    }
  else if (((value & ~0xff000000) == 0) || ((value & ~0xff000000) == 0x00ffffff))
    {
      return 1;
    }
  else
    {
        return 0;
    }
  return 1;
}

/*
   valid an signed operand according to mask
*/
static int
is_val_signed(long val, unsigned int mask)
{
  long min, max, right;

  max = mask;
  right = max & -max;
  min =  ~max & -right;
    
  if (min <= max)
    {
      /* Some people write constants with the sign extension done by
         hand but only up to 32 bits.  This shouldn't really be valid,
         but, to permit this code to assemble on a 64-bit host, we
         sign extend the 32-bit value to 64 bits if so doing makes the
         value valid.  */
      if (val > max
          && (offsetT) (val - 0x80000000 - 0x80000000) >= min
          && (offsetT) (val - 0x80000000 - 0x80000000) <= max
          && ((val - 0x80000000 - 0x80000000) & (right - 1)) == 0)
        val = val - 0x80000000 - 0x80000000;

      /* Similarly, people write expressions like ~(1<<15), and expect
         this to be OK for a 32-bit unsigned value.  */
      else if (val < min
               && (offsetT) (val + 0x80000000 + 0x80000000) >= min
               && (offsetT) (val + 0x80000000 + 0x80000000) <= max
               && ((val + 0x80000000 + 0x80000000) & (right - 1)) == 0)
        val = val + 0x80000000 + 0x80000000;

      else if (val < min
               || val > max
               || (val & (right - 1)) != 0)
        return 0;
    }
  return 1;
}


static bfd_boolean is_trailing_1(ppcvle_operand * op)
{  
  long retv1,retv2;
  
  if(op->type != TR_OT_IMM)
    return FALSE;

  if(op->value == -1)
    {
      op->value = 0;
    }
  else
    {
      if(!tf_set_mb_me(&retv1,&retv2, op->value))
        return FALSE;
      if(retv2 != 31)
        return FALSE;
      op->value = retv2 + 1 - retv1;
    }

  op->descriptor = NULL;
  return TRUE;
}


static int is_single_bit(unsigned long val,bfd_boolean set)
{
  int cnt = 0;

  if(!set)
    {
      val = ~val;
    }

  for(cnt = 0;cnt<32;cnt++)
    {
      if((val & 0x1) == 0x1)
        {
          if(val>>1 != 0)
            {
              return -1;
            }
          else
            {
              return cnt;
            }
        }
      val = val >> 1;
    }
  
  return -1;
}

bfd_boolean
ppcvle_is_op_valid(operand_constraint_t constraint, ppcvle_operand *op)
{
  TRANSL_OP_TYPE type;

  if (!op)
    return FALSE;
  type = op->type;

  switch (constraint)
    {
    case OP_INVALID:
      return FALSE;
    case OP_VLE_REG:
      return ((type == TR_OT_IMM) || (type == TR_OT_REG)) && IS_VLE_REG(op->value);
    case OP_GPR_REG:
      return ((type == TR_OT_IMM) || (type == TR_OT_REG)) && IS_GPR_REG(op->value);
    case OP_AGPR_REG:
      return ((type == TR_OT_IMM) || (type == TR_OT_REG)) && IS_AGPR_REG(op->value);
    case OP_GPR1_REG:
      return ((type == TR_OT_IMM) || (type == TR_OT_REG)) && IS_GPR1_REG(op->value);
    case OP_GPR0_REG:
      return ((type == TR_OT_IMM) || (type == TR_OT_REG)) && IS_GPR0_REG(op->value);
    case OP_CR_REG:
      return ((type == TR_OT_IMM) || (type == TR_OT_REG)) && IS_CR_REG(op->value);
    case OP_CR03_REG:
      return ((type == TR_OT_IMM) || (type == TR_OT_REG)) && IS_CR03_REG(op->value);
    case OP_CR0_REG:
      return ((type == TR_OT_IMM) || (type == TR_OT_REG)) && IS_CR0_REG(op->value);
    case OP_CR_BIT:
      return (type == TR_OT_IMM) && IS_CR_BIT(op->value);
    case OP_CR03_BIT:
      return (type == TR_OT_IMM) && IS_CR03_BIT(op->value);
    case OP_CR0_BIT:
      return (type == TR_OT_IMM) && IS_CR0_BIT(op->value);
    case OP_BO16:
      if (type != TR_OT_IMM)
        return FALSE;
      if((op->value & 0x1c) == 4) return TRUE;
      else if((op->value & 0x1c) == 0xc) return TRUE;
      return FALSE;
    case OP_BO32:
      if ((type != TR_OT_IMM) && (type != TR_OT_REG))
        return FALSE;
      if((op->value & 0x1c) == 4) return TRUE;
      else if((op->value & 0x1c) == 0xc) return TRUE;
      else if( (op->value & 0x16)== 0x10) return TRUE;
      else if( (op->value & 0x16)== 0x12) return TRUE;
      return FALSE;
    case OP_BO_UNCOND:
      if ((type != TR_OT_IMM) && (type != TR_OT_REG))
        return FALSE;
      return (op->value & 0x14) == 0x14;
    case OP_BD15:
      return ((type = TR_OT_SYM) || IS_BD15(op->value));
    case OP_BD24:
      return ((type == TR_OT_SYM) || IS_BD24(op->value));
    case OP_BD8:
      return ((type == TR_OT_SYM) || IS_BD8(op->value));
    case OP_SI16:
      return ((type == TR_OT_SYM) || ((type == TR_OT_IMM) && IS_SI16(op->value)));
    case OP_SI8:
      return (type == TR_OT_IMM) && IS_SI8(op->value);
    case OP_SCI8:
      return (type == TR_OT_IMM) && IS_SCI8(op->value);
    case OP_LI20:
      return ((type == TR_OT_SYM) || ((type == TR_OT_IMM) && IS_LI20(op->value)));
    case OP_OIM5:
      return (type == TR_OT_IMM) && IS_OIM5(op->value);
    case OP_NEGOIM5:
      return (type == TR_OT_IMM) && IS_NEGOIM5(op->value);
    case OP_SD4:
      return (type == TR_OT_IMM) && IS_SD4(op->value);
    case OP_SD41:
      return (type == TR_OT_IMM) && IS_SD41(op->value);
    case OP_SD42:
      return (type == TR_OT_IMM) && IS_SD42(op->value);
    case OP_UI5:
      return (type == TR_OT_IMM) && IS_UI5(op->value);
    case OP_UI7:
      return (type == TR_OT_IMM) && IS_UI7(op->value);
    case OP_UI16:
      return ((op->type == TR_OT_SYM) || ((type == TR_OT_IMM) && IS_UI16(op->value)));
    case OP_ISVALID:
      return TRUE;
    default:
  as_bad(_("invalid operand constraint %d"),constraint);
      return FALSE;
    }
  return FALSE;
}

/* test additonal insn constraints
   return 
        - 0 if the constraint is valid
        - 1 if the constraint is valid for operand 1
        - 2 if the constraint is valid for operand 2
*/
bfd_boolean
ppcvle_is_op_list_valid(insn_constraint_t constraint,
                 ppcvle_operand *op0,
                 ppcvle_operand *op1,
                 ppcvle_operand *op2,
                 ppcvle_operand *op3,
                 ppcvle_operand *op4 )
{
  if (!op0)
    return FALSE;
  switch (constraint)
    {
    case OPS_ISVALID:
      return TRUE;
    case OPS_EQ:
      if (op1 && (op0->type == op1->type) && (op0->value == op1->value))
        return TRUE;
      return FALSE;
    case OPS_NEQ:
      if (op1 && ((op0->type != op1->type) || (op0->value != op1->value)))
        return TRUE;
      return FALSE;
    case OPS_OP1_LT_OP2:
      if (op1 && (op0->type == op1->type) && (op0->value < op1->value))
        return TRUE;
      return FALSE;
    case OPS_TRAIL1:
      return IS_TRAIL1(op0);
    case OPS_EQ_SINGLEBIT:
      if (op1 && (op0->type == op1->type) && (op0->value == op1->value)
          && op2 && (op2->type == TR_OT_IMM))
        {
          int pos = IS_SINGLEBIT(op2->value);
          if(pos > -1)
            {
              op2->value = 31 - pos;
              op2->descriptor = NULL;
              return TRUE;
            }
        }
      return FALSE;
    case OPS_EQ_SINGLE_NOTMASK:
      // clear a single bit parameter of rlwinm r0, r1, SH, MB, ME 
      if (op1 && (op0->type == op1->type) && (op0->value == op1->value)
          && op2 && (op2->type == TR_OT_IMM))
        {
          int pos = op3->value;
          if (pos == 0) pos = 32;
          if ((op2->value == 0) && ((pos - op4->value) == 2))
            {
              op2->value = pos - 1;
              op2->descriptor = NULL;
              return TRUE;
            }
        }
      return FALSE;
    case OPS_INVALID:
      return FALSE;
    default:
  as_bad(_("illegal insn constraint %d"),constraint);
      return FALSE;
    }
  return FALSE;
}


