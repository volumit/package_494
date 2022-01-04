/* Definitions of target machine for GNU compiler, for Infineon TriCore
   Copyright (C) 2010-2014 Free Software Foundation, Inc.
   Contributed by Georg Lay (georg.lay@hightec-rt.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */


/* We define the as specific configures from auto-host.h here to get an
   consistent version.  Especially the HAVE_GAS_SHF_MERGE should be undefined
   to avoid problems in the extmap from the linker and the tool
   tricore-lsyms.  */
#undef  HAVE_AS_LEB128
#define HAVE_AS_LEB128 1

#undef  HAVE_GAS_BALIGN_AND_P2ALIGN
#define HAVE_GAS_BALIGN_AND_P2ALIGN 1

#undef  HAVE_GAS_HIDDEN
#define HAVE_GAS_HIDDEN 1

#undef  HAVE_GAS_MAX_SKIP_P2ALIGN
#define HAVE_GAS_MAX_SKIP_P2ALIGN 1

#undef  HAVE_GAS_SHF_MERGE
#define HAVE_GAS_SHF_MERGE 0

#undef  HAVE_GAS_SUBSECTION_ORDERING
#define HAVE_GAS_SUBSECTION_ORDERING 1

#undef  HAVE_GAS_WEAK
#define HAVE_GAS_WEAK 1

/* GPRs of upper context. These Regs need not to be saved/restored by
   a function that destroys their content, even if the respective Regs
   are call(ee) saved. */
#define UPPER_CTX_MASK 0xfc00ff00


/***********************************************************************/
/* Specs */

extern const char *tric_device_to_startfile (int argc, const char **argv);
extern const char *tric_device_to_ld (int argc, const char **argv);
extern const char *tric_device_to_as (int argc, const char **argv);
extern const char *tric_self_specs (int argc, const char **argv);
extern const char *insert_tooldir_spec_function (int argc, const char **argv);

#ifndef CPP_SPEC
#define CPP_SPEC  \
 "%{fshort-double:-D__HAVE_SHORT_DOUBLE__}"
#endif

#define DRIVER_SELF_SPECS                     \
  "%:tric_self_specs(%{mtc*:mtc%*}%{mcpu=*:mcpu=%*}%{mprint-derivatives}) "
    
#define EXTRA_SPEC_FUNCTIONS                            \
  { "device_to_startfile", tric_device_to_startfile },  \
  { "device_to_ld", tric_device_to_ld },                \
  { "device_to_as", tric_device_to_as },                \
  { "tric_self_specs", tric_self_specs },               \
  { "insert-tooldir", insert_tooldir_spec_function }, 

#undef  ASM_SPEC
#define ASM_SPEC                                \
  "%{G*} %{n} "                                 \
  "%{Ym,*} %{Yd,*} %{Wa,*:%*} "                 \
  "%:device_to_as() "

#undef  ASM_DEBUG_SPEC
#define ASM_DEBUG_SPEC "%{g|g2|g3|gdwarf*|ggdb*:-gdwarf2}"

#define LINK_SPEC                               \
  "%{h*} "                                      \
  "%{v:-V} "                                    \
  "%{b} "                                       \
  "%{relax:-relax}"                             \
  "%{relax-24rel:-relax-24rel}"                 \
  "%{static:-dn -Bstatic} "                     \
  "%{shared:-G -Bdynamic} "                     \
  "%{symbolic:-Bsymbolic} "                     \
  "%{G*} "                                      \
  "%{YP,*} "                                    \
  "%:device_to_ld() "

/* Will be overridden in tricore-dinkum.h if configured with
      --with-newlib=no
*/
#define LIB_SPEC                                \
  "%{!shared:-lc -los -lm -lc}"

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC                                                  \
  "crti%O%s "                                                           \
  "crtbegin%O%s "                                                       \
  "%{nocrt0:} %:device_to_startfile() "

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC                            \
  " crtend%O%s crtn%O%s"

