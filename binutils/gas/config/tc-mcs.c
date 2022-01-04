/* tc-mcs.c - RB GTM MCS assembler.
   Copyright 2011   Free Software Foundation, Inc.
   Contributed by Horst Lehser <Horst.Lehser@hightec-rt.com>

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with GAS; see the file COPYING.  If not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include <string.h>
#include <stdlib.h>

#include "as.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include "bfd.h"
#include "safe-ctype.h"
#include "opcodes/mcs-desc.h"
#include "opcodes/mcs-opc.h"
#include "cgen.h"
#include "elf/mcs.h"

typedef struct
{
  const CGEN_INSN *insn;
  const CGEN_INSN *orig_insn;
  CGEN_FIELDS fields;
#if CGEN_INT_INSN_P
  CGEN_INSN_INT buffer [1];
#define INSN_VALUE(buf) (*(buf))
#else
  unsigned char buffer[CGEN_MAX_INSN_SIZE];
#define INSN_VALUE(buf) (buf)
#endif
  char *addr;
  fragS *frag;
  int num_fixups;
  fixS *fixups[GAS_CGEN_MAX_FIXUPS];
  int indices[MAX_OPERAND_INSTANCES];
} mcs_insn;

extern int mcs_no_use_extended_regs;

/* Configuration options */


/* Target specific assembler tokens / delimiters.  */

const char comment_chars[]        = "#";
const char line_comment_chars[]   = "#";
const char line_separator_chars[] = ";";
const char EXP_CHARS[]            = "eE";
const char FLT_CHARS[]            = "dD";

/* Target specific assembly directives.  */

extern void s_lit24(int size);
extern void s_ulit24(int size);
const pseudo_typeS md_pseudo_table[] =
{
  { "align",   s_align_bytes,       0 },
  { "byte",    cons,                1 },
  { "hword",   cons,                2 },
  { "lit24",   s_lit24,             3 }, /* 24-bit aligned to 32-bit */
  { "ulit24",  s_ulit24,            3 }, /* 24-bit unaligned */
  { "word",    cons,                4 },
  { "dword",   cons,                8 },
  {(char *)0 , (void(*)(int))0,     0}
};

/* Target specific command line options.  */
#define OPTION_V1      (OPTION_MD_BASE + 0)
#define OPTION_V2      (OPTION_MD_BASE + 1)
#define OPTION_V3      (OPTION_MD_BASE + 2)
#define OPTION_V31     (OPTION_MD_BASE + 3)
#define OPTION_NOXREG  (OPTION_MD_BASE + 4)

const char * md_shortopts = "";

struct option md_longopts[] =
{
    { "mV1",    no_argument, NULL, OPTION_V1},
    { "mV2",    no_argument, NULL, OPTION_V2},
    { "mV3",    no_argument, NULL, OPTION_V3},
    { "mV31",    no_argument, NULL, OPTION_V31},
    { "mno-ext-regs", no_argument, NULL, OPTION_NOXREG},
    { NULL, 0, NULL, 0}
};
static int mach_bits = 0;

size_t md_longopts_size = sizeof (md_longopts);

/* Display architecture specific options.  */

void
md_show_usage (FILE * fp)
{
  fprintf (fp, _("MCS specific options:\n\
  -mV1          assemble for GTM MCS version 1.x\n\
  -mV2          assemble for GTM MCS version 2.x\n\
  -mV3          assemble for GTM MCS version 3.0\n\
  -mV31         assemble for GTM MCS version 3.1\n\
  -mno-ext-regs do not use extended register set from next channel\n\
"));
}

/* This is called back by cons () and aligns constants if required.  */
void
mcs_cons_align(int nbytes)
{
  int nalign;

  nalign = 0;
  while ((nbytes & 1) == 0)
    {
      ++nalign;
      nbytes >>= 1;
    }

  if (nalign == 0)
    return;

  if (now_seg == absolute_section)
    {
      if ((abs_section_offset & ((1 << nalign) - 1)) != 0)
        as_bad (_("Misaligned data in absolute section"));
      return;
    }

  frag_var (rs_align_code, 1, 1, (relax_substateT) 0,
  		(symbolS *) NULL, (offsetT) nalign, (char *) NULL);

  record_alignment (now_seg, nalign);
}
/* Parse command line options.  */

int
md_parse_option (int c, char * arg ATTRIBUTE_UNUSED)
{
  mach_bits |= (1 << MACH_BASE);
  switch (c)
    {
    case OPTION_V1:
      mach_bits |= (1 << MACH_GTMMCSV1);
      break;
    case OPTION_V2:
      mach_bits |= (1 << MACH_GTMMCSV2);
      break;
    case OPTION_V3:
      mach_bits |= (1 << MACH_GTMMCSV3);
      break;
    case OPTION_V31:
      mach_bits |= (1 << MACH_GTMMCSV31);
      break;
    case OPTION_NOXREG:
      mcs_no_use_extended_regs = 1;
      break;
    default:
      return 0;
    }
  return 1;
}

/* Do any architecture specific initialisation.  */

