/* Assembler interface for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

   THIS FILE IS MACHINE GENERATED WITH CGEN.
   - the resultant file is machine generated, cgen-asm.in isn't

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2005, 2007, 2008
   Free Software Foundation, Inc.

   This file is part of libopcodes.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */


/* ??? Eventually more and more of this stuff can go to cpu-independent files.
   Keep that in mind.  */

#include "sysdep.h"
#include <stdio.h>
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "mcs-desc.h"
#include "mcs-opc.h"
#include "opintl.h"
#include "xregex.h"
#include "libiberty.h"
#include "safe-ctype.h"

#undef  min
#define min(a,b) ((a) < (b) ? (a) : (b))
#undef  max
#define max(a,b) ((a) > (b) ? (a) : (b))

static const char * parse_insn_normal
  (CGEN_CPU_DESC, const CGEN_INSN *, const char **, CGEN_FIELDS *);

/* -- assembler routines inserted here.  */

/* -- asm.c */

#include "elf/mep.h"

#define CGEN_VALIDATE_INSN_SUPPORTED
#define OPTION_MASK (MACH_GTMMCSV3 | MACH_BASE)

int mcs_no_use_extended_regs = 0;
static const char *
parse_wxlb (CGEN_CPU_DESC cd,
            const char **strp,
            int opindex,
            unsigned long *valuep)
{
  const char *errmsg;
  unsigned long value;

  errmsg = cgen_parse_unsigned_integer (cd, strp, opindex, & value);
  if (value > 23)
    errmsg = _("bit index out of range");
  else
    *valuep = value;

  return errmsg;
}

/* parse extended bit index 0..23 */

/* Handle unsigned 12 Bit literal.  */
static const char *
parse_lit12 (CGEN_CPU_DESC cd,
             const char **strp,
             int opindex,
             unsigned long *valuep)
{
  const char *errmsg;
  signed long value;

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, & value);
  if (errmsg == NULL)
    {
      unsigned long x = value & 0xFFFFF000;
      if (x != 0)
        errmsg = _("immediate 12-Bit value out of range");
      else
        *valuep = (value & 0xFFF);
    }
  return errmsg;
}

/* Handle unsigned 14 Bit literal.  */
static const char *
parse_lit14 (CGEN_CPU_DESC cd,
             const char **strp,
             int opindex,
             unsigned long *valuep)
{
  const char *errmsg;
  signed long value;

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, & value);
  if (errmsg == NULL)
    {
      unsigned long x = value & 0xFFFFc000;
      if (x != 0)
        errmsg = _("immediate 14-Bit value out of range");
      else
        *valuep = (value & 0x3FFF);
    }
  return errmsg;
}

/* Handle unsigned 16 Bit literal.  */

static const char *
parse_lit16 (CGEN_CPU_DESC cd,
             const char **strp,
             int opindex,
             unsigned long *valuep)
{
  const char *errmsg;
  signed long value;

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, & value);
  if (errmsg == NULL)
    {
      unsigned long x = value & 0xFFFF0000;
      if (x != 0)
        errmsg = _("immediate 16-Bit value out of range");
      else
        *valuep = (value & 0xFFFF);
    }
  return errmsg;
}

/* Handle unsigned 24 Bit literal.  */

static const char *
parse_lit24 (CGEN_CPU_DESC cd,
             const char **strp,
             int opindex,
             unsigned long *valuep)
{
  const char *errmsg;
  signed long value;

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, & value);
  if (errmsg == NULL)
    {
      unsigned long x = value & 0xFF000000;
      if (x != 0)
        errmsg = _("immediate 24-Bit value out of range");
      else
        *valuep = (value & 0xFFFFFF);
    }
  return errmsg;
}

