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

;; Define constants for hard register numbers.

(define_constants
  [(REG_D0   0)   (REG_D1   1)   (REG_D2   2)   (REG_D3   3)
   (REG_D4   4)   (REG_D5   5)   (REG_D6   6)   (REG_D7   7)
   (REG_D8   8)   (REG_D9   9)   (REG_D10 10)   (REG_D11 11)
   (REG_D12 12)   (REG_D13 13)   (REG_D14 14)   (REG_D15 15)

   (REG_A0  16)   (REG_A1  17)   (REG_A2  18)   (REG_A3  19)
   (REG_A4  20)   (REG_A5  21)   (REG_A6  22)   (REG_A7  23)
   (REG_A8  24)   (REG_A9  25)   (REG_A10 26)   (REG_A11 27)
   (REG_A12 28)   (REG_A13 29)   (REG_A14 30)   (REG_A15 31)

   (REG_ARGP  32)
   (REG_PSW   33)
   ])

(define_constants
  [(REG_SP  REG_A10)
   (REG_FP  REG_A14)
   ;; REG_PIC must be a D register that survives function calls.
   (REG_PIC REG_D14)
   ])

;; Define unspec used to represent TriCore specific stuff

(define_c_enum "unspec"
  [UNSPEC_ADDX
   UNSPEC_ADDC
   UNSPEC_ASHR
   UNSPEC_CMP_F
   UNSPEC_CODE_PIC
   UNSPEC_COPYSIGN
   UNSPEC_DIV_INIT
   UNSPEC_DIV_STEP
   UNSPEC_DIV_ADJ
   UNSPEC_FCALL
   UNSPEC_FRAME_BLOCKAGE
   UNSPEC_IMASK
   UNSPEC_INSERT
   UNSPEC_LSHR
   UNSPEC_MAP_COMBINE
   UNSPEC_MEMSET
   UNSPEC_MOVMEM
   UNSPEC_MOVSTR
   UNSPEC_NEG
   UNSPEC_SUBX
   UNSPEC_SUBC
   UNSPEC_SYSCALL
   UNSPEC_TABLEJUMP
   UNSPEC_UDIV_INIT
   UNSPEC_UDIV_STEP
   ])

(define_c_enum "unspecv"
  [UNSPECV_CMPSWAP_W
   UNSPECV_LDMST
   UNSPECV_MEMORY_BARRIER
   UNSPECV_MFCR
   UNSPECV_NOP
   UNSPECV_SWAP_W
   UNSPECV_SWAPMSK_W
   ])


;; Define bit positions of result produced by cmp.f

(define_constants
  [(CMP_F_LT 0)
   (CMP_F_EQ 1)
   (CMP_F_GT 2)
   (CMP_F_UNORDERED 3)
   (CMP_F_DENORM_A  4)
   (CMP_F_DENORM_B  5)])


;; Instruction lengths

(define_attr "type"
  "branch0_U4_S8_d15, branch0_U4, jump, branch_s4, branch_u4, branch_U4_d15,
   loop1,
  other"
  (const_string "other"))

(define_attr "length" ""
  (cond [(and (ior (and (eq_attr "type" "branch0_U4_S8_d15")
                        (match_operand 1 "D15_reg_operand"))
                   (eq_attr "type" "jump"))
              (gt (minus (match_dup 0) (pc))
                  (const_int -258))
              (lt (minus (match_dup 0) (pc))
                  (const_int 254)))
         (const_int 2)

         (and (ior (eq_attr "type" "branch0_U4")
                   (eq_attr "type" "branch0_U4_S8_d15")
                   (and (eq_attr "type" "branch_U4_d15")
                        (match_operand 1 "D15_reg_operand")))
              (gt (minus (match_dup 0) (pc))
                  (const_int -2))
              (lt (minus (match_dup 0) (pc))
                  (const_int 30)))
         (const_int 2)

         (and (eq_attr "type" "loop1")
              (gt (minus (match_dup 1) (pc))
                  (const_int -32))
              (lt (minus (match_dup 1) (pc))
                  (const_int 0)))
         (const_int 2)

         (and (eq_attr "type" "branch_s4")
              (not (match_operand 2 "reg_or_s4_operand")))
         (const_int 6)
                       
         (and (eq_attr "type" "branch_u4")
              (not (match_operand 2 "reg_or_u4_operand")))
         (const_int 6)

         ] (const_int 4)))

;; Tells how insn length is to be adjusted.  Used in ADJUST_INSN_LENGTH
;; aka. tric_adjust_insn_length.

(define_attr "adjust"
  "mov64, mov32, mov16,mov16s, mov8,mov8s,
   add32, sub32, addsc,
   sat, bitop, seq,
  no"
  (const_string "no"))


;; Flavours of instruction set archirecture, used in enabled attribute

(define_attr "isa" "standard,
                    tc16, tc161, tc162, tc162up, tc16up,
                    tc13, tc131, tc13x,
                    cpu_tc081,no_cpu_tc081"
  (const_string "standard"))

;; Flavours of optimizations, used in enabled attribute

(define_attr "opt" "standard,size,speed"
  (const_string "standard"))


;; Sub-attribute of "enabled" below

(define_attr "enable_opt" ""
  (cond [(eq_attr "opt" "standard")
         (const_int 1)

         (and (eq_attr "opt" "size")
              (ne (symbol_ref "optimize_size")  (const_int 0)))
         (const_int 1)
         
         (and (eq_attr "opt" "speed")
              (eq (symbol_ref "optimize_size") (const_int 0)))
         (const_int 1)

         ] (const_int 0)))

;; Enabling/disabling insn alternatives.
;; Eventually, this attribute is a combination of "isa" and "opt".

(define_attr "enabled" ""
  (cond [(eq_attr "isa" "standard")
         (attr "enable_opt")
         
         (and (eq_attr "isa" "tc16")
              (ne (symbol_ref "TRIC_16")  (const_int 0)))
         (attr "enable_opt")
         
         (and (eq_attr "isa" "tc161")
              (ne (symbol_ref "TRIC_161")  (const_int 0)))
         (attr "enable_opt")
         
         (and (eq_attr "isa" "tc16up")
              (ne (symbol_ref "TRIC_16UP")  (const_int 0)))
         (attr "enable_opt")
         
         (and (eq_attr "isa" "tc162up")
              (ne (symbol_ref "TRIC_162UP")  (const_int 0)))
         (attr "enable_opt")
         
         (and (eq_attr "isa" "tc13x")
              (ne (symbol_ref "TRIC_13X") (const_int 0)))
         (attr "enable_opt")
         
         (and (eq_attr "isa" "tc131")
              (ne (symbol_ref "TRIC_131") (const_int 0)))
         (attr "enable_opt")
         
         (and (eq_attr "isa" "tc13")
              (ne (symbol_ref "TRIC_13")  (const_int 0)))
         (attr "enable_opt")

         (and (eq_attr "isa" "cpu_tc081")
              (ne (symbol_ref "TRIC_ERRATA_081")  (const_int 0)))
         (attr "enable_opt")
         
         (and (eq_attr "isa" "no_cpu_tc081")
              (eq (symbol_ref "TRIC_ERRATA_081")  (const_int 0)))
         (attr "enable_opt")

         ] (const_int 0)))



;; Specifying relative costs of insns.

;; The TARGET_RTX_COSTS hook queries the machine description
;; for RTX costs. This technique is intended to keep that hook
;; maintainable. Most insns will be matched without effort and
;; their attribute value for "space" resp. "ticks" will be used
;; to describe the rtx cost when optimizing for space resp. speed.
;;
;; However, there are cases where this doesn't work. These cases are
;;  -- Expanders which expand to other code than the hook is called with.
;;  -- Insns using (match_dup 0)
;; For example, div gets expanded to some weird construct describing the
;; instruction's effect on the registers involved rather than expanding to DIV.
;; However, in the startup phase and maybe later during CSE, rtx hook will get
;; called in order to compute costs for div rtx. Note that the costs of the
;; weird stuff is needed also.
;;
;; There are two ways to work around this:
;;  -- using C-code in the rtx cost hook
;;  -- using cost insns
;;
;; Cost insns are insns whose sole purpose is to provide rtx costs. They are
;; written in the same way as ordinary insns, with some differences
;;  -- cost insns must always have COST_INSN as their condition. This condition
;;     is enabled/disabled in the cost hook and ensures such insns just match
;;     during cost computation and nowhere else.
;;  -- cost insns must have COST_PATTERN as their C-snippet. This maps to an
;;     assertion that is always false.
;;  -- a cost insn should have "*@cost.foo" as its name if it computes costs
;;     for insn "foo".

;; The (expected) size in bytes if optimizing for size.

(define_attr "space" "" (const_int 4))

;; The (expected) time the instruction takes to perform in units of a MOV
;; instruction, where a mov is set to take 2 ticks. This allows a finer-grained
;; cost model.  For example, we set the speed-cost for MUL to 5 so that
;; x*15 gets compiled to
;;     SH  Da, Db, 4     # 2
;;     SUB Da, Db        # 2
;; whereas x*14 gets compiled to
;;     MUL Da, Db, 14    # 5
;; and not to 
;;     SH  Da, Db, 1     # 2
;;     SH  Dc, Db, 4     # 2
;;     SUB Dc, Da        # 2
;; which would happen if the cost of mul was 6.

(define_attr "ticks" "" (const_int 2))


;; Operand and operator predicates

(include "predicates.md")
(include "constraints.md")
(include "iterators.md")



(include "tricore-dfa.md")


;; Insns to load HIGH and LO_SUM
(define_insn "movsi_high"
  [(set (match_operand:SI 0 "register_operand"            "=*d,*a")
        (high:SI (match_operand:SI 1 "immediate_operand"   "i ,i ")))]
  ""
  "@
    movh\t%0, hi:%1
    movh.a\t%0, hi:%1"
  [(set_attr "pipe" "ip,aalu")])

(define_insn "addsi_low"
  [(set (match_operand:SI 0 "register_operand"             "=*d,*d,*a")
        (lo_sum:SI (match_operand:SI 1 "register_operand"   "0 ,d ,a")
                   (match_operand:SI 2 "immediate_operand"  "n ,i ,i")))]
  ""
  {
    static const char * asmcode[] =
    {
      "addi\t%0, %0, lo:%2",
      "addi\t%0, %1, lo:%2",
      "lea\t%0, [%1] lo:%2",
    };

    /* When splitting DI constants, we can do better in some rare cases */
    if (0 == which_alternative)
      {
        rtx xval = gen_int_mode (OPVAL(2), HImode);
        if (xval == const0_rtx)
          return "";
        else if (satisfies_constraint_Ks4 (xval))
          {
            operands[2] = xval;
            return "add\t%0, %2";
          }
      }

    return asmcode[which_alternative];
  }
  [(set_attr "ticks" "4")
   (set_attr "pipe" "ip,ip,aalu")])

;;; Move instructions

;; "movqi" "movhi" "movsi"
(define_expand "mov<mode>"
  [(set (match_operand:QIHISI 0 "nonimmediate_operand" "")
        (match_operand:QIHISI 1 "general_operand" ""))]
  ""
  {
    if (tric_emit_move (operands, <MODE>mode))
      DONE;
  })

(define_expand "movsf"
  [(set (match_operand:SF 0 "nonimmediate_operand" "")
        (match_operand:SF 1 "general_operand" ""))]
  ""
  {
    if (tric_emit_move (operands, SFmode))
      DONE;
  })

(define_insn "*movsi_insn"
  [(set (match_operand:SI 0 "nonimmediate_operand" "=d    ,a,a  ,!*????a,!*????d,Wm,d ,D ,Wa,a ,RnS,A ,D  ,a ,a     ,d  ,d  ,a  ,d  ,*a ,!a,!d")
        (match_operand:SI 1 "mov_input_operand"     "d Ks4,a,Ku4,d      ,a      ,d ,Wm,Wm,a ,Wm,Wm ,Wm,Ku8,Ss,Sa KSa,Ksg,Kug,Khg,Khg,Khi,n ,n "))]
  "tric_mov_operands (operands, SImode)"
  "@
    mov\t%0, %1
    mov.aa\t%0, %1
    mov.a\t%0, %1
    mov.a\t%0, %1
    mov.d\t%0, %1
    st.w\t%0, %1
    ld.w\t%0, %1
    ld.w\t%0, %1
    st.a\t%0, %1
    ld.a\t%0, %1
    ld.a\t%0, %1
    ld.a\t%0, %1
    mov\t%0, %1
    lea\t%0, [%%A0] SM:%1
    lea\t%0, %1
    mov\t%0, %1
    mov.u\t%0, %1
    movh.a\t%0, %B1
    movh\t%0, %B1
    lha\t%0, %1
    #
    #"
  [(set_attr "pipe" "ip,aalu,aalu,mixds,mixdd,std,ldd,ldd,sta,lda,lda,lda,ip,aalu,aalu,ip,ip,aalu,ip,aalu,*,*")
   (set_attr "isa" "*,*,*,*,*,*,*,*,*,no_cpu_tc081,cpu_tc081,*,*,*,*,*,*,*,*,tc162up,*,*")
   (set_attr "length" "2,2,2,2,2,*,*,*,*,*,*,*,2,4,4,4,4,4,4,4,*,*")
   (set_attr "adjust" "*,*,*,*,*,mov32,mov32,mov32,mov32,mov32,mov32,mov32,*,*,*,*,*,*,*,*,mov32,mov32")])

(define_split
  [(set (match_operand:SI 0 "register_operand"  "")
        (match_operand:SI 1 "immediate_operand" ""))]
  "reload_completed
   && tric_anchor_completed()
   && tric_split_mov_insn_p (operands, SImode)"
  [(set (match_dup 0)
        (high:SI (match_dup 1)))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))]
  {
    if (CONST_INT_P (operands[1]))
      {
        tric_emit_move_CONST_INT (operands[0], operands[1]);
        DONE;
      }
  })

(define_insn_and_split "*movsf_insn"
  [(set (match_operand:SF 0 "nonimmediate_operand" "=d    ,a,*!????a,*!????d,Wm,d ,Wa,a  ,d  ,d  ,a  ,d,a")
        (match_operand:SF 1 "mov_input_operand"     "d G00,a,d      ,a      ,d ,Wm,a ,Wm ,Gsg,Ghg,Ghg,i,i"))]
  "tric_mov_operands (operands, SFmode)"
  "@
    mov\t%0, %1
    mov.aa\t%0, %1
    mov.a\t%0, %1
    mov.d\t%0, %1
    st.w\t%0, %1
    ld.w\t%0, %1
    st.a\t%0, %1
    ld.a\t%0, %1
    mov\t%0, %1
    movh\t%0, %B1
    movh.a\t%0, %B1
    movh\t%0, hi:%1\;addi\t%0, %0, lo:%1
    movh.a\t%0, hi:%1\;lea\t%0, [%0] lo:%1"
  "&& reload_completed
   && const_double_operand (operands[1], SFmode)"
  [(const_int 1)]
  {
    if (satisfies_constraint_Gsg (operands[1])
        || satisfies_constraint_Ghg (operands[1]))
      {
        FAIL;
      }

    tric_emit_move_CONST_INT (operands[0], operands[1]);
    DONE;
  }
  [(set_attr "pipe" "ip,aalu,mixds,mixdd,std,ldd,sta,lda,ip,ip,aalu,ip,aalu")
   (set_attr "length" "2,2,2,2,*,*,*,*,4,4,4,8,8")
   (set_attr "adjust" "*,*,*,*,mov32,mov32,mov32,mov32,*,*,*,*,*")])

(define_insn "*movqi_insn"
  [(set (match_operand:QI 0 "nonimmediate_operand" "=d,a,*!????a,*!????d,m,d ,D  ,d  ,d  ,a  ,a  ,*a")
        (match_operand:QI 1 "general_operand"       "d,a,d      ,a      ,d,m ,Ku8,Ksg,Kug,Ku4,KSa,i"))]
  "tric_mov_operands (operands,QImode)"
  "@
    mov\t%0, %1
    mov.aa\t%0, %1
    mov.a\t%0, %1
    mov.d\t%0, %1
    st.b\t%0, %1
    ld.b\t%0, %1
    mov\t%0, %1
    mov\t%0, %1
    mov.u\t%0, %1
    mov.a\t%0, %1
    lea\t%0, %1
    mov.a\t%0, 0\;lea\t%0, [%0] lo:%1"
  [(set_attr "pipe" "ip,aalu,mixds,mixdd,std,ldd,ip,ip,ip,aalu,aalu,aalu")
   (set_attr "length" "2,2,2,2,*,*,2,*,4,2,4,6")
   (set_attr "adjust" "*,*,*,*,mov8,mov8s,*,mov8,*,*,*,*")])

(define_insn "*movhi_insn"
  [(set (match_operand:HI 0 "nonimmediate_operand" "=d,a,*!????d,*!????a,m,d ,D  ,d  ,d  ,a  ,a  ,a  ,a")
        (match_operand:HI 1 "general_operand"       "d,a,a      ,d      ,d,m ,Ku8,Ksg,Kug,Ku4,KSa,Khi,i"))]
  "tric_mov_operands (operands,HImode)"
  "@
    mov\t%0, %1
    mov.aa\t%0, %1
    mov.d\t%0, %1
    mov.a\t%0, %1
    st.h\t%0, %1
    ld.h\t%0, %1
    mov\t%0, %1
    mov\t%0, %1
    mov.u\t%0, %1
    mov.a\t%0, %1
    lea\t%0, %1
    lha\t%0, %1
    movh.a\t%0, hi:%1\;lea\t%0, [%0] lo:%1"
  [(set_attr "pipe"   "ip,aalu,mixds,mixdd,std,ldd,ip,ip,ip,aalu,aalu,aalu,aalu")
   (set_attr "isa"    "*,*,*,*,*,*,*,*,*,*,*,tc162up,*")
   (set_attr "length" "2,2,2,2,*,*,2,4,4,2,*,*,8")
   (set_attr "adjust" "*,*,*,*,mov16,mov16s,*,*,*,*,*,*,*")])

(define_expand "movhf"
  [(set (match_operand:HF 0 "nonimmediate_operand")
        (match_operand:HF 1 "general_operand"))]
  ""
  {
    if (tric_emit_move (operands, HFmode))
      DONE;
  })

(define_insn "*movhf_insn"
  [(set (match_operand:HF 0 "nonimmediate_operand" "=d,Wm,d ,d  ,a,d,a,*a  ,*a  ,*a")
        (match_operand:HF 1 "general_operand"       "d,d ,Wm,Gsg,a,a,d,Gu4 ,GSa ,i"))]
  "tric_mov_operands (operands,HFmode)"
  "@
    mov\t%0, %1
    st.h\t%0, %1
    ld.h\t%0, %1
    mov\t%0, %1
    mov.aa\t%0, %1
    mov.d\t%0, %1
    mov.a\t%0, %1
    mov.a\t%0, %1
    lea\t%0, %1
    movh.a\t%0, hi:%1\;lea\t%0, [%0] lo:%1"
  [(set_attr "pipe"   "ip,std,ldd,ip,aalu,ip,ip,aalu,aalu,aalu")
   (set_attr "length" "2,4,4,4,2,2,2,2,4,8")
   (set_attr "adjust" "*,mov16,mov16s,mov16s,*,*,*,*,*,*")])


(define_insn "truncsfhf2"
  [(set (match_operand:HF 0 "register_operand" "=d")
        (float_truncate:HF (match_operand:SF 1 "register_operand" "d")))
   (clobber (reg:SI REG_PSW))]
  "TRIC_HAVE_HP_CONVERT"
  "ftohp\t%0, %1"
  [(set_attr "pipe"   "fp")
   (set_attr "length" "4")])

(define_insn "extendhfsf2"
  [(set (match_operand:SF 0 "register_operand" "=d")
        (float_extend:SF (match_operand:HF 1 "register_operand" "d")))
   (clobber (reg:SI REG_PSW))]
  "TRIC_HAVE_HP_CONVERT"
  "hptof\t%0, %1"
  [(set_attr "pipe"   "fp")
   (set_attr "length" "4")])

;; Fixed <--> Floating conversion insns

;; "floatsihf2"
;; "floatdihf2"
(define_expand "float<mode>hf2"
  [(set (match_operand:HF 0 "general_operand")
        (float:HF (match_operand:SIDI 1 "general_operand")))]
  ""
  {
    rtx op1 = gen_reg_rtx (SFmode);
    expand_float (op1, operands[1], 0);
    op1 = convert_to_mode (HFmode, op1, 0);
    emit_move_insn (operands[0], op1);
    DONE;
  })

;; "fix_trunchfsi2"
;; "fix_trunchfdi2"
(define_expand "fix_trunchf<mode>2"
  [(set (match_operand:SIDI 0 "general_operand")
        (fix:SI (fix:HF (match_operand:HF 1 "general_operand"))))]
  ""
  {
    rtx op1 = convert_to_mode (SFmode, operands[1], 0);
    expand_fix (operands[0], op1, 0);
    DONE;
  })

