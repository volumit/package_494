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

;; TriCore has three pipelines: 
;; o   one for integer operations
;; o   one for load/store
;; o   one for loops

;; dual - any
;; ip   - IP any (1 cycle) (IP1)
;; ip2  - IP2              (IP2)
;; ipdi - dvinit           (IP2)
;; ipds - dvstep           (IP3)
;; ip3  - IP3              (IP3)
;; ipm  - IP_M             (IP_M)
;; lsp  - LS any          
;; lp   - loop             (LP)
;; jip  - IP jump          (IP_JUMP)
;; jls  - LS jump          (LS_J1)     
;; ctx  - context ops      (LS_OTHER/LS_J2)
;; lda  - LS load A        (LD)
;; ldd  - LS load D        (LD)
;; sta  - LS store A       (ST)
;; std  - LS store D       (ST)
;; aalu - LS alu           (LS_A)
;; mixdd - LS mixdd        (LS_DD)
;; mixds - LS mixds        (LS_DS)
;; fp   - float (1 cycle)  (IP1)
;; fp2  - float (2 cycles) (IP2)
;; fp3  - float (3 cycles) (IP3)
;; fpdiv - float div       (IP3)

(define_attr "pipe" "ip,ip2,ipdi,ipds,ip3,ipm,lsp,lp,dual,ctx,jip,aalu,jls,lda,ldd,sta,std,mixdd,mixds,fp,fp2,fp3,fpdiv" 
  (const_string "ip"))

(define_attr "pipevariant" "standard,tc16x"
  (const (symbol_ref "TRIC_PIPEVARIANT_VALUE")))

(define_automaton "tricore_pipeline_standard")
;; (automata_option "no-minimization")

(automata_option "v")

;; entry to 3 pipelines
(define_cpu_unit "tc_ip_dec,tc_ls_dec,tc_lp_dec" "tricore_pipeline_standard")
(define_cpu_unit "tc_ctx, tc_dmbi, tc_jump"      "tricore_pipeline_standard")


;; Integer (IP) instructions
(define_insn_reservation "r_ip1" 1
  (and (eq_attr "pipevariant" "standard") (ior (eq_attr "pipe" "ip") (eq_attr "pipe" "fp")))
  "tc_ip_dec")
(define_insn_reservation "r_jip" 1
  (and (eq_attr "pipevariant" "standard") (eq_attr "pipe" "jip"))
  "tc_ip_dec+tc_jump")
(define_insn_reservation "r_ip2" 2
  (and (eq_attr "pipevariant" "standard") (ior (eq_attr "pipe" "ip2") (eq_attr "pipe" "fp2")))
  "tc_ip_dec")
(define_insn_reservation "r_mul" 3
  (and (eq_attr "pipevariant" "standard") (eq_attr "pipe" "ipm"))
  "tc_ip_dec")
(define_insn_reservation "r_ip3" 3
  (and (eq_attr "pipevariant" "standard") (ior (eq_attr "pipe" "ip3") (ior (eq_attr "pipe" "fp3") (eq_attr "pipe" "fpdiv"))))
  "tc_ip_dec")

;; DVINIT
(define_insn_reservation "r_dvi" 3
  (and (eq_attr "pipe" "ipdi") (eq_attr "pipevariant" "standard"))
  "tc_ip_dec")

;; DVSTEP
(define_insn_reservation "r_dvs" 3
  (and (eq_attr "pipe" "ipds") (eq_attr "pipevariant" "standard"))
  "tc_ip_dec")