/* handle the different displacements test for proper alignment */
static const char *
parse_sdisp12 (CGEN_CPU_DESC cd,
               const char **strp,
               int opindex,
               long *valuep)
{
  const char *errmsg;
  signed long value;

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, & value);
  if (value < -(1 << 11) || value > ((1 << 11) - 1))
    errmsg = _("signed 12-bit displacement out of bounds");
  else
    if (value & 3)
      errmsg = _("displacement must be a multiple of 4");
    else
      *valuep = value & ~3;
  return errmsg;
}
static const char *
parse_sdisp15 (CGEN_CPU_DESC cd,
               const char **strp,
               int opindex,
               long *valuep)
{
  const char *errmsg;
  signed long value;

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, & value);
  if (value < -(1 << 14) || value > ((1 << 14) - 1))
    errmsg = _("signed 15-bit displacement out of bounds");
  else
    if (value & 3)
      errmsg = _("displacement must be a multiple of 4");
    else
      *valuep = value & ~3;
  return errmsg;
}
static const char *
parse_disp12 (CGEN_CPU_DESC cd,
              const char **strp,
              int opindex,
              unsigned long *valuep)
{
  const char *errmsg = parse_lit12 (cd, strp, opindex, valuep);

  if (errmsg == NULL)
    {
      if (*valuep & 3)
        errmsg = _("displacement must be a multiple of 4");
    }
  return errmsg;
}
static const char *
parse_disp14 (CGEN_CPU_DESC cd,
              const char **strp,
              int opindex,
              unsigned long *valuep)
{
  const char *errmsg = parse_lit14 (cd, strp, opindex, valuep);

  if (errmsg == NULL)
    {
      if (*valuep & 0x3)
        errmsg = _("displacement must be a multiple of 4");
    }
  return errmsg;
}
static const char *
parse_disp15 (CGEN_CPU_DESC cd,
              const char **strp,
              int opindex,
              unsigned long *valuep)
{
  const char *errmsg = parse_lit16 (cd, strp, opindex, valuep);

  if (errmsg == NULL)
    {
      if (*valuep & 0x3)
        errmsg = _("displacement must be a multiple of 4");
    }
  return errmsg;
}
static const char *
parse_disp16 (CGEN_CPU_DESC cd,
              const char **strp,
              int opindex,
              unsigned long *valuep)
{
  const char *errmsg = parse_lit16 (cd, strp, opindex, valuep);

  if (errmsg == NULL)
    {
      if (*valuep & 0x3)
        errmsg = _("displacement must be a multiple of 4");
    }
  return errmsg;
}
static const char *
parse_disp24 (CGEN_CPU_DESC cd,
              const char **strp,
              int opindex,
              unsigned long *valuep)
{
  const char *errmsg = parse_lit24 (cd, strp, opindex, valuep);

  if (errmsg == NULL)
    {
      if (*valuep & 0x3)
        errmsg = _("displacement must be a multiple of 4");
    }
  return errmsg;
}

/*
 * use or not use extended register rs0 .. rs7
 */
static const char *
parse_xoreg ( CGEN_CPU_DESC cd,
              const char **strp,
              CGEN_KEYWORD *keyword_table,
              long *valuep)
{
  if (mcs_no_use_extended_regs)
    keyword_table = &mcs_cgen_opval_h_wxreg;
  return cgen_parse_keyword (cd, strp, keyword_table, valuep);
}

int
mcs_cgen_insn_supported (CGEN_CPU_DESC cd, const CGEN_INSN *insn)
{
  int machs = CGEN_INSN_ATTR_VALUE (insn, CGEN_INSN_MACH);
  int ok1;

  /* If the insn has an option bit set that we don't want,
     reject it.  */
  if (CGEN_INSN_ATTRS (insn)->bool & OPTION_MASK)
    return 0;

  /* If attributes are absent, assume no restriction. */
  if (machs == 0)
    machs = ~0;

  ok1 = (machs & cd->machs);

  return (ok1);
}

const char * mcs_cgen_parse_operand
  (CGEN_CPU_DESC, int, const char **, CGEN_FIELDS *);

/* Main entry point for operand parsing.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `parse_insn_normal', but keeping it
   separate makes clear the interface between `parse_insn_normal' and each of
   the handlers.  */

