/* Copyright (C) 2010-2014 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License
   and a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

/* Function prototypes that cannot exist in tricore.h due to dependency
   complications.  */

#ifndef GCC_TRICORE_PROTOS_H
#define GCC_TRICORE_PROTOS_H

#if defined (DOUBLE_INT_H)
unsigned tric_double_int_pop_digit (double_int*, unsigned);
#endif /* Have double_int.h */

extern void tric_init_expanders (void);
extern enum reg_class tric_regno_reg_class (int r);

extern void tric_output_reglist (FILE*, unsigned int);

extern int tric_frame_pointer_required (void);
extern HOST_WIDE_INT tric_initial_elimination_offset (int, int);

extern void tric_cpu_cpp_builtins (struct cpp_reader*);

extern int ones_mask (int ui, int * width);
extern int ones_width (int ui);

extern void tric_emit_prologue (void);
extern void tric_emit_epilogue (int);
extern int tric_hard_regno_rename_ok (unsigned int, unsigned int);

extern int tric_simple_epilogue(void);

extern bool tric_gate_split1 (void);
extern int tric_move_ratio (int);
extern int tric_clear_ratio (int);
extern int tric_set_ratio (int);

extern int tric_zdata_max;
extern int tric_zdata_min;
extern int tric_zconst_max;
extern int tric_zconst_min;
extern int tric_sdata_max;
extern int tric_sdata_min;
extern int tric_sconst_max;
extern int tric_sconst_min;
extern struct tric_segment_trap tric_segment_trap;
extern void tric_decode_msegment_trap (const char*, const char*);

#if defined (RTX_CODE)

extern int tric_adjust_insn_length (rtx, int);
extern int intval (rtx, enum machine_mode);
extern int tric_hard_regno_mode_ok (int, enum machine_mode);
extern int extra_constraint_Wa (rtx x);
extern int extra_constraint_Wc (rtx x);
extern int extra_constraint_Wm (rtx x);
extern int extra_constraint_Wl (rtx x);
extern int extra_constraint_Ga9 (rtx x);
extern int extra_constraint_Ghg (rtx x);
extern int extra_constraint_Gsg (rtx x);
extern int extra_constraint_Gu4 (rtx x);
extern int extra_constraint_GSa (rtx x);
extern void tric_emit_casesi (rtx,rtx,rtx,rtx,rtx);
extern void tric_emit_cbranchsi4 (rtx[]);
extern void tric_emit_cbranchdi4 (rtx[]);
extern void tric_emit_cbranchsf4 (rtx[]);
extern rtx tric_emit_multiply_add (rtx, rtx, rtx, rtx);
extern rtx tric_emit_mulsidi (rtx, rtx, rtx, enum rtx_code);
extern void tric_emit_maddsidi4_const (rtx*);
extern rtx tric_emit_umaddsidi4_const (rtx*);
extern void tric_emit_ashift64 (rtx*);
extern void tric_emit_lshiftrt64 (rtx*);
extern void tric_emit_ashiftrt64 (rtx*);
extern void tric_split_const_rtx (rtx, unsigned*, unsigned*);
extern void tric_split_di (rtx[], rtx[], rtx[], int);
extern rtx tric_emit_arith_CONST_INT (rtx, rtx, rtx, rtx);
extern void tric_emit_move_CONST_INT (rtx, rtx);
extern bool tric_can_move_immediate_p (rtx, rtx);
extern bool tric_absolute_code_address_p (rtx);
extern bool tric_absolute_address_p (rtx);
extern bool tric_small16_address_p (rtx);
extern bool tric_sat_p (enum machine_mode, enum rtx_code, rtx, enum rtx_code, rtx, HOST_WIDE_INT* = NULL, HOST_WIDE_INT* = NULL);
extern bool tric_usat_p (enum rtx_code, rtx, enum rtx_code, rtx, HOST_WIDE_INT*, HOST_WIDE_INT*);
extern enum rtx_code tric_max_code (enum rtx_code, rtx, rtx);
extern enum rtx_code tric_min_code (enum rtx_code, rtx, rtx);
extern bool tric_anchor_completed (void);
extern bool tric_peep2_may_sibcall_p (rtx, rtx);
extern void tric_peep2_setup_sibcall (rtx, rtx);
extern int tric_split_const_int (rtx, rtx*, rtx*, enum rtx_code);

extern void tric_output_call (rtx,rtx*,int);

extern int symbolic_operand_p (rtx, enum machine_mode);
extern int mov_input_operand_p (rtx, enum machine_mode);

