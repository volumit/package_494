;;  Machine description for Infineon TriCore
;;  Copyright 2005-2014 Free Software Foundation, Inc.
;;  Contributed by HighTec EDV

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This file contains patterns for insn combine.
;; The compiler is supposed to work without these patterns,
;; but in some situations the produced code will be less efficient.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "*@cost.maddsi4"
  [(set (match_operand:SI 0 "register_operand"                     "=X")
        (plus:SI (mult:SI (match_operand:SI 1 "register_operand"    "X")
                          (match_operand:SI 2 "nonmemory_operand"   "X"))
                 (match_operand:SI 3 "register_operand"             "X")))]
  "COST_INSN" { COST_PATTERN; }
  [(set (attr "ticks")
        (cond [(match_operand:SI 2 "addsc_const_operand")
               (const_int 2)

               (match_operand:SI 2 "reg_or_s9_operand")
               ;; we must make it cheaper than it actually is
               ;; in order to catch ADDSC.A ...
               (const_int 4)]
              (const_int 8)))
   (set (attr "space")
        (cond [(match_operand:SI 2 "addsc_const_operand")
               (const_int 2)
               (match_operand:SI 2 "reg_or_s9_operand")
               (const_int 4)]
              (const_int 8)))])

(define_insn_and_split "maddsi4"
  [(set (match_operand:SI 0 "register_operand"                     "=d     ,a   ,?a")
        (plus:SI (mult:SI (match_operand:SI 1 "register_operand"   "%d     ,d   ,d ")
                          (match_operand:SI 2 "reg_or_s9_operand"   "d Ks9 ,Kxa ,d Kxb"))
                 (match_operand:SI 3 "register_operand"             "d     ,a   ,a ")))
   (clobber (match_scratch:SI 4                                    "=X     ,X   ,&d"))]
  ""
  "@
    madd\t%0, %3, %1, %2
    addsc.a\t%0, %3, %1, %p2
    #"
  "reload_completed"
  [(set (match_dup 4)
        (match_dup 5))
   (set (match_dup 0)
        (plus:SI (match_dup 3)
                 (match_dup 4)))]
  {
    int ival, ex;

    /* must compute which_alternative first */
    extract_constrain_insn_cached (curr_insn);

    if (2 != which_alternative)
      FAIL;

    if (REG_P (operands[2]))
      {
        operands[5] = gen_rtx_MULT (SImode, operands[1], operands[2]);
        goto emit;
      }

    ival = OPVAL (2);
    ex = exact_log2 (ival);

    if (ival == 9 || ival == 10 || ival == 12 || ival == 16)
      {
        /* decompose $2 = 8 + ($2 - 8) */
        emit_insn (gen_maddsi4 (operands[0], operands[1], GEN_INT(8),
                   operands[3]));
        emit_insn (gen_maddsi4 (operands[0], operands[1], GEN_INT(ival-8),
                   operands[0]));
        DONE;
      }
    else if (ival == 5 || ival == 6)
      {
        /* decompose $2 = 4 + ($2 - 4) */
        emit_insn (gen_maddsi4 (operands[0], operands[1], GEN_INT(4),
                   operands[3]));
        emit_insn (gen_maddsi4 (operands[0], operands[1], GEN_INT(ival-4),
                   operands[0]));
        DONE;
      }
    else if (ival == 3)
      {
        /* decompose $2 = 2 + ($2 - 2) */
        emit_insn (gen_maddsi4 (operands[0], operands[1], GEN_INT(2),
                   operands[3]));
        emit_insn (gen_maddsi4 (operands[0], operands[1], GEN_INT(ival-2),
                   operands[0]));
        DONE;
      }
    else if (ex >= 1)
      {
        operands[5] = gen_rtx_ASHIFT (SImode, operands[1], GEN_INT (ex));
      }
    else
      {
        operands[5] = gen_rtx_MULT (SImode, operands[1], operands[2]);
      }

    emit:;
  }
  [(set_attr "ticks" "5")
   (set_attr "space" "4")
   (set_attr "pipe" "ipm,mixds,*")
   (set_attr "length" "4,*,8")
   (set_attr "adjust" "*,addsc,*")])


(define_insn "*@cost.msubsi4.ic"
  [(set (match_operand:SI 0 "register_operand"                      "=X")
        (minus:SI (match_operand:SI 1 "register_operand"             "X")
                  (mult:SI (match_operand:SI 2 "register_operand"    "X")
                           (match_operand:SI 3 "nonmemory_operand"   "X"))))]
  "COST_INSN" { COST_PATTERN; }
  [(set (attr "ticks")
        (if_then_else (match_operand:SI 3 "reg_or_s9_operand" "")
                      (const_int 5)
                      (const_int 9)))
   (set (attr "space")
        (if_then_else (match_operand:SI 3 "reg_or_s9_operand" "")
                      (const_int 4)
                      (const_int 8)))])

(define_insn "msubsi4"
  [(set (match_operand:SI 0 "register_operand"                      "=d")
        (minus:SI (match_operand:SI 3 "register_operand"             "d")
                  (mult:SI (match_operand:SI 1 "register_operand"   "%d")
                           (match_operand:SI 2 "nonmemory_operand"   "d Ks9"))))]
  ""
  "msub\t%0, %3, %1, %2"
  [(set_attr "ticks" "5")
   (set_attr "space" "4")
   (set_attr "pipe" "ipm")])


(define_insn_and_split "*maddsicc.mul.cond.ic"
  [(set (match_operand:SI 0 "register_operand"                                        "=d")
        (plus:SI (mult:SI (match_operator:SI 1 "tric_comparison_operator"
                                             [(match_operand:SI 2 "register_operand"   "d")
                                              (match_operand:SI 3 "nonmemory_operand"  "d n")])
                          (match_operand:SI 4 "reg_or_s9_operand"                      "d Ks9"))
                 (match_operand:SI 5 "register_operand"                                "d")))]
  "tric_gate_split1 ()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (plus:SI (match_dup 5)
                 (if_then_else:SI (ne (match_dup 6)
                                      (const_int 0))
                                  (match_dup 4)
                                  (const_int 0))))]
  {
    operands[6] = gen_reg_rtx (SImode);
    tric_emit_setcompare (GET_CODE (operands[1]), operands[2], operands[3],
                          operands[6]);
  })

(define_insn "*iorsi3_ones.ic"
  [(set (match_operand:SI 0 "register_operand"           "=d")
        (ior:SI (match_operand:SI 1 "register_operand"   "%d")
                (match_operand:SI 2 "ones4_operand"       "n")))]
  ""
  "insert\t%0, %1, 15, %P2, %W2"
  [(set_attr "pipe" "ip2")])

;; Allow ORing ones to faciliate LDMST
(define_insn_and_split "*iorsi3.ones"
  [(set (match_operand:SI 0 "register_operand"         "=d")
        (ior:SI (match_operand:SI 1 "register_operand" "%d")
                (match_operand:SI 2 "ones_operand"      "d")))]
  "tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (ior:SI (match_dup 1)
                (match_dup 2)))]
  {
    if (!s10_operand (operands[2], SImode))
      operands[2] = force_reg (SImode, operands[2]);
  })

(define_insn "*andsi3_zeroes.insert.ic"
  [(set (match_operand:SI 0 "register_operand"              "=D  ,d  ,!d ,d")
        (and:SI (match_operand:SI 1 "register_operand"      "%0  ,d  ,d  ,d")
                (match_operand:SI 2 "zeros_operand"          "Ko8,Ko8,Ko8,Kx8")))]
  "const0_rtx != operands[2]"
  {
    if (OPVAL (2) == 255)
      return (0 == which_alternative) ? "and\t%0, 255" : "and\t%0, %1, 255";
    operands[2] = gen_int_mode (~OPVAL (2), SImode);
    return "insert\t%0, %1, 0, %P2, %W2";
  }
  [(set_attr "opt" "*,speed,size,*")
   (set_attr "pipe" "ip,ip,ip,ip2")])

(define_insn "*xorsi3_one.ic"
  [(set (match_operand:SI 0 "register_operand"              "=d")
        (xor:SI (match_operand:SI 1 "register_operand"      "%d")
                (match_operand:SI 2 "const_int_operand"     "n")))]
  "exact_log2 (OPVAL(2)) >= 9"
  "insn.t\t%0, %1,%P2, %1,%P2"
  [(set_attr "pipe" "ip")])

;; $0 = ($1 << 1) | 1
(define_insn_and_split "*ashlsi2-1.ior-1.ic"
  [(set (match_operand:SI 0 "register_operand"                    "=d,d,*?a")
        (ior:SI (ashift:SI (match_operand:SI 1 "register_operand"  "0,d,a")
                           (const_int 1))
                (const_int 1)))]
  ""
  "@
	sh.eq\t%0, %0, %0
	#
	#"
  "reload_completed"
  [(set (match_dup 0)
        (ashift:SI (match_dup 1)
                   (const_int 1)))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (const_int 1)))]
  {
    if (D_REG_P (operands[0])
        && REGNO (operands[0]) == REGNO (operands[1]))
      FAIL;
  }
  [(set_attr "pipe" "ip")])

;; $0 = $2 | ($3 << $1)
(define_insn "*iorsi3_ashift_ones.ic"
  [(set (match_operand:SI 0 "register_operand"                     "=d")
        (ior:SI (ashift:SI (match_operand:SI 3 "u4_operand"         "Ku4")
                           (match_operand:SI 1 "register_operand"   "d"))
                (match_operand:SI 2 "register_operand"              "d")))]
  "ones_mask (OPVAL(3), &mWidth) == 0
   && mWidth  <= 4"
  "insert\t%0, %2, %3, %1, %W3"
  [(set_attr "pipe" "ip2")])

;; $0 = $2 & ~(1 << $1)
(define_insn "*andsi3_rotate_zero.ic"
  [(set (match_operand:SI 0 "register_operand"                     "=d")
        (and:SI (rotate:SI (const_int -2)
                           (match_operand:SI 1 "register_operand"   "d"))
                (match_operand:SI 2 "register_operand"              "d")))]
  ""
  "insert\t%0, %2, 0, %1, 1"
  [(set_attr "pipe" "ip2")])


;; "*andnot1si3.r.ic"
;; "*iornot1si3.r.ic"
;; "*xornot1si3.r.ic"
(define_insn "*<code>not1si3.r.ic"
  [(set (match_operand:SI 0 "register_operand"                        "=d")
        (tric_bitop:SI (not:SI (match_operand:SI 1 "register_operand"  "d"))
                       (match_operand:SI 2 "register_operand"          "d")))]
  ""
  "<tric_bitop_not_op>\t%0, %2, %1"
  [(set_attr "pipe" "ip")])

