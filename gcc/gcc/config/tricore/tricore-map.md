;;  Machine description for Infineon TriCore
;;  Copyright 2005-2014 Free Software Foundation, Inc.
;;  Contributed by HighTec EDV

;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.
;;
;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.


;; This file containts split patterns used to canonicalize RTXes cooked
;; up during insn combine.  They work basically the same way like non-matching
;; splits but have different restrictions and map patterns to patterns rather
;; than supplying a split point for a non-matching combination.  All patterns
;; are used by `tric_htc_canonicalize_combined_rtx'.  The target patterns are
;; wrapped into unspecs so that they can be distinguished from results of
;; ordinary matching or non-matching splits.  `insn' is not available
;; and set to NULL.


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 64-bit widening multiplication
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_split
  [(set (match_operand:DI 0 "register_operand" "")
        (ashift:DI (any_extend:DI (match_operand:SI 1 "register_operand" ""))
                   (match_operand:DI 2 "const_int_operand" "")))]
  "tric_map_combine"
  [(unspec
    ;; "*mulsidi3_const"
    ;; "*umulsidi3_const"
    ;; "*mulsidi3_big_const"
    ;; "*umulsidi3_big_const"
    [(set (match_dup 0)
          (mult:DI (any_extend:DI (match_dup 1))
                   (match_dup 2)))]
    UNSPEC_MAP_COMBINE)]
  {
    operands[2] = gen_int_mode (1ull << (UOPVAL(2) % 64), DImode);
  })


(define_split
  [(set (match_operand:DI 0 "register_operand" "")
        (neg:DI (sign_extend:DI (match_operand:SI 1 "register_operand" ""))))]
  "tric_map_combine"
  [(unspec
    ;; "*mulsidi3_const"
    [(set (match_dup 0)
          (mult:DI (sign_extend:DI (match_dup 1))
                   (const_int -1)))]
    UNSPEC_MAP_COMBINE)])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 64-bit multiply-add and -sub
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_split
  [(set (match_operand:DI 0 "register_operand" "")
        (minus:DI (match_operand:DI 1 "register_operand" "")
                  (match_operand:DI 2 "any_extend_operand" "")))]
  "tric_map_combine"
  [(unspec
    ;; "maddsidi4_const"
    ;; "umaddsidi4_const"
    [(set (match_dup 0)
          (plus:DI (mult:DI (match_dup 2)
                            (const_int -1))
                   (match_dup 1)))]
    UNSPEC_MAP_COMBINE)])


(define_split
  [(set (match_operand:DI 0 "register_operand" "")
        (minus:DI (match_operand:DI 1 "register_operand" "")
                  (mult:DI (match_operand:DI 2 "any_extend_operand" "")
                           (match_operand:DI 3 "const_int_operand" ""))))]
  "tric_map_combine"
  [(unspec
    ;; "maddsidi4_const"
    ;; "umaddsidi4_const"
    ;; "*maddsidi4_big_const"
    ;; "*umaddsidi4_big_const"
    [(set (match_dup 0)
          (plus:DI (mult:DI (match_dup 2)
                            (match_dup 3))
                   (match_dup 1)))]
    UNSPEC_MAP_COMBINE)]
  {
    operands[3] = simplify_unary_operation (NEG, DImode, operands[3], DImode);
  })


(define_split
  [(set (match_operand:DI 0 "register_operand" "")
        (plus:DI (ashift:DI (match_operand:DI 1 "any_extend_or_reg_operand" "")
                            (match_operand:DI 2 "u6_operand" ""))
                 (match_operand:DI 3 "register_operand" "")))]
  "tric_map_combine"
  [(unspec
    ;; "madddi4"
    ;; "*maddsidi3_const"
    ;; "*umaddsidi3_const"
    [(set (match_dup 0)
          (plus:DI (mult:DI (match_dup 1)
                            (match_dup 2))
                   (match_dup 3)))]
    UNSPEC_MAP_COMBINE)]
  {
    operands[2] = gen_int_mode (1ull << (UOPVAL(2) % 64), DImode);
  })


