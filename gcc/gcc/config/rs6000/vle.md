;; Machine description for PowerPC special vle instructions.
;; Copyright (C) 2014 Free Software Foundation, Inc.
;; Contributed by Horst Lehser

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

(define_insn "setmem_internal"
  [(set (mem:BLK (match_operand:SI 0 "register_operand"                 "b")) ;; dest
		   (const_int 0))
   (use (match_operand:SI 1 "register_operand"                         "+wr")) ;; loop counter
   (use (match_operand:SI 2 "register_operand"                    "r")) ;; set value const0_rtx
   (set (match_operand:SI 3 "register_operand"                        "=0")
	(plus:SI (match_dup 0) (match_operand:SI 4 "immediate_operand"     "n"))) ;; incr of dest
   (set (match_dup 1) (const_int 0))
   (use (match_operand:SI 5 "immediate_operand"                         "n")) ;; align
   (clobber (reg:CC CR0_REGNO))
   ]
  "TARGET_VLE"
  {
    output_asm_insn (ASM_COMMENT_START "#bytes=%4 align=%5", operands);

    switch (INTVAL(operands[5]))
      {
      case 1:
        output_asm_insn ("stb %2, 0(%0)", operands);
        output_asm_insn ("0:\n\tse_subi. %1, 1\n\tstbu %2, 1(%0)\n\tbne 0b;",
                            operands);
        break;
      case 2:
        output_asm_insn ("sth %2, 0(%0)", operands);
        output_asm_insn ("0:\n\tse_subi. %1, 1\n\tsthu %2, 2(%0)\n\tbne 0b;",
                            operands);
        break;
      default:
        output_asm_insn ("stw %2, 0(%0)", operands);
        output_asm_insn ("0:\n\tse_subi. %1, 1\n\tstwu %2, 4(%0)\n\tbne 0b;",
                            operands);
        break;
      }

    return "";
  }
  [(set_attr "type" "store_ux")
   (set_attr "length" "20")])

(define_insn "movmem_internal"
  [(set (mem:BLK (match_operand:SI 0 "register_operand"                 "b")) ;; dest
        (mem:BLK (match_operand:SI 1 "register_operand"    "b")) ;; src
                     )
   (use (match_operand:SI 2 "register_operand"                         "+wr")) ;; loop counter
   (clobber (match_operand:SI 3 "register_operand"                    "=&r")) ;; clobber register
   (set (match_operand:SI 4 "register_operand"                        "=0")
	(plus:SI (match_dup 0) (match_operand:SI 6 "immediate_operand"    "n"))) ;; incr of dest
   (set (match_operand:SI 5 "register_operand"                        "=1")
	(plus:SI (match_dup 1) (match_dup 6)))
   (set (match_dup 2) (const_int 0))
   (use (match_operand:SI 7 "immediate_operand"                        "n")) ;; align
   (clobber (reg:CC CR0_REGNO))
   ]
  "TARGET_VLE"
  {
    output_asm_insn (ASM_COMMENT_START "#bytes=%6 align=%7", operands);

    switch (INTVAL(operands[7]))
      {
      case 1:
        output_asm_insn ("lbz %3, 0(%1)\n\tstb %3, 0(%0)", operands);
        output_asm_insn ("0:\n\tlbzu %3, 1(%1)\n\tse_subi. %2, 1\n\tstbu %3, 1(%0)\n\tbne 0b;",
                            operands);
        break;
      case 2:
        output_asm_insn ("lhz %3, 0(%1)\n\tsth %3, 0(%0)", operands);
        output_asm_insn ("0:\n\tlhzu %3, 2(%1)\n\tse_subi. %2, 1\n\tsthu %3, 2(%0)\n\tbne 0b;",
                            operands);
        break;
      default:
        output_asm_insn ("lwz %3, 0(%1)\n\tstw %3, 0(%0)", operands);
        output_asm_insn ("0:\n\tlwzu %3, 4(%1)\n\tse_subi. %2, 1\n\tstwu %3, 4(%0)\n\tbne 0b;",
                            operands);
        break;
      }

    return "";
  }
  [(set_attr "type" "store_ux")
   (set_attr "length" "20")])