;; "*andnot2si3.r.ic"
;; "*iornot2si3.r.ic"
;; "*xornot2si3.r.ic"
(define_insn "*<code>not2si3.ic"
  [(set (match_operand:SI 0 "register_operand"                        "=d")
        (tric_bitop:SI (match_operand:SI 1 "register_operand"          "d")
                       (not:SI (match_operand:SI 2 "register_operand"  "d"))))]

  ""
  "<tric_bitop_not_op>\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

;; "*andnot1si3.i.ic"
;; "*iornot1si3.i.ic"
;; "*xornot1si3.i.ic"
(define_insn "*<code>not1si3.i.ic"
  [(set (match_operand:SI 0 "register_operand"                         "=d")
        (tric_bitop:SI (not:SI (match_operand:SI 1 "register_operand"  "d"))
                       (match_operand:SI 2 "c9_operand"                "Kcb")))]
  ""
  "<tric_demorgan_bitop_op>\t%0, %1, ~(%2)"
  [(set_attr "pipe" "ip")])

;; "*andsi3_and1.ic"
;; "*iorsi3_and1.ic"
;; "*xorsi3_and1.ic"
(define_insn "*<code>si3_and1.ic"
  [(set (match_operand:SI 0 "register_operand"                          "=d")
        (and:SI (tric_bitop:SI (match_operand:SI 1 "register_operand"   "%d")
                               (match_operand:SI 2 "register_operand"    "d"))
                (const_int 1)))]
  ""
  "<tric_op>.t\t%0, %1,0, %2,0"
  [(set_attr "pipe" "ip")])

;; "*andsi3_shiftrt_and1.ic"
;; "*iorsi3_shiftrt_and1.ic"
;; "*xorsi3_shiftrt_and1.ic"
(define_insn "*<code>si3_shiftrt_and1.ic"
  [(set (match_operand:SI 0 "register_operand"                                      "=d")
        (and:SI (tric_bitop:SI (lshiftrt:SI (match_operand:SI 1 "register_operand"   "d")
                                            (match_operand:SI 2 "u5_operand"         "Ku5"))
                               (match_operand:SI 3 "register_operand"                "d"))
                (const_int 1)))]
  ""
  "<tric_op>.t\t%0, %1,%2, %3,0"
  [(set_attr "pipe" "ip")])

(define_insn "*extzv_xor_1bit.ic"
  [(set (match_operand:SI 0 "register_operand"                           "=d")
        (zero_extract:SI (xor:SI (match_operand:SI 1 "register_operand"   "d")
                                 (match_operand:SI 2 "const_int_operand"  "n"))     ; mask = 1 << pos
                         (const_int 1)                                              ; width
                         (match_operand:SI 3 "u5_operand"                 "Ku5")))] ; pos
  "OPVAL(2) & (1 << OPVAL(3))"
  "nand.t\t%0, %1,%3, %1,%3"
  [(set_attr "pipe" "ip")])

;; "*extzv_and_1bit.ic"
;; "*extzv_ior_1bit.ic"
;; "*extzv_xor_1bit.ic"
(define_insn "*extzv_<code>_1bit.ic"
  [(set (match_operand:SI 0 "register_operand"                                  "=d")
        (zero_extract:SI (tric_bitop:SI (match_operand:SI 1 "register_operand"   "d")
                                        (match_operand:SI 2 "register_operand"   "d"))
                         (const_int 1)                                                     ; width
                         (match_operand:SI 3 "u5_operand"                        "Ku5")))] ; pos
  ""
  "<tric_op>.t\t%0, %1,%3, %2,%3"
  [(set_attr "pipe" "ip")])

(define_insn "*insertqi.a.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"  "+d")
                         (const_int 8) ; width
                         (match_operand:SI 1 "const_int_operand"  "n"))
        (zero_extend:SI (match_operand:QI 2 "register_operand"    "d")))]
  "OPVAL(1) <= 24 && OPVAL(1) % 8 == 0"
  "insert\t%0, %0, %2, %1, 8"
  [(set_attr "pipe" "ip2")])

(define_insn "*subsi3_one_cmpl_1bit.ic"
  [(set (match_operand:SI 0 "register_operand"                                    "=d")
        (minus:SI (match_operand:SI 1 "register_operand"                           "0")
                  (zero_extract:SI (xor:SI (match_operand:SI 2 "register_operand"  "d")
                                           (match_operand:SI 3 "const_int_operand" "n"))      ; mask = 1 << pos
                                   (const_int 1)                                              ; width
                                   (match_operand:SI 4 "u5_operand"                "Ku5"))))] ; pos
  "optimize_size
   && exact_log2(OPVAL(3)) == OPVAL(4)"
  "jnz.t\t%2,%4, 0f\;add\t%0, -1\;0:"
  [(set_attr "length" "6")])

;; "*subsicc.eq0.ic"
;; "*subsicc.ne0.ic"
(define_insn "*subsicc.<code>0.ic"
  [(set (match_operand:SI 0 "register_operand"                  "=d,d")
        (minus:SI (match_operand:SI 2 "register_operand"         "d,d")
                  (if_then_else:SI
                   (eq_ne (match_operand:SI 1 "register_operand" "d,d")
                          (const_int 0))
                   (match_operand:SI 3 "reg_or_m9_operand"       "d,Km9")
                   (const_int 0))))]
  ""
  "@
    csub<tric_n_not_op>\t%0, %1, %2, %3
    cadd<tric_n_not_op>\t%0, %1, %2, %n3"
  [(set_attr "pipe" "ip")])


;; "*movsicc.eq.2const.ic"
;; "*movsicc.ne.2const.ic"
(define_insn_and_split "*movsicc.<code>.2const.ic"
  [(set (match_operand:SI 0 "register_operand"         "=d")
        (if_then_else:SI
         (eq_ne (match_operand:SI 1 "register_operand"  "d")
                (const_int 0))
         (match_operand:SI 2 "const_int_operand"        "n")
         (match_operand:SI 3 "const_int_operand"        "n")))]
  "!reload_completed
   && tric_gate_split1 ()
   && (s9_operand (operands[2], SImode)
       != s9_operand (operands[3], SImode))"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [;; "*movsicc.eq"
   ;; "*movsicc.ne"
   (set (match_dup 0)
        (if_then_else:SI (match_dup 1)
                         (match_dup 4)
                         (match_dup 5)))]
  {
    // A conditional move with 2 compile time constants with exactly one
    // fitting SEL[N], i.e. a signed 9-bit constant.  Make sure the 9-bit
    // constant ($5) is at the optimal position, and reload the other
    // constant ($4) to a register.

    enum rtx_code code = <eq_ne:CODE>;

    if (s9_operand (operands[3], SImode))
      {
        operands[4] = force_reg (SImode, operands[2]);
        operands[5] = operands[3];
      }
    else if (s9_operand (operands[2], SImode))
      {
        code = reverse_condition (code);
        operands[4] = force_reg (SImode, operands[3]);
        operands[5] = operands[2];
      }
    else
      gcc_unreachable();

    operands[1] = gen_rtx_fmt_ee (code, VOIDmode, operands[1], const0_rtx);
  })


;; "*branch_zerox1qi_eq0.ic"
;; "*branch_zerox1hi_eq0.ic"
;; "*branch_zerox1si_eq0.ic"
;; "*branch_zerox1qi_ne0.ic"
;; "*branch_zerox1hi_ne0.ic"
;; "*branch_zerox1si_ne0.ic"
(define_insn "*branch_zerox1<mode>_<code>0.ic"
  [(set (pc)
        (if_then_else (eq_ne
                       (zero_extract:SI (match_operand:QIHISI 1 "register_operand"  "d")
                                        (const_int 1)                                       ; width
                                        (match_operand:SI 2 "u5_operand"            "Ku5")) ; pos
                       (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  "OPVAL(2) < GET_MODE_BITSIZE(<MODE>mode)"
  "j<tric_n_op>z.t\t%1, %2, %0"
  [(set_attr "pipe" "jip")
   (set_attr "type" "branch_U4_d15")])

;; See http://gcc.gnu.org/ml/gcc/2008-12/msg00329.html
;; PR middle-end/38751
(define_insn_and_split "*patch-38751"
  [(set (match_operand:SI 0 "register_operand"                    "=d")
        (mult:SI (minus:SI (const_int 1)
                           (match_operand:SI 1 "register_operand"  "d"))
                 (const_int -2)))]
  "tric_opt_split_insns"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (plus:SI (match_dup 1)
                 (const_int -1)))
   (set (match_dup 0)
        (ashift:SI (match_dup 0)
                   (const_int 1)))])

(define_insn "*movsicc.ic"
  [(set (match_operand:SI 0 "register_operand"                   "=d    ")
        (if_then_else:SI (match_operand:SI 1 "register_operand"   "d    ")
                         (match_operand:SI 2 "reg_or_s9_operand"  "d Ks9")
                         (match_operand:SI 3 "register_operand"   "d    ")))]
  ""
  "seln\t%0, %1, %3, %2"
  [(set_attr "pipe" "ip")])

;; seen in libgcc2.divsc3, used in insv-expander
(define_insn "insv_1_ic"
  [(set (match_operand:SI 0 "register_operand"                  "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"   "d")
                        (match_operand:SI 2 "const_int_operand"  "n"))    ; 0b0000...1111
                (and:SI (match_operand:SI 3 "register_operand"   "d")
                        (match_operand:SI 4 "const_int_operand"  "n"))))] ; 0b1111...0000
  "GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(2) ^ OPVAL(4)))
   && ones_mask (OPVAL(2), NULL) == 0"
  "insert\t%0, %3, %1, 0, %W2"
  [(set_attr "pipe" "ip2")])

;; dito
(define_insn "*insv.2.ic"
  [(set (match_operand:SI 0 "register_operand"                  "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"   "d")
                        (match_operand:SI 2 "const_int_operand"  "n"))    ; 0b1111...0000
                (and:SI (match_operand:SI 3 "register_operand"   "d")
                        (match_operand:SI 4 "const_int_operand"  "n"))))] ; 0b0000...1111
  "GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(2) ^ OPVAL(4)))
   && ones_mask (OPVAL(4), NULL) == 0"
  "insert\t%0, %1, %3, 0, %W4"
  [(set_attr "pipe" "ip2")])


(define_insn "*insert.ashift.zeroxhi.ic"
  [(set (match_operand:SI 0 "register_operand"                          "=d")
        (ior:SI (ashift:SI (match_operand:SI 1 "register_operand"        "d")
                           (const_int 16))
                (zero_extend:SI (match_operand:HI 2 "register_operand"   "d"))))]
  ""
  "insert\t%0, %2, %1, 16, 16"
  [(set_attr "pipe" "ip2")])

(define_insn "*insert.zeroxhi.ic"
  [(set (match_operand:SI 0 "register_operand"                          "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"           "d")
                        (const_int -65536)) ;; 0xffff0000
                (zero_extend:SI (match_operand:HI 2 "register_operand"   "d"))))]
  ""
  "insert\t%0, %1, %2, 0, 16"
  [(set_attr "pipe" "ip2")])

(define_insn "*insert.madd.zeroxhi.ic"
  [(set (match_operand:SI 0 "register_operand"                                     "=d")
        (plus:SI (mult:SI (match_operand:SI 1 "register_operand"                    "d")
                          (const_int 65536))
                 (zero_extend:SI (subreg:HI (match_operand:SI 2 "register_operand"  "d") 0))))]
  ""
  "insert\t%0, %2, %1, 16, 16"
  [(set_attr "pipe" "ip2")])