;; DFmode -> HFmode conversions have to go through SFmode.
(define_expand "truncdfhf2"
  [(set (match_operand:HF 0 "general_operand" "")
        (float_truncate:HF (match_operand:DF 1 "general_operand" "")))]
  ""
  {
    rtx op1;
    op1 = convert_to_mode (SFmode, operands[1], 0);
    op1 = convert_to_mode (HFmode, op1, 0);
    emit_move_insn (operands[0], op1);
    DONE;
  })

;; HFmode -> DFmode conversions have to go through SFmode.
(define_expand "extendhfdf2"
  [(set (match_operand:DF 0 "general_operand")
        (float_extend:DF (match_operand:HF 1 "general_operand"  "")))]
  ""
  {
    rtx op1;
    op1 = convert_to_mode (SFmode, operands[1], 0);
    op1 = convert_to_mode (DFmode, op1, 0);
    emit_insn (gen_movdf (operands[0], op1));
    DONE;
  })

(define_expand "addsi3"
  [(set (match_operand:SI 0 "register_operand" "")
        (plus:SI (match_operand:SI 1 "register_operand" "")
                 (match_operand:SI 2 "nonmemory_operand" "")))]
  ""
  {
    if (!add_input_operand (operands[2], SImode))
    {
      if (CONST_INT_P (operands[2]))
        {
          rtx part1, part2;
          tric_split_const_int (operands[2], &part1, &part2, PLUS);

          emit_move_insn (operands[0],
                          gen_rtx_PLUS (SImode, operands[1], part1));
          if (NULL_RTX != part2)
            emit_move_insn (operands[0],
                            gen_rtx_PLUS (SImode, operands[0], part2));
          DONE;
        }
      operands[2] = copy_to_mode_reg (SImode, operands[2]);
    }
  })

;; Note that split2 won't generate a-a-d alternative
(define_insn "*addsi3"
  [(set (match_operand:SI 0 "register_operand"          "=d    ,d,D    ,d    ,d  ,d     ,d,!d,!*RSP,a    ,a,a,!a,a  ,a     ,a,??a,a,a,??a,a")
        (plus:SI (match_operand:SI 1 "register_operand" "%0    ,d,d    ,D    ,d  ,d     ,d,d ,0    ,0    ,a,a ,a,a  ,a     ,a  ,a,a,d  ,d,D")
                 (match_operand:SI 2 "add_input_operand" "d Ks4,0,d Ks4,d Ks4,Ksg,Khg Sh,d,d ,Kn8  ,a Ks4,0,a ,a,Ksg,Khg Sh,d  ,d,D,a  ,a,a")))]
  "!tric_star
   && add_input_operand (operands[1], SImode)"
  "@
    add\t%0, %2
    add\t%0, %1
    add\t%0, %1, %2
    add\t%0, %1, %2
    addi\t%0, %1, %2
    addih\t%0, %1, %B2
    add\t%0, %1, %2
    add\t%0, %1, %2
    sub.a\t%0, %n2
    add.a\t%0, %2
    add.a\t%0, %1
    add.a\t%0, %1, %2
    add.a\t%0, %1, %2
    lea\t%0, [%1] %2
    addih.a\t%0, %1, %B2
    addsc.a\t%0, %1, %2, 0
    addsc.a\t%0, %1, %2, 0
    addsc.a\t%0, %1, %2, 0
    addsc.a\t%0, %2, %1, 0
    addsc.a\t%0, %2, %1, 0
    addsc.a\t%0, %2, %1, 0"
  [(set_attr "opt" "*,*,*,*,*,*,speed,size,*,*,*,speed,size,*,*,speed,size,*,speed,size,*")
   (set_attr "pipe" "ip,ip,ip,ip,ip,ip,ip,ip,aalu,aalu,aalu,aalu,aalu,aalu,aalu,mixds,mixds,mixds,mixds,mixds,mixds")
   (set_attr "adjust" "add32")])

(define_insn "*addsi3"
  [(set (match_operand:SI 0 "register_operand"          "=*d   ,*d ,*d    ,!*RSP,*a ,*a ,*a ,*a    ,*a ,*a")
        (plus:SI (match_operand:SI 1 "register_operand" "%d    ,d  ,d     ,0    ,0  ,a  ,a  ,a     ,a  ,d ")
                 (match_operand:SI 2 "add_input_operand" "d Ks4,Ksg,Khg Sh,Kn8  ,Ks4,a  ,Ksg,Khg Sh,d  ,a ")))]
  "tric_star
   && add_input_operand (operands[1], SImode)"
  "@
    add\t%0, %1, %2
    addi\t%0, %1, %2
    addih\t%0, %1, %B2
    sub.a\t%0, %n2
    add.a\t%0, %2
    add.a\t%0, %1, %2
    lea\t%0, [%1] %2
    addih.a\t%0, %1, %B2
    addsc.a\t%0, %1, %2, 0
    addsc.a\t%0, %2, %1, 0"
  [(set_attr "pipe" "ip,ip,ip,aalu,aalu,aalu,aalu,aalu,mixds,mixds")
   (set_attr "adjust" "add32")])

;; ??? Some passes leave a mess when lowering DI operations to SI.
;; We fix one case here but note that there are many places that
;; get compiler suboptimal, see _mulhelperUDA from fixed-bit.c.

(define_split ; "*addsi3_0"
  [(set (match_operand:SI 0 "register_operand"          "")
        (plus:SI (match_operand:SI 1 "register_operand" "")
                 (const_int 0)))]
  "!tric_map_combine"
  [(set (match_dup 0)
        (match_dup 1))])


(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand"              "=d ,D ,d ,d ,!d ,d   ,a")
        (minus:SI (match_operand:SI 1 "reg_or_s9_operand"    "0 ,d ,D ,d ,d  ,Ks9 ,a")
                  (match_operand:SI 2 "register_operand"     "d ,d ,d ,d ,d  ,d   ,a")))]
  ""
  "@
    sub\t%0, %2
    sub\t%0, %1, %2
    sub\t%0, %1, %2
    sub\t%0, %1, %2
    sub\t%0, %1, %2
    rsub\t%0, %2, %1
    sub.a\t%0, %1, %2"
  [(set_attr "opt" "*,*,*,speed,size,*,*")
   (set_attr "pipe" "ip,ip,ip,ip,ip,ip,aalu")
   (set_attr "adjust" "sub32")])

(define_insn "load_pc"
  [(set (reg:SI REG_A11)
        (unspec:SI [(const_int 0)] UNSPEC_CODE_PIC))
   (use (match_operand:SI 0 "" ""))]
  "tric_opt_code_pic && tric_opt_dynamic_code_pic"
  "jl\t%0\n%0:"
  [(set_attr "pipe" "ctx")
   (set_attr "length" "4")])

(define_insn "*add_pic_low"
  [(set (match_operand:SI 0 "register_operand"              "=a,d")
        (unspec:SI [(match_operand:SI 1 "register_operand"   "a,d")
                    (match_operand:SI 2 "symbolic_operand"   "s,s")
                    ] UNSPEC_CODE_PIC))]
  "tric_opt_code_pic && tric_opt_dynamic_code_pic"
  "@
	lea\t%0, [%1] lo:%2
	addi\t%0, %1, lo:%2"
  [(set_attr "pipe" "aalu,ip")])

(define_insn_and_split "load_pic_symbol"
  [(set (match_operand:SI 0 "register_operand"             "=a,d")
        (unspec:SI [(reg:SI REG_PIC)                               ;; code offset
                    (match_operand:SI 1 "symbolic_operand"  "s,s") ;; input to be offsetted
                    (match_operand:SI 2 "symbolic_operand"  "s,s") ;; .PICOFF<N>
                    ] UNSPEC_CODE_PIC))]
  "tric_opt_code_pic
   && tric_opt_dynamic_code_pic"
  "#"
  "&& !tric_map_combine"
  [(set (match_dup 0)
        (high:SI (match_dup 1)))
   ;; "*add_pic_low"
   (set (match_dup 0)
        (unspec:SI [(match_dup 0)
                    (match_dup 1)] UNSPEC_CODE_PIC))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (reg:SI REG_PIC)))])


(define_expand "movsicc"
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (match_operand 1 "" "")
                         (match_operand:SI 2 "register_operand" "")
                         (match_operand:SI 3 "reg_or_s9_operand" "")))]
  ""
  {
    enum rtx_code code = GET_CODE (operands[1]);

    if ((NE != code && EQ != code)
        || const0_rtx != XEXP (operands[1], 1)
        || !register_operand (XEXP (operands[1], 0), SImode))
      {
        rtx reg = gen_reg_rtx (SImode);
        rtx op0 = XEXP (operands[1], 0);
        rtx op1 = XEXP (operands[1], 1);
        
        if (SImode == GET_MODE (op0)  ||  SImode == GET_MODE (op1))
          tric_emit_setcompare (code, op0, op1, reg);
        else
          FAIL;
        operands[1] = gen_rtx_NE (VOIDmode, reg, const0_rtx);
      }
  })


;; "*movsicc.eq"
;; "*movsicc.ne"
(define_insn "*movsicc.<code>"
  [(set (match_operand:SI 0 "register_operand"         "=d     ,d ,d")
        (if_then_else:SI
         (eq_ne (match_operand:SI 1 "register_operand"  "D     ,D ,d")
                (const_int 0))
         (match_operand:SI 2 "register_operand"         "0     ,d ,d")
         (match_operand:SI 3 "nonmemory_operand"        "d Ks4 ,0 ,d Ks9")))]
  ""
  "@
    cmov<tric_n_op>\t%0, %1, %3
    cmov<tric_n_not_op>\t%0, %1, %2
    sel<tric_n_not_op>\t%0, %1, %2, %3"
  [(set_attr "pipe" "ip")
   (set_attr "length" "2,2,4")])


;; "*movsfcc.eq"
;; "*movsfcc.ne"
(define_insn "*movsfcc.eq"
  [(set (match_operand:SF 0 "register_operand"         "=d    ,d ,d")
        (if_then_else:SF
         (eq_ne (match_operand:SI 1 "register_operand"  "D    ,D ,d")
                (const_int 0))
         (match_operand:SF 2 "register_operand"         "0    ,d ,d")
         (match_operand:SF 3 "reg_or_0_operand"         "d G00,0 ,d G00")))]
  ""
  "@
    cmov<tric_n_op>\t%0, %1, %3
    cmov<tric_n_not_op>\t%0, %1, %2
    sel<tric_n_not_op>\t%0, %1, %2, %3"
  [(set_attr "pipe" "ip")
   (set_attr "length" "2,2,4")])


(define_expand "addsicc"
  [(set (match_operand:SI 0 "register_operand" "")
        (plus:SI (match_operand:SI 2 "register_operand" "")
                 (if_then_else:SI (match_operand 1 "" "")
                                  (match_operand:SI 3 "reg_or_s9_operand" "")
                                  (const_int 0))))]
  ""
  {
    enum rtx_code code = GET_CODE (operands[1]);
    if ((NE != code && EQ != code)
        || const0_rtx != XEXP (operands[1], 1)
        || !register_operand (XEXP (operands[1], 0), SImode))
      {
        rtx reg = gen_reg_rtx (SImode);
        tric_emit_setcompare (code, XEXP (operands[1], 0),
                              XEXP (operands[1], 1), reg);
        operands[1] = gen_rtx_fmt_ee (NE, VOIDmode, reg, const0_rtx);
      }
  })


;; "*addsicc.eq"
;; "*addsicc.ne"
(define_insn "*addsicc.<code>"
  [(set (match_operand:SI 0 "register_operand"                                         "=d   ,d")
        (plus:SI (match_operand:SI 2 "register_operand"                                 "0   ,d")
                 (if_then_else:SI (eq_ne:SIVOID (match_operand:SI 1 "register_operand"  "D   ,d")
                                                (const_int 0))
                                  (match_operand:SI 3 "reg_or_s9_operand"               "Ks4 ,d Ks9")
                                  (const_int 0))))]
  ""
  "@
    cadd<tric_n_not_op>\t%0, %1, %3
    cadd<tric_n_not_op>\t%0, %1, %2, %3"
  [(set_attr "pipe" "ip")
   (set_attr "length" "2,4")])


(define_insn "*addsicc.<code>.2"
  [(set (match_operand:SI 0 "register_operand"                    "=d ,d")
        (plus:SI (eq_ne:SI (match_operand:SI 1 "register_operand"  "D ,d")
                           (const_int 0))
                 (match_operand:SI 2 "register_operand"            "0 ,d")))]
  ""
  "@
    cadd<tric_n_not_op>\t%0, %1, 1
    cadd<tric_n_not_op>\t%0, %1, %2, 1"
  [(set_attr "pipe" "ip")
   (set_attr "length" "2,4")])

;; "movdi"
;; "movdf"
;; "movpdi"
(define_expand "mov<mode>"
  [(set (match_operand:DIPDIDF 0 "nonimmediate_operand" "")
        (match_operand:DIPDIDF 1 "general_operand" ""))]
  ""
  {
    /* One of the ops has to be in a register.  */
    if (!register_operand (operands[0], <MODE>mode)
        && !register_operand (operands[1], <MODE>mode))
      {
        gcc_assert (can_create_pseudo_p());
        operands[1] = copy_to_mode_reg (<MODE>mode, operands[1]);
      }
  })

;; "*movdi_insn"
;; "*movdf_insn"
;; "*movpdi_insn"
(define_insn_and_split "*mov<mode>_insn"
  [(set (match_operand:DIPDIDF 0 "nonimmediate_operand" "=d,d ,m,m ,d,a ,d  ,d  ,d   ,d  ,a,*!???d,*!???a ,da")
        (match_operand:DIPDIDF 1 "general_operand"       "d,d ,d,a ,m,m ,Ksg,Ksg,G00 ,Kug,a,a     ,d      ,n E"))]
  "tric_mov_operands (operands, <MODE>mode)"
  "@
    #
    mov\t%A0, %H1, %L1
    st.d\t%0, %A1
    st.da\t%0, %A1
    ld.d\t%A0, %1
    ld.da\t%A0, %1
    #
    mov\t%A0, %1
    mov\t%A0, 0
    #
    #
    #
    #
    #"
  "reload_completed
   && REG_P (operands[0])
   && nonmemory_operand (operands[1], <MODE>mode)"
  [(set (match_dup 2)
        (match_dup 4))
   (set (match_dup 3)
        (match_dup 5))]
  {
    if (TRIC_HAVE_MOV64
        && E_REG_P (operands[0])
        && (E_REG_P (operands[1])
            || satisfies_constraint_Ksg (operands[1])
            || operands[1] == CONST0_RTX (<MODE>mode)))
      // No need to split these 64-bit moves as hardware supports them.
      FAIL;

    // For the # alternatives.  Constant loads have already been handled by pass tric-anchor.
    operands[2] = simplify_gen_subreg (SImode, operands[0], <MODE>mode, 0);
    operands[3] = simplify_gen_subreg (SImode, operands[0], <MODE>mode, 4);
    operands[4] = simplify_gen_subreg (SImode, operands[1], <MODE>mode, 0);
    operands[5] = simplify_gen_subreg (SImode, operands[1], <MODE>mode, 4);
  }
  [(set_attr "isa" "tc13x,tc16up,*,*,*,*,tc13x,tc16up,tc16up,*,*,*,*,*")
   (set_attr "ticks" "8")
   (set_attr "space" "12")
   (set_attr "pipe" "*,ip2,std,sta,ldd,lda,*,ip2,ip2,*,*,*,*,*")
   (set_attr "adjust" "*,*,*,*,*,*,*,mov64,mov64,*,*,*,*,*")])


(define_expand "imask2"
  [(set (match_dup 4)
        (match_operand:SI 1 "register_operand" ""))  ; val
   (set (match_dup 5)                                ; mask
        (match_operand:SI 2 "register_operand" ""))
   (set (match_operand:DI 0 "register_operand" "")
        (match_dup 3))]                              ; mask:val
  ""
  {
    operands[3] = gen_reg_rtx (DImode);
    operands[4] = simplify_gen_subreg (SImode, operands[3], DImode, 0);
    operands[5] = simplify_gen_subreg (SImode, operands[3], DImode, 4);
  })

(define_expand "imask"
  [(set (match_operand:DI 0 "register_operand" "")
        (unspec:DI [(match_operand:SI 1 "register_operand" "")   ; val
                    (match_operand:SI 2 "nonmemory_operand" "")  ; pos
                    (match_operand:SI 3 "nonmemory_operand" "")] ; width
                   UNSPEC_IMASK))]
  ""
  {
    tric_emit_imask (operands);
    DONE;
  })


(define_insn "imaskdi"
  [(set (match_operand:DI 0 "register_operand"               "=d")
        (unspec:DI [(match_operand:SI 1 "reg_or_u4_operand"   "Ku4 d") ; const4 = val
                    (match_operand:SI 2 "reg_or_u5_operand"   "Ku5 d") ; pos
                    (match_operand:SI 3 "u5_operand"          "Ku5")]  ; width
                   UNSPEC_IMASK))]
  ""
  "imask\t%A0, %1, %2, %3"
  [(set_attr "pipe" "ip2")])

(define_expand "truncdipdi2"
  [(parallel [(match_operand:PDI 0 "register_operand" "")
              (match_operand:DI 1  "register_operand" "")])]
  ""
  {
    emit_move_insn (operands[0],
                    simplify_gen_subreg (PDImode, operands[1], DImode, 0));
    DONE;
  })

(define_expand "truncpdidi2"
  [(parallel [(match_operand:DI  0 "register_operand" "")
              (match_operand:PDI 1  "register_operand" "")])]
  ""
  {
    emit_move_insn (operands[0],
                    simplify_gen_subreg (DImode, operands[1], PDImode, 0));
    DONE;
  })

(define_expand "extendpdidi2"
  [(parallel [(match_operand:DI  0 "register_operand" "")
              (match_operand:PDI 1 "register_operand" "")])]
  ""
  {
    emit_move_insn (operands[0],
                    simplify_gen_subreg (DImode, operands[1], PDImode, 0));
    DONE;
  })

(define_expand "zero_extendpdidi2"
  [(parallel [(match_operand:DI  0 "register_operand" "")
              (match_operand:PDI 1 "register_operand" "")])]
  ""
  {
    emit_move_insn (operands[0],
                    simplify_gen_subreg (DImode, operands[1], PDImode, 0));
    DONE;
  })

(define_insn_and_split "zero_extendsidi2"
  [(set (match_operand:DI 0 "register_operand"                 "=d,d")
        (zero_extend:DI (match_operand:SI 1 "register_operand"  "0,d")))]
  ""
  "@
	mov\t%H0, 0
	mul.u\t%A0, %1, 1"
  "reload_completed
   && (TRIC_13
       || REGNO (operands[0]) == REGNO (operands[1]))"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 2)
        (const_int 0))]
  {
    operands[2] = simplify_gen_subreg (SImode, operands[0], DImode, 4);
    operands[0] = simplify_gen_subreg (SImode, operands[0], DImode, 0);
  }
  [(set_attr "pipe" "ip,ipm")])

(define_insn "extendsidi2"
  [(set (match_operand:DI 0 "register_operand"                 "=d,d,d")
        (sign_extend:DI (match_operand:SI 1 "register_operand"  "d,0,d")))]
  ""
  "@
    mov\t%A0, %1
    shas\t%H0, %L0, -31
    mul\t%A0, %1, 1"
  [(set_attr "isa" "tc16up,tc13x,tc13x")
   (set_attr "pipe" "ip2,ip,ipm")])

(define_insn_and_split "<signed_prefix_su>sum_widendi3"
  [(set (match_operand:DI 0 "register_operand"                          "=d")
        (plus:DI (any_extend:DI (match_operand:SI 1 "register_operand"   "d"))
                 (match_operand:DI 2 "register_operand"                  "d")))]
  "!reload_completed"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  ;; "maddsidi4_const"
  ;; "umaddsidi4_const"
  [(set (match_dup 0)
        (plus:DI (mult:DI (any_extend:DI (match_dup 1))
                          (const_int 1))
                 (match_dup 2)))]
  ""
  [(set_attr "ticks" "3")])


(define_insn_and_split "adddi3"
  [(set (match_operand:DI 0 "register_operand"           "=d")
        (plus:DI (match_operand:DI 1 "register_operand"   "d")
                 (match_operand:DI 2 "nonmemory_operand"  "dn")))
   (clobber (reg:SI REG_PSW))]
  "tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 0))]
  {
    rtx lo[3], hi[3];

    tric_split_di (operands, lo, hi, 3);

    if (lo[2] == const0_rtx)
      {
        emit_move_insn (lo[0], lo[1]);
        emit_insn (gen_addsi3 (hi[0], hi[1], hi[2]));
        DONE;
      }

    if (!reg_or_s9_operand (lo[2], SImode))
      lo[2] = force_reg (SImode, lo[2]);

    if (!reg_or_s9_operand (hi[2], SImode))
      hi[2] = force_reg (SImode, hi[2]);

    emit_insn (gen_addx (lo[0], lo[1], lo[2]));
    emit_insn (gen_addc (hi[0], hi[1], hi[2]));
    DONE;
  }
  [(set_attr "ticks" "4")
   (set_attr "space" "8")
   (set_attr "length" "8")])


