(define_mode_iterator QIHI [(QI "") (HI "")])

(define_mode_iterator QIHISI [(QI "") (HI "") (SI "")])

(define_mode_iterator QIHISIDI [(QI "") (HI "") (SI "") (DI "")])

(define_mode_iterator QISI [(QI "") (SI "")])

(define_mode_iterator SIDI    [(SI "") (DI "")])

(define_mode_iterator DIDF    [(DI "") (DF "")])
(define_mode_iterator DIPDI   [(DI "") (PDI "")])
(define_mode_iterator DIPDIDF [(DI "") (DF "") (PDI "")])

(define_mode_attr load_suffix
  [(QI "bu")
   (HI "h")
   (SI "w")
   (DI "d")])

(define_mode_attr store_suffix
  [(QI "b")
   (HI "h")
   (SI "w")
   (DI "d")])

;; We use several incarnations of this iterator
;; so that we can build cross products
(define_mode_iterator SISF   [(SI "") (SF "")])
(define_mode_iterator SISF_B [(SI "") (SF "")])

(define_mode_iterator SIVOID [SI VOID])

(define_code_iterator tric_bitop
  [and ior xor])

(define_code_iterator tric_bitop_andor
  [and ior])

(define_code_iterator tric_divmod
  [div mod])

(define_code_iterator tric_udivmod
  [udiv umod])

(define_code_iterator minmax
  [umin umax smin smax])

(define_code_iterator minmax2
  [umin umax smin smax])

; Bit operations inverting the second operand
(define_code_attr tric_bitop_not_op 
  [(and "andn")
   (ior "orn")
   (xor "xnor")])

; Bit operations inverting the result
(define_code_attr tric_not_bitop_op 
  [(and "nand")
   (ior "nor")
   (xor "xnor")])

; Bit operation inverting both arguments
(define_code_attr tric_demorgan_bitop_op 
  [(and "nor")
   (ior "nand")
   (xor "xor")])

(define_mode_attr tric_mode_suffix [(QI "b") (HI "h")])

(define_mode_attr tric_mode_bitsize [(QI "8") (HI "16")])

;; shifting 0s in
(define_code_iterator tric_lshift
  [ashift lshiftrt])

; Just a right shift where the sign is of no interest
; Used in bitfield extractions and insertions
(define_code_iterator tric_shiftrt
  [ashiftrt lshiftrt])

(define_code_iterator tric_shiftrt2
  [ashiftrt lshiftrt])

(define_code_iterator any_extend
  [zero_extend sign_extend])

(define_code_iterator eq_ne
  [eq ne])

(define_code_iterator tric_signed_compare
  [ge lt])

(define_code_iterator tric_unsigned_compare
  [geu ltu])

; Signed comparisons that allow the second operand to be
; a reg_or_s9_operand.
; We introduce this twice to allow cross products.
(define_code_iterator tric_scmp
  [eq ne lt ge])
(define_code_iterator tric_scmp2
  [eq ne lt ge])

; Dito for unsigned comparisons, i.e. the second operand is
; a reg_or_u9_operand
(define_code_iterator tric_ucmp
  [ltu geu])
(define_code_iterator tric_ucmp2
  [ltu geu])

(define_code_iterator gt_le
  [gt le])

(define_code_iterator gt_le_ge_lt
  [gt le ge lt])

(define_code_iterator ge_geu_lt_ltu
  [ge geu lt ltu])

(define_code_iterator leu_ltu
  [leu ltu])

(define_code_iterator le_lt
  [le lt])

(define_code_iterator ge_gt
  [ge gt])

(define_code_iterator greater
  [gt gtu ge geu])

(define_code_iterator smaller
  [lt ltu le leu])

(define_code_iterator minimum
  [umin smin])

(define_code_iterator maximum
  [umax smax])

; Standard vanilla opcodes
; Note that for unsigned or address comparisons
; the .u resp. .a will be added in the output template.
(define_code_attr tric_op
  [(and   "and")
   (eq    "eq")
   (ge    "ge")
   (geu   "ge")
   (gt    "gt")
   (ior   "or")
   (le    "le")
   (lt    "lt")
   (ltu   "lt")
   (minus "sub")
   (ne    "ne")
   (plus  "add")
   (xor "xor")])

;; Used in sel[n], cmov[n], cadd[n], csub[n], *beq, *bne
(define_code_attr tric_n_op
  [(eq  "")
   (ne  "n")])

(define_code_attr tric_n_not_op
  [(eq  "n")
   (ne  "")])

(define_code_attr signed_suffix
  [(sign_extend "")
   (zero_extend ".u")])

(define_code_attr signed_prefix
  [(sign_extend "")
   (zero_extend "u")])

(define_code_attr signed_prefix_su
  [(sign_extend "s")
   (zero_extend "u")])