(define_split
  [(set (match_operand:DI 0 "register_operand" "")
        (minus:DI (match_operand:DI 1 "register_operand" "")
                  (ashift:DI (match_operand:DI 2 "any_extend_or_reg_operand" "")
                             (match_operand:DI 3 "u6_operand" ""))))]
  "tric_map_combine"
  [(unspec
    ;; "madddi4"
    ;; "*maddsidi3_const"
    ;; "*umaddsidi3_const"
    [(set (match_dup 0)
          (plus:DI (mult:DI (match_dup 2)
                            (match_dup 3))
                   (match_dup 1)))]
    UNSPEC_MAP_COMBINE)]
  {
    operands[3] = gen_int_mode (-(1ull << (UOPVAL(3) % 64)), DImode);
  })


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 32-bit shift and rotate
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (ior:SI (ashift:SI (match_operand:SI 3 "register_operand" "")
                           (match_operand:SI 4 "u5_operand" ""))
                (lshiftrt:SI (match_operand:SI 1 "register_operand" "")
                             (match_operand:SI 2 "u5_operand" ""))))]
  "tric_map_combine
   && 32 == OPVAL(2) + OPVAL(4)"
  [(unspec
    ;; Swap IOR's operands
    ;; "dextr_const"
    [(set (match_dup 0)
          (ior:SI (lshiftrt:SI (match_dup 1)
                               (match_dup 2))
                  (ashift:SI (match_dup 3)
                             (match_dup 4))))]
    UNSPEC_MAP_COMBINE)])


(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (plus:SI (mult:SI (match_operand:SI 1 "register_operand" "")
                          (match_operand:SI 2 "const_int_operand" ""))
                 (lshiftrt:SI (match_operand:SI 3 "register_operand" "")
                              (match_operand:SI 4 "u5_operand" ""))))]
  "tric_map_combine
   && exact_log2 (OPVAL(2)) >= 1
   && 32 == exact_log2 (OPVAL(2)) + OPVAL(4)"
  [(unspec
    ;; "dextr_const"
    [(set (match_dup 0)
          (ior:SI (lshiftrt:SI (match_dup 3)
                               (match_dup 4))
                  (ashift:SI (match_dup 1)
                             (match_dup 2))))]
    UNSPEC_MAP_COMBINE)]
  {
    operands[2] = GEN_INT (exact_log2 (OPVAL(2)));
  })


(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (ior:SI (ashift:SI (match_operand:SI 3 "register_operand" "")
                           (match_operand:SI 2 "register_operand" ""))
                (lshiftrt:SI (match_operand:SI 1 "register_operand" "")
                             (minus:SI (const_int 32)
                                       (match_dup 2)))))]
  "tric_map_combine"
  [(unspec
    ;; Swap IOR's operands
    ;; "dextr_reg"
    [(set (match_dup 0)
          (ior:SI (lshiftrt:SI (match_dup 1)
                               (minus:SI (const_int 32)
                                         (match_dup 2)))
                  (ashift:SI (match_dup 3)
                             (match_dup 2))))]
    UNSPEC_MAP_COMBINE)])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 32-bit multiply-add and -sub
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (plus:SI (match_operand:SI 3 "register_operand" "")
                 (mult:SI (match_operand:SI 1 "register_operand" "")
                          (match_operand:SI 2 "reg_or_s9_operand" ""))))]
  "tric_map_combine"
  [(unspec
    ;; "maddsi4"
    [(parallel [(set (match_dup 0)
                     (plus:SI (mult:SI (match_dup 1)
                                       (match_dup 2))
                              (match_dup 3)))
                (clobber (scratch:SI))])]
    UNSPEC_MAP_COMBINE)])


(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (plus:SI (plus:SI (match_operand:SI 1 "register_operand" "")
                          (match_operand:SI 2 "register_operand" ""))
                 (match_dup 2)))]
  "tric_map_combine"
  [(unspec
    ;; "maddsi4"
    [(parallel [(set (match_dup 0)
                     (plus:SI (mult:SI (match_dup 2)
                                       (match_dup 3))
                              (match_dup 1)))
                (clobber (scratch:SI))])]
    UNSPEC_MAP_COMBINE)]
  {
    operands[3] = GEN_INT (2);
  })