(define_insn "addx"
  [(set (match_operand:SI 0 "register_operand"                     "=d")
        (unspec:SI [(match_operand:SI 1 "register_operand"          "d")
                    (match_operand:SI 2 "reg_or_s9_operand"         "d Ks9")
                    ] UNSPEC_ADDX))
   (set (reg:SI REG_PSW)
        (unspec:SI [(match_dup 1)
                    (match_dup 2)] UNSPEC_ADDC))]
  ""
  "addx\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "addc"
  [(set (match_operand:SI 0 "register_operand"                     "=d")
        (unspec:SI [(match_operand:SI 1 "register_operand"          "d")
                    (match_operand:SI 2 "reg_or_s9_operand"         "d Ks9")
                    (reg:SI REG_PSW)] UNSPEC_ADDC))
   (set (reg:SI REG_PSW)
        (unspec:SI [(match_dup 1)
                    (match_dup 2)] UNSPEC_ADDC))]
  ""
  "addc\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "subx"
  [(set (match_operand:SI 0 "register_operand"                     "=d")
        (unspec:SI [(match_operand:SI 1 "register_operand"          "d")
                    (match_operand:SI 2 "register_operand"          "d")
                    ] UNSPEC_SUBX))
   (set (reg:SI REG_PSW)
        (unspec:SI [(match_dup 1)
                    (match_dup 2)] UNSPEC_SUBC))]
  ""
  "subx\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "subc"
  [(set (match_operand:SI 0 "register_operand"                     "=d")
        (unspec:SI [(match_operand:SI 1 "register_operand"          "d")
                    (match_operand:SI 2 "register_operand"          "d")
                    (reg:SI REG_PSW)] UNSPEC_SUBC))
   (set (reg:SI REG_PSW)
        (unspec:SI [(match_dup 1)
                    (match_dup 2)] UNSPEC_SUBC))]
  ""
  "subc\t%0, %1, %2"
  [(set_attr "pipe" "ip")])


(define_insn_and_split "subdi3"
  [(set (match_operand:DI 0 "register_operand"            "=d")
        (minus:DI (match_operand:DI 1 "register_operand"   "d")
                  (match_operand:DI 2 "register_operand"   "d")))
   (clobber (reg:SI REG_PSW))]
  ""
  "subx\t%L0, %L1, %L2\;subc\t%H0, %H1, %H2"
  "reload_completed"
  [(clobber (const_int 1))]
  {
    rtx lo[3], hi[3];

    tric_split_di (operands, lo, hi, 3);
    emit_insn (gen_subx (lo[0], lo[1], lo[2]));
    emit_insn (gen_subc (hi[0], hi[1], hi[2]));
    DONE;
  }
  [(set_attr "length" "8")])


(define_insn_and_split "negdi2"
  [(set (match_operand:DI 0 "register_operand"           "=d")
        (neg:DI (match_operand:DI 1 "register_operand"    "d")))]
  "tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 4)
        (neg:SI (match_dup 5)))
   (set (match_dup 2)
        (neg:SI (match_dup 3)))
   (set (match_dup 4)
        (plus:SI (match_dup 4)
                 (if_then_else:SI (ne:SI (match_dup 3)
                                         (const_int 0))
                                  (const_int -1)
                                  (const_int 0))))]
  {
    tric_split_di (operands, operands + 2, operands + 4, 2);
  }
  [(set_attr "ticks" "6")
   (set_attr "space" "10")])


(define_expand "ashldi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (ashift:DI (match_operand:DI 1 "register_operand" "")
                   (match_operand:SI 2 "nonmemory_operand" "")))]
  ""
  {
    if (CONST_INT_P (operands[2]))
      operands[2] = GEN_INT (UOPVAL(2) % 64);
    else if (optimize_size)
      FAIL;
  })

(define_insn_and_split "*ashldi3"
  [(set (match_operand:DI 0 "register_operand"            "=d")
        (ashift:DI (match_operand:DI 1 "register_operand"  "d")
                   (match_operand:SI 2 "nonmemory_operand" "nd")))]
  ""
  { gcc_unreachable(); }
  "!reload_completed"
  [(clobber (const_int 1))]
  {
    tric_emit_ashift64 (operands);
    DONE;
  })


(define_insn_and_split "*unegsidi2"
  [(set (match_operand:DI 0 "register_operand"                        "=d")
        (neg:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "d"))))]
  "tric_gate_split1 ()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(set (match_dup 2)
        (neg:SI (match_dup 1)))
   ;; "*movsicc.eq"
   ;; hi = lo == 0 ? lo : -1
   (set (match_dup 3)
        (if_then_else:SI (eq (match_dup 2)
                             (const_int 0))
                         (match_dup 2)
                         (const_int -1)))]
  {
    tric_split_di (operands, operands + 2, operands + 3, 1);
  })


(define_expand "lshrdi3"
  [(set (match_operand:DI 0 "register_operand"               "")
        (lshiftrt:DI (match_operand:DI 1 "register_operand"  "")
                     (match_operand:SI 2 "nonmemory_operand" "")))]
  ""
  {
    if (CONST_INT_P (operands[2]))
      operands[2] = GEN_INT (UOPVAL(2) % 64);
    else if (optimize_size)
      FAIL;
  })


(define_insn_and_split "*lshrdi3"
  [(set (match_operand:DI 0 "register_operand"              "=d")
        (lshiftrt:DI (match_operand:DI 1 "register_operand"  "d")
                     (match_operand:SI 2 "nonmemory_operand" "dn")))]
  "!reload_completed"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    tric_emit_lshiftrt64 (operands);
    DONE;
  })

;; dp-bit.c:_unpack_df
(define_insn "*extr.u-subreg.di-hi"
  [(set (match_operand:SI 0 "register_operand" "=d")
        (and:SI (subreg:SI (lshiftrt:DI (match_operand:DI 1 "register_operand" "d")
                                        (match_operand:DI 2 "u6_operand" "n")) 0)
                (match_operand:SI 3 "const_int_operand" "n")))]
  "OPVAL(2) >= 32
   && ones_mask (GET_MODE_MASK (SImode) & OPVAL(3), NULL) == 0"
  "extr.u\t%0, %H1, %2-32, %W3"
  [(set_attr "pipe" "ip2")])

(define_expand "ashrdi3"
  [(parallel [(set (match_operand:DI 0 "register_operand" "")
                   (ashiftrt:DI (match_operand:DI 1 "register_operand" "")
                                (match_operand:SI 2 "nonmemory_operand" "")))
              (clobber (reg:SI REG_PSW))])]
  ""
  {
    if (CONST_INT_P (operands[2]))
      {
        operands[2] = GEN_INT (UOPVAL(2) % 64);

        if (32 == UOPVAL(2)
            && TRIC_HAVE_MOV64)
          {
            emit_insn (gen_ashrdi3_32 (operands[0], operands[1]));
            DONE;
          }
      }
    else if (optimize_size)
      FAIL;
  })

(define_insn_and_split "*ashrdi3"
  [(set (match_operand:DI 0 "register_operand"              "=d")
        (ashiftrt:DI (match_operand:DI 1 "register_operand"  "d")
                     (match_operand:SI 2 "nonmemory_operand" "dn")))
   (clobber (reg:SI REG_PSW))]
  "!reload_completed"
  "#"
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    tric_emit_ashiftrt64 (operands);
    DONE;
  })

(define_insn "dextr_const"
  [(set (match_operand:SI 0 "register_operand"                          "=d  ")
        (ior:SI (lshiftrt:SI (match_operand:SI 1 "register_operand"      "d  ")
                             (match_operand:SI 2 "u5_operand"            "Ku5"))
                (ashift:SI (match_operand:SI 3 "register_operand"        "d  ")
                           (match_operand:SI 4 "u5_operand"              "Ku5"))))]
  "32 == OPVAL(2) + OPVAL(4)"
  "dextr\t%0, %3, %1, %4"
  [(set_attr "pipe" "ip2")])


(define_insn "dextr_reg"
  [(set (match_operand:SI 0 "register_operand"                                "=d")
        (ior:SI (lshiftrt:SI (match_operand:SI 1 "register_operand"            "d")
                             (minus:SI (const_int 32)
                                       (match_operand:SI 2 "register_operand"  "d")))
                (ashift:SI (match_operand:SI 3 "register_operand"              "d")
                           (match_dup 2))))]
  ""
  "dextr\t%0, %3, %1, %2")


(define_expand "rotldi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (rotate:DI (match_operand:DI 1 "register_operand" "")
                   (match_operand:DI 2 "const_int_operand" "")))]
  "TRIC_HAVE_MOV64"
  {
    if (!CONST_INT_P (operands[2])
        || 32 != OPVAL(2))
      FAIL;
  })

(define_insn "ashrdi3_32"
  [(set (match_operand:DI 0 "register_operand"              "=d")
        (ashiftrt:DI (match_operand:DI 1 "register_operand"  "d")
                     (const_int 32)))]
  "TRIC_HAVE_MOV64"
  "mov\t%A0, %H1"
  [(set_attr "pipe" "ip2")])

(define_insn "*rotatedi3_32"
  [(set (match_operand:DI 0 "register_operand"            "=d")
        (rotate:DI (match_operand:DI 1 "register_operand"  "d")
                   (const_int 32)))]
  "TRIC_HAVE_MOV64"
  "mov\t%A0, %L1, %H1"
  [(set_attr "pipe" "ip2")])

;; "andsi3" "iorsi3" "xorsi3"
(define_expand "<code>si3"
  [(set (match_operand:SI 0 "register_operand" "")
        (tric_bitop:SI (match_operand:SI 1 "register_operand"  "")
                       (match_operand:SI 2 "nonmemory_operand" "")))]
  ""
  {
    if (!register_operand (operands[1], SImode))
      operands[1] = copy_to_mode_reg (SImode, operands[1]);

    if (AND == <CODE>
        && optimize && tric_opt_split_insns
        && CONST_INT_P (operands[2])
        && ((mPos = ones_mask (GET_MODE_MASK (SImode) & OPVAL(2), &mWidth))) >= 1
        && !reg_or_s10_operand (operands[2], SImode)
        && mPos + mWidth <= 31)
      {
        emit_insn (gen_andsi3_ones_split (operands[0], operands[1], operands[2],
                                          gen_rtx_SCRATCH (SImode)));
        DONE;
      }

    // Remains coming from lowering 64-bit operations.  For some
    // reason the middle-end does not optimize these subreg operations.

    if (const0_rtx == operands[2])
      {
        emit_move_insn (operands[0],
                        AND == <CODE> ? const0_rtx : operands[1]);
        DONE;
      }
    if (constm1_rtx == operands[2])
      {
        emit_move_insn (operands[0],
                        AND == <CODE> ? operands[1]
                        : IOR == <CODE> ? constm1_rtx
                        : gen_rtx_NOT (SImode, operands[1]));
        DONE;
      }

    if (!reg_or_s10_operand (operands[2], SImode))
      operands[2] = copy_to_mode_reg (SImode, operands[2]);
  })

(define_insn "*andsi3_255"
  [(set (match_operand:SI 0 "register_operand"          "=D")
        (and:SI (match_operand:SI 1 "register_operand"   "0")
                (const_int 255)))]
  "optimize_size"
  "and\t%0, 255"
  [(set_attr "pipe" "ip")
   (set_attr "length" "2")])

;; "*andsi3" "*iorsi3" "*xorsi3"
(define_insn "*<code>si3"
  [(set (match_operand:SI 0 "register_operand"                  "=d,d ,d ,!d ,d   ,d")
        (tric_bitop:SI (match_operand:SI 1 "register_operand"   "%0,d ,d ,d  ,d   ,d")
                       (match_operand:SI 2 "reg_or_s10_operand"  "d,0 ,d ,d  ,Ku9 ,Kc9")))]
  ""
  "@
    <tric_op>\t%0, %2
    <tric_op>\t%0, %1
    <tric_op>\t%0, %1, %2
    <tric_op>\t%0, %1, %2
    <tric_op>\t%0, %1, %2
    <tric_bitop_not_op>\t%0, %1, ~(%2)"
  [(set_attr "opt" "*,*,speed,size,*,*")
   (set_attr "pipe" "ip")
   (set_attr "adjust" "bitop")])

(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand"         "=d ,d ,!d")
        (not:SI (match_operand:SI 1 "register_operand"  "0 ,d ,d")))]
  ""
  "@
    not\t%0
    nor\t%0, %1, 0
    nor\t%0, %1, 0"
  [(set_attr "opt" "*,speed,size")
   (set_attr "pipe" "ip")
   (set_attr "length" "2,4,4")])

(define_insn "negsi2"
  [(set (match_operand:SI 0 "register_operand"         "=d ,d ,!d")
        (neg:SI (match_operand:SI 1 "register_operand"  "0 ,d ,d")))]
  ""
  "@
    rsub\t%0
    rsub\t%0, %1, 0
    rsub\t%0, %1, 0"
  [(set_attr "opt" "*,speed,size")
   (set_attr "pipe" "ip")
   (set_attr "adjust" "sat")])

(define_insn "*@cost.mulsi3"
  [(set (match_operand:SI 0 "register_operand"            "=X")
        (mult:SI (match_operand:SI 1 "register_operand"    "X")
                 (match_operand:SI 2 "nonmemory_operand"   "X")))]
  "COST_INSN" { COST_PATTERN; }
  [(set (attr "ticks")
        (if_then_else (match_operand:SI 2 "reg_or_s9_operand" "")
                      (const_int 5)
                      (const_int 9)))
   (set (attr "space")
        (if_then_else (match_operand:SI 2 "reg_or_s9_operand" "")
                      (const_int 4)
                      (const_int 8)))])

(define_insn "*@cost.umulsidi3_highpart"
  [(set (match_operand:SI 0 "register_operand"                                                    "=X")
        (truncate:SI (lshiftrt:DI (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand"  "X"))
                                           (zero_extend:DI (match_operand:SI 2 "register_operand"  "X")))
                                  (const_int 32))))]
  "COST_INSN" { COST_PATTERN; }
  [(set_attr "ticks" "5")])

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand"             "=d,d ,!d ,d ,d  ")
        (mult:SI (match_operand:SI 1 "register_operand"    "%d,0 ,d  ,d ,d  ")
                 (match_operand:SI 2 "reg_or_s9_operand"    "0,d ,d  ,d ,Ks9")))]
  ""
  "@
    mul\t%0, %1
    mul\t%0, %2
    mul\t%0, %1, %2
    mul\t%0, %1, %2
    mul\t%0, %1, %2"
  [(set_attr "ticks" "3")
   (set_attr "opt" "*,*,size,speed,*")
   (set_attr "pipe" "ipm")])

(define_insn "umulsidi3"
  [(set (match_operand:DI 0 "register_operand"                          "=d")
        (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand"  "d"))
                 (zero_extend:DI (match_operand:SI 2 "register_operand"  "d"))))]
  ""
  "mul.u\t%A0, %1, %2"
  [(set_attr "ticks" "3")
   (set_attr "pipe" "ipm")])

(define_insn "*umulsidi3_const"
  [(set (match_operand:DI 0 "register_operand"                          "=d")
        (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand"  "d"))
                 (match_operand:DI 2 "u9_operand"                        "Ku9")))]
  ""
  "mul.u\t%A0, %1, %2"
  [(set_attr "ticks" "3")
   (set_attr "pipe" "ipm")])

(define_insn "mulsidi3"
  [(set (match_operand:DI 0 "register_operand"                           "=d")
        (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand"   "d"))
                 (sign_extend:DI (match_operand:SI 2 "register_operand"   "d"))))]
  ""
  "mul\t%A0, %1, %2"
  [(set_attr "ticks" "3")
   (set_attr "pipe" "ipm")])

(define_insn "*mulsidi3_const"
  [(set (match_operand:DI 0 "register_operand"                            "=d")
        (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand"   "%d"))
                 (match_operand:DI 2 "s9_operand"                         "Ks9")))]
  ""
  "mul\t%A0, %1, %2"
  [(set_attr "ticks" "3")
   (set_attr "pipe" "ipm")])

(define_insn_and_split "*mulsidi3_big_const"
  [(set (match_operand:DI 0 "register_operand"                            "=d")
        (mult:DI (sign_extend:DI (match_operand:SI 2 "register_operand"   "%d"))
                 (match_operand:DI 1 "no_s9_operand"                       "n")))]
  ;; !reload_completed is not strict enough because reload might
  ;; come up again with this insn which is ICE.
  "tric_gate_split1 ()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    rtx lo[2], hi[2], op2 = operands[2];

    tric_split_di (operands, lo, hi, 2);

    if (const0_rtx == lo[1])
      {
        tric_emit_multiply_add (hi[0], op2, hi[1], const0_rtx);
        emit_move_insn (lo[0], const0_rtx);
      }
    else if (single_one_operand (operands[1], DImode))
      {
        int off = exact_log2 (UINTVAL (operands[1]) & GET_MODE_MASK (SImode));
        rtx pat_lo = gen_ashlsi3 (lo[0], op2, GEN_INT (off));
        // Don't use arithmetic right shift because that would clobber PSW.
        rtx pat_hi = gen_extv (hi[0], op2, GEN_INT (off), GEN_INT (32 - off));
        bool overlap_hi = reg_overlap_mentioned_p (hi[0], op2);
        emit_insn (overlap_hi ? pat_lo : pat_hi);
        emit_insn (overlap_hi ? pat_hi : pat_lo);
      }
    else
      {
        if (reg_overlap_mentioned_p (operands[0], op2))
          emit_move_insn (op2 = gen_reg_rtx (SImode), operands[2]);

        if (INTVAL (lo[1]) < 0)
          {
            // lo[1] will be sign-extended.  Adjust the high part so that we have
            // xop[1] = lo[1] + hi[1] * 2**32  again.
            hi[1] = plus_constant (SImode, hi[1], 1);
          }
        rtx lo1 = tric_emit_mulsidi (operands[0], op2, lo[1], SIGN_EXTEND);
        rtx hi1 = REG_P (lo1)
          ? tric_emit_arith_CONST_INT (NULL_RTX, hi[1], lo1, lo[1])
          : hi[1];
        tric_emit_multiply_add (hi[0], op2, hi1 ? hi1 : hi[1], hi[0]);
      }

    DONE;
  }
  [(set_attr "ticks" "5")
   (set_attr "pipe" "ipm")])


(define_insn_and_split "*umulsidi3_big_const"
  [(set (match_operand:DI 0 "register_operand"                            "=d")
        (mult:DI (zero_extend:DI (match_operand:SI 2 "register_operand"   "%d"))
                 (match_operand:DI 1 "no_u9_operand"                       "n")))]
  ;; !reload_completed is not strict enough because reload might
  ;; come up again with this insn which is ICE.
  "tric_gate_split1 ()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    rtx lo[2], hi[2], op2 = operands[2];

    tric_split_di (operands, lo, hi, 2);

    if (const0_rtx == lo[1])
      {
        tric_emit_multiply_add (hi[0], op2, hi[1], const0_rtx);
        emit_move_insn (lo[0], const0_rtx);
      }
    else if (single_one_operand (operands[1], DImode))
      {
        int off = exact_log2 (UINTVAL (operands[1]) & GET_MODE_MASK (SImode));
        rtx pat_lo = gen_ashlsi3 (lo[0], op2, GEN_INT (off));
        rtx pat_hi = gen_lshrsi3 (hi[0], op2, GEN_INT (32 - off));
        bool overlap_hi = reg_overlap_mentioned_p (hi[0], op2);
        emit_insn (overlap_hi ? pat_lo : pat_hi);
        emit_insn (overlap_hi ? pat_hi : pat_lo);
      }
    else if (lo[1] == const1_rtx
             && hi[1] == const1_rtx)
      {
        // This special case is used by "*ldmst.or.ic".
        emit_move_insn (lo[0], operands[2]);
        emit_move_insn (hi[0], lo[0]);
      }
    else
      {
        if (reg_overlap_mentioned_p (operands[0], op2))
          emit_move_insn (op2 = gen_reg_rtx (SImode), operands[2]);

        rtx lo1 = tric_emit_mulsidi (operands[0], op2, lo[1], ZERO_EXTEND);
        rtx hi1 = REG_P (lo1)
          ? tric_emit_arith_CONST_INT (NULL_RTX, hi[1], lo1, lo[1])
          : hi[1];
        tric_emit_multiply_add (hi[0], op2, hi1 ? hi1 : hi[1],
                                const1_rtx == lo[1] ? const0_rtx : hi[0]);
      }

    DONE;
  }
  [(set_attr "ticks" "5")
   (set_attr "pipe" "ipm")])


