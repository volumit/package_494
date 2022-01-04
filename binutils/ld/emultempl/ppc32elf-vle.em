# This shell script emits a C file. -*- C -*-
#   Copyright 2001, 2002, 2003, 2005, 2007 Free Software Foundation, Inc.
#
# This file is part of the GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
# GNU General Public License for more details.
#    
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.
#    
     
# This file is sourced from elf32.em and used to define MMIX and ELF
# specific things.  First include what we have in common with mmo.

source_em ${srcdir}/emultempl/ppc32elf.em

LDEMUL_GET_CORE_NUMBER=htc_elf32_get_core_number
LDEMUL_ADD_EXPORTED_SYMBOL=htc_elf32_add_exported_symbol
LDEMUL_ADD_MEMORY_MAP=htc_elf32_add_memory_map
LDEMUL_GET_CORE_NUMBER=htc_elf32_get_core_number
LDEMUL_SET_CORE_NUMBER=htc_elf32_set_core_number
LDEMUL_GET_CORE_NUMBER_FROM_NAME=htc_elf32_get_core_number_from_name
LDEMUL_SET_CORE_ALIAS=htc_elf32_set_core_alias
LDEMUL_GET_CORE_NAME=htc_elf32_get_core_name

PARSE_AND_LIST_PROLOGUE=$PARSE_AND_LIST_PROLOGUE'
#define OPTION_CORE_NUMBER	    (OPTION_NO_STUBSYMS + 1)
#define OPTION_CORE_ARCH		(OPTION_CORE_NUMBER + 1)
#define OPTION_EXPORT_SYMS		(OPTION_CORE_ARCH + 1)
'
PARSE_AND_LIST_LONGOPTS=$PARSE_AND_LIST_LONGOPTS'
  { "core", required_argument, NULL, OPTION_CORE_NUMBER },
  { "mcpu", required_argument, NULL, OPTION_CORE_ARCH },
  { "export", required_argument, NULL, OPTION_EXPORT_SYMS },
'
PARSE_AND_LIST_ARGS_CASES=$PARSE_AND_LIST_ARGS_CASES'
    case OPTION_CORE_NUMBER:
      parse_core_number(optarg);
      break;
    case OPTION_CORE_ARCH:
      parse_core_arch(optarg);
      break;
    case OPTION_EXPORT_SYMS:
      parse_export_args(optarg);
      break;
'
PARSE_AND_LIST_OPTIONS=$PARSE_AND_LIST_OPTIONS'
  fprintf(file,_("\
  --export=FILE               Define exported symbols\n"));
  fprintf(file,_("\
  --core=N                    Assign all sections and all symbols to core CPU0|CPU1|CPU2|GLOBAL or an alias\n"));
  fprintf(file,_("\
  --mcpu=CORE                 Link for specified core architecture\n"));
'
fragment <<EOF

extern void htc_elf32_do_export_symbols(struct bfd_link_info *info);
extern void htc_elf32_add_exported_symbol(const char *, bfd_boolean , bfd_size_type , const char *);
extern void htc_elf32_add_memory_map(unsigned int ,bfd_vma ,bfd_size_type , bfd_vma );
extern void htc_elf32_set_core_number(unsigned int);
extern unsigned int htc_elf32_check_core_name(const char *);
extern unsigned int htc_elf32_get_core_number(void);
extern unsigned int htc_elf32_get_core_number_from_name(const char *);
extern void htc_elf32_set_core_alias(const char *,const char *);
extern const char *htc_elf32_get_core_name(void);
static int ppcvle_set_core_properties (char* args);


/* get the filename of the export file */

static void
parse_export_args(char *args)
{
  struct stat s;
  if ((args == NULL) || (*args == '\0'))
    einfo (_("%P%F: error: missing name of export file\n"));

  if (stat (args, &s) < 0
          || S_ISDIR(s.st_mode))
    {
      einfo (_("%P%F: error: cannot open export file %s\n"), args);
    }
  else 
    {
      lang_add_input_file(args,
            lang_input_file_is_symbols_only_enum,
            NULL);
    }

}
/* get core number */
static void
parse_core_number(char *args)
{
  int core_number;
//  const char *nptr;

  htc_elf32_set_core_number(0);
  if ((args == NULL) || (*args == '\0'))
    {
      einfo (_("%P%F: error: missing core number\n"));
      return;
    }
  core_number =  htc_elf32_check_core_name(args);
  if (core_number < 0)
  {
      einfo (_("%P%F: error: invalid core name %s\n"),args);
      return;
  }
  htc_elf32_set_core_number(core_number);
}

/* Set core arch */
static void
parse_core_arch(char *args)
{
  if (args == NULL)
    {
      einfo (_("%P%F: error: missing parameter for option -mcpu\n"));
      return;
    }
  if (ppcvle_set_core_properties(args))
    {
      einfo (_("%P%F: error: invalid core name for option -mcpu\n"));
      return;
    }
}


static void
ppcvle_before_parse (void)
{

  gld${EMULATION_NAME}_before_parse ();
  lang_default_entry ("${ENTRY}");
}

static void
ppcvle_finish (void)
{
  /* e_entry on PPC VLE points to the function descriptor for
     _start.  If _start is missing, default to the first function
     descriptor in the .text_vle section.  */
  entry_section = ".text_vle";

  finish_default ();
}

static void
ppcvle_after_allocation (void)
{
  htc_elf32_do_export_symbols(&link_info);
  gld${EMULATION_NAME}_after_allocation();
}

/* Processor table.  */
struct ppc_ptt
{
  const char *const name;		/* Canonical processor name.  */
  const unsigned int sat_version;	/* Highest supported version of saturation.  */
};

static struct ppc_ptt const processor_target_table[] =
{
#define DEF_CPU(NAME, SAT_VER) { NAME, SAT_VER },
#include "ppc-htc-cores.def"
#undef DEF_CPU
};

static int
ppcvle_set_core_properties (char* args)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (processor_target_table); i++)
    if (! strcmp (args, processor_target_table[i].name))
    {
      config.max_sat_version = (signed int)processor_target_table[i].sat_version;
      return 0;
    }
  return -1;
}

EOF

LDEMUL_AFTER_ALLOCATION=ppcvle_after_allocation
LDEMUL_BEFORE_PARSE=ppcvle_before_parse
LDEMUL_FINISH=ppcvle_finish