void
md_begin (void)
{
  /* always enable base mach */
  if (mach_bits == 0)
    {
      /* no specific architecture specified 
         take V1
      */
      mach_bits |= (1 << MACH_BASE);
      mach_bits |= (1 << MACH_GTMMCSV1);
    }
  /* Initialize the `cgen' interface.  */

  /* Set the machine number and endian.  */
  gas_cgen_cpu_desc = mcs_cgen_cpu_open (CGEN_CPU_OPEN_MACHS, mach_bits,
					   CGEN_CPU_OPEN_ENDIAN,
					   (target_big_endian)?
                                           CGEN_ENDIAN_BIG : CGEN_ENDIAN_LITTLE,
					   CGEN_CPU_OPEN_END);
  mcs_cgen_init_asm (gas_cgen_cpu_desc);

  /* This is a callback from cgen to gas to parse operands.  */
  cgen_set_parse_operand_fn (gas_cgen_cpu_desc, gas_cgen_parse_operand);
}

/* Turn an integer of n bytes (in val) into a stream of bytes appropriate
   for use in the a.out file, and stores them in the array pointed to by buf. */

void
md_number_to_chars (char * buf, valueT val, int n)
{
  if (target_big_endian)
    number_to_chars_bigendian (buf, val, n);
  else
    number_to_chars_littleendian (buf, val, n);
}

/* Turn a string in input_line_pointer into a floating point constant
   of type TYPE, and store the appropriate bytes in *LITP.  The number
   of LITTLENUMS emitted is stored in *SIZEP.  An error message is
   returned, or NULL on OK.  */

char *
md_atof (int type, char *litP, int *sizeP)
{
  int i;
  int prec;
  LITTLENUM_TYPE words[4];

  char *t;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;
    case 'd':
      prec = 4;
      break;
    default:
      *sizeP = 0;
      return _("bad call to md_atof");
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * sizeof (LITTLENUM_TYPE);

  if (target_big_endian)
    {
      for (i = 0; i < prec; i++)
	{
	  md_number_to_chars (litP, (valueT) words[i],
			      sizeof (LITTLENUM_TYPE));
	  litP += sizeof (LITTLENUM_TYPE);
	}
    }
  else
    {
      for (i = prec - 1; i >= 0; i--)
	{
	  md_number_to_chars (litP, (valueT) words[i],
			      sizeof (LITTLENUM_TYPE));
	  litP += sizeof (LITTLENUM_TYPE);
	}
    }

  return NULL;
}

/* Called for each undefined symbol. */

symbolS *
md_undefined_symbol (char * name ATTRIBUTE_UNUSED)
{
  return 0;
}

/* Round up a section size to the appropriate boundary.  */

valueT
md_section_align (asection *seg, valueT addr)
{
  int align = bfd_get_section_alignment (stdoutput, seg);
  return ((addr + (1 << align) - 1) & (-1 << align));
}

/* This function assembles the instructions. It emits the frags/bytes to the
   sections and creates the relocation entries.  */

void
md_assemble (char * str)
{
  mcs_insn insn;
  char * errmsg;

  /* Initialize GAS's cgen interface for a new instruction.  */
  gas_cgen_init_parse ();

  insn.insn = mcs_cgen_assemble_insn
      (gas_cgen_cpu_desc, str, &insn.fields, insn.buffer, &errmsg);

  if (!insn.insn)
    {
      as_bad ("%s", errmsg);
      return;
    }

  gas_cgen_finish_insn (insn.insn, insn.buffer,
			CGEN_FIELDS_BITSIZE (&insn.fields), 1, NULL);
}

/* Return the bfd reloc type for OPERAND of INSN at fixup FIXP.
   Returns BFD_RELOC_NONE if no reloc type can be found.
   *FIXP may be modified if desired.  */

bfd_reloc_code_real_type
md_cgen_lookup_reloc (const CGEN_INSN *insn ATTRIBUTE_UNUSED,
		      const CGEN_OPERAND *operand,
		      fixS *fixP ATTRIBUTE_UNUSED)
{
  switch (operand->type)
    {
    case MCS_OPERAND_UIMM14:
    case MCS_OPERAND_LIT14:
    case MCS_OPERAND_DISP14:
      return BFD_RELOC_MCS_14;
    case MCS_OPERAND_UIMM16:
    case MCS_OPERAND_LIT16:
    case MCS_OPERAND_DISP15:
    case MCS_OPERAND_DISP16:
      return BFD_RELOC_16;
    case MCS_OPERAND_UIMM24:
    case MCS_OPERAND_LIT24:
    case MCS_OPERAND_DISP24:
      return BFD_RELOC_MCS_24;
    case MCS_OPERAND_LB:
      return BFD_RELOC_MCS_4;
    case MCS_OPERAND_WXLB:
      return BFD_RELOC_MCS_WXLB;
    default:
      break;
    }
  return BFD_RELOC_NONE;
}

/* Return the position from which the PC relative adjustment for a PC relative
   fixup should be made.  */

long
md_pcrel_from (fixS *fixP)
{
  /* Shouldn't get called.  */
  abort ();
  /* Return address of current instruction.  */
  return fixP->fx_where + fixP->fx_frag->fr_address;
}

/* Relaxation isn't required/supported on this target.  */

int
md_estimate_size_before_relax (fragS *fragp ATTRIBUTE_UNUSED,
			       asection *seg ATTRIBUTE_UNUSED)
{
  abort ();
  return 0;
}

void
md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED,
		 asection *sec ATTRIBUTE_UNUSED,
		 fragS *fragP ATTRIBUTE_UNUSED)
{
  abort ();
}

void
md_apply_fix (fixS * fixP, valueT * valP, segT seg)
{
  gas_cgen_md_apply_fix (fixP, valP, seg);
  return;
}