;; Used by tric_arith_CONST_INT.
(define_insn "insert_uconst_ic"
  [(set (match_operand:SI 0 "register_operand"                      "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"       "d")
                        (match_operand:SI 2 "const_int_operand"      "n"))
                (match_operand:SI 3 "const_int_operand"              "n")))]
  "((mPos = ones_mask (GET_MODE_MASK(SImode) & ~OPVAL(2), NULL))) >= 0
   && 0 == (GET_MODE_MASK(SImode) & OPVAL(2) & OPVAL(3))
   && 15 >= ((unsigned int) OPVAL(3)) >> mPos"
  {
    operands[3] = GEN_INT ((UOPVAL(3) & GET_MODE_MASK(SImode))
                           >> ones_mask (GET_MODE_MASK(SImode) & ~OPVAL(2), NULL));

    operands[2] = GEN_INT (~OPVAL(2));

    return "insert\t%0, %1, %3, %P2, %W2";
  }
  [(set_attr "pipe" "ip2")])


; "*jz_andsi3_extract.ic"
; "*jnz_andsi3_extract.ic"
; eq -> jump if bit is clear
; ne -> jump if bit is set
(define_insn "*j<tric_n_op>z_andsi3_extract.ic"
  [(set (pc)
        (if_then_else (eq_ne
                       (if_then_else:SI (zero_extract:SI (match_operand:SI 1 "register_operand"  "d")
                                                         (const_int 1)                                 ; width
                                                         (match_operand:SI 2 "const_int_operand" "n")) ; pos
                                        (match_operand:SI 3 "const_int_operand"                  "n")
                                        (const_int 0))
                       (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  "OPVAL(2) == exact_log2 (OPVAL(3))"
  "j<tric_n_op>z.t\t%1, %2, %0"
  [(set_attr "pipe" "jip")
   (set_attr "type" "branch_U4_d15")])

(define_insn "*ins.t.0.ic"
  [(set (match_operand:SI 0 "register_operand"                            "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"             "d")
                        (match_operand:SI 2 "const_int_operand"            "n"))
                (and:SI (ashift:SI (match_operand:SI 3 "register_operand"  "d")
                                   (match_operand:SI 4 "const_int_operand" "n"))
                        (match_operand:SI 5 "const_int_operand"            "n"))))]
  "OPVAL(4) >= 0
   && OPVAL(4) == exact_log2 (OPVAL(5))
   && OPVAL(4) == exact_log2 (~OPVAL(2))"
  "ins.t\t%0, %1,%4, %3,0"
  [(set_attr "pipe" "ip")])

(define_insn "*ins.t.1.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand" "+d")
                         (const_int 1)                                  ; width
                         (match_operand:SI 1 "u5_operand"       "Ku5")) ; pos
        (tric_shiftrt:SI (match_operand:SI 2 "register_operand" "d")
                         (match_operand:SI 3 "u5_operand"       "Ku5")))]
  ""
  "ins.t\t%0, %0,%1, %2,%3"
  [(set_attr "pipe" "ip")])

(define_insn "*ins.t.2.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand" "+d")
                         (const_int 1)                                    ; width
                         (match_operand:SI 1 "u5_operand"       "Ku5"))   ; pos
        (zero_extract:SI (match_operand:SI 2 "register_operand" "d")
                         (const_int 1)                                    ; width
                         (match_operand:SI 3 "u5_operand"       "Ku5")))] ; pos
  ""
  "ins.t\t%0, %0,%1, %2,%3"
  [(set_attr "pipe" "ip")])

(define_insn "*ins.t.3.ic"
  [(set (match_operand:SI 0 "register_operand"                               "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"                "d")
                        (match_operand:SI 2 "const_int_operand"               "n"))
                (and:SI (lshiftrt:SI (match_operand:SI 3 "register_operand"   "d")
                                     (match_operand:SI 4 "const_int_operand"  "n"))
                        (match_operand:SI 5 "const_int_operand"               "n"))))]
  "exact_log2 (OPVAL(5)) >= 0
   && GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(5) ^ OPVAL(2)))"
  "ins.t\t%0, %1,%P5, %3,%P5+%4"
  [(set_attr "pipe" "ip")])

; "*ins.t.4.qi.ic"
; "*ins.t.4.hi.ic"
(define_insn "*ins.t.4.<mode>.ic"
  [(set (match_operand:QIHI 0 "register_operand"                                            "=d")
        (ior:QIHI (and:QIHI (match_operand:HI 1 "register_operand"                           "d")
                            (match_operand:SI 2 "const_int_operand"                          "n"))
                  (if_then_else:QIHI
                   (subreg:QIHI (and:SI (lshiftrt:SI (match_operand:SI 3 "register_operand"  "d")
                                                     (match_operand:SI 4 "const_int_operand" "n"))
                                        (const_int 1)) 0)
                   (match_operand:SI 5 "const_int_operand"                                   "n")
                   (const_int 0))))]
  "exact_log2 (OPVAL(5)) >= 0
   && GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(5) ^ OPVAL(2)))
   && GET_MODE_BITSIZE(<MODE>mode) > exact_log2 (OPVAL(5))"
  "ins.t\t%0, %1,%P5, %3,%4"
  [(set_attr "pipe" "ip")])

;; Algebraically, this is identical to "insv".
;; However, #0 may be different from #4.
;; Used in insv-expander and tric_emit_arith_CONST_INT.
(define_insn "insv_3a_ic"
  [(set (match_operand:SI 0 "register_operand"                            "=d")
        (ior:SI (and:SI (ashift:SI (match_operand:SI 1 "register_operand"  "d")
                                   (match_operand:SI 2 "const_int_operand" "n"))
                        (match_operand:SI 3 "const_int_operand"            "n"))     ; 000..11..000
                (and:SI (match_operand:SI 4 "register_operand"             "d")
                        (match_operand:SI 5 "const_int_operand"            "n"))))]  ; 111..00..111
  "((mPos = ones_mask (GET_MODE_MASK (SImode) & OPVAL(3), NULL))) > 0
   && GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(3) ^ OPVAL(5)))
   && OPVAL(2) == mPos"
  "insert\t%0, %4, %1, %P3, %W3"
  [(set_attr "pipe" "ip2")])

;; dito, with args to IOR interchanged
(define_insn "*insv.3b.ic"
  [(set (match_operand:SI 0 "register_operand"                            "=d")
        (ior:SI (and:SI (match_operand:SI 4 "register_operand"             "d")
                        (match_operand:SI 5 "const_int_operand"            "n"))    ; 111..00..111
                (and:SI (ashift:SI (match_operand:SI 1 "register_operand"  "d")
                                   (match_operand:SI 2 "const_int_operand" "n"))
                        (match_operand:SI 3 "const_int_operand"            "n"))))] ; 000..11..000
  "((mPos = ones_mask (GET_MODE_MASK (SImode) & OPVAL(3), NULL))) > 0
   && GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(3) ^ OPVAL(5)))
   && OPVAL(2) == mPos"
  "insert\t%0, %4, %1, %P3, %W3"
  [(set_attr "pipe" "ip2")])

(define_insn "*insv.4.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"        "+d")
                         (match_operand:SI 1 "const_int_operand"        "n")  ; width
                         (match_operand:SI 2 "const_int_operand"        "n")) ; pos
        (tric_shiftrt:SI (match_operand:SI 3 "register_operand"         "d")
                         (match_operand:SI 4 "const_int_operand"        "n")))]
  "OPVAL(4) == OPVAL(2) && (OPVAL(1) == (32 - OPVAL(2)))"
  "insert\t%0, %3, %0, 0, %2"
  [(set_attr "pipe" "ip2")])

(define_insn "*insv.5.ic"
  [(set (match_operand:SI 0 "register_operand"                        "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"         "d")
                        (match_operand:SI 2 "const_int_operand"        "n"))
                (ashift:SI (match_operand:SI 3 "register_operand"      "d")
                           (match_operand:SI 4 "u5_operand"            "Ku5"))))]
  "ones_mask (OPVAL(2), &mWidth) == 0
   && mWidth == OPVAL(4)"
  "insert\t%0, %1, %3, %4, 32-%4"
  [(set_attr "pipe" "ip2")])

(define_insn "*insn.t.0.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"          "+d")
                         (const_int 1)                                             ; width
                         (match_operand:SI 1 "u5_operand"                "Ku5"))   ; pos
        (xor:SI (match_operand:SI 2 "register_operand"                   "d")
                (match_operand:SI 3 "const_int_operand"                  "n")))]
  "OPVAL(3) & 1"
  "insn.t\t%0, %0,%1, %2,0"
  [(set_attr "pipe" "ip")])

(define_insn "*insn.t.1.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"          "+d")
                         (const_int 1)                                             ; width
                         (match_operand:SI 1 "u5_operand"                "Ku5"))   ; pos
        (not:SI (match_operand:SI 2 "register_operand"                   "d")))]
  ""
  "insn.t\t%0, %0,%1, %2,0"
  [(set_attr "pipe" "ip")])

(define_insn "*insn.t.2.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"          "+d")
                         (const_int 1)                                             ; width1
                         (match_operand:SI 1 "u5_operand"                "Ku5"))   ; pos1
        (zero_extract:SI (xor:SI (match_operand:SI 2 "register_operand"  "d")
                                 (match_operand:SI 4 "const_int_operand" "n"))     ; 1 << pos2
                         (const_int 1)                                             ; width2
                         (match_operand:SI 3 "u5_operand"                "Ku5")))] ; pos2
  "OPVAL(4) & (1 << OPVAL(3))"
  "insn.t\t%0, %0,%1, %2,%3"
  [(set_attr "pipe" "ip")])

(define_insn "*insn.t.3.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"          "+d")
                         (const_int 1)                                             ; width
                         (match_operand:SI 1 "u5_operand"                "Ku5"))   ; pos
        (not:SI (tric_shiftrt:SI (match_operand:SI 2 "register_operand"  "d")
                                 (match_operand:SI 3 "u5_operand"        "Ku5"))))]
  ""
  "insn.t\t%0, %0,%1, %2,%3"
  [(set_attr "pipe" "ip")])

(define_insn "*insn.t.4.ic"
  [(set (match_operand:SI 0 "register_operand"                                        "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"                         "d")
                        (match_operand:SI 2 "const_int_operand"                        "n"))
                (and:SI (lshiftrt:SI (xor:SI (match_operand:SI 3 "register_operand"    "d")
                                             (match_operand:SI 4 "const_int_operand"   "n"))
                                     (match_operand:SI 5 "const_int_operand"           "n"))
                        (match_operand:SI 6 "const_int_operand"                        "n"))))]
  "exact_log2 (OPVAL(6)) >= 0
   && GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(6) ^ OPVAL(2)))
   && (OPVAL(4) & (OPVAL(6) << OPVAL(5)))"
  "insn.t\t%0, %1,%P6, %3,%P6+%5"
  [(set_attr "pipe" "ip")])

