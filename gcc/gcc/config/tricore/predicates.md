;; Predicate definitions for Infineon TriCore.
;; Copyright (C) 2005-2014 Free Software Foundation, Inc.
;;
;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.
;;
;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;; Return nonzero if OP is a register or an integer constant.
(define_predicate "reg_or_const_int_operand"
  (ior (match_operand 0 "register_operand")
       (match_code "const_int")))

(define_predicate "a_reg_operand"
  (and (match_code "reg")
       (match_test "A_REGNO_P (REGNO(op))")))

(define_predicate "d_reg_operand"
  (and (match_code "reg")
       (match_test "D_REGNO_P (REGNO(op))")))

(define_predicate "D15_reg_operand"
  (and (match_code "reg")
       (match_test "REG_D15 == REGNO(op)")))

(define_predicate "e_reg_operand"
  (and (match_code "reg")
       (match_test "E_REGNO_P (REGNO(op))")))

(define_predicate "u3_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<3)")))

(define_predicate "s4_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= -(1<<3) && INTVAL(op) < (1<<3)")))

(define_predicate "u4_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<4)")))

(define_predicate "u5_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<5)")))

(define_predicate "u6_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<6)")))

(define_predicate "u8_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<8)")))

(define_predicate "s9_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= -(1<<8) && INTVAL(op) < (1<<8)")))

(define_predicate "u9_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<9)")))

(define_predicate "u10_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<10)")))

(define_predicate "s10_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= -(1<<9) && INTVAL(op) < (1<<9)")))

(define_predicate "s10n_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) > -(1<<9) && INTVAL(op) < (1<<9)")))

(define_predicate "s11_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= -(1<<10) && INTVAL(op) < (1<<10)")))

(define_predicate "u11_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<11)")))

(define_predicate "no_s9_operand"
  (and (match_code "const_int")
       (not (match_operand 0 "s9_operand"))))

(define_predicate "no_u9_operand"
  (and (match_code "const_int")
       (not (match_operand 0 "u9_operand"))))

(define_predicate "no_s10n_operand"
  (and (match_code "const_int")
       (not (match_operand 0 "s10n_operand"))))

(define_predicate "any_extend_operand"
  (and (match_code "sign_extend, zero_extend")
       (match_test "mode == GET_MODE_2XWIDER_MODE (GET_MODE (XEXP (op, 0)))")
       (match_test "register_operand (XEXP (op, 0), GET_MODE (XEXP (op, 0)))")))

(define_predicate "any_extend_or_reg_operand"
  (and (match_code "sign_extend, zero_extend, reg, subreg")
       (ior (match_operand 0 "register_operand")
            (match_operand 0 "any_extend_operand"))))

;; We can add the const without using a clobber reg if
;; both lo and hi part are s9 operands
(define_predicate "add64_noclobber_operand"
  (and (match_code "const_int")
       (ior (match_operand 0 "s9_operand")
            (match_test "extra_constraint_Ga9(op)"))))

(define_predicate "addsc_const_operand"
  (and (match_code "const_int")
       (ior (match_test "1 == INTVAL(op)")
            (match_test "2 == INTVAL(op)")
            (match_test "4 == INTVAL(op)")
            (match_test "8 == INTVAL(op)"))))

(define_predicate "c9_operand"
  (and (match_code "const_int")
       (match_test "~INTVAL(op) >= 0 && ~INTVAL(op) < (1<<9)")))

(define_predicate "m9_operand"
  (and (match_code "const_int")
       (match_test "-INTVAL(op) >= -(1 << 8) && -INTVAL(op) < (1<<8)")))

(define_predicate "s9_m9_operand"
  (ior (match_operand 0 "s9_operand")
       (match_operand 0 "m9_operand")))

(define_predicate "s12_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= -(1<<11) && INTVAL(op) < (1<<11)")))

(define_predicate "s16_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= -(1<<15) && INTVAL(op) < (1<<15)")))

(define_predicate "u16_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0 && INTVAL(op) < (1<<16)")))

;; Return true if OP is a constant that contains only one 1 in its
;; binary representation.
(define_predicate "single_one_operand"
  (and (match_code "const_int")
       (match_test "exact_log2(INTVAL (op) & GET_MODE_MASK (mode)) >= 0")))

;; Return true if OP is a constant that contains only one 0 in its
;; binary representation.
(define_predicate "single_zero_operand"
  (and (match_code "const_int")
       (match_test "exact_log2(~INTVAL (op) & GET_MODE_MASK (mode)) >= 0")))

;; Return true if OP contains a sequence of ones in its
;; binary representation and is zero, otherwise.
(define_predicate "ones_operand"
  (and (match_code "const_int")
       (match_test "ones_mask (INTVAL (op) & GET_MODE_MASK (mode), NULL) >= 0")))

;; Return true if OP contains a sequence of at most 4 ones in its
;; binary representation and zeros, otherwise.
(define_predicate "ones4_operand"
  (and (match_code "const_int")
       (match_test "ones_mask (INTVAL (op) & GET_MODE_MASK (mode), &mWidth) >= 0 && mWidth <= 4")))

;; Return true if OP contains a sequence of zeros in its
;; binary representation and ones, otherwise.
(define_predicate "zeros_operand"
  (and (match_code "const_int")
       (match_test "ones_mask (~INTVAL (op) & GET_MODE_MASK (mode), NULL) >= 0")))

