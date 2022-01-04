(define_constants
  [(UNSPEC_VLE_DIVDW  1000)
  (UNSPEC_VLE_DIVDWU  1001)])

;; Saturation from unsigned to unsigned
(define_insn "vle_satu<mode>u"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"            "=r")
        (us_truncate:QHSI (match_operand:SI 1 "gpc_reg_operand"    "r")))]
  "TARGET_VLE_SAT"
  "satu<wd>u\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Saturation from unsigned to unsigned
(define_insn "vle_satu<mode>u_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"            "=r")
        (zero_extend:SI(us_truncate:QHSI (match_operand:SI 1 "gpc_reg_operand"    "r"))))]
  "TARGET_VLE_SAT"
  "satu<wd>u\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Saturation from unsigned to signed
(define_insn "vle_sats<mode>u"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"            "=r")
        (ss_truncate:QHSI (us_truncate:QHSI (match_operand:SI 1 "gpc_reg_operand"    "r"))))]
  "TARGET_VLE_SAT"
  "sats<wd>u\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Saturation from unsigned to signed with sign extension
(define_insn "vle_sats<mode>u_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"            "=r")
        (sign_extend:SI (ss_truncate:QHSI (us_truncate:QHSI (match_operand:SI 1 "gpc_reg_operand"    "r")))))]
  "TARGET_VLE_SAT"
  "sats<wd>u\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Saturation from signed to unsigned
(define_insn "vle_satu<mode>s"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"            "=r")
        (us_truncate:QHSI (smax:SI (match_operand:SI 1 "gpc_reg_operand"    "r")
                                   (const_int 0))))]
  "TARGET_VLE_SAT"
  "satu<wd>s\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Saturation from signed to unsigned with zero extension
(define_insn "vle_satu<mode>s_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"            "=r")
        (zero_extend:SI (us_truncate:QHSI (smax:SI (match_operand:SI 1 "gpc_reg_operand"    "r")
                                   (const_int 0)))))]
  "TARGET_VLE_SAT"
  "satu<wd>s\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Saturation from signed to signed
(define_insn "vle_sats<mode>s"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"            "=r")
        (ss_truncate:QHSI (match_operand:SI 1 "gpc_reg_operand"    "r")))]
  "TARGET_VLE_SAT"
  "sats<wd>s\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Saturation from signed to signed with sign extension
(define_insn "vle_sats<mode>s_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"            "=r")
        (sign_extend:SI (ss_truncate:QHSI (match_operand:SI 1 "gpc_reg_operand"    "r"))))]
  "TARGET_VLE_SAT"
  "sats<wd>s\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Absolute value saturation to signed
(define_insn "vle_abss<mode>"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"                     "=r")
        (ss_truncate:QHSI (abs:SI (match_operand:SI 1 "gpc_reg_operand"    "r"))))]
  "TARGET_VLE_SAT"
  "abss<wd>\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Absolute value saturation to signed with sign extension
(define_insn "vle_abss<mode>_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (sign_extend:SI (ss_truncate:QHSI (abs:SI (match_operand:SI 1 "gpc_reg_operand"    "r")))))]
  "TARGET_VLE_SAT"
  "abss<wd>\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Absolute value saturation to unsigned
(define_insn "vle_absu<mode>"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"                     "=r")
        (us_truncate:QHSI (abs:SI (match_operand:SI 1 "gpc_reg_operand"    "r"))))]
  "TARGET_VLE_SAT"
  "absu<wd>\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Absolute value saturation to unsigned with zero extension
(define_insn "vle_absu<mode>_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (zero_extend:SI (us_truncate:QHSI (abs:SI (match_operand:SI 1 "gpc_reg_operand"    "r")))))]
  "TARGET_VLE_SAT"
  "absu<wd>\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Signed saturated add
(define_insn "vle_add<mode>ss"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"                     "=r")
        (ss_plus:QHSI (match_operand:QHSI 1 "gpc_reg_operand"                "%r")
                      (match_operand:QHSI 2 "gpc_reg_operand"                " r")))]
  "TARGET_VLE_SAT"
  "add<wd>ss\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])


