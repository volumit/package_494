;; Constraint definitions for Infineon TriCore micro controllers.
;; Copyright (C) 2006-2014 Free Software Foundation, Inc.
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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Register Constraints
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_register_constraint "a" "REGCLASS_A"
  "Address registers (a0--a15)")

(define_register_constraint "d" "REGCLASS_D"
  "Data registers (d0--d15)")

(define_register_constraint "D" "REGCLASS_D15"
  "Data register d15 (implicit GPR)")

(define_register_constraint "A" "REGCLASS_A15"
  "Address register a15 (implicit GPR)")

(define_register_constraint "RSP" "REGCLASS_A10"
  "Address register a10 (SP)")

(define_register_constraint "RnS" "REGCLASS_NA10"
  "Address register except a10 (SP)")
(define_register_constraint "RnA" "REGCLASS_NA15"
  "Address registers except a15")
(define_register_constraint "RnD" "REGCLASS_ND15"
  "Data registers except d15")


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Memory Constraints
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_memory_constraint "Wa"
  "@internal
  ..."
  (and (match_code "mem")
       (match_test "extra_constraint_Wa (op)")))

(define_memory_constraint "Wc"
  "@internal
  ..."
  (and (match_code "mem")
       (match_test "extra_constraint_Wc (op)")))

(define_memory_constraint "Wl"
  "@internal
  ..."
  (and (match_code "mem")
       (match_test "extra_constraint_Wl (op)")))

(define_memory_constraint "Wm"
  "@internal
  ..."
  (and (match_code "mem")
       (match_test "extra_constraint_Wm (op)")))

