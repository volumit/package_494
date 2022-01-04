/* Core target definitions for GNU compiler
   for PowerPC embedded targeted systems e200zx support
   Copyright (C) 2010
   Free Software Foundation, Inc.
   Contributed by Horst Lehser (Horst.Lehser@hightec-rt.com).

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#undef TARGET_SPE_ABI
#undef TARGET_SPE
#undef TARGET_E500
#undef TARGET_FPRS
#undef TARGET_E500_SINGLE
#undef TARGET_E500_DOUBLE
#undef CHECK_E500_OPTIONS

#define TARGET_SPE_ABI rs6000_spe_abi
#define TARGET_SPE rs6000_spe
#define TARGET_E500 (rs6000_cpu == PROCESSOR_PPC8540)
#define TARGET_FPRS (rs6000_float_gprs == 0)
#define TARGET_E500_SINGLE (TARGET_HARD_FLOAT && rs6000_float_gprs == 1)
#define TARGET_E500_DOUBLE (TARGET_HARD_FLOAT && rs6000_float_gprs == 2)
#define CHECK_E500_OPTIONS						\
  do {									\
    if (TARGET_E500 || TARGET_SPE || TARGET_SPE_ABI			\
	|| TARGET_E500_SINGLE || TARGET_E500_DOUBLE)			\
      {									\
	if (TARGET_ALTIVEC)						\
	  error ("AltiVec and E500 instructions cannot coexist");	\
	if (TARGET_VSX)							\
	  error ("VSX and E500 instructions cannot coexist");		\
	if (TARGET_64BIT)						\
	  error ("64-bit E500 not supported");				\
	if (TARGET_HARD_FLOAT && TARGET_FPRS)				\
	  error ("E500 and FPRs not supported");			\
      }									\
  } while (0)

/* Override rs6000.h definition.  */
#undef HARD_REGNO_CALLER_SAVE_MODE
/* When setting up caller-save slots (MODE == VOIDmode) ensure we
   allocate space for DFmode.  Save gprs in the correct mode too.  */
#define HARD_REGNO_CALLER_SAVE_MODE(REGNO, NREGS, MODE) \
  (TARGET_E500_DOUBLE && ((MODE) == VOIDmode || (MODE) == DFmode)	\
   ? DFmode								\
   : choose_hard_reg_mode ((REGNO), (NREGS), false))
#undef  TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_STRICT_ALIGN | MASK_EABI)

#undef TARGET_CPU_DEFAULT
#define TARGET_CPU_DEFAULT "e200zx"

/* TODO: check on how can we add it (removed for now) */
/*#undef  TARGET_VERSION
#define TARGET_VERSION fprintf (stderr, " (PowerPC Embedded E200ZX)"); */

/* e500 and zen architecture have no lwsync (use plain sync) */
#undef TARGET_NO_LWSYNC
#define TARGET_NO_LWSYNC        (TARGET_E500 || TARGET_VLE)

#undef  SUBSUBTARGET_OVERRIDE_OPTIONS
#define SUBSUBTARGET_OVERRIDE_OPTIONS \
  if (!global_options_set.x_rs6000_cpu_index) \
    rs6000_cpu = PROCESSOR_PPCE200ZX; \
  if (!global_options_set.x_rs6000_spe_abi  && !TARGET_SOFT_FLOAT) \
    rs6000_spe_abi = 1; \
  if (!global_options_set.x_rs6000_float_gprs  && !TARGET_SOFT_FLOAT) \
    rs6000_float_gprs = 1; \
  if (!global_options_set.x_rs6000_spe  && !TARGET_SOFT_FLOAT) \
    rs6000_spe = 1; \
  if (rs6000_isa_flags & MASK_64BIT) \
    error ("-m64 not supported in this configuration");	\
  if (flag_pic && TARGET_VLE)                           \
    {                                                   \
      error_at (UNKNOWN_LOCATION, "-f%s and -mvle are incompatible",		\
	     (flag_pic > 1) ? "PIC" : "pic");		\
    }					        	\