(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (minus:SI (match_operand:SI 1 "register_operand" "")
                  (ashift:SI (match_operand:SI 2 "register_operand" "")
                             (match_operand:SI 3 "u3_operand" ""))))]
  "tric_map_combine"
  [(unspec
    ;; "msubsi4"
    [(set (match_dup 0)
          (minus:SI (match_dup 1)
                    (mult:SI (match_dup 2)
                             (match_dup 4))))]
    UNSPEC_MAP_COMBINE)]
  {
    operands[4] = gen_int_mode (1u << OPVAL(3), SImode);
  })


(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (plus:SI (mult:SI (match_operand:SI 1 "register_operand" "")
                          (const_int 2))
                 (const_int 1)))]
  "tric_map_combine"
  [(unspec
    ;; "*ashlsi2-1.ior-1.ic"
    [(set (match_dup 0)
          (ior:SI (ashift:SI (match_dup 1)
                             (const_int 1))
                  (const_int 1)))]
    UNSPEC_MAP_COMBINE)])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; min, max as if_then_else
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;; UMIN rendering an extension superfluous.

;; Provided  0 <= $3 <= [U]INT8_16_MAX:
;; $0 = ($1 < $3) ? ([u]int8/16_t) $1 : $3   -->   $0 = min ($1, $3)
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (leu_ltu:SIVOID (match_operand:SI 1 "" "")
                                         (match_operand:SI 2 "const_int_operand" ""))
                         (any_extend:SI (subreg:QIHI (match_dup 1) 0))
                         (match_operand:SI 3 "const_int_operand" "")))]
  "tric_map_combine
   && UMIN == tric_min_code (<leu_ltu:CODE>, operands[2], operands[3])"
  [(unspec
    ;; "uminsi3"
    [(set (match_dup 0)
          (umin:SI (match_dup 1)
                   (match_dup 3)))]
    UNSPEC_MAP_COMBINE)]
  {
    HOST_WIDE_INT upper = USIval (operands[3]);
    if (upper > tric_max_for_mode (<QIHI:MODE>mode, SIGN_EXTEND == <any_extend:CODE>))
      FAIL;
  })


;; $0 = ($1 > $3) ? $1 : $3   -->   $0 = max ($1, $3)

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (greater:SIVOID (match_operand:SI 1 "" "")
                                         (match_operand:SI 2 "reg_or_const_int_operand" ""))
                         (match_dup 1)
                         (match_operand:SI 3 "reg_or_const_int_operand" "")))]
  "tric_map_combine"
  [(unspec
    ;; "umaxsi4"
    ;; "smaxsi4"
    [(set (match_dup 0)
          (match_dup 4))]
    UNSPEC_MAP_COMBINE)]
  {
    enum rtx_code max_code = tric_max_code (<greater:CODE>, operands[2], operands[3]);
    if (UNKNOWN == max_code)
      FAIL;

    operands[4] = gen_rtx_fmt_ee (max_code, SImode, operands[1], operands[3]);
  })


;; IF_THEN_ELSE comprising U/SMIN resp. U/SMAX around U/SMAX resp. U/SMIN.

;; Provided  $3 <= $4:
;; $0 = ($1 < $4) ? max ($1, $3) : $4    -->   $0 = min (max ($1, $3), $4)
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (smaller:SIVOID (match_operand:SI 1 "" "")
                                         (match_operand:SI 2 "const_int_operand" ""))
                         (maximum:SI (match_dup 1)
                                     (match_operand:SI 3 "const_int_operand" ""))
                         (match_operand:SI 4 "const_int_operand" "")))]
  "tric_map_combine
   && tric_codes_same_sign_p (<smaller:CODE>, <maximum:CODE>)"
  [(unspec
    ;; "umin_umaxsi4"
    ;; "smin_smaxsi4"
    [(set (match_dup 0)
          (match_dup 5))]
    UNSPEC_MAP_COMBINE)]
  {
    bool signed_p = tric_code_signed_p (<smaller:CODE>);
    HOST_WIDE_INT lower = SIval (operands[3], signed_p);
    HOST_WIDE_INT upper = SIval (operands[4], signed_p);
    enum rtx_code min_code = tric_min_code (<smaller:CODE>, operands[2], operands[4]);

    if (lower > upper
        || min_code == UNKNOWN)
      FAIL;

    rtx xmax = gen_rtx_fmt_ee (<maximum:CODE>, SImode, operands[1], operands[3]);
    operands[5] = gen_rtx_fmt_ee (min_code, SImode, xmax, operands[4]);
  })