extern int addr_register_operand_p (rtx, enum machine_mode);
extern int data_register_operand_p (rtx, enum machine_mode);

extern void tric_emit_insert (rtx*);
extern void tric_emit_imask (rtx*);
extern bool tric_emit_move (rtx *, enum machine_mode);
extern void tric_emit_branch (enum rtx_code, rtx, rtx, rtx);
extern rtx tric_emit_setcompare (enum rtx_code, rtx, rtx, rtx);
extern void tric_emit_setcompare_di (enum rtx_code, rtx, rtx, rtx);
extern int tric_mov_operands (rtx*, enum machine_mode);
extern int tric_split_mov_insn_p (rtx*, enum machine_mode);
extern rtx tric_return_addr_rtx (int);
extern int tric_local_regno (unsigned int);
extern enum machine_mode tric_mode_for_align (HOST_WIDE_INT, rtx);
extern bool tric_code_signed_p (enum rtx_code);
extern bool tric_codes_same_sign_p (enum rtx_code, enum rtx_code);

static inline HOST_WIDE_INT
tric_max_for_mode (enum machine_mode mode, bool signed_p)
{
  gcc_assert (SCALAR_INT_MODE_P (mode));

  gcc_assert (GET_MODE_PRECISION (mode) < HOST_BITS_PER_WIDE_INT);

  return GET_MODE_MASK (mode) >> signed_p;
}

static inline HOST_WIDE_INT
tric_min_for_mode (enum machine_mode mode, bool signed_p)
{
  return signed_p ? -1 - tric_max_for_mode (mode, signed_p) : 0;
}

static inline HOST_WIDE_INT
USIval (rtx x)
{
  return INTVAL (x) & GET_MODE_MASK (SImode);
}

static inline HOST_WIDE_INT
SIval (rtx x, bool signed_p)
{
  gcc_assert (CONST_INT_P (x));
  return signed_p ? INTVAL (x) : USIval (x);
}


#define TRIC_WRAP_MODULUS (HOST_WIDE_INT_1 << BITS_PER_WORD)
#define TRIC_UINT_MAX (TRIC_WRAP_MODULUS - 1)
#define TRIC_INT_MAX (TRIC_UINT_MAX >> 1)
#define TRIC_INT_MIN (-TRIC_INT_MAX - 1)

#endif /* RTX_CODE */

#if defined (TREE_CODE)
/* Used in md */
extern int tric_interrupt_function_p (const_tree);
extern int tric_interrupt_handler_function_p (const_tree);
extern int tric_pxhndcall_function_p (const_tree);
extern int tric_longcall_function_p (const_tree);
extern void tric_asm_output_aligned_var (FILE*, tree, const char*, int, int, int);
extern void tric_asm_output_subsection (FILE*, section*, tree, int);
extern void tric_asm_output_external (FILE*, tree, const char*);
extern unsigned tric_eabi_adjust_field_align (tree, unsigned);
extern unsigned tric_eabi_data_alignment (tree, unsigned);
extern unsigned tric_eabi_round_type_align (tree, unsigned, unsigned);
#endif /* TREE_CODE */

#if defined (RTX_CODE) && defined (TREE_CODE)

extern void tric_init_cumulative_args (CUMULATIVE_ARGS *, tree, rtx, int);

#endif /* RTX_CODE && TREE_CODE */

extern void tric_register_pragmas (void);

#define OPVAL(n) INTVAL(operands[n])
#define UOPVAL(n) UINTVAL(operands[n])

extern int tric_have_cost_insns;
extern int mWidth;
extern int mPos;
extern int tric_map_combine;

#define COST_INSN (tric_have_cost_insns)
#define COST_PATTERN gcc_unreachable(); return ""

#define PEEP2_ORDER_REGS(A,X,Y)                                         \
do {                                                                    \
  if (D_REG_P (operands[A]))                                            \
    {                                                                   \
      if (D_REG_P (operands[X]) && !D_REG_P(operands[Y]))               \
        {                                                               \
          rtx x; x = operands[X]; operands[X] = operands[Y]; operands[Y] = x; \
        }                                                               \
    }                                                                   \
  else                                                                  \
    {                                                                   \
      if (A_REG_P (operands[X]) && !A_REG_P(operands[Y]))               \
        {                                                               \
          rtx x; x = operands[X]; operands[X] = operands[Y]; operands[Y] = x; \
        }                                                               \
    }                                                                   \
 } while(0)

#endif /* GCC_TRICORE_PROTOS_H */