(define_memory_constraint "Wz"
  "@internal
  ..."
  (and (match_code "mem")
       (match_test "tric_absolute_address_p (XEXP (op, 0))")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Compile Time Constant Constraints
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; We use the following naming conventions:
;; <n> stands for a positive digit. Letters mean:
;; a=10, b=11, ... f=15, g=16, ... k=20, ...
;;    Ku<n>  is an unsigned, non-negative integer that can be 
;;           represented using <n> bits: 0 <= K < 2**<n>.
;;    Ks<n>  is a signed integer that can be represented 
;;           using <n> bits: - 2**(<n>-1) <= K < 2**(<n>-1).
;;    Ks<n>n both <n> and -<n> are in Ks<n>: - 2**(<n>-1) < K < 2**(<n>-1).
;;    Kn<n>  is -Ku<n> (negative unsigned).
;;    Kc<n>  is ~Ku<n> (complement).
;;    Km<n>  is -Ks<n> (negative signed)
;;    Ko<n>  is the number with the <n> LSBs all 1. The remaining MSBs are 0.
;;    Kh<n>  is a number which has 1-bits only in the <n> MSBs.
;;           The remaining 32-<n> LSBs are all 0.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Single Numbers

(define_constraint "K00"
  "Integer constant 0."
  (and (match_code "const_int")
       (match_test "ival == 0")))

(define_constraint "K01"
  "Integer constant 1."
  (and (match_code "const_int")
       (match_test "ival == 1")))

(define_constraint "K02"
  "Integer constant 2."
  (and (match_code "const_int")
       (match_test "ival == 2")))

(define_constraint "K03"
  "Integer constant 3."
  (and (match_code "const_int")
       (match_test "ival == 3")))

(define_constraint "K04"
  "Integer constant 4."
  (and (match_code "const_int")
       (match_test "ival == 4")))

(define_constraint "K05"
  "Integer constant 5."
  (and (match_code "const_int")
       (match_test "ival == 5")))

(define_constraint "K06"
  "Integer constant 6."
  (and (match_code "const_int")
       (match_test "ival == 6")))

(define_constraint "K08"
  "Integer constant 8."
  (and (match_code "const_int")
       (match_test "ival == 8")))

(define_constraint "K10"
  "Integer constant 10."
  (and (match_code "const_int")
       (match_test "ival == 10")))

(define_constraint "K12"
  "Integer constant 12."
  (and (match_code "const_int")
       (match_test "ival == 12")))

(define_constraint "K16"
  "Integer constant 16."
  (and (match_code "const_int")
       (match_test "ival == 16")))

(define_constraint "K32"
  "Integer constant 32."
  (and (match_code "const_int")
       (match_test "ival == 32")))

(define_constraint "Km1"
  "Integer constant @minus{}1."
  (and (match_code "const_int")
       (match_test "ival == -1")))

(define_constraint "Ko8"
  "Integer constant 0xff."
  (and (match_code "const_int")
       (match_test "ival == 0xff")))

(define_constraint "Kog"
  "Integer constant 0xffff."
  (and (match_code "const_int")
       (match_test "ival == 0xffff")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Intervals

(define_constraint "Ku3"
  "Integer unsigned 3-bit constant in the range 0 @dots{} 7."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<3)")))

(define_constraint "Ks4"
  "Integer signed 4-bit constant in the range @minus{}8 @dots{} 7."
  (and (match_code "const_int")
       (match_test "ival >= -(1<<3) && ival < (1<<3)")))

(define_constraint "Ku4"
  "Integer unsigned 4-bit constant in the range 0 @dots{} 15."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<4)")))

(define_constraint "Ku5"
  "Integer unsigned 5-bit constant in the range 0 @dots{} 31."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<5)")))

(define_constraint "Kn5"
  "Integer negative of an unsigned 5-bit constant in the range @minus{} @dots{} 0."
  (and (match_code "const_int")
       (match_test "ival <= 0 && ival > -(1<<5)")))

(define_constraint "Ku8"
  "Integer unsigned 8-bit constant in the range 0 @dots{} 255."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<8)")))

(define_constraint "Kn8"
  "Integer negative of an unsigned 8-bit constant in the range @minus{}255 @dots{} 0."
  (and (match_code "const_int")
       (match_test "ival <= 0 && ival > -(1<<8)")))

(define_constraint "Ku9"
  "Integer unsigned 9-bit constant in the range 0 @dots{} 511."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<9)")))

(define_constraint "Km9"
  "Integer signed 9-bit constant in the range @minus{}255 @dots{} 256."
  (and (match_code "const_int")
       (match_test "-ival >= -(1<<8) && -ival < (1<<8)")))

(define_constraint "Ks9"
  "Integer signed 9-bit constant in the range @minus{}256 @dots{} 255."
  (and (match_code "const_int")
       (match_test "ival >= -(1<<8) && ival < (1<<8)")))

(define_constraint "Kc9"
  "Integer complement of an unsigned 9-bit constant in the range ~0 @dots{} ~511."
  (and (match_code "const_int")
       (match_test "(~ival) >= 0  && (~ival) < (1 << 9)")))

(define_constraint "Kn9"
  "Integer negative of an unsigned 9-bit constant in the range @minus{}511 @dots{} 0."
  (and (match_code "const_int")
       (match_test "ival <= 0 && ival > -(1<<9)")))

(define_constraint "Kua"
  "Integer unsigned 10-bit constant in the range 0 @dots{} 1023."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<10)")))

(define_constraint "Ksa"
  "Integer signed 10-bit constant in the range @minus{}512 @dots{} 511."
  (and (match_code "const_int")
       (match_test "ival >= -(1<<9) && ival < (1<<9)")))

(define_constraint "Kna"
  "Integer negative of an unsigned 10-bit constant in the range @minus{}1023 @dots{} 0."
  (and (match_code "const_int")
       (match_test "ival <= 0 && ival > -(1<<10)")))

(define_constraint "Ksb"
  "Integer signed 11-bit constant in the range @minus{}1024 @dots{} 1023."
  (and (match_code "const_int")
       (match_test "ival >= -(1<<10) && ival < (1<<10)")))

(define_constraint "Kcb"
  "Integer complement of an unsigned 11-bit constant in the range ~0 @dots{} ~2047."
  (and (match_code "const_int")
       (match_test "(~ival) >= 0  && (~ival) < (1 << 11)")))

(define_constraint "Kub"
  "Integer unsigned 11-bit constant in the range 0 @dots{} 2047."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<11)")))

(define_constraint "Knb"
  "Integer negative of an unsigned 11-bit constant in the range @minus{}2047 @dots{} 0."
  (and (match_code "const_int")
       (match_test "ival <= 0 && ival > -(1<<11)")))

(define_constraint "KSa"
  "Integer constant suitable for LEA instruction."
  (and (match_code "const_int")
       (match_test "(ival & 0xf0003fff) == (ival & 0xffffffff)")))

(define_constraint "Kue"
  "Integer unsigned 14-bit constant in the range 0 @dots{} 16383."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<14)")))

(define_constraint "Kcf"
  "Integer complement of an unsigned 15-bit constant in the range ~0 @dots{} ~32767."
  (and (match_code "const_int")
       (match_test "(~ival) >= 0  && (~ival) < (1 << 15)")))

(define_constraint "Kuf"
  "Integer unsigned 15-bit constant in the range 0 @dots{} 32767."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<15)")))

(define_constraint "Kug"
  "Integer unsigned 16-bit constant in the range 0 @dots{} 65535."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < (1<<16)")))

(define_constraint "Ksg"
  "Integer signed 16-bit constant in the range @minus{}32768 @dots{} 32767."
  (and (match_code "const_int")
       (match_test "ival >= -(1<<15) && ival < (1<<15)")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Immediate (no CONST_INT)
(define_constraint "Sa"
  "An absolute address."
  (match_test "tric_absolute_address_p (op)"))

(define_constraint "Ss"
  "A small16 address."
  (match_test "tric_small16_address_p (op)"))

(define_constraint "Sc"
  "An absolute code address."
  (match_test "tric_absolute_code_address_p (op)"))

(define_constraint "Sh"
  "A high operand."
  (match_code "high"))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Masks

(define_constraint "Khg"
  "Integer whose 1-bits are all in the 16 MSB, i.e. is 0 mod 65536."
  (and (match_code "const_int")
       (match_test "0 == (ival & 0xffff)")))

(define_constraint "Khi"
  "Integer whose 1-bits are all in the 18 MSB, i.e. is 0 mod 16384."
  (and (match_code "const_int")
       (match_test "0 == (ival & 0x3fff)")))

(define_constraint "Kp2"
  "Integer that is an integer power of 2."
  (and (match_code "const_int")
       (match_test "ival
                    && !(GET_MODE_MASK (SImode) & ival & (ival-1))")))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Special, rarely used

(define_constraint "Kx1"
  "Integer != 1."
  (and (match_code "const_int")
       (match_test "ival != 1")))

(define_constraint "Kx8"
  "Integer != 255."
  (and (match_code "const_int")
       (match_test "ival != 255")))

;; used in ADDSC.A
(define_constraint "Kxa"
  "Integer in {1, 2, 4, 8}"
  (and (match_code "const_int")
       (match_test "ival==1 || ival==2 || ival==4 || ival==8")))

(define_constraint "Kxb"
  "Signed 9-bit integer without Kxa ({1, 2, 4, 8})"
  (and (match_code "const_int")
       (match_test "ival >= -(1<<8) && ival < (1<<8)")
       (match_test "ival!=1 && ival!=2 && ival!=4 && ival!=8")))

;; For use with 64-bit additions
(define_constraint "K99"
  "Constant integer whose lower and upper 32 bits can be represented as 9-bit signed integers"
  (and (match_code "const_int")
       (match_test "extra_constraint_Ga9 (GEN_INT (ival))")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CONST_DOUBLE

(define_constraint "Gu4"
  "Analogon of Ku4 for const_double."
  (and (match_code "const_double")
       (match_test "extra_constraint_Gu4 (op)")))

(define_constraint "Ghg"
  "Analogon of Khg for const_double."
  (and (match_code "const_double")
       (match_test "extra_constraint_Ghg (op)")))

(define_constraint "Gsg"
  "Analogon of Ksg for const_double."
  (and (match_code "const_double")
       (match_test "extra_constraint_Gsg (op)")))

(define_constraint "GSa"
  "Analogon of KSa for const_double."
  (and (match_code "const_double")
       (match_test "extra_constraint_GSa (op)")))

(define_constraint "G00"
  "@internal
  ..."
  (and (match_code "const_double")
       (match_test "op == CONST0_RTX (mode)")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Experimental