;; Provided $3 >= $4:
;; $0 = ($1 > $4) ? min ($1, $3) : $4   -->   $0 = max (min ($1, $3), $4)
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (greater:SIVOID (match_operand:SI 1 "" "")
                                         (match_operand:SI 2 "const_int_operand" ""))
                         (minimum:SI (match_dup 1)
                                     (match_operand:SI 3 "const_int_operand" ""))
                         (match_operand:SI 4 "const_int_operand" "")))]
  "tric_map_combine
   && tric_codes_same_sign_p (<greater:CODE>, <minimum:CODE>)"
  [(unspec
    ;; "umax_uminsi4"
    ;; "smax_sminsi4"
    [(set (match_dup 0)
          (match_dup 5))]
    UNSPEC_MAP_COMBINE)]
  {
    bool signed_p = tric_code_signed_p (<greater:CODE>);
    HOST_WIDE_INT upper = SIval (operands[3], signed_p);
    HOST_WIDE_INT lower = SIval (operands[4], signed_p);
    enum rtx_code max_code = tric_max_code (<greater:CODE>, operands[2], operands[4]);

    if (lower > upper
        || max_code == UNKNOWN)
      FAIL;

    rtx xmin = gen_rtx_fmt_ee (<minimum:CODE>, SImode, operands[1], operands[3]);
    operands[5] = gen_rtx_fmt_ee (max_code, SImode, xmin, operands[4]);
  })


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; casted minmax
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;; UMIN rendering the extension superfluous.

;; Provided  $1 >= 0  and  0 <= $2 <= [U]INT8/16_MAX:
;; X := min ($1, $2)
;; $0 = (X > $4) ? ([u]int8/16_t) X : $4
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (greater:SIVOID (umin:SI (match_operand:SI 1 "" "")
                                                  (match_operand:SI 2 "const_int_operand" ""))
                                         (match_operand:SI 3 "const_int_operand" ""))
                         (any_extend:SI (subreg:QIHI (umin:SI (match_dup 1)
                                                              (match_dup 2)) 0))
                         (match_operand:SI 4 "const_int_operand" "")))]
  "tric_map_combine"
  [(unspec
    ;; "uminsi3"
    ;; "umin_?maxsi4"
    [(set (match_dup 0)
          (match_dup 5))]
    UNSPEC_MAP_COMBINE)]
  {
    enum rtx_code max_code = tric_max_code (<greater:CODE>, operands[3], operands[4]);
    HOST_WIDE_INT upper = USIval (operands[2]);
    HOST_WIDE_INT lower = SIval (operands[4], tric_code_signed_p (<greater:CODE>));

    if (UNKNOWN == max_code
        || lower > upper
        || upper > tric_max_for_mode (<QIHI:MODE>mode, SIGN_EXTEND == <any_extend:CODE>))
      FAIL;

    operands[5] = gen_rtx_UMIN (SImode, operands[1], operands[2]);

    // UMIN comes up with a positive result, hence the outer U/SMAX is void
    // if the MAX constant (lower) is less or equal to zero.

    if (lower > 0)
      operands[5] = gen_rtx_fmt_ee (max_code, SImode, operands[5], operands[4]);
  })


;; The sign-extension / zero-extension will never see a value outside its image,
;; hence the extension may be dropped.  Moreover, map IF_THEN_ELSE to the
;; much more convenient UMIN / UMAX / SMIN / SMAX.