;; Signed saturated add with sign extension
(define_insn "vle_add<mode>ss_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (sign_extend:SI (ss_plus:QHSI (match_operand:QHSI 1 "gpc_reg_operand" "%r")
                                      (match_operand:QHSI 2 "gpc_reg_operand" " r"))))]
  "TARGET_VLE_SAT"
  "add<wd>ss\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Unsigned saturated add
(define_insn "vle_add<mode>us"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"                     "=r")
        (us_plus:QHSI (match_operand:QHSI 1 "gpc_reg_operand"                "%r")
                      (match_operand:QHSI 2 "gpc_reg_operand"                " r")))]
  "TARGET_VLE_SAT"
  "add<wd>us\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])


;; Unsigned saturated add with zero extension
(define_insn "vle_add<mode>us_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (zero_extend:SI (us_plus:QHSI (match_operand:QHSI 1 "gpc_reg_operand" "%r")
                                      (match_operand:QHSI 2 "gpc_reg_operand" " r"))))]
  "TARGET_VLE_SAT"
  "add<wd>us\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Signed saturated sub
(define_insn "vle_sub<mode>ss"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"                     "=r")
        (ss_minus:QHSI (match_operand:QHSI 1 "gpc_reg_operand"                "%r")
                       (match_operand:QHSI 2 "gpc_reg_operand"                " r")))]
  "TARGET_VLE_SAT"
  "subf<wd>ss\t%0, %2, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])


;; Signed saturated sub with sign extension
(define_insn "vle_sub<mode>ss_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (sign_extend:SI (ss_minus:QHSI (match_operand:QHSI 1 "gpc_reg_operand" "%r")
                                       (match_operand:QHSI 2 "gpc_reg_operand" " r"))))]
  "TARGET_VLE_SAT"
  "subf<wd>ss\t%0, %2, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])

;; Unsigned saturated sub
(define_insn "vle_sub<mode>us"
  [(set (match_operand:QHSI 0 "gpc_reg_operand"                     "=r")
        (us_minus:QHSI (match_operand:QHSI 1 "gpc_reg_operand"                "%r")
                       (match_operand:QHSI 2 "gpc_reg_operand"                " r")))]
  "TARGET_VLE_SAT"
  "subf<wd>us\t%0, %2, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])


;; Unsigned saturated sub with zero extension
(define_insn "vle_sub<mode>us_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (zero_extend:SI (us_minus:QHSI (match_operand:QHSI 1 "gpc_reg_operand" "%r")
                                       (match_operand:QHSI 2 "gpc_reg_operand" " r"))))]
  "TARGET_VLE_SAT"
  "subf<wd>us\t%0, %2, %1"
  [(set_attr "length" "4")
   (set_attr "type" "integer")
   (set_attr "cpu_facility" "vle")])


;; Signed saturated mul
(define_insn "vle_mul<mode>ss"
  [(set (match_operand:HSI 0 "gpc_reg_operand"                     "=r")
        (ss_mult:HSI (match_operand:HSI 1 "gpc_reg_operand"                "%r")
                     (match_operand:HSI 2 "gpc_reg_operand"                " r")))]
  "TARGET_VLE_SAT"
  "mul<wd>ss\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "satmul")
   (set_attr "cpu_facility" "vle")])


;; Signed saturated mul with sign extension
(define_insn "vle_mul<mode>ss_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (sign_extend:SI (ss_mult:HSI (match_operand:HSI 1 "gpc_reg_operand" "%r")
                                     (match_operand:HSI 2 "gpc_reg_operand" " r"))))]
  "TARGET_VLE_SAT"
  "mul<wd>ss\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "satmul")
   (set_attr "cpu_facility" "vle")])

;; Unsigned saturated mul
(define_insn "vle_mul<mode>us"
  [(set (match_operand:HSI 0 "gpc_reg_operand"                     "=r")
        (us_mult:HSI (match_operand:HSI 1 "gpc_reg_operand"                "%r")
                     (match_operand:HSI 2 "gpc_reg_operand"                " r")))]
  "TARGET_VLE_SAT"
  "mul<wd>us\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "satmul")
   (set_attr "cpu_facility" "vle")])