/* The e500 ABI says that either long doubles are 128 bits, or if
   implemented in any other size, the compiler/linker should error out.
   We have no emulation libraries for 128 bit long doubles, and I hate
   the dozens of failures on the regression suite.  So I'm breaking ABI
   specifications, until I properly fix the emulation.

   Enable these later.
#define RS6000_DEFAULT_LONG_DOUBLE_SIZE (TARGET_SPE ? 128 : 64)
*/

/* we will always use save inline with lmw/stmw */
#undef GP_SAVE_INLINE
#define GP_SAVE_INLINE(FIRST_REG) ((FIRST_REG) < 32)

#undef  ASM_DEFAULT_SPEC
#define	ASM_DEFAULT_SPEC "%{!msoft-float: -mspe} -me200zx"

/* Common ASM definitions used by ASM_SPEC among the various targets for
   handling -mcpu=xxx switches.  this is a copy from rs6000.h without -many */
#undef ASM_CPU_SPEC
#define ASM_CPU_SPEC \
 "%{!mcpu*: "\
 "%{mpower: %{!mpower2: -mpwr}} "\
 "%{mpower2: -mpwrx} "\
 "%{mpowerpc64*: -mppc64} "\
 "%{!mpowerpc64*: %{mpowerpc*: -mppc}} "\
 "%{mno-power: %{!mpowerpc*: -mcom}} "\
 "%{!mno-power: %{!mpower*: %(asm_default)}}} "\
 "%{mcpu=native: %(asm_cpu_native)} "\
 "%{mcpu=common: -mcom} "\
 "%{mcpu=cell: -mcell} "\
 "%{mcpu=power: -mpwr} "\
 "%{mcpu=power2: -mpwrx} "\
 "%{mcpu=power3: -mppc64} "\
 "%{mcpu=power4: -mpower4} "\
 "%{mcpu=power5: %(asm_cpu_power5)} "\
 "%{mcpu=power5+: %(asm_cpu_power5)} "\
 "%{mcpu=power6: %(asm_cpu_power6) -maltivec} "\
 "%{mcpu=power6x: %(asm_cpu_power6) -maltivec} "\
 "%{mcpu=power7: %(asm_cpu_power7)} "\
 "%{mcpu=a2: -ma2} "\
 "%{mcpu=powerpc: -mppc} "\
 "%{mcpu=rios: -mpwr} "\
 "%{mcpu=rios1: -mpwr} "\
 "%{mcpu=rios2: -mpwrx} "\
 "%{mcpu=rsc: -mpwr} "\
 "%{mcpu=rsc1: -mpwr} "\
 "%{mcpu=rs64a: -mppc64} "\
 "%{mcpu=401: -mppc} "\
 "%{mcpu=403: -m403} "\
 "%{mcpu=405: -m405} "\
 "%{mcpu=405fp: -m405} "\
 "%{mcpu=440: -m440} "\
 "%{mcpu=440fp: -m440} "\
 "%{mcpu=464: -m440} "\
 "%{mcpu=464fp: -m440} "\
 "%{mcpu=476: %(asm_cpu_476)} "\
 "%{mcpu=476fp: %(asm_cpu_476)} "\
 "%{mcpu=505: -mppc} "\
 "%{mcpu=601: -m601} "\
 "%{mcpu=602: -mppc} "\
 "%{mcpu=603: -mppc} "\
 "%{mcpu=603e: -mppc} "\
 "%{mcpu=ec603e: -mppc} "\
 "%{mcpu=604: -mppc} "\
 "%{mcpu=604e: -mppc} "\
 "%{mcpu=620: -mppc64} "\
 "%{mcpu=630: -mppc64} "\
 "%{mcpu=740: -mppc} "\
 "%{mcpu=750: -mppc} "\
 "%{mcpu=G3: -mppc} "\
 "%{mcpu=7400: -mppc -maltivec} "\
 "%{mcpu=7450: -mppc -maltivec} "\
 "%{mcpu=G4: -mppc -maltivec} "\
 "%{mcpu=801: -mppc} "\
 "%{mcpu=821: -mppc} "\
 "%{mcpu=823: -mppc} "\
 "%{mcpu=860: -mppc} "\
 "%{mcpu=970: -mpower4 -maltivec} "\
 "%{mcpu=G5: -mpower4 -maltivec} "\
 "%{mcpu=8540: -me500} "\
 "%{mcpu=8548: -me500} "\
 "%{mcpu=e300c2: -me300} "\
 "%{mcpu=e300c3: -me300} "\
 "%{mcpu=e500mc: -me500mc} "\
 "%{mcpu=e500mc64: -me500mc64} "\
 "%{maltivec: -maltivec} "\
 "%{mcpu=e200zx: -me200zx %{!mno-regnames: -mregnames}} "\
 "%{mregnames} %{mno-regnames} "\
 "%{mvle: -mvle %{!mno-regnames: -mregnames}} "\
 "%{mcpu=z225n3: -mz225n3 %{!mno-regnames: -mregnames}} "\
 "%{mcpu=z410n3: -mz410n3 -msat %{!mno-regnames: -mregnames}} "\
 "%{mcpu=z420n3: -mz420n3 %{!mno-regnames: -mregnames}} "\
 "%{mcpu=z425n3: -mz425n3 %{!mno-regnames: -mregnames}} "\
 "%{mcpu=z425Bn3: -mz425Bn3 -msat %{!mno-regnames: -mregnames}} "\
 "%{mcpu=z710n3: -mz710n3 -msat %{!mno-regnames: -mregnames}} "\
 "%{mcpu=z720n3: -mz720n3 %{!mno-regnames: -mregnames}} "\
 "%{mcpu=MCU_JDP_UC1: -mz720n3 -mlsp %{!mno-regnames: -mregnames}} "\
 "%{mlsp: -mlsp %{!mno-regnames: -mregnames}}"\
 "%{ffixed-*: -mfixed-%*} " \
 "%{mfixed-sda: -mfixed-r14 -mfixed-r15 -mfixed-r16 -mfixed-r17 }"
 