const char *
mcs_cgen_parse_operand (CGEN_CPU_DESC cd,
			   int opindex,
			   const char ** strp,
			   CGEN_FIELDS * fields)
{
  const char * errmsg = NULL;
  /* Used by scalar operands that still need to be parsed.  */
  long junk ATTRIBUTE_UNUSED;

  switch (opindex)
    {
    case MCS_OPERAND_DISP12 :
      errmsg = parse_disp12 (cd, strp, MCS_OPERAND_DISP12, (unsigned long *) (& fields->f_c12));
      break;
    case MCS_OPERAND_DISP14 :
      errmsg = parse_disp14 (cd, strp, MCS_OPERAND_DISP14, (unsigned long *) (& fields->f_c14));
      break;
    case MCS_OPERAND_DISP15 :
      errmsg = parse_disp15 (cd, strp, MCS_OPERAND_DISP15, (unsigned long *) (& fields->f_c15));
      break;
    case MCS_OPERAND_DISP16 :
      errmsg = parse_disp16 (cd, strp, MCS_OPERAND_DISP16, (unsigned long *) (& fields->f_c16));
      break;
    case MCS_OPERAND_DISP24 :
      errmsg = parse_disp24 (cd, strp, MCS_OPERAND_DISP24, (unsigned long *) (& fields->f_c24));
      break;
    case MCS_OPERAND_LB :
      errmsg = cgen_parse_unsigned_integer (cd, strp, MCS_OPERAND_LB, (unsigned long *) (& fields->f_opb_4));
      break;
    case MCS_OPERAND_LIT12 :
      errmsg = parse_lit12 (cd, strp, MCS_OPERAND_LIT12, (unsigned long *) (& fields->f_c12));
      break;
    case MCS_OPERAND_LIT14 :
      errmsg = parse_lit14 (cd, strp, MCS_OPERAND_LIT14, (unsigned long *) (& fields->f_c14));
      break;
    case MCS_OPERAND_LIT16 :
      errmsg = parse_lit16 (cd, strp, MCS_OPERAND_LIT16, (unsigned long *) (& fields->f_c16));
      break;
    case MCS_OPERAND_LIT24 :
      errmsg = parse_lit24 (cd, strp, MCS_OPERAND_LIT24, (unsigned long *) (& fields->f_c24));
      break;
    case MCS_OPERAND_RAA :
      errmsg = cgen_parse_keyword (cd, strp, & mcs_cgen_opval_h_areg, & fields->f_opa_4);
      break;
    case MCS_OPERAND_RAB :
      errmsg = cgen_parse_keyword (cd, strp, & mcs_cgen_opval_h_areg, & fields->f_opb_4);
      break;
    case MCS_OPERAND_RGA :
      errmsg = cgen_parse_keyword (cd, strp, & mcs_cgen_opval_h_greg, & fields->f_opa_4);
      break;
    case MCS_OPERAND_RGB :
      errmsg = cgen_parse_keyword (cd, strp, & mcs_cgen_opval_h_greg, & fields->f_opb_4);
      break;
    case MCS_OPERAND_ROA :
      errmsg = cgen_parse_keyword (cd, strp, & mcs_cgen_opval_h_oreg, & fields->f_opa_4);
      break;
    case MCS_OPERAND_ROB :
      errmsg = cgen_parse_keyword (cd, strp, & mcs_cgen_opval_h_oreg, & fields->f_opb_4);
      break;
    case MCS_OPERAND_RWXB :
      errmsg = cgen_parse_keyword (cd, strp, & mcs_cgen_opval_h_wxreg, & fields->f_opxb);
      break;
    case MCS_OPERAND_RXOA :
      errmsg = parse_xoreg (cd, strp, & mcs_cgen_opval_h_xoreg, & fields->f_opxa);
      break;
    case MCS_OPERAND_RXOB :
      errmsg = parse_xoreg (cd, strp, & mcs_cgen_opval_h_xoreg, & fields->f_opxb);
      break;
    case MCS_OPERAND_SDISP12 :
      errmsg = parse_sdisp12 (cd, strp, MCS_OPERAND_SDISP12, (long *) (& fields->f_simm12));
      break;
    case MCS_OPERAND_SDISP15 :
      errmsg = parse_sdisp15 (cd, strp, MCS_OPERAND_SDISP15, (long *) (& fields->f_simm15));
      break;
    case MCS_OPERAND_UIMM14 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, MCS_OPERAND_UIMM14, (unsigned long *) (& fields->f_c14));
      break;
    case MCS_OPERAND_UIMM15 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, MCS_OPERAND_UIMM15, (unsigned long *) (& fields->f_c15));
      break;
    case MCS_OPERAND_UIMM16 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, MCS_OPERAND_UIMM16, (unsigned long *) (& fields->f_c16));
      break;
    case MCS_OPERAND_UIMM24 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, MCS_OPERAND_UIMM24, (unsigned long *) (& fields->f_c24));
      break;
    case MCS_OPERAND_UIMM5 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, MCS_OPERAND_UIMM5, (unsigned long *) (& fields->f_c5));
      break;
    case MCS_OPERAND_UIMM9 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, MCS_OPERAND_UIMM9, (unsigned long *) (& fields->f_c9));
      break;
    case MCS_OPERAND_WXLB :
      errmsg = parse_wxlb (cd, strp, MCS_OPERAND_WXLB, (unsigned long *) (& fields->f_opxb));
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while parsing.\n"), opindex);
      abort ();
  }

  return errmsg;
}