;; Unsigned saturated mul with zero extension
(define_insn "vle_mul<mode>us_ext"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (zero_extend:SI (us_mult:HSI (match_operand:HSI 1 "gpc_reg_operand" "%r")
                                     (match_operand:HSI 2 "gpc_reg_operand" " r"))))]
  "TARGET_VLE_SAT"
  "mul<wd>us\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "satmul")
   (set_attr "cpu_facility" "vle")])

;; Signed saturated div
(define_insn "vle_divw_sat"
  [(set (match_operand:SI 0 "gpc_reg_operand"                    "=r")
        (ss_div:SI (match_operand:SI 1 "gpc_reg_operand"            "%r")
                   (match_operand:SI 2 "gpc_reg_operand"            " r")))]
  "TARGET_VLE_SAT"
  "divw\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "idiv")
   (set_attr "cpu_facility" "vle")])


;; Unsigned saturated div
(define_insn "vle_divwu_sat"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (us_div:SI (match_operand:SI 1 "gpc_reg_operand"          "%r")
                   (match_operand:SI 2 "gpc_reg_operand"          " r")))]
  "TARGET_VLE_SAT"
  "divwu\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "idiv")
   (set_attr "cpu_facility" "vle")])

;; Signed saturated div extended
(define_insn "vle_divwe_sat"
  [(set (match_operand:SI 0 "gpc_reg_operand"                    "=r")
        (ss_div:SI (ashift:SI (match_operand:SI 1 "gpc_reg_operand" "%r")
                           (const_int 32))
                   (match_operand:SI 2 "gpc_reg_operand"            " r")))]
  "TARGET_VLE_SAT"
  "divwe\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "idiv")
   (set_attr "cpu_facility" "vle")])

;; Unsigned saturated div extended
(define_insn "vle_divweu_sat"
  [(set (match_operand:SI 0 "gpc_reg_operand"                     "=r")
        (us_div:SI (ashift:SI (match_operand:SI 1 "gpc_reg_operand" "%r")
                            (const_int 32))
                   (match_operand:SI 2 "gpc_reg_operand"            " r")))]
  "TARGET_VLE_SAT"
  "divweu\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "idiv")
   (set_attr "cpu_facility" "vle")])

;; Signed saturated div doubleword
(define_insn "vle_divdwo_sat2"
  [(set (match_operand:SI 0 "gpc_reg_operand"                              "+r")
        (truncate:SI (ss_div:DI (ior:DI (ashift:DI (match_dup 0) (const_int 32))
                                     (match_operand:SI 1 "gpc_reg_operand" "%r"))
                                (match_operand:SI 2 "gpc_reg_operand"         " r"))))]
  "TARGET_VLE_SAT"
  "divdwo\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "idiv")
   (set_attr "cpu_facility" "vle")])

;; Signed saturated div doubleword
(define_insn "vle_divdwo_sat"
  [(set (match_operand:SI 0 "gpc_reg_operand"                              "=r")
        (unspec:SI[(match_operand:SI 1 "gpc_reg_operand"                   "0 ")
                   (match_operand:SI 2 "gpc_reg_operand"                   "r ")
                   (match_operand:SI 3 "gpc_reg_operand"                   "r ")]UNSPEC_VLE_DIVDW))]
  "TARGET_VLE_SAT"
  "divdwo\t%0, %2, %3"
  [(set_attr "length" "4")
   (set_attr "type" "idiv")
   (set_attr "cpu_facility" "vle")])

;; Unsigned saturated div doubleword
(define_insn "vle_divdwuo_sat2"
  [(set (match_operand:SI 0 "gpc_reg_operand"                              "+r")
        (truncate:SI (us_div:DI (ior:DI (ashift:DI (match_dup 0) (const_int 32))
                                      (match_operand:SI 1 "gpc_reg_operand" "%r"))
                                (match_operand:SI 2 "gpc_reg_operand"         " r"))))]
  "TARGET_VLE_SAT"
  "divdwuo\t%0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "type" "idiv")
   (set_attr "cpu_facility" "vle")])

;; Unsigned saturated div doubleword
(define_insn "vle_divdwuo_sat"
  [(set (match_operand:SI 0 "gpc_reg_operand"                              "=r")
        (unspec:SI[(match_operand:SI 1 "gpc_reg_operand"                   "0 ")
                   (match_operand:SI 2 "gpc_reg_operand"                   "r ")
                   (match_operand:SI 3 "gpc_reg_operand"                   "r ")]UNSPEC_VLE_DIVDWU))]
  "TARGET_VLE_SAT"
  "divdwuo\t%0, %2, %3"
  [(set_attr "length" "4")
   (set_attr "type" "idiv")
   (set_attr "cpu_facility" "vle")])