#undef	CPP_OS_DEFAULT_SPEC
#define	CPP_OS_DEFAULT_SPEC \
 "%{mvle:-D__PPC_VLE__} "\
 "%{!mcpu=*: -D__PPC_E200ZX__} "\
 "%{mcpu=z225n3: -D__PPC_Z225n3__ -D__PPC_E200ZX__ -D__PPC_EFPU2__ -D__PPC_LSP__ -D__PPC_VLE__} "\
 "%{mcpu=z410n3: -D__PPC_Z410n3__ -D__PPC_E200ZX__ -D__PPC_EFPU2__ -D__PPC_SAT__ -D__PPC_VLE__} "\
 "%{mcpu=z420n3: -D__PPC_Z420n3__ -D__PPC_E200ZX__ -D__PPC_EFPU2__ -D__PPC_VLE__} "\
 "%{mcpu=z425n3: -D__PPC_Z425n3__ -D__PPC_E200ZX__ -D__PPC_EFPU2__ -D__PPC_LSP__ -D__PPC_VLE__} "\
 "%{mcpu=z425Bn3: -D__PPC_Z425Bn3__ -D__PPC_E200ZX__ -D__PPC_EFPU2__ -D__PPC_LSP__ -D__PPC_SAT__ -D__PPC_VLE__} "\
 "%{mcpu=z710n3: -D__PPC_Z710n3__ -D__PPC_E200ZX__ -D__PPC_EFPU2__ -D__PPC_SAT__ -D__PPC_VLE__} "\
 "%{mcpu=z720n3: -D__PPC_Z720n3__ -D__PPC_E200ZX__ -D__PPC_EFPU2__ -D__PPC_VLE__} "\
 "%{mcpu=e200zx: -D__PPC_E200ZX__} "\
 "%{mcpu=MCU_JDP_UC1: -D__MCU_JDP_UC1__ -D__PPC_Z720n3__ -D__PPC_E200ZX__ -D__PPC_EFPU2__ -D__PPC_VLE__ } "\
 "%{mefpu2: -D__PPC_EFPU2__} "\
 "%{mlsp: -D__PPC_LSP__} "\
 "%{fshort-double:-D__HAVE_SHORT_DOUBLE__}"