#define CC1_LICENSE_CHECK \
 "%{!mlicense-dir*:%:insert-tooldir(-mlicense-dir= licenses)}"

#define EXTRA_SPECS \
  { "cc1_license", CC1_LICENSE_CHECK },

#define CC1_SPEC "%(cc1_license)"

#ifdef BUILD_FREE_ENTRY_TOOLCHAIN
#define LICENSE_PRODUCT "tricore-free-gcc"
#else
#define LICENSE_PRODUCT "tricore-gcc"
#endif

/* Maximum number of library ids we permit */
#define MAX_LIBRARY_ID 255


/***********************************************************************/
/* Basic Stack Layout */

#define STACK_GROWS_DOWNWARD
#define STACK_PUSH_CODE PRE_DEC
#define FRAME_GROWS_DOWNWARD 1
#define STARTING_FRAME_OFFSET 0
#define FIRST_PARM_OFFSET(DECL) 0
#define RETURN_ADDR_RTX(COUNT, FRAME) tric_return_addr_rtx (COUNT)
#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (Pmode, REG_A11)

#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG
#define DWARF_FRAME_RETURN_COLUMN DWARF_FRAME_REGNUM (REG_A11)
#define DWARF2_ASM_LINE_DEBUG_INFO 1
#define DWARF_CIE_DATA_ALIGNMENT 1
#define DWARF2_UNWIND_INFO 0


/***********************************************************************/
/* Registers That Address the Stack Frame */

#define STACK_POINTER_REGNUM REG_SP
#define FRAME_POINTER_REGNUM REG_FP
#define ARG_POINTER_REGNUM REG_ARGP
#define STATIC_CHAIN_REGNUM REG_A3


/***********************************************************************/
/* Eliminating Frame Pointer and Arg Pointer */

#define ELIMINABLE_REGS                         \
{{ ARG_POINTER_REGNUM, STACK_POINTER_REGNUM},   \
 { ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM},   \
 { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM}}

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET) \
  ((OFFSET) = tric_initial_elimination_offset ((FROM), (TO)))


/***********************************************************************/
/* Passing Function Arguments on the Stack */

#define ACCUMULATE_OUTGOING_ARGS 1


/***********************************************************************/
/* Passing Arguments in Registers */

typedef struct
{
  const char * name;      /* != NULL for support funcs */
  const char * args;      /* String encoding the prototype */
  char ret;               /* 'p' = void*, 'i' = int, 'v' = void */
  int  fast;              /* fastcall version available? */
} tric_libfunc_info_t;

/* Bits of CUMULATIVE_ARGS.call_cookie */

#define CALLCOOKIE_SIBLING_MASK             (1 << 0)
#define CALLCOOKIE_INTERRUPT_MASK           (1 << 1)
#define CALLCOOKIE_INTERRUPT_HANDLER_MASK   (1 << 2)
#define CALLCOOKIE_LONGCALL_MASK            (1 << 3)
#define CALLCOOKIE_NO_SIBCALL_MASK          (1 << 4)
#define CALLCOOKIE_USE_CALL_MASK            (1 << 5)
#define CALLCOOKIE_PXHNDCALL_MASK           (1 << 6)
#define CALLCOOKIE_FASTCALL_MASK            (1 << 7)

typedef struct
{
  int call_cookie;          /* Do special things for this call */
  int argno;                /* Argument count */
  tree fntype;              /* fntype as passed to INIT_CUMULATIVE_ARGS */
  int libfunc_p;            /* Is this a support function? */
  int outgoing;             /* args outgoing or incoming? */
  int update;               /* internal use: update or not? */
  int this_argno;           /* Number of current argument */
  unsigned int args_mask;   /* Which regs are used as arg */
  tric_libfunc_info_t libfunc;
} CUMULATIVE_ARGS;

#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,INDIRECT,N_NAMED_ARGS) \
  (tric_init_cumulative_args (&CUM, FNTYPE, LIBNAME, N_NAMED_ARGS))

#define FUNCTION_ARG_PADDING(MODE, TYPE)        \
  upward
                                            