(define_insn_and_split "*umulsidi3_reg32"
  [(set (match_operand:DI 0 "register_operand"                            "=d")
        (mult:DI (zero_extend:DI (match_operand:SI 2 "register_operand"    "d"))
                 (match_operand:DI 1 "register_operand"                    "d")))]
  ;; !reload_completed is not strict enough because reload might
  ;; come up again with this insn which is ICE.
  "tric_gate_split1 ()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [;; "umulsidi3"
   (set (match_dup 0)
        (mult:DI (zero_extend:DI (match_dup 2))
                 (zero_extend:DI (match_dup 3))))
   ;; "maddsi4"
   (parallel[(set (match_dup 5)
                  (plus:SI (mult:SI (match_dup 2)
                                    (match_dup 4))
                           (match_dup 5)))
             (clobber (scratch:SI))])
  (set (match_dup 6)
       (match_dup 0))]
  {
    operands[6] = operands[0];
    if (reg_overlap_mentioned_p (operands[0], operands[1])
        || reg_overlap_mentioned_p (operands[0], operands[2]))
      operands[0] = gen_reg_rtx (DImode);

    rtx lo[2], hi[2];
    tric_split_di (operands, lo, hi, 2);

    operands[3] = lo[1];
    operands[4] = hi[1];
    operands[5] = hi[0];
  }
  [(set_attr "ticks" "5")
   (set_attr "pipe" "ipm")])


;; "maddsidi4"
;; "umaddsidi4"
(define_insn "<signed_prefix>maddsidi4"
  [(set (match_operand:DI 0 "register_operand"                                   "=d")
        (plus:DI (mult:DI (any_extend:DI (match_operand:SI 1 "register_operand"   "d"))
                          (any_extend:DI (match_operand:SI 2 "register_operand"   "d")))
                 (match_operand:DI 3 "register_operand"                           "d")))]
  ""
  "madd<signed_suffix>\t%A0, %A3, %1, %2"
  [(set_attr "ticks" "3")
   (set_attr "pipe" "ipm")])

(define_insn "maddsidi4_const"
  [(set (match_operand:DI 0 "register_operand"                                    "=d   ,d")
        (plus:DI (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand"  "%d   ,d"))
                          (match_operand:DI 2 "s9_m9_operand"                      "Ks9 ,Km9"))
                 (match_operand:DI 3 "register_operand"                            "d   ,d")))]
  ""
  "@
	madd\t%A0, %A3, %1, %2
	msub\t%A0, %A3, %1, %n2"
  [(set_attr "ticks" "3")
   (set_attr "pipe" "ipm")])

(define_insn "umaddsidi4_const"
  [(set (match_operand:DI 0 "register_operand"                                    "=d   ,d")
        (plus:DI (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand"  "%d   ,d"))
                          (match_operand:DI 2 "s10n_operand"                       "Ku9 ,Kn9"))
                 (match_operand:DI 3 "register_operand"                            "d   ,d")))]
  ""
  "@
	madd.u\t%A0, %A3, %1, %2
	msub.u\t%A0, %A3, %1, %n2"
  [(set_attr "ticks" "3")
   (set_attr "pipe" "ipm")])

(define_insn_and_split "*maddsidi4_big_const"
  [(set (match_operand:DI 0 "register_operand"                                  "=d")
        (plus:DI (mult:DI (sign_extend:DI (match_operand:SI 3 "register_operand" "d"))
                          (match_operand:DI 2 "no_s9_operand"                    "n"))
                 (match_operand:DI 1 "register_operand"                          "d")))]
  "tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    tric_emit_maddsidi4_const (operands);
    DONE;
  }
  [(set_attr "ticks" "5")
   (set_attr "pipe" "ipm")])

(define_insn_and_split "*umaddsidi4_big_const"
  [(set (match_operand:DI 0 "register_operand"                                  "=d")
        (plus:DI (mult:DI (zero_extend:DI (match_operand:SI 3 "register_operand" "d"))
                          (match_operand:DI 2 "no_s10n_operand"                  "n"))
                 (match_operand:DI 1 "register_operand"                          "d")))]
  "tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    tric_emit_umaddsidi4_const (operands);
    DONE;
  }
  [(set_attr "ticks" "5")
   (set_attr "pipe" "ipm")])


(define_insn_and_split "*umaddsidi4_reg32"
  [(set (match_operand:DI 0 "register_operand"                                  "=d")
        (plus:DI (mult:DI (zero_extend:DI (match_operand:SI 3 "register_operand" "d"))
                          (match_operand:DI 2 "register_operand"                 "d"))
                 (match_operand:DI 1 "register_operand"                          "d")))]
  "tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [;; "umaddsidi4"
   (set (match_dup 0)
        (plus:DI (mult:DI (zero_extend:DI (match_dup 3))
                          (zero_extend:DI (match_dup 4)))
                 (match_dup 1)))
   ;; "maddsi4"
   (parallel[(set (match_dup 6)
                  (plus:SI (mult:SI (match_dup 3)
                                    (match_dup 5))
                           (match_dup 6)))
             (clobber (scratch:SI))])

   (set (match_dup 7)
        (match_dup 0))]
  {
    operands[7] = operands[0];
    if (reg_overlap_mentioned_p (operands[0], operands[2])
        || reg_overlap_mentioned_p (operands[0], operands[3]))
      operands[0] = gen_reg_rtx (DImode);

    rtx lo[3], hi[3];
    tric_split_di (operands, lo, hi, 3);

    operands[4] = lo[2];
    operands[5] = hi[2];
    operands[6] = hi[0];
  }
  [(set_attr "ticks" "5")
   (set_attr "pipe" "ipm")])


(define_insn_and_split "*umsubsidi4_reg32"
  [(set (match_operand:DI 0 "register_operand"                                   "=d")
        (minus:DI (match_operand:DI 1 "register_operand"                          "d")
                  (mult:DI (zero_extend:DI (match_operand:SI 3 "register_operand" "d"))
                           (match_operand:DI 2 "register_operand"                 "d"))))]
  "tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [;; "umsubsidi4"
   (set (match_dup 0)
        (minus:DI (match_dup 1)
                  (mult:DI (zero_extend:DI (match_dup 3))
                           (zero_extend:DI (match_dup 4)))))
   ;; "msubsi4"
   (set (match_dup 6)
        (minus:SI (match_dup 6)
                  (mult:SI (match_dup 3)
                           (match_dup 5))))
   (set (match_dup 7)
        (match_dup 0))]
  {
    operands[7] = operands[0];
    if (reg_overlap_mentioned_p (operands[0], operands[2])
        || reg_overlap_mentioned_p (operands[0], operands[3]))
      operands[0] = gen_reg_rtx (DImode);

    rtx lo[3], hi[3];
    tric_split_di (operands, lo, hi, 3);

    operands[4] = lo[2];
    operands[5] = hi[2];
    operands[6] = hi[0];
  }
  [(set_attr "ticks" "5")
   (set_attr "pipe" "ipm")])


;; "umsubsidi4"
;; "msubsidi4"
(define_insn "<signed_prefix>msubsidi4"
  [(set (match_operand:DI 0 "register_operand"                                   "=d")
        (minus:DI (match_operand:DI 3 "register_operand"                          "d")
                  (mult:DI (any_extend:DI (match_operand:SI 1 "register_operand"  "d"))
                           ;; Constants are canonizalized in tricore-map.md
                           (any_extend:DI (match_operand:SI 2 "register_operand"  "d")))))]
  ""
  "msub<signed_suffix>\t%A0, %A3, %1, %2"
  [(set_attr "ticks" "3")
   (set_attr "pipe" "ipm")])

(define_insn_and_split "muldi3"
  [(set (match_operand:DI 0 "register_operand"               "=d")
        (mult:DI (match_operand:DI 1 "register_operand"       "d")
                 (match_operand:DI 2 "nonmemory_operand"      "dn")))]
  "optimize
   && tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    rtx lo[3], hi[3], res = operands[0];

    if (reg_overlap_mentioned_p (res, operands[1])
        || reg_overlap_mentioned_p (res, operands[2]))
      operands[0] = gen_reg_rtx (DImode);

    tric_split_di (operands, lo, hi, 3);

    if (const0_rtx == lo[2])
      {
        tric_emit_multiply_add (hi[0], lo[1], hi[2], const0_rtx);
        emit_move_insn (lo[0], const0_rtx);
      }
    else
      {
        rtx lo2 = tric_emit_mulsidi (operands[0], lo[1], lo[2], ZERO_EXTEND);
        rtx hi2 = REG_P (lo2) && CONST_INT_P (lo[2])
          ? tric_emit_arith_CONST_INT (NULL_RTX, hi[2], lo2, lo[2])
          : hi[2];
        rtx hi0 = tric_emit_multiply_add (hi[0], lo[1], hi2 ? hi2 : hi[2],
                                          lo[2] == const1_rtx ? const0_rtx : hi[0]);
        tric_emit_multiply_add (hi[0], hi[1], lo2, hi0);
      }

    emit_move_insn (res, operands[0]);
    DONE;
  }
  [(set_attr "ticks" "7")
   (set_attr "space" "12")])


(define_insn_and_split "madddi4"
  [(set (match_operand:DI 0 "register_operand"                      "=d")
        (plus:DI (mult:DI (match_operand:DI 1 "register_operand"     "d")
                          (match_operand:DI 2 "nonmemory_operand"    "dn"))
                 (match_operand:DI 3 "register_operand"              "d")))]
  "optimize
   && tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    rtx lo[4], hi[4], lo2, res = operands[0];

    if (reg_overlap_mentioned_p (res, operands[1])
        || reg_overlap_mentioned_p (res, operands[2])
        || reg_overlap_mentioned_p (res, operands[3]))
      operands[0] = gen_reg_rtx (DImode);

    tric_split_di (operands, lo, hi, 4);
    lo2 = lo[2];

    if (CONST_INT_P (operands[2]))
      {
        rtx xop[4] = { operands[0], operands[3], operands[2], lo[1] };
        lo2 = tric_emit_umaddsidi4_const (xop);
      }
    else
      {
        emit_insn (gen_umaddsidi4 (operands[0], lo[1], lo[2], operands[3]));
        tric_emit_multiply_add (hi[0], lo[1], hi[2], hi[0]);
      }

    if (NEG == GET_CODE (lo2))
      emit_insn (gen_msubsi4 (hi[0], hi[1], XEXP (lo2, 0), hi[0]));
    else
      tric_emit_multiply_add (hi[0], hi[1], lo2, hi[0]);

    emit_move_insn (res, operands[0]);
    DONE;
  }
  [(set_attr "ticks" "7")
   (set_attr "space" "12")])


(define_insn_and_split "msubdi4"
  [(set (match_operand:DI 0 "register_operand"                      "=d")
        (minus:DI (match_operand:DI 3 "register_operand"             "d")
                  (mult:DI (match_operand:DI 1 "register_operand"    "d")
                           (match_operand:DI 2 "nonmemory_operand"   "dn"))))]
  "optimize
   && tric_gate_split1()"
  { gcc_unreachable(); }
  "&& !tric_map_combine"
  [(clobber (const_int 1))]
  {
    if (CONST_INT_P (operands[2]))
      {
        operands[2] = simplify_unary_operation (NEG, DImode, operands[2], DImode);
        emit_insn (gen_madddi4 (operands[0], operands[1], operands[2], operands[3]));
        DONE;
      }

    rtx lo[4], hi[4], res = operands[0];

    if (reg_overlap_mentioned_p (res, operands[1])
        || reg_overlap_mentioned_p (res, operands[2])
        || reg_overlap_mentioned_p (res, operands[3]))
      operands[0] = gen_reg_rtx (DImode);

    tric_split_di (operands, lo, hi, 4);

    emit_insn (gen_umsubsidi4 (operands[0], lo[1], lo[2], operands[3]));
    emit_insn (gen_msubsi4 (hi[0], lo[1], hi[2], hi[0]));
    emit_insn (gen_msubsi4 (hi[0], hi[1], lo[2], hi[0]));
    emit_move_insn (res, operands[0]);
    DONE;
  }
  [(set_attr "ticks" "7")
   (set_attr "space" "12")])


(define_expand "divmodsi4"
  [(set (match_dup 4)
        (ior:DI (ashift:DI (zero_extend:DI (mod:SI (match_operand:SI 1 "register_operand" "")
                                                   (match_operand:SI 2 "register_operand" "")))
                           (const_int 32))
                (zero_extend:DI (div:SI (match_dup 1)
                                        (match_dup 2)))))
   (set (match_operand:SI 0 "register_operand" "")
        (match_dup 5))
   (set (match_operand:SI 3 "register_operand" "")
        (match_dup 6))]
  ""
  {
    if (!TRIC_HAVE_DIV && !tric_opt_fast_div)
      {
        emit_insn (gen_divmodsi4_call (operands[0], operands[1], operands[2],
                                       operands[3]));
        DONE;
      }
    else
      {
        rtx reg = gen_reg_rtx (DImode);
        operands[4] = reg;
        operands[5] = simplify_gen_subreg (SImode, reg, DImode, 0);
        operands[6] = simplify_gen_subreg (SImode, reg, DImode, 4);
     }
  })

(define_expand "udivmodsi4"
  [(set (match_dup 4)
        (ior:DI (ashift:DI (zero_extend:DI (umod:SI (match_operand:SI 1 "register_operand" "")
                                                    (match_operand:SI 2 "register_operand" "")))
                           (const_int 32))
                (zero_extend:DI (udiv:SI (match_dup 1)
                                         (match_dup 2)))))
   (set (match_operand:SI 0 "register_operand" "")
        (match_dup 5))
   (set (match_operand:SI 3 "register_operand" "")
        (match_dup 6))]
  ""
  {
   if (!TRIC_HAVE_DIV && !tric_opt_fast_div)
     {
       emit_insn (gen_udivmodsi4_call (operands[0], operands[1],
                                       operands[2], operands[3]));
       DONE;
     }
   else
     {
       rtx reg = gen_reg_rtx (DImode);
       operands[4] = reg;
       operands[5] = simplify_gen_subreg (SImode, reg, DImode, 0);
       operands[6] = simplify_gen_subreg (SImode, reg, DImode, 4);
     }
  })

(define_insn_and_split "*divmodsi4"
  [(set (match_operand:DI 0 "register_operand"                                            "=&d ,d")
        (ior:DI (ashift:DI (zero_extend:DI (mod:SI (match_operand:SI 1 "register_operand"   "d ,d")
                                                   (match_operand:SI 2 "register_operand"   "d ,d")))
                           (const_int 32))
                (zero_extend:DI (div:SI (match_dup 1)
                                        (match_dup 2)))))]
  ""
  "@
    #
    div\t%A0, %1, %2"
  "!TRIC_HAVE_DIV
   && reload_completed"
  [(set (match_dup 0) (unspec:DI [(match_dup 1) (match_dup 2)] UNSPEC_DIV_INIT))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_DIV_STEP))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_DIV_STEP))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_DIV_STEP))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_DIV_STEP))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_DIV_ADJ))]
  {}
  [(set_attr "isa" "tc13x,tc16up")
   (set_attr "ticks" "72,11")
   (set_attr "pipe" "*,ip3")])

(define_insn_and_split "*udivmodsi4"
  [(set (match_operand:DI 0 "register_operand"                                             "=&d ,d")
        (ior:DI (ashift:DI (zero_extend:DI (umod:SI (match_operand:SI 1 "register_operand"   "d ,d")
                                                    (match_operand:SI 2 "register_operand"   "d ,d")))
                           (const_int 32))
                (zero_extend:DI (udiv:SI (match_dup 1)
                                         (match_dup 2)))))]
  ""
  "@
    #
    div.u\t%A0, %1, %2"
  "!TRIC_HAVE_DIV
   && reload_completed"
  [(set (match_dup 0) (unspec:DI [(match_dup 1) (match_dup 2)] UNSPEC_UDIV_INIT))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_UDIV_STEP))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_UDIV_STEP))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_UDIV_STEP))
   (set (match_dup 0) (unspec:DI [(match_dup 0) (match_dup 2)] UNSPEC_UDIV_STEP))]
  {}
  [(set_attr "isa" "tc13x,tc16up")
   (set_attr "ticks" "60,11")
   (set_attr "pipe" "*,ip3")])

;; Note that we must clobber A2 because the linker might relax jumps.
;; In that case the linker loads the target address into A2 and jumps indirect.
(define_expand "divmodsi4_call"
  [(set (reg:SI 4) (match_operand:SI 1 "register_operand" ""))
   (set (reg:SI 5) (match_operand:SI 2 "register_operand" ""))
   (parallel [(set (reg:DI 2)
                   (ior:DI (ashift:DI (zero_extend:DI (mod:SI (reg:SI 4)
                                                              (reg:SI 5)))
                                      (const_int 32))
                           (zero_extend:DI (div:SI (reg:SI 4)
                                                   (reg:SI 5)))))
              (clobber (reg:SI REG_A2))])
   (set (match_operand:SI 0 "register_operand" "")
        (reg:SI 2))
   (set (match_operand:SI 3 "register_operand" "")
        (reg:SI 3))])

;; Note that we must clobber A2 because the linker might relax jumps.
;; In that case the linker loads the target address into A2 and jumps indirect.
(define_insn "*divmodsi4_call"
  [(set (reg:DI REG_D2)
        (ior:DI (ashift:DI (zero_extend:DI (mod:SI (reg:SI REG_D4)
                                                   (reg:SI REG_D5)))
                           (const_int 32))
                (zero_extend:DI (div:SI (reg:SI REG_D4)
                                        (reg:SI REG_D5)))))
   (clobber (reg:SI REG_A2))]
  "!TRIC_HAVE_DIV && !tric_opt_fast_div"
  "call\t__divmodsi4"
  [(set_attr "pipe" "ctx")])

;; Note that we must clobber A2 because the linker might relax jumps.
;; In that case the linker loads the target address into A2 and jumps indirect.
(define_expand "udivmodsi4_call"
  [(set (reg:SI REG_D4) (match_operand:SI 1 "register_operand" ""))
   (set (reg:SI REG_D5) (match_operand:SI 2 "register_operand" ""))
   (parallel [(set (reg:DI REG_D2)
                   (ior:DI (ashift:DI (zero_extend:DI (umod:SI (reg:SI REG_D4)
                                                               (reg:SI REG_D5)))
                                      (const_int 32))
                           (zero_extend:DI (udiv:SI (reg:SI REG_D4)
                                                    (reg:SI REG_D5)))))
              (clobber (reg:SI REG_A2))])
   (set (match_operand:SI 0 "register_operand" "")
        (reg:SI REG_D2))
   (set (match_operand:SI 3 "register_operand" "")
        (reg:SI REG_D3))])

;; Note that we must clobber A2 because the linker might relax jumps.
;; In that case the linker loads the target address into A2 and jumps indirect.
(define_insn "*udivmodsi4_call"
  [(set (reg:DI REG_D2)
        (ior:DI (ashift:DI (zero_extend:DI (umod:SI (reg:SI REG_D4)
                                                    (reg:SI REG_D5)))
                           (const_int 32))
                (zero_extend:DI (udiv:SI (reg:SI REG_D4)
                                         (reg:SI REG_D5)))))
   (clobber (reg:SI REG_A2))]
  "!TRIC_HAVE_DIV && !tric_opt_fast_div"
  "call\t__udivmodsi4"
  [(set_attr "pipe" "ctx")])

(define_insn "div_init"
  [(set (match_operand:DI 0 "register_operand"            "=d")
        (unspec:DI [(match_operand:SI 1 "register_operand" "d")
                    (match_operand:SI 2 "register_operand" "d")]
                   UNSPEC_DIV_INIT))]
  "!TRIC_HAVE_DIV"
  "dvinit\t%A0, %1, %2"
  [(set_attr "pipe" "ipdi")])

(define_insn "udiv_init"
  [(set (match_operand:DI 0 "register_operand"            "=d")
        (unspec:DI [(match_operand:SI 1 "register_operand" "d")
                    (match_operand:SI 2 "register_operand" "d")]
                   UNSPEC_UDIV_INIT))]
  "!TRIC_HAVE_DIV"
  "dvinit.u\t%A0, %1, %2"
  [(set_attr "pipe" "ipdi")])

(define_insn "div_step"
  [(set (match_operand:DI 0 "register_operand"            "=d")
        (unspec:DI [(match_operand:DI 1 "register_operand" "d")
                    (match_operand:SI 2 "register_operand" "d")]
                   UNSPEC_DIV_STEP))]
  "!TRIC_HAVE_DIV"
  "dvstep\t%A0, %A1, %2"
  [(set_attr "pipe" "ipds")])

(define_insn "udiv_step"
  [(set (match_operand:DI 0 "register_operand"            "=d")
        (unspec:DI [(match_operand:DI 1 "register_operand" "d")
                    (match_operand:SI 2 "register_operand" "d")]
                   UNSPEC_UDIV_STEP))]
  "!TRIC_HAVE_DIV"
  "dvstep.u\t%A0, %A1, %2"
  [(set_attr "pipe" "ipds")])