(define_insn "*nand.t.1.ic"
  [(set (match_operand:SI 0 "register_operand"                  "=d")
        (and:SI (not:SI (match_operand:SI 1 "register_operand"   "d"))
                (const_int 1)))]
  ""
  "nand.t\t%0, %1,0, %1,0"
  [(set_attr "pipe" "ip")])

; "*not_andsi3.ic"
; "*not_iorsi3.ic"
; "*not_xorsi3.ic"
(define_insn "*not_<code>si3.ic"
  [(set (match_operand:SI 0 "register_operand"                           "=d")
        (not:SI (tric_bitop:SI (match_operand:SI 1 "register_operand"    "%d")
                               (match_operand:SI 2 "reg_or_u9_operand"    "d Ku9"))))]
  ""
  "<tric_not_bitop_op>\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

; "*and_not.t.ic"
; "*ior_not.t.ic"
; "*xor_not.t.ic"
(define_insn "*<code>_not.t.ic"
  [(set (match_operand:SI 0 "register_operand"                                               "=d")
        (tric_bitop:SI (zero_extract:SI (xor:SI (match_operand:SI 1 "register_operand"        "d")
                                                (match_operand:SI 2 "const_int_operand"       "n"))
                                        (const_int 1)
                                        (match_operand:SI 3 "u5_operand"                      "Ku5"))
                       (zero_extract:SI (match_operand:SI 4 "register_operand"                "d")
                                        (const_int 1)
                                        (match_operand:SI 5 "u5_operand"                      "Ku5"))))]
  "OPVAL(2) & (1 << OPVAL(3))"
  "<tric_bitop_not_op>.t\t%0, %4,%5, %1,%3"
  [(set_attr "pipe" "ip")])

; "*and.t.ic"
; "*ior.t.ic"
; "*xor.t.ic"
(define_insn "*<tric_bitop:code>.t.ic"
  [(set (match_operand:SI 0 "register_operand"                                           "=d")
        (and:SI (tric_bitop:SI (tric_shiftrt:SI (match_operand:SI 1 "register_operand"    "d")
                                                (match_operand:SI 2 "u5_operand"          "Ku5"))
                               (tric_shiftrt2:SI (match_operand:SI 3 "register_operand"   "d")
                                                 (match_operand:SI 4 "u5_operand"         "Ku5")))
                (const_int 1)))]
  ""
  "<tric_op>.t\t%0, %1,%2, %3,%4"
  [(set_attr "pipe" "ip")])

; "*and-not.t.ic"
; "*ior-not.t.ic"
; "*xor-not.t.ic"
(define_insn "*<tric_bitop:code>-not.t.ic"
  [(set (match_operand:SI 0 "register_operand"                                                   "=d")
        (and:SI (tric_bitop:SI (tric_shiftrt:SI (xor:SI (match_operand:SI 1 "register_operand"    "d")
                                                        (match_operand:SI 2 "const_int_operand"   "n"))
                                                (match_operand:SI 3 "u5_operand"                  "Ku5"))
                               (tric_shiftrt2:SI (match_operand:SI 4 "register_operand"           "d")
                                                 (match_operand:SI 5 "u5_operand"                 "Ku5")))
                (const_int 1)))]
  "OPVAL(2) & (1 << OPVAL(3))"
  "<tric_bitop_not_op>.t\t%0, %4,%5, %1,%3"
  [(set_attr "pipe" "ip")])

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand" "")
                        (match_operand:SI 2 "const_int_operand" ""))   ;; 000..11..000
                (and:SI (match_operand:SI 3 "register_operand" "")
                        (match_operand:SI 4 "const_int_operand" "")))) ;; 111..00..111
   (clobber (match_operand:SI 5 "register_operand" ""))]
  "tric_opt_split_insns
   && !tric_map_combine
   && !reload_completed
   && ones_mask (GET_MODE_MASK (SImode) & OPVAL(2), NULL) > 0
   && GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(2) ^ OPVAL(4)))"
  [(set (match_dup 5)
        (lshiftrt:SI (match_dup 1)
                     (match_dup 6)))
   (set (match_dup 0)
        (ior:SI (and:SI (ashift:SI (match_dup 5)
                                   (match_dup 6))
                        (match_dup 2))
                (and:SI (match_dup 3)
                        (match_dup 4))))]
  {
    operands[6] = GEN_INT (ones_mask (OPVAL(2), NULL));
  })

;; dito, with args of IOR interchanged
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (ior:SI (and:SI (match_operand:SI 3 "register_operand" "")
                        (match_operand:SI 4 "const_int_operand" ""))   ;; 111..00..111
                (and:SI (match_operand:SI 1 "register_operand" "")
                        (match_operand:SI 2 "const_int_operand" "")))) ;; 000..11..000
   (clobber (match_operand:SI 5 "register_operand" ""))]
  "tric_opt_split_insns
   && !tric_map_combine
   && !reload_completed
   && ones_mask (GET_MODE_MASK (SImode) & OPVAL(2), NULL) > 0
   && GET_MODE_MASK(SImode) == (GET_MODE_MASK(SImode) & (OPVAL(2) ^ OPVAL(4)))"
  [(set (match_dup 5)
        (lshiftrt:SI (match_dup 1)
                     (match_dup 6)))
   (set (match_dup 0)
        (ior:SI (and:SI (ashift:SI (match_dup 5)
                                   (match_dup 6))
                        (match_dup 2))
                (and:SI (match_dup 3)
                        (match_dup 4))))]
  {
    operands[6] = GEN_INT (ones_mask (OPVAL(2), NULL));
  })


(define_insn_and_split "*split.acc-ss.{<tric_bitop:code>(<tric_scmp:code>,<tric_scmp2:code>)}.ic"
  [(set (match_operand:SI 0 "register_operand"                                   "=&d   ")
        (tric_bitop:SI (tric_scmp:SI (match_operand:SI 1 "register_operand"       "d    ")
                                     (match_operand:SI 2 "reg_or_s9_operand"      "d Ks9"))
                       (tric_scmp2:SI (match_operand:SI 3 "register_operand"      "d    ")
                                      (match_operand:SI 4 "reg_or_s9_operand"     "d Ks9"))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (tric_scmp:SI (match_dup 1)
                      (match_dup 2)))
   (set (zero_extract:SI (match_dup 0)
                         (const_int 1)
                         (const_int 0))
        (tric_bitop:SI (tric_scmp2:SI (match_dup 3)
                                      (match_dup 4))
                       (match_dup 0)))])

(define_insn_and_split "*split.acc-us.and(geu,ge).ic"
  [(set (match_operand:SI 0 "register_operand"                      "=d")
        (and:SI (geu:SI (match_operand:SI 1 "register_operand"       "d")
                        (match_operand:SI 2 "reg_or_u9_operand"      "d Ku9"))
                (ge:SI (match_operand:SI 3 "register_operand"        "d")
                       (const_int 32767))))]
  "!reload_completed"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 5)
        (const_int 32767))
   (set (match_dup 4)
        (geu:SI (match_dup 1)
                (match_dup 2)))
   (set (zero_extract:SI (match_dup 4)
                         (const_int 1)
                         (const_int 0))
        (and:SI (ge:SI (match_dup 3)
                       (match_dup 5))
                (match_dup 4)))
   (set (match_dup 0)
        (match_dup 4))]
  {
    operands[4] = gen_reg_rtx (SImode);
    operands[5] = gen_reg_rtx (SImode);
  })


(define_insn_and_split "*split.acc-su.{<tric_bitop:code>(<tric_scmp:code>,<tric_ucmp2:code>)}.ic"
  [(set (match_operand:SI 0 "register_operand"                                  "=&d   ")
        (tric_bitop:SI (tric_scmp:SI (match_operand:SI 1 "register_operand"      "d    ")
                                     (match_operand:SI 2 "reg_or_s9_operand"     "d Ks9"))
                       (tric_ucmp2:SI (match_operand:SI 3 "register_operand"     "d    ")
                                      (match_operand:SI 4 "reg_or_u9_operand"    "d Ku9"))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (tric_scmp:SI (match_dup 1)
                      (match_dup 2)))
   (set (zero_extract:SI (match_dup 0)
                         (const_int 1)
                         (const_int 0))
        (tric_bitop:SI (tric_ucmp2:SI (match_dup 3)
                                      (match_dup 4))
                       (match_dup 0)))])

(define_insn_and_split "*split.acc-us.{<tric_bitop:code>(<tric_ucmp:code>,<tric_scmp2:code>)}.ic"
  [(set (match_operand:SI 0 "register_operand"                                  "=&d   ")
        (tric_bitop:SI (tric_ucmp:SI (match_operand:SI 1 "register_operand"      "d    ")
                                     (match_operand:SI 2 "reg_or_u9_operand"     "d Ku9"))
                       (tric_scmp2:SI (match_operand:SI 3 "register_operand"     "d    ")
                                      (match_operand:SI 4 "reg_or_s9_operand"    "d Ks9"))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (tric_ucmp:SI (match_dup 1)
                      (match_dup 2)))
   (set (zero_extract:SI (match_dup 0)
                         (const_int 1)
                         (const_int 0))
        (tric_bitop:SI (tric_scmp2:SI (match_dup 3)
                                      (match_dup 4))
                       (match_dup 0)))])

(define_insn_and_split "*split.acc-uu.{<tric_bitop:code>(<tric_ucmp:code>,<tric_ucmp2:code>)}.ic"
  [(set (match_operand:SI 0 "register_operand"                                  "=&d   ")
        (tric_bitop:SI (tric_ucmp:SI (match_operand:SI 1 "register_operand"      "d    ")
                                     (match_operand:SI 2 "reg_or_u9_operand"     "d Ku9"))
                       (tric_ucmp2:SI (match_operand:SI 3 "register_operand"     "d    ")
                                      (match_operand:SI 4 "reg_or_u9_operand"    "d Ku9"))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (tric_ucmp:SI (match_dup 1)
                      (match_dup 2)))
   (set (zero_extract:SI (match_dup 0)
                         (const_int 1)
                         (const_int 0))
        (tric_bitop:SI (tric_ucmp2:SI (match_dup 3)
                                      (match_dup 4))
                       (match_dup 0)))])

(define_insn_and_split "*split.acc.and.xor.u.{<tric_ucmp:code>}.ic"
  [(set (match_operand:SI 0 "register_operand"                          "=&d   ")
        (and:SI (xor:SI (match_operand:SI 1 "register_operand"           "d    ")
                        (const_int 1))
                (tric_ucmp:SI (match_operand:SI 2 "register_operand"     "d    ")
                              (match_operand:SI 3 "reg_or_u9_operand"    "d Ku9"))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (zero_extract:SI (xor:SI (match_dup 1)
                                 (const_int 1))
                         (const_int 1)       ; width
                         (const_int 0)))     ; pos
   (set (zero_extract:SI (match_dup 0)
                         (const_int 1)
                         (const_int 0))
        (and:SI (tric_ucmp:SI (match_dup 2)
                              (match_dup 3))
                (match_dup 0)))])

(define_insn_and_split "*split.acc.and.xor.s.{<tric_scmp:code>}.ic"
  [(set (match_operand:SI 0 "register_operand"                          "=&d   ")
        (and:SI (xor:SI (match_operand:SI 1 "register_operand"           "d    ")
                        (const_int 1))
                (tric_scmp:SI (match_operand:SI 2 "register_operand"     "d    ")
                              (match_operand:SI 3 "reg_or_s9_operand"    "d Ks9"))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (zero_extract:SI (xor:SI (match_dup 1)
                                 (const_int 1))
                         (const_int 1)       ; width
                         (const_int 0)))     ; pos
   (set (zero_extract:SI (match_dup 0)
                         (const_int 1)
                         (const_int 0))
        (and:SI (tric_scmp:SI (match_dup 2)
                              (match_dup 3))
                (match_dup 0)))])

(define_insn "*cmp_acc_s{<tric_bitop:code>(<tric_scmp:code>)}.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"                  "+d")
                         (const_int 1)
                         (const_int 0))
        (tric_bitop:SI (tric_scmp:SI (match_operand:SI 1 "register_operand"      "d")
                                     (match_operand:SI 2 "reg_or_s9_operand"     "d Ks9"))
                       (match_dup 0)))]
  ""
  "<tric_bitop:tric_op>.<tric_scmp:tric_op>\t%0, %1, %2")

(define_insn "*cmp_acc_u{<tric_bitop:code>(<tric_ucmp:code>)}.ic"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"                  "+d")
                         (const_int 1)
                         (const_int 0))
        (tric_bitop:SI (tric_ucmp:SI (match_operand:SI 1 "register_operand"      "d")
                                     (match_operand:SI 2 "reg_or_u9_operand"     "d Ku9"))
                       (match_dup 0)))]
  ""
  "<tric_bitop:tric_op>.<tric_ucmp:tric_op>.u\t%0, %1, %2"
  [(set_attr "pipe" "ip")])