#define FUNCTION_ARG_REGNO_P(REGNO)                     \
    (((REGNO) >= REG_D4 && (REGNO) <= REG_D7)           \
     || ((REGNO) >= REG_A4 && (REGNO) <= REG_A7)        \
     || (REGNO) == REG_A15                              \
     || (REGNO) == REG_D2                               \
     )


/***********************************************************************/
/* How Scalar Function Values Are Returned */

#define LIBCALL_VALUE(MODE)                     \
    gen_rtx_REG (MODE, REG_D2)

#define FUNCTION_VALUE_REGNO_P(N)               \
    ((N) == REG_D2 || (N) == REG_A2)


/***********************************************************************/
/* How Large Values Are Returned */

#define DEFAULT_PCC_STRUCT_RETURN 0


/***********************************************************************/
/* Function Entry and Exit */

#define EPILOGUE_USES(N) ((N) == REG_A11)


/***********************************************************************/
/* Trampolines for Nested Functions */

#define TRAMPOLINE_SIZE 0


/***********************************************************************/
/* Addressing Modes */

#define HAVE_POST_INCREMENT 1
#define HAVE_POST_DECREMENT 1
#define HAVE_POST_MODIFY_DISP 1
#define HAVE_PRE_INCREMENT (tric_segment_trap.have_pre_inc)
#define HAVE_PRE_DECREMENT (tric_segment_trap.have_pre_dec)
#define HAVE_PRE_MODIFY_DISP (HAVE_PRE_INCREMENT || HAVE_PRE_DECREMENT)

#define CONSTANT_ADDRESS_P(X)   (CONSTANT_P (X))
#define MAX_REGS_PER_ADDRESS 1


/***********************************************************************/
/* Misc */

#define WORD_REGISTER_OPERATIONS 1

#define LOAD_EXTEND_OP(MODE)                            \
    (((MODE) == QImode) ? ZERO_EXTEND : SIGN_EXTEND)

#define FUNCTION_MODE    HImode

#define Pmode            SImode

#define STORE_FLAG_VALUE 1

#define DOLLARS_IN_IDENTIFIERS 0

#define REGISTER_TARGET_PRAGMAS() tric_register_pragmas()

#undef  TARGET_CPU_CPP_BUILTINS
#define TARGET_CPU_CPP_BUILTINS()   tric_cpu_cpp_builtins (pfile)

#define ADJUST_INSN_LENGTH(INSN, LENGTH)                \
  (LENGTH = tric_adjust_insn_length (INSN, LENGTH))


/***********************************************************************/
/* Basic Characteristics of Registers */

#define FIRST_PSEUDO_REGISTER 34

#define FIXED_REGISTERS                                                 \
{                                                                       \
/* d0  d1  d2  d3  d4  d5  d6  d7     d8  d9 d10 d11 d12 d13 d14 d15 */ \
    0,  0,  0,  0,  0,  0,  0,  0,     0,  0,  0,  0,  0,  0,  0,  0,   \
/* a0  a1  a2  a3  a4  a5  a6  a7     a8  a9 a10 a11 a12 a13 a14 a15 */ \
    1,  1,  0,  0,  0,  0,  0,  0,     1,  1,  1,  1,  0,  0,  0,  0,   \
/* ARGP, PSW */                                                         \
    1,  1                                                               \
}

#define CALL_USED_REGISTERS                                             \
{                                                                       \
/* d0  d1  d2  d3  d4  d5  d6  d7     d8  d9 d10 d11 d12 d13 d14 d15 */ \
    1,  1,  1,  1,  1,  1,  1,  1,     0,  0,  0,  0,  0,  0,  0,  0,   \
/* a0  a1  a2  a3  a4  a5  a6  a7     a8  a9 a10 a11 a12 a13 a14 a15 */ \
    1,  1,  1,  1,  1,  1,  1,  1,     1,  1,  1,  1,  0,  0,  0,  0,   \
/* ARGP, PSW */                                                         \
    1, 1                                                                \
}