cgen_parse_fn * const mcs_cgen_parse_handlers[] = 
{
  parse_insn_normal,
};

void
mcs_cgen_init_asm (CGEN_CPU_DESC cd)
{
  mcs_cgen_init_opcode_table (cd);
  mcs_cgen_init_ibld_table (cd);
  cd->parse_handlers = & mcs_cgen_parse_handlers[0];
  cd->parse_operand = mcs_cgen_parse_operand;
#ifdef CGEN_ASM_INIT_HOOK
CGEN_ASM_INIT_HOOK
#endif
}



/* Regex construction routine.

   This translates an opcode syntax string into a regex string,
   by replacing any non-character syntax element (such as an
   opcode) with the pattern '.*'

   It then compiles the regex and stores it in the opcode, for
   later use by mcs_cgen_assemble_insn

   Returns NULL for success, an error message for failure.  */

char * 
mcs_cgen_build_insn_regex (CGEN_INSN *insn)
{  
  CGEN_OPCODE *opc = (CGEN_OPCODE *) CGEN_INSN_OPCODE (insn);
  const char *mnem = CGEN_INSN_MNEMONIC (insn);
  char rxbuf[CGEN_MAX_RX_ELEMENTS];
  char *rx = rxbuf;
  const CGEN_SYNTAX_CHAR_TYPE *syn;
  int reg_err;

  syn = CGEN_SYNTAX_STRING (CGEN_OPCODE_SYNTAX (opc));

  /* Mnemonics come first in the syntax string.  */
  if (! CGEN_SYNTAX_MNEMONIC_P (* syn))
    return _("missing mnemonic in syntax string");
  ++syn;

  /* Generate a case sensitive regular expression that emulates case
     insensitive matching in the "C" locale.  We cannot generate a case
     insensitive regular expression because in Turkish locales, 'i' and 'I'
     are not equal modulo case conversion.  */

  /* Copy the literal mnemonic out of the insn.  */
  for (; *mnem; mnem++)
    {
      char c = *mnem;

      if (ISALPHA (c))
	{
	  *rx++ = '[';
	  *rx++ = TOLOWER (c);
	  *rx++ = TOUPPER (c);
	  *rx++ = ']';
	}
      else
	*rx++ = c;
    }

  /* Copy any remaining literals from the syntax string into the rx.  */
  for(; * syn != 0 && rx <= rxbuf + (CGEN_MAX_RX_ELEMENTS - 7 - 4); ++syn)
    {
      if (CGEN_SYNTAX_CHAR_P (* syn)) 
	{
	  char c = CGEN_SYNTAX_CHAR (* syn);

	  switch (c) 
	    {
	      /* Escape any regex metacharacters in the syntax.  */
	    case '.': case '[': case '\\': 
	    case '*': case '^': case '$': 

#ifdef CGEN_ESCAPE_EXTENDED_REGEX
	    case '?': case '{': case '}': 
	    case '(': case ')': case '*':
	    case '|': case '+': case ']':
#endif
	      *rx++ = '\\';
	      *rx++ = c;
	      break;

	    default:
	      if (ISALPHA (c))
		{
		  *rx++ = '[';
		  *rx++ = TOLOWER (c);
		  *rx++ = TOUPPER (c);
		  *rx++ = ']';
		}
	      else
		*rx++ = c;
	      break;
	    }
	}
      else
	{
	  /* Replace non-syntax fields with globs.  */
	  *rx++ = '.';
	  *rx++ = '*';
	}
    }

  /* Trailing whitespace ok.  */
  * rx++ = '['; 
  * rx++ = ' '; 
  * rx++ = '\t'; 
  * rx++ = ']'; 
  * rx++ = '*'; 

  /* But anchor it after that.  */
  * rx++ = '$'; 
  * rx = '\0';

  CGEN_INSN_RX (insn) = xmalloc (sizeof (regex_t));
  reg_err = regcomp ((regex_t *) CGEN_INSN_RX (insn), rxbuf, REG_NOSUB);

  if (reg_err == 0) 
    return NULL;
  else
    {
      static char msg[80];

      regerror (reg_err, (regex_t *) CGEN_INSN_RX (insn), msg, 80);
      regfree ((regex_t *) CGEN_INSN_RX (insn));
      free (CGEN_INSN_RX (insn));
      (CGEN_INSN_RX (insn)) = NULL;
      return msg;
    }
}