;; min max instructions mapped to saturation

(define_insn "usathi_ic"
  [(set (match_operand:SI 0 "gpc_reg_operand"            "=r")
        (umin:SI (match_operand:SI 1 "gpc_reg_operand"    "r")
                 (const_int 65535)))]
  "TARGET_VLE_SAT"
  "satuhu\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "*usathi_ic_1"
  [(set (match_operand:SI 0 "gpc_reg_operand"            "=r")
        (zero_extend:SI (subreg:HI (umin:SI (match_operand:SI 1 "gpc_reg_operand"    "r")
                 (const_int 65535)) 2)))]
  "TARGET_VLE_SAT"
  "satuhu\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "usatqi_ic"
  [(set (match_operand:SI 0 "gpc_reg_operand"            "=r")
        (umin:SI (match_operand:SI 1 "gpc_reg_operand"    "r")
                 (const_int 255)))]
  "TARGET_VLE_SAT"
  "satubu\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "*usatqi_ic_1"
  [(set (match_operand:SI 0 "gpc_reg_operand"            "=r")
        (zero_extend:SI (subreg:QI (umin:SI (match_operand:SI 1 "gpc_reg_operand"    "r")
                 (const_int 255)) 3)))]
  "TARGET_VLE_SAT"
  "satubu\t%0, %1"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])


(define_expand "uminsi3"
  [(set (match_operand:SI 0 "gpc_reg_operand" "")
        (umin:SI (match_operand:SI 1 "gpc_reg_operand" "")
		  (match_operand:SI 2 "reg_or_u_short_operand" "")))]
  "TARGET_VLE_SAT"
  {
    if (INTVAL (operands[2]) == 65535)
     {
       emit_insn( gen_usathi_ic (operands[0], operands[1]));
       DONE;
     }
    else
    if (INTVAL (operands[2]) == 255)
     {
       emit_insn( gen_usatqi_ic (operands[0], operands[1]));
       DONE;
     }
     else
     {
     operands[2] = force_reg(SImode, operands[2]);
     emit_insn( gen_uminsi3_isel (operands[0], operands[1], operands[2]));
     DONE;
     }
  })

(define_insn "uminsi3_isel"
   [(set (match_operand:SI 0 "gpc_reg_operand" "=r")
         (umin:SI (match_operand:SI 1 "gpc_reg_operand" "b")
 		  (match_operand:SI 2 "gpc_reg_operand" "r")))
    (clobber (match_scratch:CC 3 "=x"))]
   "TARGET_VLE_SAT"
   "cmplw cr0,%1,%2\;isellt %0,%1,%2"
   [(set_attr "length" "8")
    (set_attr "type" "two")
    (set_attr "cpu_facility" "vle")])

(define_code_iterator SAT [smin smax])
(define_code_iterator SATrev [smin smax])
(define_code_attr SATlo [(smin "1") (smax "2")])
(define_code_attr SAThi [(smin "2") (smax "1")])

(define_insn "*satsi_<SAT:code>"
  [(set (match_operand:SI 0 "gpc_reg_operand" "=r")
        (SAT:SI (SATrev:SI (match_operand:SI 3 "gpc_reg_operand" "r")
                           (match_operand:SI 1 "const_int_operand" "i"))
                (match_operand:SI 2 "const_int_operand" "i")))]
  "TARGET_VLE_SAT && <SAT:CODE> != <SATrev:CODE>
   && ppcvle_sat_operator_match (operands[<SAT:SATlo>], operands[<SAT:SAThi>], NULL, NULL)"
{
  int  hword;
  bool signed_sat;
  if (!ppcvle_sat_operator_match (operands[<SAT:SATlo>], operands[<SAT:SAThi>],
                               &hword, &signed_sat))
    gcc_unreachable ();

  if (signed_sat)
      return (hword)? "satshs\t%0, %3" : "satsbs\t%0, %3";
  else
      return (hword)? "satuhu\t%0, %3" : "satubu\t%0, %3";
}
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")
   (set_attr "type" "integer")]
)