#define CALL_REALLY_USED_REGISTERS                                      \
{                                                                       \
/* d0  d1  d2  d3  d4  d5  d6  d7     d8  d9 d10 d11 d12 d13 d14 d15 */ \
    1,  1,  1,  1,  1,  1,  1,  1,     0,  0,  0,  0,  0,  0,  0,  0,   \
/* a0  a1  a2  a3  a4  a5  a6  a7     a8  a9 a10 a11 a12 a13 a14 a15 */ \
    0,  0,  1,  1,  1,  1,  1,  1,     0,  0,  0,  0,  0,  0,  0,  0,   \
/* ARGP, PSW */                                                         \
    0, 0                                                                \
}

#define LOCAL_REGNO(REGNO)                      \
  tric_local_regno (REGNO)                      \


/* Order of Allocation of Registers */

#define REG_ALLOC_ORDER                         \
{                                               \
    REG_D15,                                    \
    REG_D2,                                     \
    REG_D3,                                     \
    REG_D4,                                     \
    REG_D5,                                     \
    REG_D6,                                     \
    REG_D7,                                     \
    REG_D0,                                     \
    REG_D1,                                     \
    REG_D8,                                     \
    REG_D9,                                     \
    REG_D10,                                    \
    REG_D11,                                    \
    REG_D12,                                    \
    REG_D13,                                    \
    REG_D14,                                    \
                                                \
    REG_A15,                                    \
    REG_A2,                                     \
    REG_A3,                                     \
    REG_A4,                                     \
    REG_A5,                                     \
    REG_A6,                                     \
    REG_A7,                                     \
    REG_A12,                                    \
    REG_A13,                                    \
    REG_A14,                                    \
    REG_A0,                                     \
    REG_A1,                                     \
    REG_A8,                                     \
    REG_A9,                                     \
    REG_A10,                                    \
    REG_A11,                                    \
                                                \
    REG_ARGP,                                   \
    REG_PSW                                     \
}

#define HONOR_REG_ALLOC_ORDER 1


/***********************************************************************/
/* How Values Fit in Registers */

#define HARD_REGNO_NREGS(REGNO, MODE)                                   \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

#define HARD_REGNO_MODE_OK(REGNO, MODE)         \
  tric_hard_regno_mode_ok((REGNO), (MODE))

#define HARD_REGNO_RENAME_OK(FROM, TO)          \
  tric_hard_regno_rename_ok (FROM, TO)

#define MODES_TIEABLE_P(MODE1, MODE2)               \
 ((MODE1) == (MODE2)                                \
  || (GET_MODE_SIZE(MODE1) <= UNITS_PER_WORD        \
      && GET_MODE_SIZE(MODE2) <= UNITS_PER_WORD))

#define A_REGNO_P(X) ((X) >= REG_A0 && (X) <= REG_A15)
#define D_REGNO_P(X) (((unsigned int) (X)) <= REG_D15)
#define E_REGNO_P(X) (D_REGNO_P(X) && ((X) % 2 == 0))
#define EA_REGNO_P(X) ((X) >= REG_A2 && (X) <= REG_A15 && (X) % 2 == 0)

#define A_REG_P(X) (REG_P(X) && (A_REGNO_P(REGNO(X))))
#define D_REG_P(X) (REG_P(X) && (D_REGNO_P(REGNO(X))))
#define E_REG_P(X) (REG_P(X) && (E_REGNO_P(REGNO(X))))

#define REGISTER_PREFIX "%"

#define REGISTER_NAMES                                  \
{                                                       \
  "d0", "d1", "d2",  "d3",  "d4",  "d5",  "d6",  "d7",  \
  "d8", "d9", "d10", "d11", "d12", "d13", "d14", "d15", \
  "a0", "a1", "a2",  "a3",  "a4",  "a5",  "a6",  "a7",  \
  "a8", "a9", "SP", "a11", "a12", "a13", "a14", "a15",  \
  "ARGP", "PSW"                                         \
}