(define_insn "div_adj"
  [(set (match_operand:DI 0 "register_operand"            "=d")
        (unspec:DI [(match_operand:DI 1 "register_operand" "d")
                    (match_operand:SI 2 "register_operand" "d")]
                   UNSPEC_DIV_ADJ))]
  "!TRIC_HAVE_DIV"
  "dvadj\t%A0, %A1, %2"
  [(set_attr "pipe" "ip2")])


(define_insn "addsf3"
  [(set (match_operand:SF 0 "register_operand"          "=d")
        (plus:SF (match_operand:SF 1 "register_operand"  "d")
                 (match_operand:SF 2 "register_operand"  "d")))
   (clobber (reg:SI REG_PSW))]
  ""
  "add.f\t%0, %1, %2"
  [(set_attr "pipe" "fp2")])

(define_insn "subsf3"
  [(set (match_operand:SF 0 "register_operand"           "=d")
        (minus:SF (match_operand:SF 1 "register_operand"  "d")
                  (match_operand:SF 2 "register_operand"  "d")))
   (clobber (reg:SI REG_PSW))]
  ""
  "sub.f\t%0, %1, %2"
  [(set_attr "pipe" "fp2")])

(define_insn "divsf3"
  [(set (match_operand:SF 0 "register_operand"         "=d")
        (div:SF (match_operand:SF 1 "register_operand"  "d")
                (match_operand:SF 2 "register_operand"  "d")))
   (clobber (reg:SI REG_PSW))]
  ""
  "div.f\t%0, %1, %2"
  [(set_attr "pipe" "fpdiv")])

(define_insn "mulsf3"
  [(set (match_operand:SF 0 "register_operand"          "=d")
        (mult:SF (match_operand:SF 1 "register_operand"  "d")
                 (match_operand:SF 2 "register_operand"  "d")))
   (clobber (reg:SI REG_PSW))]
  ""
  "mul.f\t%0, %1, %2"
  [(set_attr "pipe" "fp2")])

;; Just the same as optabs would expand it without this insn, but a
;; neg:SF is much better to combine, e.g. in fnma.
(define_insn "negsf2"
  [(set (match_operand:SF 0 "register_operand"         "=d,!*a")
        (neg:SF (match_operand:SF 1 "register_operand"  "d,a")))]
  ""
  "@
	addih\t%0, %1, 0x8000
	addih.a\t%0, %1, 0x8000"
  [(set_attr "pipe" "ip,aalu")])

(define_expand "smaxsf3"
  [(parallel [(set (match_dup 3)
                   (unspec:SI [(match_operand:SF 1 "register_operand"  "")
                               (match_operand:SF 2 "reg_or_0_operand"  "")]
                              UNSPEC_CMP_F))
              (clobber (reg:SI REG_PSW))])
   (set (match_dup 3)
        (and:SI (match_dup 3)
                (match_dup 4)))
   ;; "*movsfcc.ne"
   (set (match_operand:SF 0 "register_operand" "")
        (if_then_else:SF (ne (match_dup 3)
                             (const_int 0))
                         (match_dup 1)
                         (match_dup 2)))]
  ""
  {
    operands[3] = gen_reg_rtx (SImode);
    operands[4] = gen_int_mode (1 << CMP_F_GT, SImode);
  })

(define_expand "sminsf3"
  [(parallel [(set (match_dup 3)
                   (unspec:SI [(match_operand:SF 1 "register_operand"  "")
                               (match_operand:SF 2 "reg_or_0_operand"  "")]
                              UNSPEC_CMP_F))
              (clobber (reg:SI REG_PSW))])
   (set (match_dup 3)
        (and:SI (match_dup 3)
                (match_dup 4)))
   ;; "*movsfcc.eq"
   (set (match_operand:SF 0 "register_operand" "")
        (if_then_else:SF (eq (match_dup 3)
                             (const_int 0))
                         (match_dup 1)
                         (match_dup 2)))]
  ""
  {
    operands[3] = gen_reg_rtx (SImode);
    operands[4] = gen_int_mode (1 << CMP_F_GT, SImode);
  })

(define_expand "copysignsf3"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:SF 1 "register_operand" "")
                    (match_operand:SF 2 "register_operand" "")
                    ] UNSPEC_COPYSIGN))])

(define_insn "*copysignsf3"
  [(set (match_operand:SF 0 "register_operand"             "=d")
        (unspec:SF [(match_operand:SF 1 "register_operand"  "d")
                    (match_operand:SF 2 "register_operand"  "d")
                    ] UNSPEC_COPYSIGN))]
  ""
  "ins.t\t%0, %1,31, %2,31"
  [(set_attr "pipe" "ip")])

(define_insn "*ncopysignsf3"
  [(set (match_operand:SF 0 "register_operand"                     "=d")
        (neg:SF (unspec:SF [(match_operand:SF 1 "register_operand"  "d")
                            (match_operand:SF 2 "register_operand"  "d")
                            ] UNSPEC_COPYSIGN)))]
  ""
  "insn.t\t%0, %1,31, %2,31"
  [(set_attr "pipe" "ip")])

(define_insn "*ncopysignsf3.2"
  [(set (match_operand:SF 0 "register_operand"                     "=d")
        (unspec:SF [(match_operand:SF 1 "register_operand"          "d")
                    (neg:SF (match_operand:SF 2 "register_operand"  "d"))
                    ] UNSPEC_COPYSIGN))]
  ""
  "insn.t\t%0, %1,31, %2,31"
  [(set_attr "pipe" "ip")])

(define_insn "cmp_f"
  [(set (match_operand:SI 0 "register_operand"             "=d")
        (unspec:SI [(match_operand:SF 1 "register_operand"  "d")
                    (match_operand:SF 2 "register_operand"  "d")] UNSPEC_CMP_F))
   (clobber (reg:SI REG_PSW))]
  ""
  "cmp.f\t%0, %1, %2"
  [(set_attr "pipe" "fp")])

(define_insn "floatsisf2"
  [(set (match_operand:SF 0 "register_operand"          "=d")
        (float:SF (match_operand:SI 1 "register_operand" "d")))
   (clobber (reg:SI REG_PSW))]
  ""
  "itof\t%0, %1"
  [(set_attr "pipe" "fp2")])

(define_insn "floatunssisf2"
  [(set (match_operand:SF 0 "register_operand"                   "=d")
        (unsigned_float:SF (match_operand:SI 1 "register_operand" "d")))
   (clobber (reg:SI REG_PSW))]
  ""
  "utof\t%0, %1"
  [(set_attr "pipe" "fp2")])

(define_expand "fix_truncsfsi2"
  [(set (reg:SF REG_D4)
        (match_operand:SF 1 "register_operand" ""))
   (parallel [(set (reg:SI REG_D2)
                   (fix:SI (reg:SF REG_D4)))
              (clobber (reg:SI REG_A2))
              (clobber (reg:DI REG_D0))])
   (set (match_operand:SI 0 "register_operand" "")
        (reg:SI REG_D2))]
  ""
  {
    if (TRIC_HAVE_FTOIZ)
      {
        emit_insn (gen_fix_truncsfsi2_insn (operands[0], operands[1]));
        DONE;
      }
  })

(define_expand "fixuns_truncsfsi2"
  [(set (reg:SF REG_D4)
        (match_operand:SF 1 "register_operand" ""))
   (parallel [(set (reg:SI REG_D2)
                   (unsigned_fix:SI (reg:SF REG_D4)))
              (clobber (reg:SI REG_A2))
              (clobber (reg:DI REG_D0))])
   (set (match_operand:SI 0 "register_operand" "")
        (reg:SI REG_D2))]
  ""
  {
    if (TRIC_HAVE_FTOIZ)
      {
        emit_insn (gen_fixuns_truncsfsi2_insn (operands[0], operands[1]));
        DONE;
      }
  })

(define_insn "fix_truncsfsi2_insn"
  [(set (match_operand:SI 0 "register_operand"         "=d")
        (fix:SI (match_operand:SF 1 "register_operand"  "d")))
   (clobber (reg:SI REG_PSW))]
  "TRIC_HAVE_FTOIZ"
  "ftoiz\t%0, %1"
  [(set_attr "pipe" "fp2")])

(define_insn "fixuns_truncsfsi2_insn"
  [(set (match_operand:SI 0 "register_operand"                 "=d")
        (unsigned_fix:SI (match_operand:SF 1 "register_operand" "d")))
   (clobber (reg:SI REG_PSW))]
  "TRIC_HAVE_FTOIZ"
  "ftouz\t%0, %1"
  [(set_attr "pipe" "fp2")])

(define_insn "*fix_truncsfsi2_call"
  [(set (reg:SI REG_D2)
        (fix:SI (reg:SF REG_D4)))
   (clobber (reg:SI REG_A2))
   (clobber (reg:DI REG_D0))]
  "!TRIC_HAVE_FTOIZ"
  "call\t__fixsfsi"
  [(set_attr "pipe" "ctx")])

(define_insn "*fixuns_truncsfsi2_call"
  [(set (reg:SI REG_D2)
        (unsigned_fix:SI (reg:SF REG_D4)))
   (clobber (reg:SI REG_A2))
   (clobber (reg:DI REG_D0))]
  "!TRIC_HAVE_FTOIZ"
  "call\t__fixunssfsi"
  [(set_attr "pipe" "ctx")])

;; $0 = $3 + $1 * $2
(define_insn "fmasf4"
  [(set (match_operand:SF 0 "register_operand"          "=d")
        (fma:SF (match_operand:SF 1 "register_operand"   "d")
                (match_operand:SF 2 "register_operand"   "d")
                (match_operand:SF 3 "register_operand"   "d")))
   (clobber (reg:SI REG_PSW))]
  "!TRIC_ERRATA_076"
  "madd.f\t%0, %3, %1, %2"
  [(set_attr "pipe" "fp3")])

;; $0 = $1 * $2 - $3
(define_expand "fmssf4"
  [(parallel [(set (match_dup 4)
                   (fma:SF (neg:SF (match_operand:SF 1 "register_operand" ""))
                           (match_operand:SF 2 "register_operand" "")
                           (match_operand:SF 3 "register_operand" "")))
              (clobber (reg:SI REG_PSW))])
   (set (match_operand:SF 0 "register_operand" "")
        (neg:SF (match_dup 4)))]
  "!TRIC_ERRATA_076"
  {
    operands[4] = gen_reg_rtx (SFmode);
  })

;; $0 = - $1 * $2 + $3
(define_insn "fnmasf4"
  [(set (match_operand:SF 0 "register_operand"                 "=d")
        (fma:SF (neg:SF (match_operand:SF 1 "register_operand"  "d"))
                (match_operand:SF 2 "register_operand"          "d")
                (match_operand:SF 3 "register_operand"          "d")))
   (clobber (reg:SI REG_PSW))]
  "!TRIC_ERRATA_076"
  "msub.f\t%0, %3, %1, %2"
  [(set_attr "pipe" "fp3")])

;; $0 = - $1 * $2 - $3
(define_expand "fnmssf4"
  [(parallel [(set (match_dup 4)
                   (fma:SF (match_operand:SF 1 "register_operand" "")
                           (match_operand:SF 2 "register_operand" "")
                           (match_operand:SF 3 "register_operand" "")))
              (clobber (reg:SI REG_PSW))])
   (set (match_operand:SF 0 "register_operand" "")
        (neg:SF (match_dup 4)))]
  "!TRIC_ERRATA_076"
  {
    operands[4] = gen_reg_rtx (SFmode);
  })

;; "andsi3_zerox1"
;; "iorsi3_zerox1"
;; "xorsi3_zerox1"
;; This pattern is used in cbranchsf4-expander (for IOR)
(define_insn "<code>si3_zerox1"
  [(set (match_operand:SI 0 "register_operand"                                  "=d  ")
        (tric_bitop:SI (zero_extract:SI (match_operand:SI 1 "register_operand"   "d  ")
                                        (const_int 1)
                                        (match_operand:SI 2 "u5_operand"         "Ku5"))
                       (zero_extract:SI (match_operand:SI 3 "register_operand"   "d  ")
                                        (const_int 1)
                                        (match_operand:SI 4 "u5_operand"         "Ku5"))))]
  ""
  "<tric_op>.t\t%0, %1,%2, %3,%4"
  [(set_attr "pipe" "ip")])

(define_insn_and_split "ashlsi3"
  [(set (match_operand:SI 0 "register_operand"             "=d   ,d     ,a   ,!a")
        (ashift:SI (match_operand:SI 1 "register_operand"   "0   ,d     ,a   ,a")
                   (match_operand:SI 2 "reg_or_u5_operand"  "Ku3 ,d Ku5 ,K01 ,K02")))]
  ""
  "@
    sh\t%0, %2
    sh\t%0, %1, %2
    add.a\t%0, %1, %1
    add.a\t%0, %1, %1\;add.a\t%0, %0"
  "reload_completed"
  [(set (match_dup 0)
        (plus:SI (match_dup 1)
                 (match_dup 1)))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 0)))]
  {
    extract_constrain_insn_cached (curr_insn);

    if (3 != which_alternative)
      FAIL;
  }
  [(set_attr "pipe" "ip,ip,aalu,*")
   (set_attr "length" "2,4,4,6")])

;; We must not split LSHIFTRT/ASHIFTRT with variable offset into
;; a NEG and a SH/SHA with negative offset because this is a right shift
;; and NOT a left shift (with negative offset).
;; Note that left and right shifts differ fundamentally. E.g. a left shift
;; is equivalent to some multiplication whilst a right shift is not.
;; So optimizers would mess up. Therefore, when splitting,
;; we must use UNSPECs to hide what arithmetic is going on.

(define_insn_and_split "lshrsi3"
  [(set (match_operand:SI 0 "register_operand"                 "=&d ,d       ,d")
        (lshiftrt:SI (match_operand:SI 1 "register_operand"      "d ,0       ,d")
                     (match_operand:SI 2 "reg_or_u5_operand"     "d ,Ku3 K08 ,Ku5")))]
  ""
  "@
    #
    sh\t%0, %n2
    sh\t%0, %1, %n2"
  "reload_completed
   && register_operand (operands[2], SImode)"
  [(set (match_dup 0)
        (neg:SI (match_dup 2)))
   (set (match_dup 0)
        (unspec:SI [(match_dup 1)
                    (match_dup 0)] UNSPEC_LSHR))]
  ""
  [(set_attr "pipe" "*,ip,ip")
   (set_attr "length" "*,2,4")])

(define_insn_and_split "ashrsi3"
  [(set (match_operand:SI 0 "register_operand"               "=&d ,d       ,d")
        (ashiftrt:SI (match_operand:SI 1 "register_operand"    "d ,0       ,d")
                     (match_operand:SI 2 "reg_or_u5_operand"   "d ,Ku3 K08 ,Ku5")))
   (clobber (reg:SI REG_PSW))]
  ""
  "@
    #
    sha\t%0, %n2
    sha\t%0, %1, %n2"
  "reload_completed
   && register_operand (operands[2], SImode)"
  [(set (match_dup 0)
        (neg:SI (match_dup 2)))
   (set (match_dup 0)
        (unspec:SI [(match_dup 1)
                    (match_dup 0)] UNSPEC_ASHR))]
  ""
  [(set_attr "pipe" "*,ip,ip")
   (set_attr "length" "*,2,4")])

(define_insn "lshrsi3_unspec"
  [(set (match_operand:SI 0 "register_operand"              "=d")
        (unspec:SI [(match_operand:SI 1 "register_operand"   "d")
                    (match_operand:SI 2 "register_operand"   "d")]
                   UNSPEC_LSHR))]
  ""
  "sh\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "ashrsi3_unspec"
  [(set (match_operand:SI 0 "register_operand"              "=d  ,d")
        (unspec:SI [(match_operand:SI 1 "register_operand"   "d  ,d")
                    (match_operand:SI 2 "nonmemory_operand"  "Kn5,d")]
                   UNSPEC_ASHR))]
  ""
  "@
	shas\t%0, %1, %2
	shas\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "rotlsi3"
  [(set (match_operand:SI 0 "register_operand"             "=d")
        (rotate:SI (match_operand:SI 1 "register_operand"   "d")
                   (match_operand:SI 2 "reg_or_u5_operand"  "d Ku5")))]
  ""
  "dextr\t%0, %1, %1, %2"
  [(set_attr "pipe" "ip2")])

;; "zero_extendqisi2"
;; "zero_extendhisi2"
(define_expand "zero_extend<mode>si2"
  [(set (match_operand:SI 0 "register_operand"                  "")
        (zero_extend:SI (match_operand:QIHI 1 "extend_operand"  "")))])

;; "extendqisi2"
;; "extendhisi2"
(define_expand "extend<mode>si2"
  [(set (match_operand:SI 0 "register_operand"                 "")
        (sign_extend:SI (match_operand:QIHI 1 "extend_operand" "")))])

(define_insn "*extendqisi2"
  [(set (match_operand:SI 0 "register_operand"               "=d ,d")
        (sign_extend:SI (match_operand:QI 1 "extend_operand"  "m ,d")))]
  ""
  "@
    ld.b\t%0, %1
    extr\t%0, %1, 0, 8"
  [(set_attr "pipe" "ldd,ip2")
   (set_attr "length" "*,4")
   (set_attr "adjust" "mov8s,*")])

(define_insn "*zero_extendqisi2"
  [(set (match_operand:SI 0 "register_operand"               "=d ,D ,d ,?d")
        (zero_extend:SI (match_operand:QI 1 "extend_operand"  "m ,0 ,d ,d")))]
  ""
  "@
    ld.bu\t%0, %1
    and\t%0, 255
    and\t%0, %1, 255
    and\t%0, %1, 255"
  [(set_attr "opt" "*,*,speed,size")
   (set_attr "pipe" "ldd,ip,ip,ip")
   (set_attr "length" "*,2,4,4")
   (set_attr "adjust" "mov8,*,*,*")])

(define_insn "*extendhisi2"
  [(set (match_operand:SI 0 "register_operand"               "=d ,d")
        (sign_extend:SI (match_operand:HI 1 "extend_operand"  "m ,d")))]
  ""
  "@
    ld.h\t%0, %1
    extr\t%0, %1, 0, 16"
  [(set_attr "pipe" "ldd,ip2")
   (set_attr "length" "*,4")
   (set_attr "adjust" "mov16s,*")])

(define_insn "*zero_extendhisi2"
  [(set (match_operand:SI 0 "register_operand"               "=d ,d")
        (zero_extend:SI (match_operand:HI 1 "extend_operand"  "m ,d")))]
  ""
  "@
    ld.hu\t%0, %1
    extr.u\t%0, %1, 0, 16"
  [(set_attr "pipe" "ldd,ip2")
   (set_attr "length" "*,4")
   (set_attr "adjust" "mov16,*")])

;; Jump instructions

(define_insn "jump"
  [(set (pc)
        (label_ref (match_operand 0 "" "")))]
  ""
  "j\t%0"
  [(set_attr "pipe" "jls")
   (set_attr "type" "jump")])

(define_insn "indirect_jump"
  [(set (pc)
        (match_operand:SI 0 "register_operand" "a"))]
  ""
  "ji\t%0"
  [(set_attr "pipe" "jls")
   (set_attr "length" "2")])

;; Special insns used by rtl-factoring

(define_expand "fcall"
  [(set (pc)
        (unspec[(label_ref (match_operand 0 "nonmemory_operand" ""))]
               UNSPEC_FCALL))]
  "TRIC_HAVE_FCALL")

(define_insn "*fcall"
  [(set (pc)
        (unspec[(match_operand 0 "nonmemory_operand" "i,a")]
               UNSPEC_FCALL))]
  "TRIC_HAVE_FCALL"
  "@
	fcall\t%0
	fcalli\t%0"
  [(set_attr "pipe" "jls")])


(define_insn "tablejump"
  [(set (pc)
        ;; Wrap the jump target into an unspec.  The jump table is a dispatch
        ;; table holding direct jumps to the cases.  The jump tables (and
        ;; the label associated to it) are not in any basic block, hence
        ;; jumping to such a location is no good.  CSE might come up with
        ;; optimizations that turn a tablejump into a direct jump to $1 which
        ;; finally ICEs, cf. HDP-906.
        (unspec:SI [(match_operand:SI 0 "register_operand" "a")]
                   UNSPEC_TABLEJUMP))
   (use (label_ref (match_operand 1 "" "")))]
  ""
  "ji\t%0"
  [(set_attr "pipe" "jls")
   (set_attr "length" "2")])

;; Implement switch statements.