#if 0
/* TODO: add pdnk when insert-tooldir is available */
 "%{pdnk: %:insert-tooldir(-iprefix  ppc-ht-eabi/ ) -iwithprefixbefore dinkum}"\

#endif


#undef LIB_DEFAULT_SPEC
/*#define LIB_DEFAULT_SPEC "%{!pdnk: -lc -lnosys} %{pdnk: -ldnk_c -lnosys}"*/
#define LIB_DEFAULT_SPEC "-ldnk_c -los %{lm: }"

/* the math library is included in the dinkumware C-library */
#undef MATH_LIBRARY
#define MATH_LIBRARY	""

#undef LINK_SPEC
#define LINK_SPEC "%{mcpu=*: --mcpu=%*}"

#undef	STARTFILE_DEFAULT_SPEC 
#define	STARTFILE_DEFAULT_SPEC "ecrti.o%s %{!nocrt0: crt0.o%s} crtbegin.o%s"

#define CC1_LICENSE_CHECK \
 "%{!mlicense-dir*:%:insert-tooldir(-mlicense-dir= licenses)}"

#undef	SUBSUBTARGET_EXTRA_SPECS
#define	SUBSUBTARGET_EXTRA_SPECS \
  { "cc1_license",	CC1_LICENSE_CHECK },

/* do not define .jcr section in crtstuff, because we did not support java yet
 */
#define TARGET_USE_JCR_SECTION 0
#undef JCR_SECTION_NAME

/* redefine section definitions for text sections, because for VLE we need
   additional section flags
*/
#define TEXT_NON_VLE_SECTION_ASM_OP     "\t.section\t\".text\",\"ax\""
#define TEXT_VLE_SECTION_ASM_OP 	"\t.section\t\".text_vle\",\"axv\",@progbits"
#define TEXT_VLE_PIC_SECTION_ASM_OP	"\t.section\t\".pictext_vle\",\"axv\",@progbits"

#undef	TEXT_SECTION_ASM_OP
#define	TEXT_SECTION_ASM_OP	((TARGET_VLE) ? (TARGET_CODE_PIC) ? TEXT_VLE_PIC_SECTION_ASM_OP : TEXT_VLE_SECTION_ASM_OP : TEXT_NON_VLE_SECTION_ASM_OP )
#undef	INIT_SECTION_ASM_OP
#define	INIT_SECTION_ASM_OP     ((TARGET_VLE)? "\t.section\t\".init_vle\",\"axv\",@progbits" : \
                                 "\t.section\t\".init\",\"ax\",@progbits")
#undef	FINI_SECTION_ASM_OP
#define	FINI_SECTION_ASM_OP      ((TARGET_VLE)? "\t.section\t\".fini_vle\",\"axv\",@progbits" : \
                                 "\t.section\t\".fini\",\"ax\",@progbits")
/* we define .ctors and .dtors as readonly section */ 
#define CTORS_SECTION_ASM_OP    ".section .ctors,\"a\",@progbits"
#define DTORS_SECTION_ASM_OP    ".section .dtors,\"a\",@progbits"

/* add defines for use in target sources */
#ifdef IN_LIBGCC2
#ifdef __PPC_VLE__
#undef	TEXT_SECTION_ASM_OP
#define	TEXT_SECTION_ASM_OP	"\t.section\t\".text_vle\",\"axv\",@progbits"
#undef	INIT_SECTION_ASM_OP
#define	INIT_SECTION_ASM_OP     "\t.section\t\".init_vle\",\"axv\",@progbits"
#undef	FINI_SECTION_ASM_OP
#define	FINI_SECTION_ASM_OP     "\t.section\t\".fini_vle\",\"axv\",@progbits"
#else /* __PPC_VLE__ */
#undef	TEXT_SECTION_ASM_OP
#define	TEXT_SECTION_ASM_OP	"\t.section\t\".text\",\"ax\""
#undef	INIT_SECTION_ASM_OP
#define	INIT_SECTION_ASM_OP     "\t.section\t\".init\",\"ax\",@progbits"
#undef	FINI_SECTION_ASM_OP
#define	FINI_SECTION_ASM_OP     "\t.section\t\".fini\",\"ax\",@progbits"
#endif /* __PPC_VLE__ */
#endif /* IN_LIBGCC2 */