#define ADDITIONAL_REGISTER_NAMES               \
{                                               \
  { "D0",       REG_D0  },                      \
  { "D1",       REG_D1  },                      \
  { "D2",       REG_D2  },                      \
  { "D3",       REG_D3  },                      \
  { "D4",       REG_D4  },                      \
  { "D5",       REG_D5  },                      \
  { "D6",       REG_D6  },                      \
  { "D7",       REG_D7  },                      \
  { "D8",       REG_D8  },                      \
  { "D9",       REG_D9  },                      \
  { "D10",      REG_D10 },                      \
  { "D11",      REG_D11 },                      \
  { "D12",      REG_D12 },                      \
  { "D13",      REG_D13 },                      \
  { "D14",      REG_D14 },                      \
  { "D15",      REG_D15 },                      \
  { "A0",       REG_A0  },                      \
  { "A1",       REG_A1  },                      \
  { "A2",       REG_A2  },                      \
  { "A3",       REG_A3  },                      \
  { "A4",       REG_A4  },                      \
  { "A5",       REG_A5  },                      \
  { "A6",       REG_A6  },                      \
  { "A7",       REG_A7  },                      \
  { "A8",       REG_A8  },                      \
  { "A9",       REG_A9  },                      \
  { "A10",      REG_A10 },                      \
  { "a10",      REG_A10 },                      \
  { "A11",      REG_A11 },                      \
  { "A12",      REG_A12 },                      \
  { "A13",      REG_A13 },                      \
  { "A14",      REG_A14 },                      \
  { "A15",      REG_A15 }                       \
}

#undef  OVERLAPPING_REGISTER_NAMES
#define OVERLAPPING_REGISTER_NAMES                      \
  {                                                     \
    { "e0", REG_D0, 2 },                                \
    { "E0", REG_D0, 2 },                                \
    { "e2", REG_D2, 2 },                                \
    { "E2", REG_D2, 2 },                                \
    { "e4", REG_D4, 2 },                                \
    { "E4", REG_D4, 2 },                                \
    { "e6", REG_D6, 2 },                                \
    { "E6", REG_D6, 2 },                                \
    { "e8", REG_D8, 2 },                                \
    { "E8", REG_D8, 2 },                                \
    { "e10", REG_D10, 2 },                              \
    { "E10", REG_D10, 2 },                              \
    { "e12", REG_D12, 2 },                              \
    { "E12", REG_D12, 2 },                              \
    { "e14", REG_D14, 2 },                              \
    { "E14", REG_D14, 2 }                               \
  }


/***********************************************************************/
/* Register Classes */

enum reg_class
{
    NO_REGS,
    
    REGCLASS_D15,
    REGCLASS_A10,
    REGCLASS_A15,

    REGCLASS_ND15,
    REGCLASS_D,
    REGCLASS_NA10,
    REGCLASS_NA15,
    REGCLASS_A,
    REGCLASS_R,

    ALL_REGS,

    LIM_REG_CLASSES
};

#define GENERAL_REGS REGCLASS_R

#define N_REG_CLASSES ((int) LIM_REG_CLASSES)

#define REG_CLASS_NAMES                         \
  {                                             \
    "NO_REGS",                                  \
    "REGCLASS_D15",                             \
    "REGCLASS_A10",                             \
    "REGCLASS_A15",                             \
    "REGCLASS_D\\D15",                          \
    "REGCLASS_D",                               \
    "REGCLASS_A\\A10",                          \
    "REGCLASS_A\\A15",                          \
    "REGCLASS_A",                               \
    "REGCLASS_R",                               \
                                                \
    "ALL_REGS"                                  \
  }