(define_expand "casesi"
  [(parallel[(match_operand:SI 0 "general_operand" "")   ; index
             (match_operand:SI 1 "general_operand" "")   ; lower
             (match_operand:SI 2 "general_operand" "")   ; upper-lower
             (match_operand 3 "" "")                     ; table label
             (match_operand 4 "" "")])]                  ; default label
  ""
  {
    tric_emit_casesi (operands[0], operands[1], operands[2], operands[3],
                      operands[4]);
    DONE;
  })

;;  Call instructions.

(define_expand "call_irq_hnd_rtx"
  [(parallel[(call (match_operand 0 "memory_operand" "")
                   (match_operand 1 "const_int_operand" ""))
             (use (match_operand 2 "const_int_operand" ""))])])

(define_expand "call_irq_rtx"
  [(parallel[(call (match_operand 0 "memory_operand" "")
                   (match_operand 1 "const_int_operand" ""))
             (use (match_operand 2 "const_int_operand" ""))
             (clobber (reg:DI REG_D14))
             (clobber (reg:DI REG_D12))
             (clobber (reg:DI REG_D10))
             (clobber (reg:DI REG_D8))
             (clobber (reg:SI REG_A15))
             (clobber (reg:DI REG_A12))
             (clobber (reg:SI REG_PSW))])])

(define_expand "call_value_irq_rtx"
  [(parallel[(set (match_operand 0 "register_operand")
                  (call (match_operand 1 "memory_operand" "")
                        (match_operand 2 "const_int_operand" "")))
             (use (match_operand 3 "const_int_operand" ""))
             (clobber (reg:DI REG_D14))
             (clobber (reg:DI REG_D12))
             (clobber (reg:DI REG_D10))
             (clobber (reg:DI REG_D8))
             (clobber (reg:SI REG_A15))
             (clobber (reg:DI REG_A12))
             (clobber (reg:SI REG_PSW))])])

(define_expand "call_pxhndcall_rtx"
  [(set (reg:SI REG_A12)
        (match_dup 3))
   (parallel[(call (unspec:SI [(match_operand 0 "const_int_operand" "")
                               ] UNSPEC_SYSCALL)
                   (match_operand 1 "const_int_operand" ""))
             (use (reg:SI REG_A12))
             (use (match_operand 2 "const_int_operand" ""))])]
  ""
  {
    rtx addr = XEXP (operands[0], 0);
    operands[3] = force_reg (Pmode, addr);
    operands[0] = const1_rtx;
  })

(define_expand "call_value_pxhndcall_rtx"
  [(set (reg:SI REG_A12)
        (match_dup 4))
   (parallel[(set (match_operand 0 "register_operand" "")
                  (call (unspec:SI [(match_operand 1 "const_int_operand" "")
                                    ] UNSPEC_SYSCALL)
                        (match_operand 2 "const_int_operand" "")))
             (use (reg:SI REG_A12))
             (use (match_operand 3 "const_int_operand" ""))])]
  ""
  {
    rtx addr = XEXP (operands[1], 0);
    operands[4] = force_reg (Pmode, addr);
    operands[1] = const1_rtx;
  })

(define_expand "call"
  [(parallel[(call (match_operand 0 "memory_operand" "")
                   (match_operand 1 "const_int_operand" ""))
             (use (match_operand 2 "const_int_operand" ""))])]
  ""
  {
    rtx addr = XEXP (operands[0], 0);
    int call_cookie = OPVAL(2);

    if (SYMBOL_REF == GET_CODE (addr))
      {
        const char * fname = XSTR (addr, 0);

        /* We CALL these REG_NORETURN functions to ease debugging */

        if (! strcmp (fname, "abort")
            || ! strcmp (fname, "exit")
            || ! strcmp (fname, "_exit"))
          call_cookie |= CALLCOOKIE_USE_CALL_MASK;
      }

    operands[2] = GEN_INT (call_cookie & ~CALLCOOKIE_SIBLING_MASK);

    if (call_cookie & CALLCOOKIE_PXHNDCALL_MASK)
      {
        emit (gen_call_pxhndcall_rtx (operands[0], operands[1], operands[2]));
        DONE;
      }

    if (call_cookie & CALLCOOKIE_LONGCALL_MASK)
      operands[0] = replace_equiv_address (operands[0],
                                           force_reg (SImode,
                                                      XEXP (operands[0], 0)));

    if (call_cookie & CALLCOOKIE_INTERRUPT_HANDLER_MASK)
      {
        emit (gen_call_irq_hnd_rtx (operands[0], operands[1], operands[2]));
        DONE;
      }

    if (call_cookie & CALLCOOKIE_INTERRUPT_MASK)
      {
        emit (gen_call_irq_rtx (operands[0], operands[1], operands[2]));
        DONE;
      }
  })

(define_expand "call_value"
  [(parallel[(set (match_operand 0 "register_operand" "")
                  (call (match_operand 1 "memory_operand" "")
                        (match_operand 2 "const_int_operand" "")))
             (use (match_operand 3 "const_int_operand" ""))])]
  ""
  {
    rtx addr = XEXP (operands[1], 0);
    int call_cookie = OPVAL(3)  & ~CALLCOOKIE_SIBLING_MASK;

    if (SYMBOL_REF == GET_CODE (addr))
      {
        const char * fname = XSTR (addr, 0);

        /* Avoid peephole2 for setjmp */
        if (! strcmp (fname, "setjmp"))
          call_cookie |= CALLCOOKIE_NO_SIBCALL_MASK;
      }

    operands[3] = GEN_INT (call_cookie);

    if (call_cookie & CALLCOOKIE_PXHNDCALL_MASK)
      {
        emit (gen_call_value_pxhndcall_rtx (operands[0], operands[1],
                                            operands[2], operands[3]));
        DONE;
      }

    if (call_cookie & CALLCOOKIE_LONGCALL_MASK)
      operands[1] = replace_equiv_address (operands[1],
                                           force_reg (SImode, addr));

    if (call_cookie & CALLCOOKIE_INTERRUPT_HANDLER_MASK)
      {
        emit (gen_call_irq_hnd_rtx (operands[1], operands[2], operands[3]));
        DONE;
      }

    if (call_cookie & CALLCOOKIE_INTERRUPT_MASK)
      {
        emit (gen_call_value_irq_rtx (operands[0], operands[1], operands[2],
                                      operands[3]));
        DONE;
      }
   })

(define_insn "*call_irq_insn"
  [(call (mem (match_operand:SI 0 "nonmemory_operand" "a ,Sc ,s"))
         (match_operand:SI 1 "const_int_operand"      "n ,n  ,n"))
   (use (match_operand:SI 2 "const_int_operand"       "n ,n  ,n"))
   (clobber (reg:DI REG_D14))
   (clobber (reg:DI REG_D12))
   (clobber (reg:DI REG_D10))
   (clobber (reg:DI REG_D8))
   (clobber (reg:SI REG_A15))
   (clobber (reg:DI REG_A12))
   (clobber (reg:SI REG_PSW))]
  "OPVAL(2) & CALLCOOKIE_INTERRUPT_MASK"
  {
    tric_output_call (insn, operands, 0);
    return "";
  }
  [(set_attr "pipe" "ctx")])

(define_insn "*call_value_irq_insn"
  [(set (match_operand:SI 0 "register_operand"             "=ad ,ad ,ad")
        (call (mem (match_operand:SI 1 "nonmemory_operand"   "a ,Sc ,s"))
              (match_operand:SI 2 "const_int_operand"        "n ,n  ,n")))
   (use (match_operand:SI 3 "const_int_operand"              "n ,n  ,n"))
   (clobber (reg:DI REG_D14))
   (clobber (reg:DI REG_D12))
   (clobber (reg:DI REG_D10))
   (clobber (reg:DI REG_D8))
   (clobber (reg:SI REG_A15))
   (clobber (reg:DI REG_A12))
   (clobber (reg:SI REG_PSW))]
  "OPVAL(3) & CALLCOOKIE_INTERRUPT_MASK"
  {
    tric_output_call (insn, operands, 1);
    return "";
  }
  [(set_attr "pipe" "ctx")])

(define_insn "*call_pxhndcall_insn"
  [(call (unspec:SI [(match_operand:SI 0 "const_int_operand" "Ku9")
                     ] UNSPEC_SYSCALL)
         (match_operand:SI 1 "const_int_operand"      "n"))
   (use (reg:SI REG_A12))
   (use (match_operand:SI 2 "const_int_operand"       "n"))]
  "OPVAL(2) & CALLCOOKIE_PXHNDCALL_MASK"
  {
    tric_output_call (insn, operands, 0);
    return "";
  }
  [(set_attr "pipe" "ctx")])

(define_insn "*call_value_pxhndcall_insn"
  [(set (match_operand 0 "register_operand"                "=ad")
        (call (unspec:SI [(match_operand:SI 1 "const_int_operand" "Ku9")
                          ] UNSPEC_SYSCALL)
              (match_operand:SI 2 "const_int_operand"       "n")))
   (use (reg:SI REG_A12))
   (use (match_operand:SI 3 "const_int_operand"             "n"))]
  "OPVAL(3) & CALLCOOKIE_PXHNDCALL_MASK"
  {
    tric_output_call (insn, operands, 1);
    return "";
  }
  [(set_attr "pipe" "ctx")])

(define_insn "call_insn"
  [(call (mem:HI (match_operand:SI 0 "nonmemory_operand" "a ,Sc ,s"))
         (match_operand:SI 1 "const_int_operand"         "n ,n  ,n"))
   (use (match_operand:SI 2 "const_int_operand"          "n ,n  ,n"))]
  "REG_P (operands[0])
   || !(OPVAL(2) & CALLCOOKIE_LONGCALL_MASK)"
  {
    tric_output_call (insn, operands, 0);
    return "";
  }
  [(set_attr "pipe" "ctx")])

(define_insn "call_value_insn"
  [(set (match_operand 0 "register_operand"                 "=da ,da ,da")
        (call (mem:HI (match_operand:SI 1 "nonmemory_operand" "a ,Sc ,s"))
              (match_operand:SI 2 "const_int_operand"         "n ,n  ,n")))
   (use (match_operand:SI 3 "const_int_operand"               "n ,n  ,n"))]
  "REG_P (operands[1])
   || !(OPVAL(3) & CALLCOOKIE_LONGCALL_MASK)"
  {
    tric_output_call (insn, operands, 1);
    return "";
  }
  [(set_attr "pipe" "ctx")])

(define_expand "sibcall"
  [(parallel[(call (match_operand 0 "memory_operand" "")
                   (match_operand 1 "const_int_operand" ""))
             (use (match_operand 2 "" ""))])]
  ""
  {
    int call_cookie = OPVAL(2);
    operands[2] = GEN_INT (call_cookie | CALLCOOKIE_SIBLING_MASK);

    if (call_cookie & CALLCOOKIE_LONGCALL_MASK)
      operands[0] = replace_equiv_address (operands[0],
                                           force_reg (SImode,
                                                      XEXP (operands[0], 0)));
  })

(define_expand "sibcall_value"
  [(parallel[(set (match_operand 0 "register_operand" "")
                  (call (match_operand 1 "memory_operand" "")
                        (match_operand 2 "const_int_operand" "")))
             (use (match_operand 3 "" ""))])]
  ""
  {
    int call_cookie = OPVAL(3);
    operands[3] = GEN_INT (call_cookie | CALLCOOKIE_SIBLING_MASK);

    if (call_cookie & CALLCOOKIE_LONGCALL_MASK)
      operands[1] = replace_equiv_address (operands[1],
                                           force_reg (SImode,
                                                      XEXP (operands[1], 0)));
  })

;; The middle end does not know that the RET of our target machine will
;; restore SP and therefore misses some cases where we can issue a tail call.
;; There are three cases:

;; Case 1: void tailcalling void
(define_peephole2
  [(parallel[(call (mem (match_operand:SI 0 "nonmemory_operand"  ""))
                   (match_operand 1 "const_int_operand" ""))
             (use (match_operand:SI 2 "const_int_operand" ""))])
   (return)]
  "tric_peep2_may_sibcall_p (operands[0], operands[2])"
  [(clobber (const_int 0))]
  {
    rtx call;
    operands[2] = GEN_INT (OPVAL(2) | CALLCOOKIE_SIBLING_MASK);
    call = emit (gen_call_insn (operands[0], operands[1], operands[2]));
    tric_peep2_setup_sibcall (curr_insn, call);
    DONE;
  })

;; Case 2: non-void tailcalling non-void
(define_peephole2
  [(parallel[(set (match_operand 0 "register_operand" "")
                  (call (mem (match_operand:SI 1 "nonmemory_operand"  ""))
                        (match_operand 2 "const_int_operand" "")))
             (use (match_operand:SI 3 "const_int_operand" ""))])
   (use (match_operand 4 "register_operand" ""))
   (return)]
  "tric_peep2_may_sibcall_p (operands[1], operands[3])"
  [(clobber (const_int 0))]
  {
    rtx call;
    operands[3] = GEN_INT (OPVAL(3) | CALLCOOKIE_SIBLING_MASK);
    call = emit (gen_call_value_insn (operands[0], operands[1],
                                      operands[2], operands[3]));
    tric_peep2_setup_sibcall (curr_insn, call);
    DONE;
  })

;; Case 3: void tailcalling non-void
(define_peephole2
  [(parallel[(set (match_operand 0 "register_operand" "")
                  (call (mem (match_operand:SI 1 "nonmemory_operand"  ""))
                        (match_operand 2 "const_int_operand" "")))
             (use (match_operand:SI 3 "const_int_operand" ""))])
   (return)]
  "tric_peep2_may_sibcall_p (operands[1], operands[3])"
  [(clobber (const_int 0))]
  {
    rtx call;
    operands[3] = GEN_INT (OPVAL(3) | CALLCOOKIE_SIBLING_MASK);
    call = emit (gen_call_value_insn (operands[0], operands[1],
                                      operands[2], operands[3]));
    tric_peep2_setup_sibcall (curr_insn, call);
    DONE;
  })


(define_expand "untyped_call"
  [(parallel [(call (match_operand 0 "" "")
                    (const_int 0))
              (match_operand 1 "" "")
              (match_operand 2 "" "")])]
  ""
  {
    sorry ("__builtin_apply is not supported for this machine");
  })

;; Helper for tric_emit_branch
(define_expand "branch_rtx"
  [(set (pc)
        (if_then_else (match_operand 1 "" "")
                      (label_ref (match_operand 0 "" ""))
                      (pc)))])

(define_expand "cstoresi4"
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operator:SI 1 "tric_comparison_operator"
                           [(match_operand:SI 2 "register_operand" "")
                            (match_operand:SI 3 "nonmemory_operand" "")]))]
  ""
  {
    tric_emit_setcompare (GET_CODE (operands[1]), operands[2], operands[3],
                          operands[0]);
    DONE;
  })

;; "cstoredi4"
;; "cstorepdi4"
(define_expand "cstore<mode>4"
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operator:SI 1 "tric_comparison_operator"
                           [(match_operand:DIPDI 2 "register_operand" "")
                            (match_operand:DIPDI 3 "nonmemory_operand" "")]))]
  ""
  {
    tric_emit_setcompare_di (GET_CODE (operands[1]), operands[2], operands[3],
                             operands[0]);
    DONE;
  })