/* define section type for VLE text sections */
#define SECTION_PPC_VLE         SECTION_MACH_DEP

#define PPC_ATTR_NAKED          "naked"
#define PPC_ATTR_ASECTION       "asection"
#define PPC_ATTR_INDIRECT       "fardata"
/* */
#undef SUBTARGET_ATTRIBUTE_TABLE
#define SUBTARGET_ATTRIBUTE_TABLE \
{ PPC_ATTR_NAKED, 0, 0, true, false, false, ppcvle_handle_naked_attribute, false }, \
{ PPC_ATTR_INDIRECT, 0, 0, true, false, false, ppcvle_handle_decl_attribute, false },   \
{ "asection", 1, 3, true, false, false, htc_handle_asection_attribute, false }

/* PIC_OFFSET_TABLE_REGNUM is used for -mcode-pic to hold the offset */
#undef PIC_OFFSET_TABLE_REGNUM
#define PIC_OFFSET_TABLE_REGNUM \
   ((flag_pic  || (TARGET_CODE_PIC && TARGET_PIC_DYNAMIC_ADDR_CALC)) ? \
      RS6000_PIC_OFFSET_TABLE_REGNUM : INVALID_REGNUM)

#if 0
/* TODO: enable insert-tooldir */

/* define our own spec function to insert the tooldir prefix */
#undef EXTRA_SPEC_FUNCTIONS 
extern const char * insert_tooldir_spec_function(int argc, const char **argv);
#define EXTRA_SPEC_FUNCTIONS \
  { "insert-tooldir", insert_tooldir_spec_function },
#endif


#ifdef BUILD_FREE_ENTRY_TOOLCHAIN
#define LICENSE_PRODUCT "ppc-free-gcc"
#else
#define LICENSE_PRODUCT "ppc-vle-gcc"
#endif


#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()          \
  do                                      \
    {                                     \
      builtin_define_std ("PPC");         \
      builtin_define ("__embedded__");    \
      builtin_assert ("system=embedded"); \
      builtin_assert ("cpu=powerpc");     \
      builtin_assert ("machine=powerpc"); \
      TARGET_OS_SYSV_CPP_BUILTINS ();     \
    }                                     \
  while (0)

/* do not Invoke an initializer function  */
#undef NAME__MAIN 
#undef INVOKE__main
#undef TARGET_POSIX_IO

#define SYMBOL_FLAG_CO_PIC      (SYMBOL_FLAG_MACH_DEP << 0)
#define SYMBOL_FLAG_CO_PIC_P(X) ((SYMBOL_REF_FLAGS (X) & SYMBOL_FLAG_CO_PIC) != 0)

#undef  JUMP_TABLES_IN_TEXT_SECTION
#define JUMP_TABLES_IN_TEXT_SECTION TARGET_VLE

#undef ASM_DECLARE_OBJECT_NAME
#define ASM_DECLARE_OBJECT_NAME(FILE, NAME, DECL)			\
        ppc_asm_declare_object_name(FILE , NAME, DECL)

/* Define callback for libc function detection */
/* Replaces: TARGET_C99_FUNCTIONS and TARGET_HAS_SINCOS */
#undef TARGET_LIBC_HAS_FUNCTION
#define TARGET_LIBC_HAS_FUNCTION ppcvle_libc_has_function


/* use SJLJ exception handling */
#define DWARF2_UNWIND_INFO      0

/* Describe how to emit uninitialized local items.  */
#undef	ASM_OUTPUT_ALIGNED_DECL_LOCAL
#define	ASM_OUTPUT_ALIGNED_DECL_LOCAL(STREAM, DECL, NAME, SIZE, ALIGN)	\
 ppcvle_asm_output_aligned_block_local((STREAM), (DECL), (NAME), (SIZE), (ALIGN))