#define REG_CLASS_CONTENTS                      \
  /*  31 - 0     63-32   */                     \
  {                                             \
    { 0,             0},  /* NO_REGS        */  \
                                                \
    { 1u << 15,      0},  /* REGCLASS_D15   */  \
    { 1u << (16+10), 0},  /* REGCLASS_A10   */  \
    { 1u << (16+15), 0},  /* REGCLASS_A15   */  \
                                                \
    { 0x00007fff,      0}, /* REGCLASS_ND15 */  \
    { 0x0000ffff,      0}, /* REGCLASS_D    */  \
    { 0xfbff0000,      0}, /* REGCLASS_NA10 */  \
    { 0x7fff0000,      0}, /* REGCLASS_NA15 */  \
    { 0xffff0000,      0}, /* REGCLASS_A    */  \
    { 0xffffffff,      0}, /* REGCLASS_R    */  \
                                                \
    { 0xffffffff,      3}  /* ALL_REGS      */  \
  }

#define REGNO_REG_CLASS(REGNO) tric_regno_reg_class(REGNO)

#define BASE_REG_CLASS   REGCLASS_A

#define INDEX_REG_CLASS  NO_REGS


#define REGNO_OK_FOR_BASE_P(X)                          \
  (((X) < FIRST_PSEUDO_REGISTER && A_REGNO_P (X))       \
   || A_REGNO_P (reg_renumber[X]))

#define REGNO_OK_FOR_INDEX_P(X) 0

#define CLASS_MAX_NREGS(CLASS, MODE)                    \
  (((MODE) == DImode || (MODE) == DFmode) ? 2 : 1)


/* Call instructions don't modify the stack pointer.  */
#define INCOMING_FRAME_SP_OFFSET 0

#define LEGITIMATE_MODE_FOR_AUTOINC_P(MODE) \
      (GET_MODE_SIZE (MODE) <= 4)

#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

#define MOVE_MAX UNITS_PER_WORD

/* Some headers only include tm.h but not tm_p.h.  Thus, provide
   prototypes for the following 3 functions here.  */

extern int tric_move_ratio (int);
extern int tric_clear_ratio (int);
extern int tric_set_ratio (int);

#define MOVE_RATIO(for_speed) tric_move_ratio (for_speed)

#define CLEAR_RATIO(for_speed) tric_clear_ratio (for_speed)

#define SET_RATIO(for_speed) tric_set_ratio (for_speed)

/***********************************************************************/
/* Storage Layout */

#define SLOW_BYTE_ACCESS  1
#define SLOW_SHORT_ACCESS 1

#define BITS_BIG_ENDIAN  0
#define BYTES_BIG_ENDIAN 0
#define WORDS_BIG_ENDIAN 0

#define BITS_PER_WORD 32

#define UNITS_PER_WORD 4

#define POINTER_SIZE 32

#define TRIC_PROMOTE_MODE_P(MODE)               \
  (GET_MODE_CLASS (MODE) == MODE_INT            \
   && GET_MODE_SIZE (MODE) < UNITS_PER_WORD)

#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)     \
  if (TRIC_PROMOTE_MODE_P(MODE))                \
    {                                           \
      if (MODE == QImode)                       \
        UNSIGNEDP = UNSIGNEDP;                  \
      else if (MODE == HImode)                  \
        UNSIGNEDP = UNSIGNEDP;                  \
      (MODE) = SImode;                          \
    }

#define POINTER_BOUNDARY 32

#define PARM_BOUNDARY 32

#define STACK_BOUNDARY 64

#define FUNCTION_BOUNDARY 16

#define BIGGEST_ALIGNMENT 32

#define EMPTY_FIELD_BOUNDARY BITS_PER_UNIT

#define STRICT_ALIGNMENT tric_opt_strict_alignment

#define ADJUST_FIELD_ALIGN(field, computed)             \
  tric_eabi_adjust_field_align (field, computed)

#define DATA_ALIGNMENT(type, basic_align)       \
  tric_eabi_data_alignment (type, basic_align)

#define ROUND_TYPE_ALIGN(type, computed, specified)             \
  tric_eabi_round_type_align (type, computed, specified)

/***********************************************************************/
/* Layout of Source Language Data Types */

#define SIZE_TYPE "long unsigned int"