(define_insn "*sathi_<SAT:code>"
  [(set (match_operand:SI 0 "gpc_reg_operand" "=r")
        (sign_extend:SI (subreg:HI (SAT:SI (SATrev:SI (match_operand:SI 3 "gpc_reg_operand" "r")
                           (match_operand:SI 1 "const_int_operand" "i"))
                (match_operand:SI 2 "const_int_operand" "i")) 2)))]
  "TARGET_VLE_SAT && <SAT:CODE> != <SATrev:CODE>
   && ppcvle_sat_operator_match (operands[<SAT:SATlo>], operands[<SAT:SAThi>], NULL, NULL)"
{
  int  hword;
  bool signed_sat;
  if (!ppcvle_sat_operator_match (operands[<SAT:SATlo>], operands[<SAT:SAThi>],
                               &hword, &signed_sat))
    gcc_unreachable ();

  if (signed_sat)
      return (hword)? "satshs\t%0, %3" : "satsbs\t%0, %3";
  else
      return (hword)? "satuhu\t%0, %3" : "satubu\t%0, %3";
}
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")
   (set_attr "type" "integer")]
)

(define_insn "*satqi_<SAT:code>"
  [(set (match_operand:SI 0 "gpc_reg_operand" "=r")
        (sign_extend:SI (subreg:QI (SAT:SI (SATrev:SI (match_operand:SI 3 "gpc_reg_operand" "r")
                           (match_operand:SI 1 "const_int_operand" "i"))
                (match_operand:SI 2 "const_int_operand" "i")) 3)))]
  "TARGET_VLE_SAT && <SAT:CODE> != <SATrev:CODE>
   && ppcvle_sat_operator_match (operands[<SAT:SATlo>], operands[<SAT:SAThi>], NULL, NULL)"
{
  int  hword;
  bool signed_sat;
  if (!ppcvle_sat_operator_match (operands[<SAT:SATlo>], operands[<SAT:SAThi>],
                               &hword, &signed_sat))
    gcc_unreachable ();

  if (signed_sat)
      return (hword)? "satsbs\t%0, %3" : "satsbs\t%0, %3";
  else
      return (hword)? "satubu\t%0, %3" : "satubu\t%0, %3";
}
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")
   (set_attr "type" "integer")]
)

(define_expand "sminsi3"
  [(parallel [(set (match_operand:SI 0 "gpc_reg_operand" "")
        (smin:SI (match_operand:SI 1 "gpc_reg_operand" "")
 		  (match_operand:SI 2 "gpc_reg_operand" "")))
    (clobber (scratch:CC))] )]
  "TARGET_VLE_SAT"
  ""
  )
(define_expand "smaxsi3"
  [(parallel [(set (match_operand:SI 0 "gpc_reg_operand" "")
        (smax:SI (match_operand:SI 1 "gpc_reg_operand" "")
 		  (match_operand:SI 2 "gpc_reg_operand" "")))
    (clobber (scratch:CC))] )]

  "TARGET_VLE_SAT"
  ""
  )
(define_insn "sminsi3_2"
   [(set (match_operand:SI 0 "gpc_reg_operand" "=r")
         (smin:SI (match_operand:SI 1 "gpc_reg_operand" "b")
 		  (match_operand:SI 2 "gpc_reg_operand" "r")))
    (clobber (match_scratch:CC 3 "=x"))]
   "TARGET_VLE_SAT"
   "cmpw cr0,%1,%2\;isellt %0,%1,%2"
   [(set_attr "length" "8")
    (set_attr "type" "two")
    (set_attr "cpu_facility" "vle")])

(define_insn "smaxsi3_2"
   [(set (match_operand:SI 0 "gpc_reg_operand" "=r")
         (smax:SI (match_operand:SI 1 "gpc_reg_operand" "b")
 		  (match_operand:SI 2 "gpc_reg_operand" "r")))
    (clobber (match_scratch:CC 3 "=x"))]
   "TARGET_VLE_SAT"
   "cmpw cr0,%1,%2\;iselgt %0,%1,%2"
   [(set_attr "length" "8")
    (set_attr "type" "two")
    (set_attr "cpu_facility" "vle")])

