;; VLE add/subf  QI and HI
(define_insn "add<mode>3"
  [(set (match_operand:QHI 0 "gpc_reg_operand"       "=r")
	(plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
		  (match_operand:QHI 2 "gpc_reg_operand"     " r")))]
   "TARGET_VLE_SAT"
   "add<wd> %0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "add<mode>3_sign_extend"
  [(set (match_operand:SI 0 "gpc_reg_operand"       "=r")
	(sign_extend:SI (plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
		  (match_operand:QHI 2 "gpc_reg_operand"     " r"))))]
   "TARGET_VLE_SAT"
   "add<wd> %0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "add<mode>3_zero_extend"
  [(set (match_operand:SI 0 "gpc_reg_operand"       "=r")
	(zero_extend:SI (plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
		  (match_operand:QHI 2 "gpc_reg_operand"     " r"))))]
   "TARGET_VLE_SAT"
   "add<wd>u %0, %1, %2"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "*add<mode>3_set_compare"
  [(set (match_operand:CC 3 "cc_reg_operand" "=x")
	(compare:CC (plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
			    (match_operand:QHI 2 "gpc_reg_operand" "r"))
		    (const_int 0)))
   (set (match_operand:QHI 0 "gpc_reg_operand" "=r")
	(plus:QHI (match_dup 1)
		(match_dup 2)))]
   "TARGET_VLE_SAT"
   "add<wd>. %0, %1, %2"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "*add<mode>3_set_compare_sign_extend"
  [(set (match_operand:CC 3 "cc_reg_operand" "=x")
	(compare:CC (sign_extend:SI (plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
			    (match_operand:QHI 2 "gpc_reg_operand" "r")))
		    (const_int 0)))
   (set (match_operand:SI 0 "gpc_reg_operand" "=r")
	(sign_extend:SI (plus:QHI (match_dup 1)
		(match_dup 2))))]
   "TARGET_VLE_SAT"
   "add<wd>. %0, %1, %2"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "*add<mode>3_set_compare_zero_extend"
  [(set (match_operand:CC 3 "cc_reg_operand" "=x")
	(compare:CC (zero_extend:SI (plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
			    (match_operand:QHI 2 "gpc_reg_operand" "r")))
		    (const_int 0)))
   (set (match_operand:SI 0 "gpc_reg_operand" "=r")
	(zero_extend:SI (plus:QHI (match_dup 1)
		(match_dup 2))))]
   "TARGET_VLE_SAT"
   "add<wd>u. %0, %1, %2"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "sub<mode>3"
  [(set (match_operand:QHI 0 "gpc_reg_operand"       "=r")
	(minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
		  (match_operand:QHI 2 "gpc_reg_operand"     " r")))]
   "TARGET_VLE_SAT"
   "subf<wd> %0, %2, %1"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "sub<mode>3_sign_extend"
  [(set (match_operand:SI 0 "gpc_reg_operand"       "=r")
	(sign_extend:SI (minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
		  (match_operand:QHI 2 "gpc_reg_operand"     " r"))))]
   "TARGET_VLE_SAT"
   "subf<wd> %0, %2, %1"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "sub<mode>3_zero_extend"
  [(set (match_operand:SI 0 "gpc_reg_operand"       "=r")
	(zero_extend:SI (minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
		  (match_operand:QHI 2 "gpc_reg_operand"     " r"))))]
   "TARGET_VLE_SAT"
   "subf<wd>u %0, %2, %1"
  [(set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "*sub<mode>3_set_compare"
  [(set (match_operand:CC 3 "cc_reg_operand" "=x")
	(compare:CC (minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
			    (match_operand:QHI 2 "gpc_reg_operand" "r"))
		    (const_int 0)))
   (set (match_operand:QHI 0 "gpc_reg_operand" "=r")
	(minus:QHI (match_dup 1)
		(match_dup 2)))]
   "TARGET_VLE_SAT"
   "subf<wd>. %0, %2, %1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "*sub<mode>3_set_compare_sign_extend"
  [(set (match_operand:CC 3 "cc_reg_operand" "=x")
	(compare:CC (sign_extend:SI (minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
			    (match_operand:QHI 2 "gpc_reg_operand" "r")))
		    (const_int 0)))
   (set (match_operand:SI 0 "gpc_reg_operand" "=r")
	(sign_extend:SI (minus:QHI (match_dup 1)
		(match_dup 2))))]
   "TARGET_VLE_SAT"
   "subf<wd>. %0, %2, %1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn "*sub<mode>3_set_compare_zero_extend"
  [(set (match_operand:CC 3 "cc_reg_operand" "=x")
	(compare:CC (zero_extend:SI (minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
			    (match_operand:QHI 2 "gpc_reg_operand" "r")))
		    (const_int 0)))
   (set (match_operand:SI 0 "gpc_reg_operand" "=r")
	(zero_extend:SI (minus:QHI (match_dup 1)
		(match_dup 2))))]
   "TARGET_VLE_SAT"
   "subf<wd>u. %0, %2, %1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")
   (set_attr "cpu_facility" "vle")])