#define DEFAULT_SIGNED_CHAR 1
#define FLOAT_TYPE_SIZE     BITS_PER_WORD
#define SHORT_TYPE_SIZE     16 
#define CHAR_TYPE_SIZE      8
#define INT_TYPE_SIZE       32
#define LONG_TYPE_SIZE      32
#define LONG_LONG_TYPE_SIZE 64 

#define DOUBLE_TYPE_SIZE        64
#define LONG_DOUBLE_TYPE_SIZE   64

/* Do not put function addr into constant pool */
#define NO_FUNCTION_CSE 1

#define BRANCH_COST(speed_p, predictable_p)                     \
  (tric_opt_branch_cost ? tric_opt_branch_cost : (speed_p) ? 25 : 4)

#define CASE_VECTOR_MODE SImode

#define JUMP_TABLES_IN_TEXT_SECTION 1

#define USE_SELECT_SECTION_FOR_FUNCTIONS 1

#define IS_ASM_LOGICAL_LINE_SEPARATOR(C, STR)   \
  ((C) == ';')

#define LPREFIX "L"

#define ASM_APP_ON  "#APP\n"
#define ASM_APP_OFF "#NO_APP\n"

#define GLOBAL_ASM_OP "\t.global\t"

#define SUPPORTS_WEAK 1

#define SUPPORTS_INIT_PRIORITY 0

#define TEXT_SECTION_ASM_OP    ".section .text,\"ax\",@progbits"

#define CTORS_SECTION_ASM_OP    ".section .ctors,\"a\",@progbits"
#define DTORS_SECTION_ASM_OP    ".section .dtors,\"a\",@progbits"

/* Machine dependent section flags
   "z"  absolute addressable
   "p"  PCP code
   "cn" code tied to core n */
#define TRIC_SECTION_ABSOLUTE (SECTION_MACH_DEP << 0)
#define TRIC_SECTION_PCP      (SECTION_MACH_DEP << 1)

/* Reserve 3 bits to encode the core ID */ 
#define TRIC_SECTION_CORE     (SECTION_MACH_DEP << 2)
#define TRIC_SECTION_CORE_MASK (0x7 * TRIC_SECTION_CORE)

#define TRIC_SECTION_CORE_SET(FLAGS,ID)                         \
  (((FLAGS) & ~TRIC_SECTION_CORE_MASK)                          \
   | (((ID) * TRIC_SECTION_CORE) & TRIC_SECTION_CORE_MASK))

#define TRIC_SECTION_CORE_GET(FLAGS)                            \
  (((FLAGS) & TRIC_SECTION_CORE_MASK) / TRIC_SECTION_CORE)

/* Mask all section flags which are not setable by an attribute or pragma
   section */
#define TRIC_SECTION_MASK                                       \
  (~(SECTION_MACH_DEP - 1) | SECTION_CODE | SECTION_WRITE)


#define FUNCTION_PROFILER(...) (void) 0

#define ASM_OUTPUT_LABEL(STREAM,NAME)           \
  do {                                          \
    assemble_name (STREAM, NAME);               \
    fputs (":\n", STREAM);                      \
  } while (0)

#define ADDR_VEC_ALIGN(NEXT) 2

#define ASM_OUTPUT_ADDR_VEC_ELT(STREAM, VALUE)          \
  fprintf (STREAM, "\t.code32\n\tj\t.%s%d\n", LPREFIX, VALUE)

#define ASM_OUTPUT_ALIGN(STREAM, POWER)         \
    fprintf (STREAM, "\t.align %d\n", POWER)

#define ASM_OUTPUT_ALIGNED_DECL_COMMON(STREAM, DECL, NAME, SIZE, ALIGN) \
    tric_asm_output_aligned_var ((STREAM), (DECL), (NAME), (SIZE), (ALIGN), 1)

#undef  ASM_OUTPUT_ALIGNED_DECL_LOCAL
#define ASM_OUTPUT_ALIGNED_DECL_LOCAL(STREAM, DECL, NAME, SIZE, ALIGN ) \
    tric_asm_output_aligned_var ((STREAM), (DECL), (NAME), (SIZE), (ALIGN), 0)