(define_insn "*ior_scmp{<code>}.ic"
  [(set (match_operand:SI 0 "register_operand"                           "=d")
        (ior:SI (tric_scmp:SI (match_operand:SI 1 "register_operand"      "d")
                              (match_operand:SI 2 "reg_or_s9_operand"     "d Ks9"))
                (match_operand:SI 3 "register_operand"                    "0")))]
  ""
  "or.<tric_op>\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

;; Dito, or-ops swapped
(define_insn "*ior_scmp{<code>}-2.ic"
  [(set (match_operand:SI 0 "register_operand"                           "=d")
        (ior:SI (match_operand:SI 3 "register_operand"                    "0")
                (tric_scmp:SI (match_operand:SI 1 "register_operand"      "d")
                              (match_operand:SI 2 "reg_or_s9_operand"     "d Ks9"))))]
  ""
  "or.<tric_op>\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "*ior_ucmp{<code>}.ic"
  [(set (match_operand:SI 0 "register_operand"                           "=d")
        (ior:SI (tric_ucmp:SI (match_operand:SI 1 "register_operand"      "d")
                              (match_operand:SI 2 "reg_or_u9_operand"     "d Ku9"))
                (match_operand:SI 3 "register_operand"                    "0")))]
  ""
  "or.<tric_op>.u\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

;; Dito, or-ops swapped
(define_insn "*ior_ucmp{<code>}-2.ic"
  [(set (match_operand:SI 0 "register_operand"                           "=d")
        (ior:SI (match_operand:SI 3 "register_operand"                    "0")
                (tric_ucmp:SI (match_operand:SI 1 "register_operand"      "d")
                              (match_operand:SI 2 "reg_or_u9_operand"     "d Ku9"))))]
  ""
  "or.<tric_op>.u\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

;; seen in libm (e_atan2, ...)
(define_insn "*bitopsi3{<code>}.bit31.ic"
  [(set (match_operand:SI 0 "register_operand"                             "=d")
        (lshiftrt:SI (tric_bitop:SI (match_operand:SI 1 "register_operand"  "d")
                                    (match_operand:SI 2 "register_operand"  "d"))
                     (const_int 31)))]
  ""
  "<tric_op>.t\t%0, %1,31, %2,31"
  [(set_attr "pipe" "ip")])

;; seen in libm (e_atan2, ...)
(define_insn "*or.bitopsi3{<code>}.bit31.ic"
  [(set (match_operand:SI 0 "register_operand"                                           "=d")
        (ior:SI (lshiftrt:SI (tric_bitop_andor:SI (match_operand:SI 1 "register_operand"  "d")
                                                  (match_operand:SI 2 "register_operand"  "d"))
                             (const_int 31))
                (match_operand:SI 3 "register_operand"                                    "0")))]
  ""
  "or.<tric_op>.t\t%0, %1,31, %2,31"
  [(set_attr "pipe" "ip")])

;; seen in libm (e_atan2, ...)
(define_insn "*or.bit31.ic"
  [(set (match_operand:SI 0 "register_operand"                      "=d")
        (ior:SI (lshiftrt:SI (match_operand:SI 1 "register_operand"  "d")
                             (const_int 31))
                (match_operand:SI 2 "register_operand"               "0")))]
  ""
  "or.lt\t%0, %1, 0"
  [(set_attr "pipe" "ip")])

;; seen in libm (e_atan2, ...)
(define_insn "*and.bit31.ic"
  [(set (match_operand:SI 0 "register_operand"                      "=d")
        (and:SI (lshiftrt:SI (match_operand:SI 1 "register_operand"  "d")
                             (const_int 31))
                (match_operand:SI 2 "register_operand"               "d")))]
  ""
  "and.t\t%0, %1,31, %2,0"
  [(set_attr "pipe" "ip")])


(define_insn_and_split "*dextr-reg.split-1.ic"
  [(set (match_operand:SI 0 "register_operand"                               "=&d")
        (ior:SI (lshiftrt:SI (match_operand:SI 1 "register_operand"            "d")
                             (match_operand:SI 2 "register_operand"            "d"))
                (ashift:SI (match_operand:SI 3 "register_operand"              "d")
                           (minus:SI (const_int 32)
                                     (match_dup 2)))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (minus:SI (const_int 32)
                  (match_dup 2)))
   (set (match_dup 0)
        (ior:SI (lshiftrt:SI (match_dup 1)
                             (minus:SI (const_int 32)
                                       (match_dup 0)))
                (ashift:SI (match_dup 3)
                           (match_dup 0))))])

;; same with ior-ops swapped
(define_insn_and_split "*dextr-reg.split-2.ic"
  [(set (match_operand:SI 0 "register_operand"                               "=&d")
        (ior:SI (ashift:SI (match_operand:SI 3 "register_operand"              "d")
                           (minus:SI (const_int 32)
                                     (match_operand:SI 2 "register_operand"    "d")))
                (lshiftrt:SI (match_operand:SI 1 "register_operand"            "d")
                             (match_dup 2))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (minus:SI (const_int 32)
                  (match_dup 2)))
   (set (match_dup 0)
        (ior:SI (lshiftrt:SI (match_dup 1)
                             (minus:SI (const_int 32)
                                       (match_dup 0)))
                (ashift:SI (match_dup 3)
                           (match_dup 0))))])

;; $0 = ($1 | -$1) >> 31
(define_insn "*ne0.dup1.ic"
  [(set (match_operand:SI 0 "register_operand"                               "=d")
        (lshiftrt:SI (ior:SI (neg:SI (match_operand:SI 1 "register_operand"   "d"))
                             (match_dup 1))
                     (const_int 31)))]
  ""
  "ne\t%0, %1, 0"
  [(set_attr "pipe" "ip")])

(define_insn_and_split "*ahlsi3.1.reg.ic"
  [(set (match_operand:SI 0 "register_operand"               "=&d")
        (ashift:SI (const_int 1)
                   (match_operand:SI 1 "register_operand"     "d")))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (const_int 1))
   (set (match_dup 0)
        (ashift:SI (match_dup 0)
                   (match_dup 1)))])


;; Some patterns that catch more cases where INSERT can be used:  Operands of
;; IOR don't overlap.  This saves one instruction at least.

(define_insn_and_split "*ior.shiftl.zerox.ic"
  [(set (match_operand:SI 0 "register_operand"                          "=d")
        (ior:SI (ashift:SI (match_operand:SI 1 "register_operand"        "d")
                           (match_operand:SI 2 "u5_operand"              "Ku5"))
                (zero_extract:SI (match_operand:SI 3 "register_operand"  "d")
                                 (match_operand:SI 4 "u5_operand"        "Ku5")
                                 (match_operand:SI 5 "u5_operand"        "Ku5"))))]
  "!reload_completed
   && OPVAL(2) >= OPVAL(4)"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 6)
        (zero_extract:SI (match_dup 3)
                         (match_dup 4)
                         (match_dup 5)))
   ;; "*insv.5.ic"
   (set (match_dup 0)
        (ior:SI (and:SI (match_dup 6)
                        (match_dup 7))
                (ashift:SI (match_dup 1)
                           (match_dup 2))))]
  {
    unsigned mask = (1 << OPVAL(2)) - 1;
    operands[6] = gen_reg_rtx (SImode);
    operands[7] = gen_int_mode (GET_MODE_MASK (SImode) & mask, SImode);
  })


;; Similar
(define_insn_and_split "*ior.shiftl.and.ic"
  [(set (match_operand:SI 0 "register_operand"                    "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"     "d")
                        (match_operand:SI 2 "const_int_operand"    "n"))
                (ashift:SI (match_operand:SI 3 "register_operand"  "d")
                           (match_operand:SI 4 "u5_operand"        "Ku5"))))]
  "!reload_completed
   && ones_mask (OPVAL(2), &mWidth) == 0
   /* == case is handled by *insv.5.ic already */
   && mWidth < OPVAL(4)"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 5)
        (and:SI (match_dup 1)
                (match_dup 2)))
   ;; "*insv.5.ic"
   (set (match_dup 0)
        (ior:SI (and:SI (match_dup 5)
                        (match_dup 6))
                (ashift:SI (match_dup 3)
                           (match_dup 4))))]
  {
    unsigned mask = (1 << OPVAL(4)) - 1;
    operands[5] = gen_reg_rtx (SImode);
    operands[6] = gen_int_mode (GET_MODE_MASK (SImode) & mask, SImode);
  })