(define_predicate "reg_or_0_operand"
  (ior (match_operand 0 "register_operand")
       (match_test "op == CONST0_RTX (mode)")))

(define_predicate "reg_or_s4_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "s4_operand")))

(define_predicate "reg_or_u4_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "u4_operand")))

(define_predicate "reg_or_u5_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "u5_operand")))

(define_predicate "reg_or_u6_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "u6_operand")))

(define_predicate "reg_or_s9_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "s9_operand")))

(define_predicate "reg_or_u9_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "u9_operand")))

(define_predicate "reg_or_m9_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "m9_operand")))

(define_predicate "reg_or_s9_u9_operand"
  (ior (match_operand 0 "reg_or_s9_operand")
       (match_operand 0 "u9_operand")))

(define_predicate "reg_or_u10_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "u10_operand")))

(define_predicate "reg_or_s10_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "s10_operand")))

(define_predicate "reg_or_s11_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "s11_operand")))

(define_predicate "reg_or_u11_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "u11_operand")))

(define_predicate "reg_or_s11_u11_operand"
  (ior (match_operand 0 "reg_or_s11_operand")
       (match_operand 0 "u11_operand")))

(define_predicate "reg_or_s16_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "s16_operand")))

(define_predicate "reg_or_u16_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "u16_operand")))

(define_predicate "reg_or_add64_noclobber_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "add64_noclobber_operand")))

(define_predicate "reg_or_s12_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "s12_operand")))

;; Used for TC16 peepholes only
(define_predicate "d_reg_or_const0_or_s1_operand"
  (ior (match_operand 0 "d_reg_operand")
       (and (match_code "const_int")
            (match_test "op == const0_rtx || op == constm1_rtx"))
       (match_test "rtx_equal_p (op, CONST0_RTX (mode))")))

(define_predicate "d_reg_or_const0_or_s16_operand"
  (ior (match_operand 0 "d_reg_operand")
       (match_operand 0 "s16_operand")
       (match_test "rtx_equal_p (op, CONST0_RTX (mode))")))

(define_predicate "high_operand"
  (ior (match_code "high")
       (and (match_code "const_int")
            (match_test "(INTVAL (op) & GET_MODE_MASK (mode) & 0xffff) == 0"))))

(define_predicate "add_input_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "high_operand")
       (match_operand 0 "s16_operand")))

(define_special_predicate "mov_input_operand"
  (match_test "mov_input_operand_p (op, mode)"))

(define_predicate "symbolic_operand"
  (match_test "symbolic_operand_p (op, mode)"))

(define_predicate "absolute_address_operand"
  (and (match_test "Pmode == mode")
       (match_test "tric_absolute_address_p (op)")))

;; Return true if OP is the zero constant for MODE.
(define_predicate "const_zero_operand"
  (and (match_code "const_int,const_double")
       (match_test "op == CONST0_RTX (mode)")))

(define_predicate "const_string_operand"
  (match_code "const_string"))

;; Experimental

(define_predicate "vok_memory_operand"
  (match_code "mem")
  {
    int ok;
    int v_ok = volatile_ok;
  
    volatile_ok = 1;
    ok = memory_operand (op, mode);
    volatile_ok = v_ok;

    return ok;
  })

(define_predicate "absolute_memory_operand"
  (and (match_operand 0 "memory_operand")
       (match_test "Pmode == GET_MODE (XEXP (op, 0))")
       (match_test "tric_absolute_address_p (XEXP (op, 0))")))

(define_predicate "vok_nonimmediate_operand"
  (ior (match_operand 0 "nonimmediate_operand")
       (match_operand 0 "vok_memory_operand")))

;; LDMST is special because it traps on specific SFRs.
;; For now we just map to memory_operand which does not
;; allow volatile during combine.
(define_predicate "ldmst_operand"
  (match_operand 0 "memory_operand"))

(define_special_predicate "extend_operand"
  (ior (match_operand 0 "nonimmediate_operand")
       (and (match_test "tric_opt_extend_load")
            (match_operand 0 "vok_nonimmediate_operand"))))

;; True for comparisons
(define_special_predicate "tric_comparison_operator"
  (match_code "eq,ne,le,lt,ge,gt,geu,gtu,leu,ltu"))

;; True for signed comparisons of TriCore
(define_special_predicate "tric_signed_comparison_operator"
  (match_code "eq,ne,lt,ge"))

;; True for unsigned comparisons of TriCore
(define_special_predicate "tric_unsigned_comparison_operator"
  (match_code "geu,ltu"))

(define_special_predicate "tric_float_comparison_operator"
  (match_code "eq,ne,le,lt,ge,gt,unordered,uneq,unge,ungt,unle,unlt,ltgt"))

;; Used in peep2: all operators that support "d,d,d Ksa"
(define_special_predicate "tric_s10_operator"
  (match_code "and,ior,xor,plus"))

;; Used in peep2: all operators that support "d,d,d Ks9"
(define_special_predicate "tric_s9_operator"
  (match_code "eq,ne,ge,lt,smax,smin"))

;; Used in peep2: all operators that support "d,d,d Ku9"
(define_special_predicate "tric_u9_operator"
  (match_code "geu,ltu,ashift,ashiftrt,lshiftrt,umax,umin"))