#define ASM_COMMENT_START "\t#"

#define ASM_WEAKEN_DECL(FILE, DECL, NAME, VAL)          \
  do                                                    \
    {                                                   \
      fputs ("\t.weak\t", (FILE));                      \
      assemble_name ((FILE), (NAME));                   \
      TREE_ASM_WRITTEN(DECL_ASSEMBLER_NAME(decl)) = 1;  \
      fputc ('\n', (FILE));                             \
      if (VAL)                                          \
        {                                               \
          ASM_OUTPUT_DEF ((FILE),(NAME),(VAL));         \
        }                                               \
    }                                                   \
  while (0)

#undef  ASM_OUTPUT_EXTERNAL
#define ASM_OUTPUT_EXTERNAL(FILE,DECL,NAME)     \
  tric_asm_output_external (FILE, DECL, NAME)

#undef  NO_PROFILE_COUNTERS
#define NO_PROFILE_COUNTERS 1

#define DBX_REGISTER_NUMBER(REGNO)  (REGNO) 

struct GTY(()) machine_function
{
  /* 'true' - if the current function is a leaf function.  */
  int is_leaf;
    
  /* 'true' - if current function is an interrupt function 
     as specified by the "interrupt" attribute.  */
  int is_interrupt;

  /* 'true' - if current function must not return via tail call.
     This is a wourkaround beacause calls.c:expand call does
     not provide targetm.function_ok_for_sibcall with
     CUMULATIVE_ARGS and recreation is too painful.
     A starting point may be what s390 is doing. */
  int sibcall_fails;

  /* 'true' - if this is a pxhndcall function and we emit a diagnose that
     the function attribute is not used correctly.  This is used to avoid
     that the same diagnose is printed twice.  */
  int bogus_pxhndcall;

  int anchor_completed;

  /* Fields holding data with -mcode-pic.  This will only be initialized as
     needed, e.g. if the current function needs dynamic address calculation
     for SYMBOL_REFs and/or LABEL_REFs for a function address or switch.  */
  struct
  {
    /* A pseudo register holding the dynamically calculated offset
       between the code position as determined at link time and the
       current code position at run time.  The current run time address
       of a symbol/label FOO can be represented as

           FOO_at_runtime = FOO_at_linktime + PIC_OFFSET.REG

       If the code was shifted to a highter address than told to the linker,
       REG will contain a positive value.  */
    rtx reg;

    /* SYMBOL_REF for ".PICOFF<n>" used in "load_pc" insn to get the
       current PC value.  */
    rtx symbol;
  } pic_offset;
};

struct GTY(()) tric_segment_trap
{
  /* Whether PRE_INC and similar PRE_MODIFY addressing modes are available. */
  int have_pre_inc;

  /* Whether PRE_DEC and similar PRE_MODIFY addressing modes are available. */
  int have_pre_dec;

  /* Whether -fivopts is on.  */
  int do_ivopts;

  /* Whether tree-ssa-loop-ivopts uses base costs for address.  */
  int do_ivopts_base_costs;

  /* Whether tree-ssa-loop-ivopts considers USE_ADDRESS kind of changes.  */
  int do_ivopts_use_address;

  /* Whether insn scheduler may consider swapping insns which will lead to
     more scheduling opportunities but also to different addressing.  */
  int do_sched_change_address;
};


/* Deliberately do /not/ define PUSH_ROUNDING because STACK_PUSH_CODE
   is set to PRE_DEC, and with -msegment-trap=no-pre-dec we won't have
   respective addressing mode.  With PUSH_ROUNDING not define'd, no
   pushes will be generated.  */
#undef PUSH_ROUNDING

#define TRIC_PIPEVARIANT_VALUE ((enum attr_pipevariant)tric_opt_pipevariant)

#define INIT_EXPANDERS tric_init_expanders()