;; Similar
(define_insn_and_split "*ior.shiftl.and.ic"
  [(set (match_operand:SI 0 "register_operand"                            "=d")
        (ior:SI (and:SI (match_operand:SI 1 "register_operand"             "d")
                        (match_operand:SI 2 "const_int_operand"            "n"))
                (and:SI (ashift:SI (match_operand:SI 3 "register_operand"  "d")
                                   (match_operand:SI 4 "u5_operand"        "Ku5"))
                        (match_operand:SI 5 "const_int_operand"            "n"))))]
  "!reload_completed
   && 0 == (GET_MODE_MASK (SImode) & OPVAL(2) & OPVAL(5))
   && ones_mask (OPVAL(2), NULL) == 0
   && ones_mask (OPVAL(5), NULL) == OPVAL(4)"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 6)
        (and:SI (match_dup 1)
                (match_dup 2)))
   ;; "insv_3a_ic"
   (set (match_dup 0)
        (ior:SI (and:SI (ashift:SI (match_dup 3)
                                   (match_dup 4))
                        (match_dup 5))
                (and:SI (match_dup 6)
                        (match_dup 7))))]
  {
    operands[6] = gen_reg_rtx (SImode);
    operands[7] = gen_int_mode (GET_MODE_MASK (SImode) & ~OPVAL(5), SImode);
  })


;; Similar
(define_insn_and_split "*ior.shiftl.zext<mode>.ic"
  [(set (match_operand:SI 0 "register_operand"                           "=d")
        (ior:SI (ashift:SI (match_operand:SI 1 "register_operand"         "d")
                           (match_operand:SI 2 "u5_operand"               "Ku5"))
                (zero_extend:SI (match_operand:QIHI 3 "register_operand"  "d"))))]
  "!reload_completed
   /* == case is already handled by *insv.5.ic or *insert.ashift.zeroxhi.ic */
   && OPVAL(2) > GET_MODE_BITSIZE (<MODE>mode)"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 4)
        (zero_extend:SI (match_dup 3)))
   ;; "*insv.5.ic"
   (set (match_dup 0)
        (ior:SI (and:SI (match_dup 4)
                        (match_dup 5))
                (ashift:SI (match_dup 1)
                           (match_dup 2))))]
  {
    unsigned mask = (1 << OPVAL(2)) - 1;
    operands[4] = gen_reg_rtx (SImode);
    operands[5] = gen_int_mode (GET_MODE_MASK (SImode) & mask, SImode);
  })


(define_insn "*absdifsi3.ic"
  [(set (match_operand:SI 0 "register_operand"                     "=d")
        (abs:SI (minus:SI (match_operand:SI 1 "register_operand"   "%d")
                          (match_operand:SI 2 "register_operand"    "d"))))]
  ""
  "absdif\t%0, %1, %2"
  [(set_attr "pipe" "ip2")])

(define_insn "*absdifsi3_const.ic"
  [(set (match_operand:SI 0 "register_operand"                     "=d")
        (abs:SI (plus:SI (match_operand:SI 1 "register_operand"    "%d")
                         (match_operand:SI 2 "m9_operand"           "Km9"))))]
  ""
  "absdif\t%0, %1, %n2"
  [(set_attr "pipe" "ip2")])

(define_insn "*abssatsi2.1.ic"
  [(set (match_operand:SI 0 "register_operand"                              "=d")
        (plus:SI (ashiftrt:SI (abs:SI (match_operand:SI 1 "register_operand" "d"))
                              (const_int 31))
                 (abs:SI (match_dup 1))))]
  ""
  "abss\t%0, %1"
  [(set_attr "pipe" "ip2")])

(define_insn "*abssatsi2.2.ic"
  [(set (match_operand:SI 0 "register_operand"                                      "=d")
        (plus:SI (if_then_else:SI (lt (abs:SI (match_operand:SI 1 "register_operand" "d"))
                                      (const_int 0))
                                  (const_int -1)
                                  (const_int 0))
                 (abs:SI (match_dup 1))))]
  ""
  "abss\t%0, %1"
  [(set_attr "pipe" "ip2")])


;; Applications do not necessarily saturate to -32768/32767.
;; Together with extensions to 32 bit the front end fails to
;; recognize mix/max and expands to LE/SEL resp. GE/SEL instead.
;; The expansion is superfluous because the value to saturate is known
;; to be in a specific range.
;; Again, we need bridges for the insn combiner to push it in the right
;; direction, and if things show up not to match we have to handle the mess
;; by hand...

(define_insn_and_split "*minmax.bridge.ic"
  [(set (match_operand:SI 0 "register_operand"                                        "=d")
        (if_then_else:SI (ge_geu_lt_ltu:SIVOID (match_operand:SI 1 "register_operand"  "d")
                                               (match_operand:SI 2 "const_int_operand" "n"))
                         (match_operand:SI 3 "register_operand"                        "d")
                         (match_operand:SI 4 "nonmemory_operand"                       "dn")))]
  "!reload_completed
   && (REG_P (operands[4])
       || (CONST_INT_P (operands[4])
           && abs_hwi (OPVAL(4) - OPVAL(2)) <= 1))"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [;; "*sge"   "*slt"
   ;; "*sgeu"  "*sltu"
   (set (match_dup 5)
        (ge_geu_lt_ltu:SI (match_dup 1)
                          (match_dup 2)))
   ;; "*movsicc.ne"
   (set (match_dup 0)
        (if_then_else:SI (ne (match_dup 5)
                             (const_int 0))
                         (match_dup 3)
                         (match_dup 4)))]
  {
    operands[5] = gen_reg_rtx (SImode);
  })


(define_insn_and_split "*usathi.bridge2.ic"
  [(set (match_operand:SI 0 "register_operand"                              "=d")
        (if_then_else:SI (ltu:SIVOID (match_operand:SI 1 "register_operand"  "d")
                                     (const_int 65536))
                         (zero_extend:SI (subreg:HI (match_dup 1) 0))
                         (match_operand:SI 2 "register_operand"              "d")))]
  "!reload_completed"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 3)
        (const_int 65536))
   (set (match_dup 4)
        (ltu:SI (match_dup 1)
                (match_dup 3)))
   (set (match_dup 0)
        (if_then_else:SI (ne (match_dup 4)
                             (const_int 0))
                         (match_dup 1)
                         (match_dup 2)))]
  {
    operands[3] = gen_reg_rtx (SImode);
    operands[4] = gen_reg_rtx (SImode);
  })

(define_insn_and_split "*usathi.plus"
  [(set (match_operand:SI 0 "register_operand"                                       "=d")
        (if_then_else:SI (ltu:SIVOID (plus:SI (match_operand:SI 1 "register_operand"  "d")
                                              (match_operand:SI 2 "register_operand"  "d"))
                                     (const_int 65536))
                         (zero_extend:SI (subreg:HI (plus:SI (match_dup 1)
                                                             (match_dup 2)) 0))
                         (const_int 65535)))]
  "!reload_completed"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 3)
        (plus:SI (match_dup 1)
                 (match_dup 2)))
   (set (match_dup 0)
        (umin:SI (match_dup 3)
                 (const_int 65535)))]
  {
    operands[3] = gen_reg_rtx (SImode);
  })


(define_insn_and_split "*usathi.bridge2.ic"
  [(set (match_operand:SI 0 "register_operand"                                             "=d")
        (if_then_else:SI (ne:SIVOID (match_operand:SI 1 "register_operand"                  "d")
                                    (const_int 0))
                         (zero_extend:SI (subreg:HI (match_operand:SI 2 "register_operand"  "d") 0))
                         (match_operand:SI 3 "register_operand"                             "d")))]
  "!reload_completed"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 4)
        (zero_extract:SI (match_dup 2)
                         (const_int 16)
                         (const_int 0)))
   (set (match_dup 0)
        (if_then_else:SI (ne (match_dup 1)
                             (const_int 0))
                         (match_dup 4)
                         (match_dup 3)))]
  {
    operands[4] = gen_reg_rtx (SImode);
  })


;; "umin_umaxsi4"  "umax_uminsi4"
;; "smin_smaxsi4"  "smax_sminsi4"
;; "umin_smaxsi4"  "smax_uminsi4"
;; "umin_smaxsi4"  "smax_uminsi4"
(define_insn_and_split "<minmax:code>_<minmax2:code>si4"
  [(set (match_operand:SI 0 "register_operand"                            "=d")
        (minmax2:SI (minmax:SI (match_operand:SI 1 "register_operand"      "d")
                               (match_operand:SI 2 "nonmemory_operand"     "dn"))
                    (match_operand:SI 3 "nonmemory_operand"                "dn")))]
  ""
  {
    if (tric_sat_p (QImode, <minmax:CODE>, operands[2], <minmax2:CODE>, operands[3]))
      return "sat.b\t%0, %1";
    else if (tric_sat_p (HImode, <minmax:CODE>, operands[2], <minmax2:CODE>, operands[3]))
      return "sat.h\t%0, %1";
    else
      gcc_unreachable();
  }
  "!reload_completed
   && !tric_map_combine"
  [(set (match_dup 4)
        (minmax:SI (match_dup 1)
                   (match_dup 2)))
   (set (match_dup 0)
        (minmax2:SI (match_dup 4)
                    (match_dup 3)))]
  {
    if (tric_sat_p (QImode, <minmax:CODE>, operands[2], <minmax2:CODE>, operands[3])
        || tric_sat_p (HImode, <minmax:CODE>, operands[2], <minmax2:CODE>, operands[3]))
      FAIL;

    // Just split the expression without caring for corner cases; the tree optimizers
    // should already have rectified these.  We only improve the case...

    operands[4] = gen_reg_rtx (SImode);

    HOST_WIDE_INT lower, upper;
    if (tric_sat_p (VOIDmode, <minmax:CODE>, operands[2], <minmax2:CODE>, operands[3],
                    &lower, &upper)
        && lower >= 0
        && upper <= INT_MAX)
      {
        // ...when we have a combination of SMIN and SMAX which comprises a signed
        // saturation to an interval where the lower bound is non-negative.  In that
        // case we may use UMIN instead of SMIN provided we first perform the SMAX
        // because the latter turns the value into a non-negative signed one.
        // UMIN is perferred over SMIN because it allows larger immediate values
        // 0..511  and  65535.  For 32767 and 127 SAT.H/.B is better than UMIN.

        emit_move_insn (operands[4], gen_rtx_SMAX (SImode, operands[1], GEN_INT (lower)));

        if (upper == 32767 || upper == 127)
          // SAT.H / SAT.B are in oder because $4 >= 0.
          emit_insn (gen_smax_sminsi4 (operands[0], operands[4],
                                       GEN_INT (-1 - upper), GEN_INT (upper)));
        else
          // (smin (smax ($1 $2) $3))  -->  (umin (smax ($1 $2) $3));  0 <= $2 <= $3  resp.
          // (smax (smin ($1 $2) $3))  -->  (umin (smax ($1 $3) $2));  0 <= $3 <= $2
          emit_move_insn (operands[0], gen_rtx_UMIN (SImode, operands[4], GEN_INT (upper)));
        DONE;
      }
  }
  [(set_attr "adjust" "sat")])


(define_insn_and_split ""
  [(set (match_operand:SI 0 "register_operand"                         "=&d")
        (lshiftrt:SI (match_operand:SI 1 "register_operand"              "d")
                     (minus:SI (const_int 32)
                               (match_operand:SI 2 "register_operand"    "d"))))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (plus:SI (match_dup 2)
                 (const_int -32)))
   (set (match_dup 0)
        (unspec:SI [(match_dup 1)
                    (match_dup 0)] UNSPEC_LSHR))])