;; Provided  [U]INT8/16_MIN <= $5 <= $3 <= [U]INT8/16_MAX:
;; $0 = ($1 < $3) ? max ($1, $5) : $3   -->   $0 = min (max ($1, $5), $3)
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (le_lt:SIVOID (match_operand:SI 1 "" "")
                                       (match_operand:SI 2 "const_int_operand" ""))
                         (any_extend:SI (subreg:QIHI
                                         (smax:SI (match_dup 1)
                                                  (match_operand:SI 5 "const_int_operand" "")) 0))
                         (match_operand:SI 3 "const_int_operand" "")))]
  "tric_map_combine"
  [(unspec
    ;; "smax_sminsi4"
    [(set (match_dup 0)
          (smin:SI (smax:SI (match_dup 1)
                            (match_dup 5))
                   (match_dup 3)))]
    UNSPEC_MAP_COMBINE)]
  {
    bool signed_p = SIGN_EXTEND == <any_extend:CODE>;
    enum rtx_code min_code = tric_min_code (<le_lt:CODE>, operands[2], operands[3]);
    HOST_WIDE_INT upper = SIval (operands[3], true);
    HOST_WIDE_INT lower = SIval (operands[5], true);

    if (min_code == UNKNOWN
        || lower > upper
        || upper > tric_max_for_mode (<QIHI:MODE>mode, signed_p)
        || lower < tric_min_for_mode (<QIHI:MODE>mode, signed_p))
      FAIL;

    gcc_assert (SMIN == min_code);
  })


;; Similar as above for the case "greater + min" instead of "smaller + max":

;; Provided  [U]INT8/16_MIN <= $3 <= $5 <= [U]INT8/16_MAX:
;; $0 = ($1 > $3) ? min ($1, $5) : $3   -->   $0 = max (min ($1, $5), $3)
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (ge_gt:SIVOID (match_operand:SI 1 "" "")
                                       (match_operand:SI 2 "const_int_operand" ""))
                         (any_extend:SI (subreg:QIHI
                                         (smin:SI (match_dup 1)
                                                  (match_operand:SI 5 "const_int_operand" "")) 0))
                         (match_operand:SI 3 "const_int_operand" "")))]
  "tric_map_combine"
  [(unspec
    ;; "smin_smaxsi4"
    [(set (match_dup 0)
          (smax:SI (smin:SI (match_dup 1)
                            (match_dup 5))
                   (match_dup 3)))]
    UNSPEC_MAP_COMBINE)]
  {
    bool signed_p = SIGN_EXTEND == <any_extend:CODE>;
    enum rtx_code max_code = tric_max_code (<ge_gt:CODE>, operands[2], operands[3]);
    HOST_WIDE_INT upper = SIval (operands[5], true);
    HOST_WIDE_INT lower = SIval (operands[3], true);

    if (max_code == UNKNOWN
        || lower > upper
        || upper > tric_max_for_mode (<QIHI:MODE>mode, signed_p)
        || lower < tric_min_for_mode (<QIHI:MODE>mode, signed_p))
      FAIL;

    gcc_assert (SMAX == max_code);
  })


;; We have y = min (max (x, y), y) and hence $0 = min (max ($1, $2), $3)
;; provided $3 == $1 or $3 == $2.  Ditto for min and max swapped.
;; min and max must be of the same signedness; otherwise we might have
;; effectively $3 != $1 even if the "rtx_equal_p ($3, $2)" below is true.

;; Provided $3 = $2  or  $3 = $1:
;; $0 = max (min ($1, $2), $3)   -->   $0 = $3
;; $0 = min (max ($1, $2), $3)   -->   $0 = $3
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (minmax2:SI (minmax:SI (match_operand:SI 1 "" "")
                               (match_operand:SI 2 "" ""))
                    (match_operand:SI 3 "" "")))]
  "tric_map_combine
   && <minmax:CODE> != <minmax2:CODE>
   && tric_codes_same_sign_p (<minmax:CODE>, <minmax2:CODE>)
   && (rtx_equal_p (operands[3], operands[1])
       || rtx_equal_p (operands[3], operands[2]))"
  [(unspec
    [(set (match_dup 0)
          (match_dup 3))]
    UNSPEC_MAP_COMBINE)])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; conditional moves
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; In some rare cases we can reuse the compared reg if it contains 0.

;; $0 = $1 ? $2 : 0   -->   $0 = ($1 == 0) ? $1 : $2
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (ne:SIVOID (match_operand:SI 1 "register_operand" "")
                                    (const_int 0))
                         (match_operand:SI 2 "reg_or_s9_operand" "")
                         (const_int 0)))]
  "tric_map_combine"
  [(unspec
    ;; "*movsicc.eq"
    [(set (match_dup 0)
          (if_then_else:SI (eq (match_dup 1)
                               (const_int 0))
                           (match_dup 1)
                           (match_dup 2)))]
    UNSPEC_MAP_COMBINE)])