;; "*bgt_zero" "*ble_zero"
;; "*bge_zero" "*blt_zero"
(define_insn "*b<code>_zero"
  [(set (pc)
        (if_then_else (gt_le_ge_lt (match_operand:SI 1 "register_operand"  "d")
                                   (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "j<code>z\t%1, %0"
  [(set_attr "pipe" "jip")
   (set_attr "type" "branch0_U4")])

;; "*beq" "*bne"
(define_insn "*b<code>"
  [(set (pc)
        (if_then_else (eq_ne (match_operand:SI 1 "register_operand"  "Dd  ,a   ,Dd    ,a")
                             (match_operand:SI 2 "reg_or_s4_operand" "K00 ,K00 ,d Ks4 ,a"))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "@
    j<tric_n_op>z\t%1, %0
    j<tric_n_op>z.a\t%1, %0
    j<tric_op>\t%1, %2, %0
    j<tric_op>.a\t%1, %2, %0"
  [(set_attr "pipe" "jip,jls,jip,jls")
   (set_attr "type" "branch0_U4_S8_d15,branch0_U4,branch_U4_d15,*")])

;; "*seq" "*sne"
(define_insn "*s<code>"
  [(set (match_operand:SI 0 "register_operand"            "=!D     ,d     ,d   ,d")
        (eq_ne:SI (match_operand:SI 1 "register_operand"   "%d     ,d     ,a   ,a")
                  (match_operand:SI 2 "reg_or_s9_operand"   "d Ks4 ,d Ks9 ,K00 ,a")))]
  ""
  "@
    <tric_op>\t%0, %1, %2
    <tric_op>\t%0, %1, %2
    <tric_op>z.a\t%0, %1
    <tric_op>.a\t%0, %1, %2"
  [(set_attr "pipe" "ip,ip,aalu,aalu")
   (set_attr "length" "*,4,4,4")
   (set_attr "adjust" "seq,*,*,*")])

;; "*bge" "*blt"
(define_insn "*b<code>"
  [(set (pc)
        (if_then_else (tric_signed_compare (match_operand:SI 1 "register_operand"  "d    ")
                                           (match_operand:SI 2 "reg_or_s4_operand" "d Ks4"))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "j<tric_op>\t%1, %2, %0"
  [(set_attr "pipe" "jip")
   (set_attr "type" "branch_s4")])


;; We allow constants in op $2 that the machine cannot handle to get better code
;; when saturating.
;; ??? Note that this pattern will hinder accumulate compare stuff because these
;; ??? patterns cannot handle the large constants we allow here.

;; "*sge" "*slt"
(define_insn "*s<code>"
  [(set (match_operand:SI 0 "register_operand"                          "=d    ")
        (tric_signed_compare:SI (match_operand:SI 1 "register_operand"   "d    ")
                                (match_operand:SI 2 "nonmemory_operand"  "d Ks9")))]
  ""
  "<tric_op>\t%0, %1, %2")


;; "*bgeu" "*bltu"
(define_insn "*b<code>"
  [(set (pc)
        (if_then_else (tric_unsigned_compare (match_operand:SI 1 "register_operand"  "d    ")
                                             (match_operand:SI 2 "reg_or_u4_operand" "d Ku4"))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "j<tric_op>.u\t%1, %2, %0"
  [(set_attr "pipe" "jip")
   (set_attr "type" "branch_u4")])

;; "*sgeu" "*sltu"
(define_insn "*s<code>"
  [(set (match_operand:SI 0 "register_operand"                            "=d     ,d")
        (tric_unsigned_compare:SI (match_operand:SI 1 "register_operand"   "d     ,a")
                                  (match_operand:SI 2 "nonmemory_operand"  "d Ku9 ,a")))]
  ""
  "@
    <tric_op>.u\t%0, %1, %2
    <tric_op>.a\t%0, %1, %2"
  [(set_attr "pipe" "ip,aalu")])

(define_expand "extzv"
  [(set (match_operand:SI 0 "register_operand" "")
        (zero_extract:SI (match_operand 1 "register_operand"  "")
                         (match_operand:SI 2 "u5_operand" "")    ; width
                         (match_operand:SI 3 "u5_operand" "")))] ; pos
  ""
  {
    if (!tric_opt_extract_to_subreg
        && SUBREG == GET_CODE (operands[0]))
      FAIL;
  })

(define_insn "*extzv"
  [(set (match_operand:SI 0 "register_operand"                   "=d ")
        (zero_extract:SI (match_operand:SI 1 "register_operand"   "d ")
                         (match_operand:SI 2 "u5_operand"         "n ")     ; width
                         (match_operand:SI 3 "reg_or_u5_operand"  "dn")))]  ; pos
  ""
  "extr.u\t%0, %1, %3, %2"
  [(set_attr "pipe" "ip2")])

(define_expand "extv"
  [(set (match_operand:SI 0 "register_operand" "")
        (sign_extract:SI (match_operand:SI 1 "register_operand"  "")
                         (match_operand:SI 2 "u5_operand" "")              ; width
                         (match_operand:SI 3 "u5_operand" "")))])          ; pos

(define_insn "*extv"
  [(set (match_operand:SI 0 "register_operand"                   "=d ")
        (sign_extract:SI (match_operand:SI 1 "register_operand"   "d ")
                         (match_operand:SI 2 "u5_operand"         "n ")    ; width
                         (match_operand:SI 3 "reg_or_u5_operand"  "dn")))] ; pos
  ""
  {
    return CONST_INT_P (operands[3])
           && 32 == INTVAL (operands[2]) + INTVAL (operands[3])
      ? "shas\t%0, %1, %n3"
      : "extr\t%0, %1, %3, %2";
  }
  [(set_attr "pipe" "ip2")])

(define_expand "insv"
  [(set (zero_extract:SI (match_operand:SI 0 "nonimmediate_operand" "")
                         (match_operand:SI 1 "u5_operand"  "")    ; width
                         (match_operand:SI 2 "u5_operand"  ""))   ; pos
        (match_operand:SI 3 "reg_or_u4_operand" ""))]
  ""
  {
    if (SUBREG == GET_CODE (operands[0])
        && GET_MODE_SIZE (GET_MODE (SUBREG_REG (operands[0]))) > 4)
      FAIL;

    if (register_operand (operands[0], SImode)
        && REG_P (operands[3])
        && OPVAL (1) == 16)
      {
        /* ; Seen in PxROS when working with 16-bit components in structs.
           ; Use algebraic expansion to allow different input and output in
           ; the remainder. */
        if (OPVAL(2) == 0)
          {
            emit_insn (gen_insv_1_ic (operands[0], operands[3],
                                      GEN_INT (0xffff), operands[0],
                                      gen_int_mode (0xffff0000, SImode)));
            DONE;
          }

        if (OPVAL(2) == 16)
          {
            emit_insn (gen_insv_3a_ic (operands[0], operands[3],
                                       GEN_INT (16),
                                       gen_int_mode (0xffff0000, SImode),
                                       operands[0],
                                       gen_int_mode (0xffff, SImode)));
            DONE;
          }
      }

    if (const0_rtx == operands[3]
        && register_operand (operands[0], SImode))
      {
        unsigned int mask = (1 << OPVAL (1)) - 1;
        mask = ~(mask << OPVAL(2));
        emit_insn (gen_andsi3 (operands[0], operands[0],
                               gen_int_mode (mask, SImode)));
        DONE;
      }

    /* ??? Must check mode explicitely */

    if (!nonimmediate_operand (operands[0], SImode)
        || (MEM_P (operands[0])
            && MEM_VOLATILE_P (operands[0])))
      FAIL;
  })

(define_insn "*insv"
  [(set (zero_extract:SI (match_operand:SI 0 "register_operand"  "+d   ,d   ,d  ")
                         (match_operand:SI 1 "u5_operand"         "K01 ,n   ,Kx1")    ; width
                         (match_operand:SI 2 "u5_operand"         "n   ,n   ,n  "))   ; pos
        (match_operand:SI 3 "reg_or_u4_operand"                   "d   ,Ku4 ,d  "))]
  ""
  "@
    ins.t\t%0, %0,%2, %3,0
    insert\t%0, %0, %3, %2, %1
    insert\t%0, %0, %3, %2, %1"
  [(set_attr "pipe" "ip,ip2,ip2")])

(define_insn "abssi2"
  [(set (match_operand:SI 0 "register_operand"          "=d")
        (abs:SI (match_operand:SI 1 "register_operand"   "d")))]
  ""
  "abs\t%0, %1"
  [(set_attr "pipe" "ip")])

(define_insn "smaxsi3"
  [(set (match_operand:SI 0 "register_operand"            "=d    ")
        (smax:SI (match_operand:SI 1 "register_operand"   "%d    ")
                 (match_operand:SI 2 "nonmemory_operand"  "d Ks9")))]
  ""
  "max\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "umaxsi3"
  [(set (match_operand:SI 0 "register_operand"           "=d  ")
        (umax:SI (match_operand:SI 1 "register_operand"  "%d  ")
                 (match_operand:SI 2 "nonmemory_operand" "d Ku9")))]
  ""
  "max.u\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "sminsi3"
  [(set (match_operand:SI 0 "register_operand"           "=d  ")
        (smin:SI (match_operand:SI 1 "register_operand"  "%d  ")
                 (match_operand:SI 2 "nonmemory_operand" "d Ks9")))]
  ""
  "min\t%0, %1, %2"
  [(set_attr "pipe" "ip")])

(define_insn "uminsi3"
  [(set (match_operand:SI 0 "register_operand"            "=d  ,d   ,d  ")
        (umin:SI (match_operand:SI 1 "register_operand"   "%d  ,d   ,d  ")
                 (match_operand:SI 2 "nonmemory_operand"  "Ko8 ,Kog ,d Ku9")))]
  ""
  "@
    sat.bu\t%0, %1
    sat.hu\t%0, %1
    min.u\t%0, %1, %2"
  [(set_attr "pipe" "ip")
   (set_attr "adjust" "sat,sat,*")])


(define_insn "abssf2"
  [(set (match_operand:SF 0 "register_operand"          "=d")
        (abs:SF (match_operand:SF 1 "register_operand"   "d")))]
  ""
  "insert\t%0, %1, 0, 31, 1"
  [(set_attr "pipe" "ip2")])

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

;; Prologue and epilogue.

(define_expand "prologue"
  [(const_int 1)]
  ""
  {
    tric_emit_prologue ();
    DONE;
  })

(define_expand "epilogue"
  [(const_int 1)]
  ""
  {
    tric_emit_epilogue (0);
    DONE;
  })

(define_expand "sibcall_epilogue"
  [(const_int 1)]
  ""
  {
    tric_emit_epilogue (1);
    DONE;
  })

;; There are cases where dead regs remain and gcc fails to eliminate the
;; corresponding insns. Clobbering the used regs helps out of that.
;; Example: Splitting DF in the sin functions in libm leaves mess otherwise.
;; Note that we set flag_split_wide_types = 0 for that reason, too.
(define_expand "return"
  [(return)]
  "tric_simple_epilogue()")

(define_insn "return_insn"
  [(return)]
  "reload_completed"
  {
    if (tric_interrupt_function_p (current_function_decl))
      return "ji\t%%A11";
    else if (tric_interrupt_handler_function_p (current_function_decl))
      return "rslcx\;rfe";

    return "ret";
  }
  [(set_attr "pipe" "ctx")
   (set_attr "length" "2")])

;; Special insn used by rtl-factoring
(define_insn "freturn"
  [(set (pc)
        (unspec [(reg:SI REG_A11)
                 (const_int 0)] UNSPEC_FCALL))]
  "TRIC_HAVE_FCALL
   && reload_completed"
  "fret"
  [(set_attr "pipe" "ctx")
   (set_attr "length" "2")])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; LOOP

;; if (--$0 != 0) goto $1
(define_expand "doloop_end"
  [(parallel [(match_operand:SI 0 "register_operand"  "") ; loop register
              (match_operand:SI 1 "general_operand"   "") ; label
              ])]
  "tric_opt_loop"
  {
    if (!tric_opt_loop
        /* strange, but this is *not* uneachable */
        || SImode != GET_MODE (operands[0]))
      FAIL;

    emit_jump_insn (gen_branch_and_decrement (operands[0], operands[1]));
    DONE;
  })

(define_insn "branch_and_decrement"
  [(set (pc)
        (if_then_else (ne (match_operand:SI 0 "nonimmediate_operand" "+a,+?d,??*m")
                          (const_int 0))
                      (label_ref (match_operand 1 "" ""))
                      (pc)))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (const_int -1)))
   (clobber (match_scratch:SI 2 "X,X,=&d"))]
  "tric_opt_loop"
  "@
    loop\t%0, %1
    jned\t%0, 0, %1
    ld.w\t%2, %0\;add\t%2, -1\;st.w\t%0, %2\;jne\t%2, -1, %1"
  [(set_attr "pipe" "lp,jip,ldd")
   (set_attr "length" "*,4,14")
   (set_attr "type" "loop1,*,*")])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; SWAP.W

(define_expand "swap_w"
  [(parallel [(match_operand:SI 0 "register_operand"  "")
              (match_operand:SI 1 "register_operand"  "")
              (match_operand:SI 2 "register_operand"  "")])]
  ""
  {
    operands[1] = gen_rtx_MEM (SImode, operands[1]);
    MEM_VOLATILE_P (operands[1]) = 1;
    emit_insn (gen_swap_w_insn (operands[0], operands[1], operands[2]));
    DONE;
  })

(define_expand "swapmsk_w"
  [(parallel [(match_operand:SI 0 "register_operand"  "")
              (match_operand:SI 1 "register_operand"  "")
              (match_operand:SI 2 "register_operand"  "")
              (match_operand:SI 3 "register_operand"  "")])]
  "TRIC_HAVE_SWAPMSK_W"
  {
    rtx xreg = gen_reg_rtx (DImode);
    operands[1] = gen_rtx_MEM (SImode, operands[1]);
    MEM_VOLATILE_P (operands[1]) = 1;

    emit_move_insn (simplify_gen_subreg (SImode, xreg, DImode, 0), operands[2]);
    emit_move_insn (simplify_gen_subreg (SImode, xreg, DImode, 4), operands[3]);
    emit_insn (gen_swapmsk_w_insn (operands[0], operands[1], xreg));
    DONE;
  })

(define_expand "cmpswap_w"
  [(parallel [(match_operand:SI 0 "register_operand"  "")
              (match_operand:SI 1 "register_operand"  "")
              (match_operand:SI 2 "register_operand"  "")
              (match_operand:SI 3 "register_operand"  "")])]
  "TRIC_HAVE_CMPSWAP_W"
  {
    rtx xreg = gen_reg_rtx (DImode);
    operands[1] = gen_rtx_MEM (SImode, operands[1]);
    MEM_VOLATILE_P (operands[1]) = 1;

    emit_move_insn (simplify_gen_subreg (SImode, xreg, DImode, 0), operands[2]);
    emit_move_insn (simplify_gen_subreg (SImode, xreg, DImode, 4), operands[3]);
    emit_insn (gen_cmpswap_w_insn (operands[0], operands[1], xreg));
    DONE;
  })

(define_insn "swap_w_insn"
  [(set (match_operand:SI 0 "register_operand"                         "=d")
        (unspec_volatile:SI [(match_operand:SI 1 "vok_memory_operand"  "+Wl")
                             ] UNSPECV_SWAP_W))
   (set (match_dup 1)
        (unspec_volatile:SI [(match_operand:SI 2 "register_operand"     "0")
                             ] UNSPECV_SWAP_W))]
  ""
  "swap.w\t%1, %0"
  [(set_attr "pipe" "std")])

(define_insn "swapmsk_w_insn"
  [(set (match_operand:SI 0 "register_operand"                         "=d")
        (unspec_volatile:SI [(match_operand:SI 1 "vok_memory_operand"  "+Wc")
                             (match_operand:DI 2 "register_operand"     "0")
                             ] UNSPECV_SWAPMSK_W))
   (set (match_dup 1)
        (unspec_volatile:SI [(match_dup 1)
                             (match_dup 2)
                             ] UNSPECV_SWAPMSK_W))]
  "TRIC_HAVE_SWAPMSK_W"
  "swapmsk.w\t%1, %A0"
  [(set_attr "pipe" "std")])

(define_insn "cmpswap_w_insn"
  [(set (match_operand:SI 0 "register_operand"                         "=d")
        (unspec_volatile:SI [(match_operand:SI 1 "vok_memory_operand"  "+Wc")
                             (match_operand:DI 2 "register_operand"     "0")
                             ] UNSPECV_CMPSWAP_W))
   (set (match_dup 1)
        (unspec_volatile:SI [(match_dup 1)
                             (match_dup 2)
                             ] UNSPECV_CMPSWAP_W))]
  "TRIC_HAVE_CMPSWAP_W"
  "cmpswap.w\t%1, %A0"
  [(set_attr "pipe" "std")])


;; Some optimization passes decide that reg-reg moves are cheaper
;; than reg-const moves, amongst them are cse and postreload-cse.

;; This peep2 tries to fix that for some common cases.
;; We reorder operands so that TC16's 64-bit move might fit.
(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "s16_operand" ""))
   (set (match_operand:SI 2 "register_operand" "")
        (match_dup 0))
   (set (match_operand:SI 3 "register_operand" "")
        (match_dup 0))
   (set (match_operand:SI 4 "register_operand" "")
        (match_dup 0))]
  ""
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 3)
        (match_dup 1))
   (set (match_dup 4)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 1))]
  {
    if (!satisfies_constraint_Ku3 (operands[1]))
      {
        if (optimize_size
            || A_REG_P (operands[0])
            || A_REG_P (operands[2])
            || A_REG_P (operands[3])
            || A_REG_P (operands[4]))
          FAIL;
      }

    PEEP2_ORDER_REGS (0, 2, 3);
    PEEP2_ORDER_REGS (0, 3, 4);
    PEEP2_ORDER_REGS (0, 2, 3);
  })

;; 3-reg version of the peep2 above
(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "s16_operand" ""))
   (set (match_operand:SI 2 "register_operand" "")
        (match_dup 0))
   (set (match_operand:SI 3 "register_operand" "")
        (match_dup 0))]
  ""
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 3)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 1))]
  {
    if (!satisfies_constraint_Ku3 (operands[1]))
      {
        if (optimize_size
            || A_REG_P (operands[0])
            || A_REG_P (operands[2])
            || A_REG_P (operands[3]))
          FAIL;
      }

    PEEP2_ORDER_REGS (0, 2, 3);
  })

;; 2-reg version of the peep2 above
(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "s16_operand" ""))
   (set (match_operand:SI 2 "register_operand" "")
        (match_dup 0))]
  ""
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 1))]
  {
    if (!satisfies_constraint_Ku3 (operands[1]))
      {
        if (optimize_size
            || A_REG_P (operands[0])
            || A_REG_P (operands[2]))
          FAIL;
      }
  })


(define_insn "*mov64"
  [(parallel [(set (match_operand:SISF   0 "e_reg_operand"                  "=d,d      ,d  ")
                   (match_operand:SISF   1 "d_reg_or_const0_or_s16_operand"  "d,Kuf G00,Kcf"))
              (set (match_operand:SISF_B 2 "d_reg_operand"                  "=d,d      ,d  ")
                   (match_operand:SISF_B 3 "d_reg_or_const0_or_s1_operand"   "d,K00 G00,Km1"))])]
  "TRIC_HAVE_MOV64
   && reload_completed
   && REGNO(operands[2]) == 1+REGNO(operands[0])"
  "@
    mov\t%A0, %3, %1
    mov\t%A0, %1
    mov\t%A0, %1")

(define_peephole2
  [(set (match_operand:SISF   0 "d_reg_operand" "")
        (match_operand:SISF   1 "d_reg_or_const0_or_s16_operand" ""))
   (set (match_operand:SISF_B 2 "d_reg_operand" "")
        (match_operand:SISF_B 3 "d_reg_or_const0_or_s16_operand" ""))]
  "TRIC_HAVE_MOV64"
  [(parallel [(set (match_dup 4)
                   (match_dup 5))
              (set (match_dup 6)
                   (match_dup 7))])]
  {
    unsigned int reg0 = REGNO (operands[0]);
    unsigned int reg2 = REGNO (operands[2]);

    /* Reorder operands so that destination becomes an E-reg or fail. */

    if (E_REGNO_P(reg0)  &&  reg2 == 1 + reg0)
      {
        operands[4] = operands[0];
        operands[5] = operands[1];
        operands[6] = operands[2];
        operands[7] = operands[3];
      }
    else if (E_REGNO_P(reg2)  &&  reg0 == 1 + reg2)
      {
        operands[4] = operands[2];
        operands[5] = operands[3];
        operands[6] = operands[0];
        operands[7] = operands[1];
      }
    else
      FAIL;

    /* Filter out cases we cannot represent by means of a 64-bit move */

    if (REG_P (operands[1])  &&  REG_P (operands[3]))
      {
        /* Case 1: moving two regs. Ensure no bad dependency and no trivial move */
        if (reg0 == REGNO (operands[3])
            || REGNO (operands[4]) == REGNO (operands[5])
            || REGNO (operands[6]) == REGNO (operands[7]))
          {
            FAIL;
          }
      }
    else if (rtx_equal_p (operands[7], CONST0_RTX (GET_MODE (operands[6]))))
      {
        /* Case 2: moving 0 to HIGH reg. Ensure zero-extension of LOW part. */
        if (! (satisfies_constraint_Kuf (operands[5])
               || satisfies_constraint_G00 (operands[5])))
          {
            FAIL;
          }
      }
    else
      {
        /* Case 3: moving -1 to HIGH reg. Ensure sign-extension of LOW part. */
        if (! (satisfies_constraint_Kcf (operands[5])
               && constm1_rtx == operands[7]))
          {
            FAIL;
          }
      }
  })

(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "register_operand" ""))
   (use (match_operand 2 "register_operand" ""))
   (return)]
  ""
  [(use (match_dup 2))
   (return)]
  {
    if (reg_overlap_mentioned_p (operands[0], operands[2]))
      FAIL;
  })

(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operator:SI 4 "tric_s10_operator"
                           [(match_operand:SI 1 "register_operand"  "")
                            (match_operand:SI 2 "reg_or_s10_operand" "")]))
   (set (match_operand:SI 3 "register_operand" "")
        (match_dup 0))]
  "peep2_reg_dead_p (2, operands[0])
   && ((A_REG_P (operands[0]) && A_REG_P (operands[3]))
       || (D_REG_P (operands[0]) && D_REG_P (operands[3])))"
  [(set (match_dup 3)
        (match_dup 4))]
  {
    /* Avoid "*andsi3_255" because of narrow constraints */

    if (REG_D15 == REGNO (operands[0])
        && REG_D15 == REGNO (operands[1])
        && CONST_INT_P (operands[2])
        && 255 == OPVAL (2))
      {
        FAIL;
      }
  })

(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operator:SI 4 "tric_s9_operator"
                           [(match_operand:SI 1 "register_operand"  "")
                            (match_operand:SI 2 "reg_or_s9_operand" "")]))
   (set (match_operand:SI 3 "register_operand" "")
        (match_dup 0))]
  "peep2_reg_dead_p (2, operands[0])
   && ((A_REG_P (operands[0]) && A_REG_P (operands[3]))
       || (D_REG_P (operands[0]) && D_REG_P (operands[3])))"
  [(set (match_dup 3)
        (match_dup 4))])

(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operator:SI 4 "tric_u9_operator"
                           [(match_operand:SI 1 "register_operand"  "")
                            (match_operand:SI 2 "reg_or_u9_operand" "")]))
   (set (match_operand:SI 3 "register_operand" "")
        (match_dup 0))]
  "peep2_reg_dead_p (2, operands[0])
   && ((A_REG_P (operands[0]) && A_REG_P (operands[3]))
       || (D_REG_P (operands[0]) && D_REG_P (operands[3])))"
  [(set (match_dup 3)
        (match_dup 4))])

(define_expand "cbranchsi4"
  [(set (pc)
        (if_then_else (match_operator:SI 0 "tric_comparison_operator"
                                         [(match_operand:SI 1 "register_operand" "")
                                          (match_operand:SI 2 "nonmemory_operand" "")])
                      (label_ref (match_operand 3 "" ""))
                      (pc)))]
  ""
  {
    tric_emit_cbranchsi4 (operands);
    DONE;
  })

;; "cbranchdi4"
;; "cbranchpdi4"
(define_expand "cbranch<mode>4"
  [(set (pc)
        (if_then_else (match_operator:SI 0 "tric_comparison_operator"
                                         [(match_operand:DIPDI 1 "register_operand" "")
                                          (match_operand:DIPDI 2 "nonmemory_operand" "")])
                      (label_ref (match_operand 3 "" ""))
                      (pc)))]
  ""
  {
    tric_emit_cbranchdi4 (operands);
    DONE;
  })

(define_expand "cbranchsf4"
  [(set (pc)
        (if_then_else (match_operator:SI 0 "tric_float_comparison_operator"
                                         [(match_operand:SF 1 "register_operand" "")
                                          (match_operand:SF 2 "register_operand" "")])
                      (label_ref (match_operand 3 "" ""))
                      (pc)))]
  ""
  {
    tric_emit_cbranchsf4 (operands);
    DONE;
  })


(define_insn "clzsi2"
  [(set (match_operand:SI 0 "register_operand"          "=d")
        (clz:SI (match_operand:SI 1 "register_operand"   "d")))]
  ""
  "clz\t%0, %1"
  [(set_attr "pipe" "ip")])

(define_expand "ffssi2"
  [(set (match_dup 2)
        (neg:SI (match_operand:SI 1 "register_operand" "d")))
   (set (match_dup 3)
        (and:SI (match_dup 2)
                (match_dup 1)))
   (set (match_dup 3)
        (clz:SI (match_dup 3)))
   (set (match_operand:SI 0 "register_operand" "=d")
        (minus:SI (const_int 32)
                  (match_dup 3)))]
  ""
  {
    operands[2] = gen_reg_rtx (SImode);
    operands[3] = gen_reg_rtx (SImode);
  })


(define_insn "popcountsi2"
  [(set (match_operand:SI 0 "register_operand"              "=d")
        (popcount:SI (match_operand:SI 1 "register_operand"  "d")))]
  "TRIC_HAVE_POPCNT"
  "popcnt.w\t%0, %1"
  [(set_attr "pipe" "ip")])


(define_insn "bswapsi2"
  [(set (match_operand:SI 0 "register_operand"            "=d")
        (bswap:SI (match_operand:SI 1 "register_operand"   "d")))]
  "TRIC_HAVE_SHUFFLE"
  "shuffle\t%0, %1, 0b00011011"
  [(set_attr "pipe" "ip")])