;; Help to find DEXTR as in libgcc2.c:__divdi3
(define_insn_and_split ""
  [(set (match_operand:SI 0 "register_operand"                                 "=&d")
        (ior:SI (lshiftrt:SI (match_operand:SI 1 "register_operand"              "d")
                             (minus:SI (const_int 32)
                                       (match_operand:SI 2 "register_operand"    "d")))
                (match_operand:SI 3 "register_operand"                           "d")))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (plus:SI (match_dup 2)
                 (const_int -32)))
   (set (match_dup 0)
        (unspec:SI [(match_dup 1)
                    (match_dup 0)] UNSPEC_LSHR))
   (set (match_dup 0)
        (ior:SI (match_dup 0)
                (match_dup 3)))])

;; used in mod 2 for signed values
(define_insn "*mod2.ic"
  [(set (match_operand:SI 0 "register_operand"                                   "=d")
        (and:SI (plus:SI (tric_shiftrt:SI (match_operand:SI 1 "register_operand"  "d")
                                          (const_int 31))
                         (match_operand:SI 2 "register_operand"                   "d"))
                (const_int 1)))]
  ""
  "xor.t\t%0, %1,31, %2,0"
  [(set_attr "pipe" "ip")])

(define_insn "*extract-bit.ic"
  [(set (match_operand:SI 0 "register_operand"                                              "=d")
        (if_then_else:SI (eq:SIVOID (zero_extract:SI (match_operand:SI 1 "register_operand"  "d")
                                                     (const_int 1)  ;; width
                                                     (match_operand:SI 2 "u5_operand"        "Ku5"))
                                    (const_int 0))
                         (and:SI (match_dup 1)
                                 (match_operand:SI 3 "const_int_operand"                      "n"))
                         (const_int 1)))]
  "OPVAL (2) == exact_log2(OPVAL(3))"
  "extr.u\t%0, %1, %2, 1"
  [(set_attr "pipe" "ip2")])


(define_insn "*bridge.ic"
  [(set (match_operand:SI 0 "register_operand"                                               "=&d")
        (if_then_else:SI (ge:SIVOID (match_operand:SI 1 "register_operand"                     "d")
                                    (const_int -32768))
                         (sign_extend:SI (subreg:HI (match_dup 1) 0))
                         (const_int -32768)))]
  ""
  "mov\t%0, -32768\;jlt\t%1, %0, 0f\;extr\t%0, %1, 0, 16\;0:"
  [(set_attr "length" "12")])

;; $2 is a mask of the form 00..111..000 that
;; cannot be handled by "*andsi3". We implement this as
;; a splitting insn instead of decomposing already in
;; expand so that combiner will see the AND.
(define_insn_and_split  "andsi3_ones_split"
  [(set (match_operand:SI 0 "register_operand" "")
        (and:SI (match_operand:SI 1 "register_operand" "")
                (match_operand:SI 2 "const_int_operand" "")))
  (clobber (match_operand:SI 3 "scratch_operand" ""))]
  "tric_opt_split_insns
   && !reload_completed
   && !satisfies_constraint_Ksa (operands[2])
   && ((mPos = ones_mask (GET_MODE_MASK (SImode) & OPVAL(2), &mWidth))) >= 1
   && mPos + mWidth <= 31"
  {
    gcc_unreachable();
  }
  "&& !tric_map_combine"
  [(set (match_dup 3)
        (and:SI (match_dup 1)
                (match_dup 4)))
   (set (match_dup 0)
        (xor:SI (match_dup 3)
                (match_dup 1)))]
  {
    rtx lo, hi;

    operands[3] = gen_reg_rtx (SImode);

    if (1 == tric_split_const_int (operands[2], &lo, &hi, SET))
      {
        /* ; We can load the constant in one instruction.
           ; This is better than insert */
        emit_move_insn (operands[3], operands[2]);
        emit_move_insn (operands[0], gen_rtx_AND (SImode, operands[3],
                                                  operands[1]));

        DONE;
      }

    operands[4] = gen_int_mode (~OPVAL(2), SImode);
  })


;; $0 = ($1 & (1 << $2)) ? $3 : 0
(define_insn_and_split "*movcc-const.bit.1.ic"
  [(set (match_operand:SI 0 "register_operand"                                      "=d")
        (if_then_else:SI (eq_ne
                          (zero_extract:SI (match_operand:SI 1 "register_operand"    "d")
                                           (const_int 1)
                                           (match_operand:SI 2 "u5_operand"          "Ku5"))
                                               (const_int 0))
                         (match_operand:SI 3 "s10_operand"                           "Ku9")
                         (match_operand:SI 4 "s10_operand"                           "Ku9")))]
  "((NE == <CODE> && const0_rtx == operands[4])
       || (EQ == <CODE> && const0_rtx == operands[3]))"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (sign_extract:SI (match_dup 1)
                         (const_int 1)
                         (match_dup 2)))
   (set (match_dup 0)
        (and:SI (match_dup 0)
                (match_dup 3)))]
  {
    operands[3] = (NE == <CODE>) ? operands[3] : operands[4];
  })

;; $0 = ($1 & (1 << $2)) ? 0 : $3
(define_insn_and_split "*movcc-const.bit_ne0.2.ic"
  [(set (match_operand:SI 0                                       "register_operand"    "=d")
        (if_then_else:SI (ne (zero_extract:SI (match_operand:SI 1 "register_operand"     "d")
                                              (const_int 1)
                                              (match_operand:SI 2 "u5_operand"           "Ku5"))
                             (const_int 0))
                         (const_int 0)
                         (match_operand:SI 3 "c9_operand"                                "Kc9")))]
  ""
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (sign_extract:SI (match_dup 1)
                         (const_int 1)
                         (match_dup 2)))
   (set (match_dup 0)
        (not:SI (ior:SI (match_dup 0)
                        (match_dup 4))))]
  {
    operands[4] = gen_int_mode (~OPVAL (3), SImode);
  })

;; dito
;; $0 = ($1 & (1 << $2)) ? 0 : $3
(define_insn_and_split "*movcc-const.bit_eq0.2.ic"
  [(set (match_operand:SI 0 "register_operand"                                          "=d")
        (if_then_else:SI (eq (zero_extract:SI (match_operand:SI 1 "register_operand"     "d")
                                              (const_int 1)
                                              (match_operand:SI 2 "u5_operand"           "Ku5"))
                             (const_int 0))
                         (match_operand:SI 3 "c9_operand"         "Kc9")
                         (const_int 0)))]
  ""
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (sign_extract:SI (match_dup 1)
                         (const_int 1)
                         (match_dup 2)))
   (set (match_dup 0)
        (not:SI (ior:SI (match_dup 0)
                        (match_dup 4))))]
  {
    operands[4] = gen_int_mode (~OPVAL (3), SImode);
  })

;; $0 = ($1 & (1 << $2)) ? 0 : $3
(define_insn_and_split "*movcc-const.bit.3.ic"
  [(set (match_operand:SI 0 "register_operand"                                       "=d")
        (if_then_else:SI (eq_ne
                          (zero_extract:SI (match_operand:SI 1 "register_operand"     "d")
                                           (const_int 1)
                                           (match_operand:SI 2 "u5_operand"           "Ku5"))
                          (const_int 0))
                         (match_operand:SI 3 "s9_operand"                             "Ks9")
                         (match_operand:SI 4 "s9_operand"                             "Ks9")))]
  "(NE == <CODE> && const0_rtx == operands[3])
    || (EQ == <CODE> && const0_rtx == operands[4])"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (zero_extract:SI (not:SI (match_dup 1))
                         (const_int 1)
                         (match_dup 2)))
   (set (match_dup 0)
        (if_then_else:SI (eq (match_dup 0)
                             (const_int 0))
                         (match_dup 0)
                         (match_dup 3)))]
  {
    operands[3] = (EQ == <CODE>) ? operands[3] : operands[4];
  })

(define_insn "*extzv_not-bit.1.ic"
  [(set (match_operand:SI 0 "register_operand"                           "=d ")
        (zero_extract:SI (not:SI (match_operand:SI 1 "register_operand"   "d "))
                         (const_int 1)
                         (match_operand:SI 2 "u5_operand"                 "Ku5")))]  ; pos
  ""
  "nor.t\t%0, %1,%2, %1,%2"
  [(set_attr "pipe" "ip")])


(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (eq_ne:SIVOID (match_operand:SI 1 "register_operand" "")
                                       (const_int 0))
                         (match_operand:SI 2 "const_int_operand" "")
                         (match_operand:SI 3 "s16_operand" "")))
   (clobber (match_operand:SI 4 "register_operand" ""))]
  "tric_opt_split_insns
   && !tric_map_combine
   && !reload_completed"
  [(set (match_dup 4)
        (match_dup 3))
   (set (match_dup 0)
        (plus:SI (match_dup 4)
                 (if_then_else:SI (eq_ne (match_dup 1)
                                         (const_int 0))
                                  (match_dup 5)
                                  (const_int 0))))]
  {
    operands[5] = gen_int_mode (OPVAL(2) - OPVAL(3), SImode);

    if (!satisfies_constraint_Ks9 (operands[5]))
      FAIL;
  })

(define_insn_and_split ""
  [(set (match_operand:SI 0 "register_operand" "")
        (and:SI (tric_bitop:SI (lshiftrt:SI (match_operand:SI 1 "register_operand" "")
                                            (match_operand:SI 2 "register_operand" ""))
                               (match_operand:SI 3 "register_operand" ""))
                (const_int 1)))]
  "!reload_completed"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 4)
        (zero_extract:SI (match_dup 1)
                         (const_int 1)
                         (match_dup 2)))
   (set (match_dup 0)
        (and:SI (tric_bitop:SI (match_dup 4)
                               (match_dup 3))
                (const_int 1)))]
  {
    operands[4] = gen_reg_rtx (SImode);
  })


(define_insn "*eqsi3_zerox1"
  [(set (match_operand:SI 0 "register_operand" "=d")
        (eq:SI (zero_extract:SI (match_operand:SI 1 "register_operand" "d")
                                (const_int 1)
                                (match_operand:SI 2 "u5_operand"       "Ku5"))
               (zero_extract:SI (match_operand:SI 3 "register_operand" "d")
                                (const_int 1)
                                (match_operand:SI 4 "u5_operand"       "Ku5"))))]
  ""
  "xnor.t\t%0, %1,%2, %3,%4"
  [(set_attr "pipe" "ip")])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ST.T
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "*st.t.1.or{<mode>}"
  [(set (match_operand:QIHISI 0 "absolute_memory_operand"       "+Wz")
        (ior:QIHISI (match_dup 0)
                    (match_operand:QIHISI 1 "single_one_operand" "n")))]
  "optimize > 0"
  {
    rtx op[3];
    int bpos = exact_log2 (OPVAL(1) & GET_MODE_MASK (<MODE>mode));
    op[0] = operands[0];
    op[1] = GEN_INT (bpos / 8);
    op[2] = GEN_INT (bpos % 8);

    output_asm_insn ("st.t\t%0+%1,%2, 1", op);
    return "";
  })