;; Load-Store (LS) instructions
(define_insn_reservation "r_ls"  3
  (and (eq_attr "pipe" "lsp") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec")
(define_insn_reservation "r_lda" 3
  (and (eq_attr "pipe" "lda") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec+tc_dmbi")
(define_insn_reservation "r_ldd" 2
  (and (eq_attr "pipe" "ldd") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec+tc_dmbi")
(define_insn_reservation "r_sta" 1
  (and (eq_attr "pipe" "sta") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec")
(define_insn_reservation "r_std" 1
  (and (eq_attr "pipe" "std") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec")
(define_insn_reservation "r_aalu"  1
  (and (eq_attr "pipe" "aalu") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec")
(define_insn_reservation "r_mixdd" 1
  (and (eq_attr "pipe" "mixdd") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec")
(define_insn_reservation "r_mixds" 2
  (and (eq_attr "pipe" "mixds") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec")

(define_insn_reservation "r_jls" 1
  (and (eq_attr "pipe" "jls") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec+tc_jump")
(define_insn_reservation "r_ctx" 1
  (and (eq_attr "pipe" "ctx") (eq_attr "pipevariant" "standard"))
  "tc_ls_dec+tc_jump+tc_ctx")

;; Loop instructions
(define_insn_reservation "r_loop" 4
  (and (eq_attr "pipe" "lp") (eq_attr "pipevariant" "standard"))
  "tc_lp_dec+tc_jump")

;; instructions using both pipelines
(define_insn_reservation "r_unk" 3
  (and (eq_attr "pipe" "dual") (eq_attr "pipevariant" "standard"))
  "tc_ip_dec+tc_ls_dec")

;; Issue
(absence_set "tc_ip_dec" "tc_ls_dec")
(absence_set "tc_ip_dec" "tc_lp_dec")
(absence_set "tc_ls_dec" "tc_lp_dec")

;; Note: be relaxed about STA/D, LDA/D as the actual register type is not determind 
;; during the first pass the scheduler.
;; Note: 1st pass IP -> LS may be changed to IP -> MOV.A -> LS

;; IP1, JIP ->
(define_bypass 1 "r_ip1,r_jip" "r_ip1,r_ip2,r_jip,r_dvi,r_dvs,r_mul,r_ip3")
(define_bypass 1 "r_ip1,r_jip" "r_mixds")
(define_bypass 3 "r_ip1,r_jip" "r_sta,r_std,r_lda,r_ldd") ; address dep

;; IP2, DVINIT, FP2 ->
(define_bypass 2 "r_ip2,r_dvi" "r_ip1,r_ip2,r_jip,r_dvi,r_mul,r_ip3")
(define_bypass 2 "r_ip2,r_dvi" "r_mixds")
(define_bypass 4 "r_ip2,r_dvi" "r_sta,r_std,r_lda,r_ldd") ; 1st pass addr dep

;; IP DVINIT -> DVSTEP
(define_bypass 2 "r_ip2" "r_dvs")
(define_bypass 1 "r_dvi" "r_dvs")

;; IP FP3 ->
;; (define_bypass 0 "r_fp3" "r_std")

;; LS AALU ->
(define_bypass 1 "r_aalu" "r_aalu")
(define_bypass 1 "r_aalu" "r_mixdd,r_mixds")
(define_bypass 1 "r_aalu" "r_lda,r_ldd,r_sta,r_std")
(define_bypass 1 "r_aalu" "r_jls")
(define_bypass 1 "r_aalu" "r_ctx")
(define_bypass 1 "r_aalu" "r_ls")

;; LS MIX DD ->
(define_bypass 1 "r_mixdd" "r_ip1,r_ip2,r_jip,r_dvi,r_dvs,r_mul,r_ip3")
(define_bypass 1 "r_mixdd" "r_mixds")
(define_bypass 1 "r_mixdd" "r_sta,r_std")
(define_bypass 1 "r_mixdd" "r_jls,r_ctx")
(define_bypass 1 "r_mixdd" "r_ls")

;; LS MIX DS ->
(define_bypass 2 "r_mixds" "r_aalu,r_mixdd,r_jls")
(define_bypass 2 "r_mixds" "r_lda,r_ldd,r_sta,r_std") ; addr dependency
(define_bypass 2 "r_mixds" "r_mixds")                 ; addr input to addsc
(define_bypass 1 "r_mixds" "r_ls")

;; LS LDA ->
(define_bypass 2 "r_lda" "r_ip1,r_ip2,r_jip,r_dvi,r_dvs,r_mul,r_ip3") ; 1st pass

;; LS LDD ->
(define_bypass 2 "r_ldd" "r_ip1,r_ip2,r_jip,r_dvi,r_dvs,r_mul,r_ip3")
(define_bypass 2 "r_ldd" "r_mixds")
(define_bypass 1 "r_ldd" "r_std")
(define_bypass 1 "r_ldd" "r_aalu,r_jls,r_ls,r_mixdd,r_loop") ; address dep

;; LS undef ->
(define_bypass 2 "r_ls" "r_ip1,r_ip2,r_jip,r_dvi,r_dvs,r_mul,r_ip3")
(define_bypass 1 "r_ls" "r_aalu")
(define_bypass 1 "r_ls" "r_mixdd")
(define_bypass 1 "r_ls" "r_mixds")
(define_bypass 1 "r_ls" "r_jls")
(define_bypass 1 "r_ls" "r_ctx")
(define_bypass 1 "r_ls" "r_ls")

;; Loop
(define_bypass 1 "r_loop" "r_sta")
(define_bypass 1 "r_loop" "r_loop")


;; Automata for aurix and aurix plus devices, performance core.
(define_automaton "tricore_pipeline_16x")
(automata_option "v")

;; Issue unit characteristics: can issue into integer and load/store in paralell
;; tc16x_is_int - issue into integer pipeline
;; tc16x_is_ldst - issue into load/store pipeline
;; tc16x_is_whole - issue into other pipeline, blocks other issues
(define_cpu_unit "tc16x_is_int,tc16x_is_ldst,tc16x_is_whole" "tricore_pipeline_16x")
(exclusion_set "tc16x_is_int,tc16x_is_ldst" "tc16x_is_whole")

;; Integer pipeline characteristics
;; tc16x_ip1 - first cycle in integer pipeline
;; tc16x_ip2 - 2nd cycle in integer pipeline
;; tc16x_ipm - 3rd and later cycles in integer pipeline
(define_cpu_unit "tc16x_ip1,tc16x_ip2,tc16x_ipm" "tricore_pipeline_16x")
(define_insn_reservation "r_tc16x_ip1" 1
  (and (eq_attr "pipe" "ip") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_int+tc16x_ip1")
(define_insn_reservation "r_tc16x_jip" 4
  (and (eq_attr "pipe" "jip") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_whole*4")
(define_insn_reservation "r_tc16x_ip2" 2
  (and (eq_attr "pipe" "ip2") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_int+tc16x_ip1,tc16x_ip2")
(define_insn_reservation "r_tc16x_mul" 3
  (and (eq_attr "pipe" "ipm") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_int+tc16x_ip1,tc16x_ip2,tc16x_ipm")
(define_insn_reservation "r_tc16x_ip3" 3
  (and (eq_attr "pipe" "ip3") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_int+tc16x_ip1,tc16x_ip2,tc16x_ipm")
(define_insn_reservation "r_tc16x_dvi" 2
  (and (eq_attr "pipe" "ipdi") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_int+tc16x_ip1,tc16x_ip2")
(define_insn_reservation "r_tc16x_dvs" 6
  (and (eq_attr "pipe" "ipds") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_int+tc16x_ip1,tc16x_ip1*2,tc16x_ip2*3")

;; Load/store pipeline characteristics
;; tc16x_ld1 - first cycle in load/store pipeline
;; tc16x_ld2 - 2nd cycle in load/store pipeline
;; tc16x_ld3 - 3rd cycle in load/store pipeline
(define_cpu_unit "tc16x_ld1,tc16x_ld2,tc16x_ld3" "tricore_pipeline_16x")

(define_insn_reservation "r_tc16x_ls"  1
  (and (eq_attr "pipe" "lsp") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_ldst")
(define_insn_reservation "r_tc16x_lda" 3
  (and (eq_attr "pipe" "lda") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_ldst+tc16x_ld1,tc16x_ld2,tc16x_ld3")
(define_insn_reservation "r_tc16x_ldd" 2
  (and (eq_attr "pipe" "ldd") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_ldst+tc16x_ld1,tc16x_ld2")
(define_insn_reservation "r_tc16x_sta" 1
  (and (eq_attr "pipe" "sta") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_ldst+tc16x_ld1")
(define_insn_reservation "r_tc16x_std" 1
  (and (eq_attr "pipe" "std") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_ldst+tc16x_ld1")
(define_insn_reservation "r_tc16x_aalu"  1
  (and (eq_attr "pipe" "aalu") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_ldst+tc16x_ld1")
(define_insn_reservation "r_tc16x_mixdd" 2
  (and (eq_attr "pipe" "mixdd") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_ldst+tc16x_ld1,tc16x_ld2")
(define_insn_reservation "r_tc16x_mixds" 2
  (and (eq_attr "pipe" "mixds") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_ldst+tc16x_ld1,tc16x_ld2")

(define_insn_reservation "r_tc16x_jls" 2
  (and (eq_attr "pipe" "jls") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_whole*2")
(define_insn_reservation "r_tc16x_ctx" 7
  (and (eq_attr "pipe" "ctx") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_whole*7")


;; Loop pipeline characteristics
;; tc16x_loop - loop pipeline
(define_cpu_unit "tc16x_loop" "tricore_pipeline_16x")
(define_insn_reservation "r_tc16x_loop" 4
  (and (eq_attr "pipe" "lp") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_whole+tc16x_loop*4")

;; Floating point pipeline characteristics
;; Instructions are driven through integer pipeline
(define_insn_reservation "r_tc16x_fl1" 1
  (and (eq_attr "pipe" "fp") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_whole+tc16x_ip1")
(define_insn_reservation "r_tc16x_fl2" 2
  (and (eq_attr "pipe" "fp2") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_whole+tc16x_ip1,tc16x_ip2")
(define_insn_reservation "r_tc16x_fl3" 3
  (and (eq_attr "pipe" "fp3") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_whole+tc16x_ip1,tc16x_ip2,tc16x_ipm")
(define_insn_reservation "r_tc16x_fldiv" 8
  (and (eq_attr "pipe" "fpdiv") (eq_attr "pipevariant" "tc16x"))
  "tc16x_is_whole+tc16x_ip1*6,tc16x_ip2*2")

;; Address latencies
;; For some instructions the updated address value is available sooner, then the result
(define_bypass 1 "r_tc16x_ldd" "r_tc16x_sta,r_tc16x_jls,r_tc16x_ctx,r_tc16x_aalu")

;; Same type repetition
;; Instructions with the same type can run more dense in the same pipeline
(define_bypass 6 "r_tc16x_fldiv" "r_tc16x_fldiv")
(define_bypass 1 "r_tc16x_ip1" "r_tc16x_ip1")
(define_bypass 1 "r_tc16x_ip2" "r_tc16x_ip2")
(define_bypass 1 "r_tc16x_ip3" "r_tc16x_ip3")
(define_bypass 1 "r_tc16x_mul" "r_tc16x_mul")
(define_bypass 1 "r_tc16x_dvi" "r_tc16x_dvi")
(define_bypass 1 "r_tc16x_lda" "r_tc16x_lda")
(define_bypass 1 "r_tc16x_ldd" "r_tc16x_ldd")
(define_bypass 1 "r_tc16x_sta" "r_tc16x_sta")
(define_bypass 1 "r_tc16x_std" "r_tc16x_std")
(define_bypass 1 "r_tc16x_aalu" "r_tc16x_aalu")
(define_bypass 1 "r_tc16x_mixdd" "r_tc16x_mixdd")
(define_bypass 1 "r_tc16x_mixds" "r_tc16x_mixds")
