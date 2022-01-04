/* tc-tricore.h -- Header file for tc-tricore.c
   Copyright (C) 1998-2011 Free Software Foundation, Inc.
   Contributed by Michael Schumacher (mike@hightec-rt.com).
   Extended by Horst Lehser (Horst.Lehser@hightec-rt.com).

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Define target architecture.  */
#define TC_TRICORE

/* do a simple workaround for CPU ERRATA 48
  insert a nop before every calli, ji, jli
*/
#define SIMPLE_WORKAROUND_CPU48


#define TARGET_BYTES_BIG_ENDIAN 0
#define TARGET_ARCH bfd_arch_tricore
#define TARGET_FORMAT "elf32-tricore"
#define LISTING_HEADER "GAS Infineon TriCore Little Endian"

/* Work around a bug in read.c where it uses "#ifdef LISTING" instead
   of "#ifndef NO_LISTING".  */
#define LISTING !NO_LISTING

/* Don't try to break words.  */
#define WORKING_DOT_WORD

/* Turn "sym - ." expressions into PC-relative relocs.  */
#define DIFF_EXPR_OK

/* TriCore/PCP operands may have prefixes, but that's dealt with before
   expression() is called, and aside from prefixes, there's no need to
   treat operand expressions specially.  */
#define md_operand(x)

/* assign the core number to symbols */
extern void htc_elf32_adjust_symtab(void);
#define tc_adjust_symtab htc_elf32_adjust_symtab

/* TriCore/PCP is little endian.  */
#define md_number_to_chars number_to_chars_littleendian

/* .short/.word may or may not be auto-aligned.  */
#define md_cons_align(nbytes) tricore_cons_align (nbytes)
extern void tricore_cons_align PARAMS ((int));

/* handle symbol differences for PCP */
#define md_optimize_expr pcp_handle_symbol_diffs
/* the handled expression is symbol difference for PCP */
#define PCP_SYMBOL_DIFF		0x80

int pcp_handle_symbol_diffs
  PARAMS ((expressionS *, operatorT, expressionS *));

/* Handle PCP section flag and PCP section alignment.  */
#define md_elf_section_flags(f,a,t) tricore_elf_section_flags(f,a,t)
extern flagword tricore_elf_section_flags PARAMS ((flagword, int, int));
#define md_elf_section_letter(l,m) tricore_elf_section_letter(l,m)
extern int tricore_elf_section_letter PARAMS ((int, char **));
#define md_elf_section_change_hook tricore_elf_section_change_hook
extern void tricore_elf_section_change_hook PARAMS ((void));

/* Set machine flags in ELF header.  */
#define elf_tc_final_processing tricore_elf_final_processing
extern void tricore_elf_final_processing PARAMS ((void));

/* Handle relaxation of TriCore/PCP instructions.  */
#define TC_GENERIC_RELAX_TABLE md_relax_table
extern const struct relax_type md_relax_table[];
#define TC_HANDLES_FX_DONE
#define MD_PCREL_FROM_SECTION(f,s) md_pcrel_from_section (f, s)
extern long md_pcrel_from_section PARAMS ((struct fix *, segT));
#define md_prepare_relax_scan(fragP, address, aim, this_state, this_type) \
  do									  \
    {									  \
      if ((this_state == tricore_relax_loopu_state)			  \
          && (((aim < 0) && (aim < this_type->rlx_backward))		  \
              || ((aim >= 0) && (aim > this_type->rlx_forward))))	  \
        fragP->fr_subtype = this_type->rlx_more;			  \
      else if ((this_state == tricore_relax_loop_state) && (aim == -2))	  \
        aim = 0;							  \
    }									  \
  while (0)
extern relax_substateT tricore_relax_loop_state;
extern relax_substateT tricore_relax_loopu_state;

/* BIN-76
 * ensure that externally visible symbols are not overridden by
 * section+ offset.
 * We need this feature for vared
 */
#define EXTERN_FORCE_RELOC 1

/* Values passed to md_apply_fix don't include the symbol value.  */
# define MD_APPLY_SYM_VALUE(FIX) 		0

/* Handle relocations.  */
extern void tricore_sort_relocs PARAMS ((asection *, arelent **, unsigned int));
#define SET_SECTION_RELOCS(sec, relocs, n) tricore_sort_relocs (sec, relocs, n)

extern int tricore_fix_adjustable PARAMS ((struct fix *));

#define obj_fix_adjustable(fixP) tricore_fix_adjustable (fixP)

#define tc_fix_adjustable(fixP) \
  (!symbol_used_in_reloc_p (fixP->fx_addsy) && obj_fix_adjustable (fixP))

extern int tricore_force_relocation PARAMS ((struct fix *));
#define TC_FORCE_RELOCATION(fixP) tricore_force_relocation (fixP)
#define TC_RELOC_RTSYM_LOC_FIXUP(fixP)		\
  (((fixP)->fx_addsy == NULL)			\
   || (!S_IS_EXTERNAL ((fixP)->fx_addsy)	\
       && !S_IS_WEAK ((fixP)->fx_addsy)		\
       && S_IS_DEFINED ((fixP)->fx_addsy)	\
       && !S_IS_COMMON ((fixP)->fx_addsy)))

/* Minimum instruction length for DWARF2 line debug information entries.  */
#define DWARF2_LINE_MIN_INSN_LENGTH 2


/* Tell users that this version of GAS is supported by HighTec.  */
extern void htc_check_gas_version_flags PARAMS ((int *, char ***));
extern void htc_print_version_id PARAMS ((int));

#define HOST_SPECIAL_INIT(argc, argv) \
  htc_check_gas_version_flags (&argc, &argv)
#define HTC_GAS_VERSION VERSION

/* Enable extended listing support by setting EXT_LISTING to 1.  */
#ifndef NO_LISTING
#define EXT_LISTING 1
#endif /* !NO_LISTING  */

#if EXT_LISTING
extern void listing_notice PARAMS ((const char *));
extern char ext_listing_buf[];
extern fragS *ext_listing_frag;
#define EXT_LISTING_BUFLEN 2000
#define EXT_BUF ext_listing_buf, EXT_LISTING_BUFLEN
#define ASM_NOTICE() listing_notice (ext_listing_buf)
#define ASM_NOTICE_WORKAROUND(msg)	\
  do					\
    {					\
      if (show_internals)               \
        printf(" *** %s\n",msg);                    \
      if (listing & LISTING_LISTING)	\
        {				\
	  snprintf (EXT_BUF, msg);	\
	  ASM_NOTICE ();		\
	}				\
    }					\
  while (0)

#define TC_FRAG_TYPE	\
  struct ext_frag	\
    {			\
      char *notice;	\
      int notice_lines;	\
    }
#define FRAG_NOTICE(frag) (frag)->tc_frag_data.notice
#define FRAG_NOTICE_LINES(frag) (frag)->tc_frag_data.notice_lines
#define TC_FRAG_INIT(frag)			\
  do						\
    {						\
      FRAG_NOTICE (frag) = (char *) NULL;	\
      FRAG_NOTICE_LINES (frag) = 0;		\
    }						\
  while (0)
#else
#define ASM_NOTICE_WORKAROUND(msg)      \
  do                                    \
    {                                   \
      if (show_internals)               \
        printf("*** %s\n",msg);                    \
    }                                   \
  while (0)
    }

#endif /* EXT_LISTING  */

/* End of tc-tricore.h.  */