(define_insn "*st.t.0.and{<mode>}"
  [(set (match_operand:QIHISI 0 "absolute_memory_operand"        "+Wz")
        (and:QIHISI (match_dup 0)
                    (match_operand:QIHISI 1 "single_zero_operand" "n")))]
  "optimize > 0"
  {
    rtx op[3];
    int bpos = exact_log2 (~OPVAL(1) & GET_MODE_MASK (<MODE>mode));
    op[0] = operands[0];
    op[1] = GEN_INT (bpos / 8);
    op[2] = GEN_INT (bpos % 8);

    output_asm_insn ("st.t\t%0+%1,%2, 0", op);
    return "";
  })

;; Map st.t for QI resp. HI to respective st.t from above
(define_insn_and_split "*st.t.1.or-subreg{<mode>}"
  [(set (match_operand:QIHI 0 "absolute_memory_operand"                "+Wz")
        (subreg:QIHI (ior:SI (subreg:SI (match_dup 0) 0)
                             (match_operand:QIHI 1 "single_one_operand" "n"))
                     0))]
  "optimize > 0"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (ior:QIHI (match_dup 0)
                  (match_dup 1)))])

;; Map st.t for QI resp. HI to respective st.t from above
(define_insn_and_split "*st.t.0.and-subreg{<mode>}"
  [(set (match_operand:QIHI 0 "absolute_memory_operand"                 "+Wz")
        (subreg:QIHI (and:SI (subreg:SI (match_dup 0) 0)
                             (match_operand:QIHI 1 "single_zero_operand" "n"))
                     0))]
  "optimize > 0"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (and:QIHI (match_dup 0)
                  (match_dup 1)))])

(define_insn "*st.t.insv{<mode>}"
  [(set (zero_extract:SI (match_operand:QIHISI 0 "absolute_memory_operand" "+Wz")
                         (const_int 1)                                              ;; width
                         (match_operand:SI 1 "u5_operand"                   "Ku5")) ;; pos
        (match_operand:SI 2 "const_int_operand"                             "n"))]
  "optimize > 0"
  {
    rtx op[4];
    int bpos = OPVAL(1);
    op[0] = operands[0];
    op[1] = GEN_INT (bpos / 8);
    op[2] = GEN_INT (bpos % 8);
    op[3] = operands[2];

    output_asm_insn ("st.t\t%0+%1,%2, %3", op);
    return "";
  })


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; LDMST
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn_and_split "*ldmst.or-ones.ic"
  [(set (match_operand:SI 0 "ldmst_operand"       "+m")
        (ior:SI (match_dup 0)
                (match_operand:SI 1 "ones_operand" "n")))]
  "optimize > 0
   && tric_gate_split1()
   /* Don't undo ST.T */
   && !(single_one_operand (operands[1], SImode)
        && absolute_memory_operand (operands[0], SImode))"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 3)
        (unspec:DI [(match_dup 5)  ;; val
                    (match_dup 2)  ;; pos
                    (match_dup 4)] ;; width
                   UNSPEC_IMASK))
   (parallel[(set (zero_extract:SI (match_dup 0)
                                   (match_dup 4)  ;; width
                                   (match_dup 2)) ;; pos
                  (match_dup 6))
             (use (match_dup 3))])]
  {
    operands[2] = GEN_INT (ones_mask (OPVAL(1) & GET_MODE_MASK (SImode),
                                      &mWidth));
    operands[3] = gen_reg_rtx (DImode);
    operands[4] = GEN_INT (mWidth);
    operands[6] = gen_int_mode ((1 << mWidth) -1, SImode);
    operands[5] = mWidth > 4 ? force_reg (SImode, constm1_rtx) : operands[6];
  })


(define_insn_and_split "*ldmst.or.ic"
  [(set (match_operand:SI 0 "ldmst_operand"            "+m")
        (ior:SI (match_dup 0)
                (match_operand:SI 1 "nonmemory_operand" "dn")))]
  "optimize > 0
   && tric_gate_split1()
   /* Don't undo ST.T */
   && !(single_one_operand (operands[1], SImode)
        && absolute_memory_operand (operands[0], SImode))"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 2)
        (mult:DI (zero_extend:DI (match_dup 1))
                 (const_int 4294967297))) ;; 2^32 + 1
   (parallel[(set (match_dup 0)
                  (ior:SI (match_dup 0)
                          (match_dup 3)))
             (use (match_dup 2))])]
  {
    operands[2] = gen_reg_rtx (DImode);

    if (CONST_INT_P (operands[1]))
      {
        operands[3] = operands[1];
      }
    else
      {
        operands[3] = simplify_gen_subreg (SImode, operands[2], DImode, 0);
        emit_move_insn (operands[3], operands[1]);
      }
    
    operands[1] = force_reg (SImode, operands[1]);
  })


(define_insn_and_split "*ldmst.and-zeros.ic"
  [(set (match_operand:SI 0 "ldmst_operand"        "+m")
        (and:SI (mem:SI (match_dup 0))
                (match_operand:SI 1 "zeros_operand" "n")))]
  "optimize > 0
   && tric_gate_split1()
   /* Don't undo ST.T */
   && !(single_zero_operand (operands[1], SImode)
        && absolute_memory_operand (operands[0], SImode))"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 3)
        (unspec:DI [(const_int 0)  ;; val
                    (match_dup 2)  ;; pos
                    (match_dup 4)] ;; width
                   UNSPEC_IMASK))
   (parallel[(set (zero_extract:SI (match_dup 0)
                                   (match_dup 4)  ;; width
                                   (match_dup 2)) ;; pos
                  (const_int 0))
             (use (match_dup 3))])]
  {
    operands[2] = GEN_INT (ones_mask (~OPVAL(1) & GET_MODE_MASK (SImode),
                                      &mWidth));
    operands[3] = gen_reg_rtx (DImode);
    operands[4] = GEN_INT (mWidth);
  })

(define_insn_and_split "*ldmst.and.ic"
  [(set (match_operand:SI 0 "ldmst_operand"            "+m")
        (and:SI (match_dup 0)
                (match_operand:SI 1 "nonmemory_operand" "d")))]
  "optimize > 0
   && tric_gate_split1()
   /* Don't undo ST.T */
   && !(single_zero_operand (operands[1], SImode)
        && absolute_memory_operand (operands[0], SImode))"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(parallel[(set (match_dup 0)
                  (and:SI (match_dup 0)
                          (not:SI (match_dup 3))))
             (use (match_dup 2))])]
  {
    operands[2] = gen_reg_rtx (DImode);
    operands[3] = simplify_gen_subreg (SImode, operands[2], DImode, 4);
    emit_move_insn (simplify_gen_subreg (SImode, operands[2], DImode, 0),
                                         const0_rtx);

    if (CONST_INT_P (operands[1]))
      {
        emit_move_insn (operands[3],
                        gen_int_mode (~INTVAL (operands[1]), SImode));
      }
    else
      {
        emit_move_insn (operands[3], gen_rtx_NOT (SImode, operands[1]));
      }
  })


(define_insn_and_split "*insv.mem"
  [(set (zero_extract:SI (match_operand:SI 0 "ldmst_operand"      "+Wl")
                         (match_operand:SI 1 "u5_operand"          "Ku5")  ;; width
                         (match_operand:SI 2 "u5_operand"          "Ku5")) ;; pos
        (match_operand:SI 3 "nonmemory_operand"                    "dn"))]
  "optimize > 0
   && tric_gate_split1()
   /* Don't undo ST.T */
   && !(const1_rtx == operands[1]
        && CONST_INT_P (operands[3])
        && absolute_memory_operand (operands[0], SImode))"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 4)
        (unspec:DI [(match_dup 3)  ;; val
                    (match_dup 2)  ;; pos
                    (match_dup 1)] ;; width
                   UNSPEC_IMASK))
   (parallel[(set (zero_extract:SI (match_dup 0)
                                   (match_dup 1)  ;; width
                                   (match_dup 2)) ;; pos
                  (match_dup 3))
             (use (match_dup 4))])]
  {
    if (!reg_or_u4_operand (operands[3], SImode))
      operands[3] = force_reg (SImode, operands[3]);

    operands[4] = gen_reg_rtx (DImode);
  })

(define_insn "*ldmst"
  [(set (zero_extract:SI (match_operand:SI 0 "ldmst_operand"      "+Wl,Wl")
                         (match_operand:SI 1 "u5_operand"          "n ,n")  ;; width
                         (match_operand:SI 2 "u5_operand"          "n ,n")) ;; pos
        (match_operand:SI 3 "nonmemory_operand"                    "d ,n"))
   (use (match_operand:DI 4 "register_operand"                     "d ,d"))]
  ""
  "ldmst\t%0, %A4"
  [(set_attr "ticks" "4")
   (set_attr "space" "4")
   (set_attr "pipe" "std")])

(define_insn "*ldmst.or"
  [(set (match_operand:SI 0 "ldmst_operand"            "+Wl")
        (ior:SI (match_dup 0)
                (match_operand:SI 1 ""                   "X")))
   (use (match_operand:DI 2 "register_operand"           "d"))]
  ""
  "ldmst\t%0, %A2"
  [(set_attr "ticks" "4")
   (set_attr "space" "4")
   (set_attr "pipe" "std")])

(define_insn "*ldmst.and"
  [(set (match_operand:SI 0 "ldmst_operand"                  "+Wl")
        (and:SI (match_dup 0)
                (not:SI (match_operand:SI 1 ""                 "X"))))
   (use (match_operand:DI 2 "register_operand"                 "d"))]
  ""
  "ldmst\t%0, %A2"
  [(set_attr "ticks" "4")
   (set_attr "space" "4")
   (set_attr "pipe" "std")])


(define_insn_and_split "*sh.or.extract1"
  [(set (match_operand:SI 0 "register_operand"                          "=d")
        (ior:SI (ashift:SI (match_operand:SI 1 "register_operand"        "0")
                           (const_int 1))
                (zero_extract:SI (match_operand:SI 2 "register_operand"  "d")
                                 (const_int 1)
                                 (match_operand:SI 3 "reg_or_u5_operand" "Ku5"))))]
  "can_create_pseudo_p()
   || CONST_INT_P (operands[3])"
  "sh.or.t\t %0, %2,%3, %2,%3"
  "&& !tric_map_combine
   && register_operand (operands[3], SImode)
   && can_create_pseudo_p()"
  [(set (match_dup 4)
        (zero_extract:SI (match_dup 2)
                    (const_int 1)
                    (match_dup 3)))
   (set (match_dup 0)
        (ior:SI (ashift:SI (match_dup 1)
                           (const_int 1))
                (zero_extract:SI (match_dup 4)
                                 (const_int 1)
                                 (const_int 0))))]
  {
    operands[4] = gen_reg_rtx (SImode);
  })