/* Default insn parser.

   The syntax string is scanned and operands are parsed and stored in FIELDS.
   Relocs are queued as we go via other callbacks.

   ??? Note that this is currently an all-or-nothing parser.  If we fail to
   parse the instruction, we return 0 and the caller will start over from
   the beginning.  Backtracking will be necessary in parsing subexpressions,
   but that can be handled there.  Not handling backtracking here may get
   expensive in the case of the m68k.  Deal with later.

   Returns NULL for success, an error message for failure.  */

static const char *
parse_insn_normal (CGEN_CPU_DESC cd,
		   const CGEN_INSN *insn,
		   const char **strp,
		   CGEN_FIELDS *fields)
{
  /* ??? Runtime added insns not handled yet.  */
  const CGEN_SYNTAX *syntax = CGEN_INSN_SYNTAX (insn);
  const char *str = *strp;
  const char *errmsg;
  const char *p;
  const CGEN_SYNTAX_CHAR_TYPE * syn;

  /* For now we assume the mnemonic is first (there are no leading operands).
     We can parse it without needing to set up operand parsing.
     GAS's input scrubber will ensure mnemonics are lowercase, but we may
     not be called from GAS.  */
  p = CGEN_INSN_MNEMONIC (insn);
  while (*p && TOLOWER (*p) == TOLOWER (*str))
    ++p, ++str;

  if (* p)
    return _("unrecognized instruction");

#ifndef CGEN_MNEMONIC_OPERANDS
  if (* str && ! ISSPACE (* str))
    return _("unrecognized instruction");
#endif

  CGEN_INIT_PARSE (cd);
  cgen_init_parse_operand (cd);

  /* We don't check for (*str != '\0') here because we want to parse
     any trailing fake arguments in the syntax string.  */
  syn = CGEN_SYNTAX_STRING (syntax);

  /* Mnemonics come first for now, ensure valid string.  */
  if (! CGEN_SYNTAX_MNEMONIC_P (* syn))
    abort ();

  ++syn;

  while (* syn != 0)
    {
      /* Non operand chars must match exactly.  */
      if (CGEN_SYNTAX_CHAR_P (* syn))
	{
	  /* FIXME: While we allow for non-GAS callers above, we assume the
	     first char after the mnemonic part is a space.  */
	  /* FIXME: We also take inappropriate advantage of the fact that
	     GAS's input scrubber will remove extraneous blanks.  */
	  if (TOLOWER (*str) == TOLOWER (CGEN_SYNTAX_CHAR (* syn)))
	    {
	      ++ syn;
	      ++ str;
	    }
	  else if (*str)
	    {
	      /* Syntax char didn't match.  Can't be this insn.  */
	      static char msg [80];

	      /* xgettext:c-format */
	      sprintf (msg, _("syntax error (expected char `%c', found `%c')"),
		       CGEN_SYNTAX_CHAR(*syn), *str);
	      return msg;
	    }
	  else
	    {
	      /* Ran out of input.  */
	      static char msg [80];

	      /* xgettext:c-format */
	      sprintf (msg, _("syntax error (expected char `%c', found end of instruction)"),
		       CGEN_SYNTAX_CHAR(*syn));
	      return msg;
	    }
	  continue;
	}

      /* We have an operand of some sort.  */
      errmsg = cd->parse_operand (cd, CGEN_SYNTAX_FIELD (*syn),
					  &str, fields);
      if (errmsg)
	return errmsg;

      /* Done with this operand, continue with next one.  */
      ++ syn;
    }

  /* If we're at the end of the syntax string, we're done.  */
  if (* syn == 0)
    {
      /* FIXME: For the moment we assume a valid `str' can only contain
	 blanks now.  IE: We needn't try again with a longer version of
	 the insn and it is assumed that longer versions of insns appear
	 before shorter ones (eg: lsr r2,r3,1 vs lsr r2,r3).  */
      while (ISSPACE (* str))
	++ str;

      if (* str != '\0')
	return _("junk at end of line"); /* FIXME: would like to include `str' */

      return NULL;
    }

  /* We couldn't parse it.  */
  return _("unrecognized instruction");
}