(define_insn "mfcr"
  [(set (match_operand:SI 0 "register_operand" "=d")
        (unspec_volatile:SI [(match_operand 1 "const_string_operand" "")
                             ] UNSPECV_MFCR))]
  ""
  "mfcr\t%0, %1"
  [(set_attr "pipe" "mixdd")])

;; Implement setjmp/longjmp insns

;; Store $PCXI at offset 4 of jmp_buf.
;; The other slots are already set up.
(define_expand "builtin_setjmp_setup"
  [(set (match_dup 1)
        (unspec_volatile:SI [(match_dup 2)] UNSPECV_MFCR))
   (set (mem:SI (match_operand:SI 0 "memory_operand"))
        (match_dup 1))
   (set (mem:SI (match_dup 3))
        (reg:SI REG_A11))]
  ""
  {
    operands[0] = plus_constant (Pmode, operands[0], 3 * GET_MODE_SIZE (Pmode));
    operands[1] = gen_reg_rtx (SImode);
    operands[2] = gen_rtx_CONST_STRING (VOIDmode, "$pcxi");
    operands[3] = plus_constant (Pmode, operands[0], GET_MODE_SIZE (Pmode));
  })

(define_expand "builtin_longjmp"
  [(use (match_operand:SI 0 "register_operand" ""))]
  ""
  {
    rtx cntxt_reg = gen_reg_rtx (SImode);
    rtx receiver_label = gen_reg_rtx (SImode);
    int psize = GET_MODE_SIZE (Pmode);

    emit_move_insn (cntxt_reg,
                    gen_rtx_MEM (SImode, plus_constant (Pmode, operands[0], 3*psize)));

    emit_library_call (gen_rtx_SYMBOL_REF (Pmode,
                                           "__tric_restore_context_queue"),
                       LCT_NORMAL, VOIDmode, 1, cntxt_reg, SImode, intSI_type_node);

    emit_move_insn (frame_pointer_rtx,
                    gen_rtx_MEM (Pmode, plus_constant (Pmode, operands[0], 0)));

    emit_move_insn (receiver_label,
                    gen_rtx_MEM (Pmode, plus_constant (Pmode, operands[0], psize)));

    emit_move_insn (stack_pointer_rtx,
                    gen_rtx_MEM (Pmode, plus_constant (Pmode, operands[0], 2*psize)));

    emit_move_insn (gen_rtx_REG (Pmode, REG_A11),
                    gen_rtx_MEM (Pmode, plus_constant (Pmode, operands[0], 4*psize)));

    emit_use (gen_rtx_REG (SImode, REG_A11));

    emit_jump_insn (gen_indirect_jump (receiver_label));

    emit_barrier();
    DONE;
  })


(define_expand "setmemsi"
  [(parallel [(set (match_operand:BLK 0 "memory_operand" "")
                   (unspec:BLK [(match_operand:SI 1 "nonmemory_operand" "") ;; # bytes
                                (match_operand:SI 2 "nonmemory_operand" "") ;; value
                                (match_operand:SI 3 "const_int_operand" "") ;; alignment
                                ] UNSPEC_MEMSET))
              (clobber (scratch:SI))
              (clobber (match_dup 5))   ;; value register
              (use (match_dup 4))       ;; # remaining bytes
              (clobber (scratch:SI))])]
  ""
  {
    enum machine_mode mode = tric_mode_for_align (OPVAL(3), operands[1]);
    int usize = GET_MODE_SIZE (mode);

    operands[3] = GEN_INT (usize);
    operands[4] = const0_rtx;
    operands[5] = gen_rtx_SCRATCH (mode);
 
    if (CONST_INT_P (operands[1]))
      {
        operands[4] = GEN_INT (OPVAL(1) % usize);
        operands[1] = GEN_INT (OPVAL(1) / usize);
      }

    operands[0] = replace_equiv_address (operands[0],
                                         copy_to_mode_reg (Pmode, XEXP (operands[0], 0)));
  })

(define_mode_attr setmem_clobber
  [(QI "&2 ,&2 ,&d ,&d")
   (HI "&2 ,&2 ,&d ,&d")
   (SI "&2 ,&2 ,&d ,&d")
   (DI "&d ,&d ,&d ,&d")])

;; "*setmemsi.qi
;; "*setmemsi.hi
;; "*setmemsi.si
;; "*setmemsi.di
(define_insn "*setmemsi.<mode>"
  [(set (mem:BLK (match_operand:SI 0 "register_operand"            "a  ,a  ,a  ,a"))   ;; address
        (unspec:BLK [(match_operand:SI 1 "nonmemory_operand"       "a  ,Kue,a  ,Kue")   ;; # units
                     (match_operand:SI 2 "nonmemory_operand"       "d  ,d  ,n  ,n")     ;; value
                     (match_operand:SI 3 "const_int_operand"       "Kxa,Kxa,Kxa,Kxa")   ;; unit size
                     ] UNSPEC_MEMSET))
   (clobber (match_scratch:SI 4                                   "=&1 ,&a ,&1 ,&a"))   ;; loop register
   (clobber (match_scratch:QIHISIDI 5                             "=<setmem_clobber>"))   ;; value register
   (use (match_operand:SI 6 "const_int_operand"                    "Ku3,Ku3,Ku3,Ku3"))  ;; # bytes after loop
   (clobber (match_scratch:SI 7                                   "=&0 ,&0 ,&0 ,&0"))]  ;; address register
  ""
  {
    int remains = OPVAL (6);
    bool insert = false;
    bool looping = true;
    unsigned int unrolls = 1;

    /* Set up value */

    if (CONST_INT_P (operands[2]))
      {
        if (const0_rtx == operands[2])
          {
            if (DImode == <MODE>mode)
              output_asm_insn (TRIC_HAVE_MOV64
                               ? "mov\t%A5, 0"
                               : "imask\t%A5, 0, 0, 0", operands);
            else
              output_asm_insn ("mov\t%5, 0", operands);
          }
        else if (QImode == <MODE>mode && u3_operand (operands[2], SImode))
          {
            output_asm_insn ("mov\t%5, %2", operands);
          }
        else if (0xff == (0xff & OPVAL (2)))
          {
            if (TRIC_HAVE_MOV64 && DImode == <MODE>mode)
              output_asm_insn ("mov\t%A5, -1", operands);
            else
              {
                output_asm_insn ("mov\t%5, -1", operands);
                if (DImode == <MODE>mode)
                  output_asm_insn ("mov\t%H5, -1", operands);
              }
          }
        else
          {
            operands[2] = GEN_INT (0x101 * (0xff & OPVAL (2)));
            output_asm_insn ("mov.u\t%5, %2", operands);
            insert = true;
          }
      }
    else
      {
        if (QImode != <MODE>mode)
          output_asm_insn ("insert\t%5, %2, %2, 8, 8", operands);

        insert = true;
      }

    if (insert)
      {
        if (SImode == <MODE>mode || DImode == <MODE>mode)
          output_asm_insn ("insert\t%5, %5, %5, 16, 16", operands);

        if (DImode == <MODE>mode)
          output_asm_insn ("mov\t%H5, %L5", operands);
      }

    /* Load loop counter */

    if (CONST_INT_P (operands[1]))
      {
        if (0 == OPVAL(1))
          goto skip_loop;
    
        if (OPVAL(1) >= 1 && OPVAL(1) <= 3)
          looping = false, unrolls = OPVAL(1);
        else
          output_asm_insn ("lea\t%4, %1-1", operands);
      }
    else
      {
        output_asm_insn ("j\t1f", operands);
      }

    /* Perform set loop */

    if (looping)
      output_asm_insn ("0:", operands);

    while (unrolls--)
      {
        if (DImode == <MODE>mode)
          output_asm_insn ("st.<store_suffix>\t[%0+]%3, %A5", operands);
        else
          output_asm_insn ("st.<store_suffix>\t[%0+]%3, %5", operands);
      }

    if (!CONST_INT_P (operands[1]))
      output_asm_insn ("1:", operands);

    if (looping)
      output_asm_insn ("loop\t%4, 0b", operands);

    /* Set remaining bytes that are not covered by loop (speed only).  */
skip_loop:;

    if (remains & 4)
      output_asm_insn ("st.w\t[%0+]4, %5", operands);

    if (remains & 2)
      output_asm_insn ("st.h\t[%0+]2, %5", operands);

    if (remains & 1)
      output_asm_insn ("st.b\t[%0], %5", operands);

    return "";
  })


(define_expand "movmemsi"
  [(parallel [(set (match_operand:BLK 0 "memory_operand" "")
                   (unspec:BLK [(match_operand:BLK 1 "memory_operand" "")
                                ] UNSPEC_MOVMEM))
              (use (match_operand:SI 2 "nonmemory_operand" "")) ;; # bytes
              (use (match_operand:SI 3 "const_int_operand" "")) ;; shared align
              (use (match_dup 5))    ;; remaining bytes after loop
              (clobber (scratch:SI))  ;; &src
              (clobber (scratch:SI))  ;; &dest
              (clobber (scratch:SI))  ;; loop counter
              (clobber (match_dup 4))])] ;; tmp register
  ""
  {
    rtx src, dest;

    enum machine_mode mode = tric_mode_for_align (OPVAL (3), operands[2]);
    int chunk = GET_MODE_SIZE (mode);

    if (CONST_INT_P (operands[2]))
      {
        operands[5] = GEN_INT (UINTVAL (operands[2]) % chunk);
        operands[2] = GEN_INT (UINTVAL (operands[2]) / chunk);
      }
    else
      {
        operands[5] = const0_rtx;
      }

    if (optimize_insn_for_size_p())
      {
        if (!CONST_INT_P (operands[2])
            || OPVAL(5))
          {
            FAIL;
          }
      }

    operands[4] = gen_reg_rtx (mode);
    dest = copy_to_mode_reg (Pmode, XEXP (operands[0], 0));
    src  = copy_to_mode_reg (Pmode, XEXP (operands[1], 0));
    operands[0] = replace_equiv_address (operands[0], dest);
    operands[1] = replace_equiv_address (operands[1], src);
    operands[3] = GEN_INT (chunk);
  })

;; "*movmemsi.qi"
;; "*movmemsi.hi"
;; "*movmemsi.si"
;; "*movmemsi.di"
(define_insn "*movmemsi.<mode>"
  [(set (mem:BLK (match_operand:SI 0 "register_operand"               "a,a"))    ;; dest
        (unspec:BLK [(mem:BLK (match_operand:SI 1 "register_operand"  "a,a"))    ;; src
                     ] UNSPEC_MOVMEM))
   (use (match_operand:SI 2 "nonmemory_operand"                       "a,Kue"))  ;; # chunks
   (use (match_operand:SI 3 "const_int_operand"                       "n,n"))    ;; chunk size
   (use (match_operand:SI 6 "const_int_operand"                       "n,n"))    ;; remaining bytes after loop
   (clobber (match_scratch:SI 7                                      "=0,0"))    ;; & dest
   (clobber (match_scratch:SI 8                                      "=1,1"))    ;; & src
   (clobber (match_scratch:SI 4                                      "=2,&a"))   ;; loop counter
   (clobber (match_operand:QIHISIDI 5 "register_operand"             "=&d,&d"))] ;; clobber register
  ""
  {
    int same_addr = REGNO (operands[0]) == REGNO (operands[1]);
    int remains = OPVAL (6);
    bool looping = true;
    unsigned int unrolls = 1;

    output_asm_insn (ASM_COMMENT_START " #chunks=%2, chunksize=%3, remains=%6",
                     operands);
    /* Load loop counter */

    if (CONST_INT_P (operands[2]))
      {
        if (0 == OPVAL(2))
          goto skip_loop;
    
        if (1 == OPVAL(2) || 2 == OPVAL(2))
          looping = false, unrolls = OPVAL(2);
        else
          output_asm_insn ("lea\t%4, %2-1", operands);
      }
    else
      {
        output_asm_insn ("j\t1f", operands);
      }

    /* Perform copy loop */

    if (looping)
      output_asm_insn ("0:", operands);

    while (unrolls--)
      {
        if (DImode == <MODE>mode)
          {
            output_asm_insn (same_addr
                             ? "ld.<load_suffix>\t%A5, [%1]"
                             : "ld.<load_suffix>\t%A5, [%1+]%3", operands);
            output_asm_insn ("st.<store_suffix>\t[%0+]%3, %A5", operands);
          }
        else
          {
            output_asm_insn (same_addr
                             ? "ld.<load_suffix>\t%5, [%1]"
                             : "ld.<load_suffix>\t%5, [%1+]%3", operands);
            output_asm_insn ("st.<store_suffix>\t[%0+]%3, %5", operands);
          }
      }

    if (!CONST_INT_P (operands[2]))
      output_asm_insn ("1:", operands);

    if (looping)
      output_asm_insn ("loop\t%4, 0b", operands);

    /* Set remaining bytes that are not covered by loop (speed only).  */
skip_loop:;

    if (remains & 4)
      {
        output_asm_insn (same_addr
                         ? "ld.w\t%5, [%1]"
                         : "ld.w\t%5, [%1+]4", operands);
        output_asm_insn ("st.w\t[%0+]4, %5", operands);
      }

    if (remains & 2)
      {
        output_asm_insn (same_addr
                         ? "ld.h\t%5, [%1]"
                         : "ld.h\t%5, [%1+]2", operands);
        output_asm_insn ("st.h\t[%0+]2, %5", operands);
      }

    if (remains & 1)
      {
        output_asm_insn (same_addr
                         ? "ld.bu\t%5, [%1]"
                         : "ld.bu\t%5, [%1+]1", operands);
        output_asm_insn ("st.b\t[%0+]1, %5", operands);
      }

    return "";
  })


(define_expand "movstr"
  [(parallel [(match_operand:SI 0 "register_operand" "")    ;; location of finally stored \0
              (match_operand:BLK 1 "memory_operand"  "")    ;; destination
              (match_operand:BLK 2 "memory_operand"  "")])] ;; source
  ""
  {
    rtx lab = gen_label_rtx();
    rtx a_dest = copy_to_mode_reg (Pmode, XEXP (operands[1], 0));
    rtx a_src  = copy_to_mode_reg (Pmode, XEXP (operands[2], 0));
    rtx m_dest = gen_rtx_MEM (QImode, gen_rtx_POST_INC (Pmode, a_dest));
    rtx m_src  = gen_rtx_MEM (QImode, gen_rtx_POST_INC (Pmode, a_src));
    rtx data  = gen_reg_rtx (SImode);

    emit_label (lab);

    emit_move_insn (data, gen_rtx_ZERO_EXTEND (SImode, m_src));
    emit_move_insn (m_dest, simplify_gen_subreg (QImode, data, SImode, 0));
    emit_cmp_and_jump_insns (data, const0_rtx, NE, NULL_RTX, SImode, true, lab);

    emit_insn (gen_addsi3 (operands[0], a_dest, constm1_rtx));
    DONE;
  })

(define_expand "nopv"
  [(parallel [(unspec_volatile [(const_int 0)]
                               UNSPECV_NOP)
              (set (match_dup 0)
                   (unspec_volatile:BLK [(match_dup 0)]
                                        UNSPECV_MEMORY_BARRIER))])]
  ""
  {
    operands[0] = gen_rtx_MEM (BLKmode, gen_rtx_SCRATCH (Pmode));
    MEM_VOLATILE_P (operands[0]) = 1;
  })

(define_insn "*nopv"
  [(unspec_volatile [(const_int 0)]
                    UNSPECV_NOP)
   (set (match_operand:BLK 0 "" "")
        (unspec_volatile:BLK [(match_dup 0)] UNSPECV_MEMORY_BARRIER))]
  ""
  "nop"
  [(set_attr "pipe" "lsp")
   (set_attr "length" "2")])

(define_expand "insert"
  [(parallel [(match_operand:SI 0 "register_operand" "")
              (match_operand:SI 1 "register_operand" "")
              (match_operand:SI 2 "nonmemory_operand" "")    ;; value
              (match_operand:SI 3 "reg_or_u5_operand" "")    ;; pos
              (match_operand:SI 4 "nonmemory_operand" "")])] ;; width
  ""
  {
    tric_emit_insert (operands);
    DONE;
  })


;; Same as  INSERT $0, -1, $1, $2  except that it works with width >= 32
(define_expand "insert_mask"
  [(set (match_dup 3)
        (const_int -1))
   ;; pos zeros at the low end
   (set (match_dup 3)
        (ashift:SI (match_dup 3)
                   (match_operand:SI 1 "reg_or_u5_operand" ""))) ;; pos
   ;; pos+width zeros at the low end
   (set (match_dup 4)
        (ashift:SI (match_dup 3)
                   (match_operand:SI 2 "register_operand" ""))) ;; width
   (set (match_dup 5)
        (lshiftrt:SI (match_dup 2)
                     (const_int 5)))
   
   (set (match_dup 4)
        (if_then_else:SI (eq (match_dup 5)
                             (const_int 0))
                         (match_dup 4)
                         (const_int 0)))
   ;; Clear right number of bits at the high end.
   ;; If width >= 32, clean nothing because $4 is 0 then.
   (set (match_operand:SI 0 "register_operand" "")
        (xor:SI (match_dup 4)
                (match_dup 3)))]
  ""
  {
    operands[3] = gen_reg_rtx (SImode);
    operands[4] = gen_reg_rtx (SImode);
    operands[5] = gen_reg_rtx (SImode);
  })

(define_insn "insert_const_width"
  [(set (match_operand:SI 0 "register_operand"              "=d")
        (unspec:SI [(match_operand:SI 1 "register_operand"   "d")
                    (match_operand:SI 2 "reg_or_u4_operand"  "dKu4") ;; value
                    (match_operand:SI 3 "reg_or_u5_operand"  "dKu5") ;; pos
                    (match_operand:SI 4 "u5_operand"         "Ku5")] ;; width
                   UNSPEC_INSERT))]
  ""
  "insert\t%0, %1, %2, %3, %4"
  [(set_attr "pipe" "ip2")])

(define_insn_and_split "insert64"
  [(set (match_operand:SI 0 "register_operand"              "=d")
        (unspec:SI [(match_operand:SI 1 "register_operand"   "d")
                    (match_operand:SI 2 "reg_or_u4_operand"  "dKu4") ;; value
                    (match_operand:DI 3 "nonmemory_operand"  "d")]   ;; width:pos
                   UNSPEC_INSERT))]
  ""
  "insert\t%0, %1, %2, %A3"
  "&& !tric_map_combine
   && !reload_completed
   && CONST_INT_P (operands[3])"
  ;; "insert_const_width"
  [(set (match_dup 0)
        (unspec:SI [(match_dup 1)
                    (match_dup 2)
                    (match_dup 4)
                    (match_dup 5)]
                   UNSPEC_INSERT))]
  {
    HOST_WIDE_INT pos   = GET_MODE_MASK (SImode) & OPVAL(3);
    HOST_WIDE_INT width = GET_MODE_MASK (SImode) & (OPVAL(3) >> 32);

    if (pos >= 32)
      FAIL;

    if (width > 32)
      width = 32;

    if (width == 32 && pos == 0)
      {
        emit_move_insn (operands[0], operands[2]);
        DONE;
      }

    operands[4] = GEN_INT (pos);
    operands[5] = GEN_INT (pos + width <= 32 ? width : 32 - pos);
  }
  [(set_attr "pipe" "ip2")])

(define_expand "ldmst"
  [(parallel [(match_operand:SI 0 "register_operand" "")
              (match_operand:DI 1 "register_operand" "")])]
  ""
  {
    operands[0] = gen_rtx_MEM (SImode, operands[0]);
    MEM_VOLATILE_P (operands[0]) = 1;
    emit_insn (gen_ldmst_insn (operands[0], operands[1]));
    DONE;
  })

(define_insn "ldmst_insn"
  [(set (match_operand:SI 0 "vok_memory_operand"                  "+Wl")
        (unspec_volatile:SI [(match_dup 0)
                             (match_operand:DI 1 "register_operand" "d")]
                            UNSPECV_LDMST))]
  ""
  "ldmst\t%0, %A1"
  [(set_attr "ticks" "4")
   (set_attr "space" "4")
   (set_attr "pipe" "std")])

; HDP-1144, HDP-1147
(define_expand "frame_blockage"
  [(set (match_dup 0)
        (unspec:BLK [(match_dup 1)]
                    UNSPEC_FRAME_BLOCKAGE))]
  ""
  {
    operands[0] = gen_rtx_MEM (BLKmode, gen_rtx_SCRATCH (Pmode));
    MEM_VOLATILE_P (operands[0]) = 1;
    operands[1] = stack_pointer_rtx;
  })

(define_insn "*frame_blockage"
  [(set (match_operand:BLK 0 "" "")
        (unspec:BLK [(match_operand:SI 1 "" "")]
                    UNSPEC_FRAME_BLOCKAGE))]
  ""
  ""
  [(set_attr "length" "0")])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(include "tricore-combine.md")
(include "tricore-map.md")