(define_insn ""
  [(set (match_operand:CC 0 "cc_reg_operand" "=x")
	(compare:CC (minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
			     (match_operand:QHI 2 "gpc_reg_operand" "r"))
		    (const_int 0)))
   (clobber (match_scratch:QHI 3 "=r"))]
  "TARGET_VLE_SAT"
  "subf<wd>. %3,%2,%1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")])

(define_insn ""
  [(set (match_operand:CC 0 "cc_reg_operand" "=x")
	(compare:CC (sign_extend:SI (minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
			     (match_operand:QHI 2 "gpc_reg_operand" "r")))
		    (const_int 0)))
   (clobber (match_scratch:QHI 3 "=r"))]
  "TARGET_VLE_SAT"
  "subf<wd>. %3,%2,%1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")])

(define_insn ""
  [(set (match_operand:CC 0 "cc_reg_operand" "=x")
	(compare:CC (zero_extend:SI (minus:QHI (match_operand:QHI 1 "gpc_reg_operand" "r")
			     (match_operand:QHI 2 "gpc_reg_operand" "r")))
		    (const_int 0)))
   (clobber (match_scratch:QHI 3 "=r"))]
  "TARGET_VLE_SAT"
  "subf<wd>u. %3,%2,%1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")])

(define_insn "*add<mode>3_internal_sat2"
  [(set (match_operand:CC 0 "cc_reg_operand" "=x")
	(compare:CC (plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
			     (match_operand:QHI 2 "gpc_reg_operand" "r"))
		    (const_int 0)))
   (clobber (match_scratch:QHI 3 "=r"))]
  "TARGET_VLE_SAT"
  "add<wd>. %3,%2,%1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")])

(define_insn "*add<mode>3_internal_sat3"
  [(set (match_operand:CC 0 "cc_reg_operand" "=x")
	(compare:CC (sign_extend: SI (plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
			     (match_operand:QHI 2 "gpc_reg_operand" "r")))
		    (const_int 0)))
   (clobber (match_scratch:SI 3 "=r"))]
  "TARGET_VLE_SAT"
  "add<wd>. %3,%2,%1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")])

(define_insn "*add<mode>3_internal_sat4"
  [(set (match_operand:CC 0 "cc_reg_operand" "=x")
	(compare:CC (zero_extend: SI (plus:QHI (match_operand:QHI 1 "gpc_reg_operand" "%r")
			     (match_operand:QHI 2 "gpc_reg_operand" "r")))
		    (const_int 0)))
   (clobber (match_scratch:SI 3 "=r"))]
  "TARGET_VLE_SAT"
  "add<wd>u. %3,%2,%1"
  [(set_attr "type" "fast_compare")
   (set_attr "length" "4")])