/* Main entry point.
   This routine is called for each instruction to be assembled.
   STR points to the insn to be assembled.
   We assume all necessary tables have been initialized.
   The assembled instruction, less any fixups, is stored in BUF.
   Remember that if CGEN_INT_INSN_P then BUF is an int and thus the value
   still needs to be converted to target byte order, otherwise BUF is an array
   of bytes in target byte order.
   The result is a pointer to the insn's entry in the opcode table,
   or NULL if an error occured (an error message will have already been
   printed).

   Note that when processing (non-alias) macro-insns,
   this function recurses.

   ??? It's possible to make this cpu-independent.
   One would have to deal with a few minor things.
   At this point in time doing so would be more of a curiosity than useful
   [for example this file isn't _that_ big], but keeping the possibility in
   mind helps keep the design clean.  */

const CGEN_INSN *
mcs_cgen_assemble_insn (CGEN_CPU_DESC cd,
			   const char *str,
			   CGEN_FIELDS *fields,
			   CGEN_INSN_BYTES_PTR buf,
			   char **errmsg)
{
  const char *start;
  CGEN_INSN_LIST *ilist;
  const char *parse_errmsg = NULL;
  const char *insert_errmsg = NULL;
  int recognized_mnemonic = 0;

  /* Skip leading white space.  */
  while (ISSPACE (* str))
    ++ str;

  /* The instructions are stored in hashed lists.
     Get the first in the list.  */
  ilist = CGEN_ASM_LOOKUP_INSN (cd, str);

  /* Keep looking until we find a match.  */
  start = str;
  for ( ; ilist != NULL ; ilist = CGEN_ASM_NEXT_INSN (ilist))
    {
      const CGEN_INSN *insn = ilist->insn;
      recognized_mnemonic = 1;

#ifdef CGEN_VALIDATE_INSN_SUPPORTED 
      /* Not usually needed as unsupported opcodes
	 shouldn't be in the hash lists.  */
      /* Is this insn supported by the selected cpu?  */
      if (! mcs_cgen_insn_supported (cd, insn))
	continue;
#endif
      /* If the RELAXED attribute is set, this is an insn that shouldn't be
	 chosen immediately.  Instead, it is used during assembler/linker
	 relaxation if possible.  */
      if (CGEN_INSN_ATTR_VALUE (insn, CGEN_INSN_RELAXED) != 0)
	continue;

      str = start;

      /* Skip this insn if str doesn't look right lexically.  */
      if (CGEN_INSN_RX (insn) != NULL &&
	  regexec ((regex_t *) CGEN_INSN_RX (insn), str, 0, NULL, 0) == REG_NOMATCH)
	continue;

      /* Allow parse/insert handlers to obtain length of insn.  */
      CGEN_FIELDS_BITSIZE (fields) = CGEN_INSN_BITSIZE (insn);

      parse_errmsg = CGEN_PARSE_FN (cd, insn) (cd, insn, & str, fields);
      if (parse_errmsg != NULL)
	continue;

      /* ??? 0 is passed for `pc'.  */
      insert_errmsg = CGEN_INSERT_FN (cd, insn) (cd, insn, fields, buf,
						 (bfd_vma) 0);
      if (insert_errmsg != NULL)
        continue;

      /* It is up to the caller to actually output the insn and any
         queued relocs.  */
      return insn;
    }

  {
    static char errbuf[150];
#ifdef CGEN_VERBOSE_ASSEMBLER_ERRORS
    const char *tmp_errmsg;

    /* If requesting verbose error messages, use insert_errmsg.
       Failing that, use parse_errmsg.  */
    tmp_errmsg = (insert_errmsg ? insert_errmsg :
		  parse_errmsg ? parse_errmsg :
		  recognized_mnemonic ?
		  _("unrecognized form of instruction") :
		  _("unrecognized instruction"));

    if (strlen (start) > 50)
      /* xgettext:c-format */
      sprintf (errbuf, "%s `%.50s...'", tmp_errmsg, start);
    else 
      /* xgettext:c-format */
      sprintf (errbuf, "%s `%.50s'", tmp_errmsg, start);
#else
    if (strlen (start) > 50)
      /* xgettext:c-format */
      sprintf (errbuf, _("bad instruction `%.50s...'"), start);
    else 
      /* xgettext:c-format */
      sprintf (errbuf, _("bad instruction `%.50s'"), start);
#endif
      
    *errmsg = errbuf;
    return NULL;
  }
}
