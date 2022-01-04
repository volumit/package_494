# This shell script emits a C file. -*- C -*-
# Copyright (C) 2003 Free Software Foundation, Inc.
# Contributed by Michael Schumacher (mike@hightec-rt.com).
#
# This file is part of GLD, the Gnu Linker.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

# This file is sourced from elf32.em.  It is used for the TriCore port
# to support multiple "small data areas" and to sort the input sections
# within these, such that byte- and short-aligned data can be addressed
# with a 10-bit offset to the respective sda base pointer.

#
# Overridden emulation functions.
#
LDEMUL_AFTER_PARSE=SDA_${EMULATION_NAME}_after_parse
LDEMUL_BEFORE_ALLOCATION=SDA_${EMULATION_NAME}_before_allocation
LDEMUL_AFTER_ALLOCATION=SDA_${EMULATION_NAME}_after_allocation
LDEMUL_GET_CORE_NUMBER=htc_elf32_get_core_number
LDEMUL_SET_CORE_NUMBER=htc_elf32_set_core_number
LDEMUL_GET_CORE_NUMBER_FROM_NAME=htc_elf32_get_core_number_from_name
LDEMUL_SET_CORE_ALIAS=htc_elf32_set_core_alias
LDEMUL_GET_CORE_NAME=htc_elf32_get_core_name
LDEMUL_DEFINE_SDA_SECTION=tricore_elf32_define_sda_section
LDEMUL_ADD_EXPORTED_SYMBOL=htc_elf32_add_exported_symbol
LDEMUL_ADD_MEMORY_MAP=htc_elf32_add_memory_map
LDEMUL_FINISH=tricore_elf32_finish

#
# Additional options.
#
PARSE_AND_LIST_PROLOGUE='
#define OPTION_RELAX_BDATA		301
#define OPTION_RELAX_24REL		(OPTION_RELAX_BDATA + 1)
#define OPTION_DEBUG_RELAX		(OPTION_RELAX_24REL + 1)
#define OPTION_SORT_SDA			(OPTION_DEBUG_RELAX + 1)
#define OPTION_MULTI_SDAS		(OPTION_SORT_SDA + 1)
#define OPTION_PCP_MAP			(OPTION_MULTI_SDAS + 1)
#define OPTION_DEBUG_PCPMAP		(OPTION_PCP_MAP + 1)
#define OPTION_CHECK_SDATA		(OPTION_DEBUG_PCPMAP + 1)
#define OPTION_CORE_ARCH		(OPTION_CHECK_SDATA + 1)
#define OPTION_EXPORT_SYMS		(OPTION_CORE_ARCH + 1)
#define OPTION_CORE_NUMBER		(OPTION_EXPORT_SYMS + 1)
'
PARSE_AND_LIST_LONGOPTS='
  { "relax-bdata", no_argument, NULL, OPTION_RELAX_BDATA },
  { "relax-24rel", no_argument, NULL, OPTION_RELAX_24REL },
  { "debug-relax", no_argument, NULL, OPTION_DEBUG_RELAX },
#if 0
  { "sort-sda", no_argument, NULL, OPTION_SORT_SDA },
  { "multi-sdas", required_argument, NULL, OPTION_MULTI_SDAS },
#endif
  { "pcpmap", optional_argument, NULL, OPTION_PCP_MAP },
  { "debug-pcpmap", no_argument, NULL, OPTION_DEBUG_PCPMAP },
  { "check-sdata", no_argument, NULL, OPTION_CHECK_SDATA },
  { "mcpu", required_argument, NULL, OPTION_CORE_ARCH },
  { "export", required_argument, NULL, OPTION_EXPORT_SYMS },
  { "core", required_argument, NULL, OPTION_CORE_NUMBER },
'
PARSE_AND_LIST_OPTIONS='
  fprintf (file, _("\
  --relax-bdata         Compress bit objects contained in \".bdata\" input\n\
                          sections.  This option only takes effect in final\n\
			  (i.e., non-relocatable) link runs, and is also\n\
			  enabled implicitly by -relax.\n"
		   ));
  fprintf (file, _("\
  --relax-24rel         Relax call and jump instructions whose target address\n\
			  cannot be reached with a PC-relative offset, nor\n\
			  by switching to instructions using TriCore'\''s\n\
			  absolute addressing mode.  This option only takes\n\
			  effect in final (i.e., non-relocatable) link runs,\n\
			  and is also enabled implicitly by -relax.\n"
		   ));
#if 0
  fprintf (file, _("\
  --sort-sda            Sort the input sections of the \"small data area\"\n\
                          output section by alignment in order to minimize\n\
			  memory gaps and to give byte and short variables\n\
			  within these sections a better chance of being\n\
			  addressable via a 10-bit offset.\n"
		   ));
  fprintf (file, _("\
  --multi-sdas          Distribute the input sections of the \".sdata\"\n\
                          and \".sbss\" output sections to up to four\n\
			  such \"small data areas\" that must be present\n\
			  in the linker script.  This option also enables\n\
			  --sort-sda, which in this case performs sorting\n\
			  of all SDAs actually used.\n"
		   ));
#endif
  fprintf (file, _("\
  --pcpmap[=type]	Enable PCP/TriCore address mapping according to type.\n\
			  The following values may be given for type:\n\
			  0 or tc1796 - use TC1796-specific mapping\n\
			  If type is omitted, it defaults to 0.\n"
		  ));
  fprintf (file, _("\
  --check-sdata		Check whether small data are referenced as normal\n\
			  data (i.e., whether they are accessed without\n\
			  previously having been declared as small data).\n"
		  ));
  fprintf (file, _("\
  --mcpu=[core]		Link for TriCore architecture core.\n\
					core may be one of:\n\
			  tc12  - Core architecture V1.2\n\
			  tc13  - Core architecture V1.3\n\
			  tc131 - Core architecture V1.3.1\n\
			  tc16  - Core architecture V1.6\n\
			  tc161 - Core architecture V1.6.1\n\
			  tc162 - Core architecture V1.6.2\n\
			  Deprecated core values:\n\
			  tc16e - Core architecture V1.6E\n\
			  tc16p - Core architecture V1.6P\n\
			  tc27xx - Core architecture V1.6.1\n\
			  tc2d5d - Core architecture V1.6.1\n\
			  aurix - Core architecture aurix (V1.6.1/V1.6P/V1.6E)\n"
		));
  fprintf(file,_("\
  --export=file     define exported symbols\n"));
  fprintf(file,_("\
  --core=n     assign all sections and all symbols to core CPU0|CPU1|CPU2|GLOBAL or an alias\n"));
'
PARSE_AND_LIST_ARGS_CASES='
    case OPTION_RELAX_BDATA:
      tricore_elf32_relax_bdata = TRUE;
      break;

    case OPTION_RELAX_24REL:
      tricore_elf32_relax_24rel = TRUE;
      break;

    case OPTION_DEBUG_RELAX:
      tricore_elf32_debug_relax = TRUE;
      break;

#if 0
    case OPTION_SORT_SDA:
      sort_sda = TRUE;
      break;

    case OPTION_MULTI_SDAS:
      sort_sda = multi_sdas = TRUE;
      break;
#endif

    case OPTION_PCP_MAP:
      parse_pcpmap_args (optarg);
      break;

    case OPTION_DEBUG_PCPMAP:
      tricore_elf32_debug_pcpmap = TRUE;
      break;

    case OPTION_CHECK_SDATA:
      tricore_elf32_check_sdata = TRUE;
      break;
    case OPTION_CORE_ARCH:
      parse_arch_args(optarg);
      break;
    case OPTION_EXPORT_SYMS:
      parse_export_args(optarg);
      break;
    case OPTION_CORE_NUMBER:
      parse_core_number(optarg);
      break;
'

#
# The rest of this file implements the additional functionality in C.
#
cat >>e${EMULATION_NAME}.c <<EOF

#include "elf/tricore.h"

#if 0
/* Must be kept in sync with bfd/elf32-tricore.c.  */
typedef struct _memreg
{
  const char *name;
  bfd_vma start;
  bfd_size_type length;
  bfd_size_type used;
} memreg_t;
#endif

char *tricore_elf32_get_scriptdir PARAMS ((char *));
static void parse_pcpmap_args PARAMS ((char *));
static void parse_arch_args PARAMS ((char *));
static void parse_export_args PARAMS ((char *));
static void parse_core_number PARAMS ((char *));
static void SDA_${EMULATION_NAME}_after_parse PARAMS ((void));
static void SDA_${EMULATION_NAME}_before_allocation PARAMS ((void));
static void SDA_${EMULATION_NAME}_after_allocation PARAMS ((void));
#if 0
static memreg_t *get_memregs PARAMS ((int *));
static void tricore_elf32_sort_sdata PARAMS ((void));
static void tricore_elf32_shuffle_sdata PARAMS ((void));
static void tricore_elf32_find_sdas PARAMS ((lang_statement_union_type *));

static bfd_boolean sort_sda = FALSE;
static bfd_boolean multi_sdas = FALSE;
#endif

extern bfd_boolean tricore_elf32_relax_bdata;
extern bfd_boolean tricore_elf32_relax_24rel;
extern bfd_boolean tricore_elf32_debug_relax;
#if 0
extern FILE *tricore_elf32_map_file;
extern char *tricore_elf32_map_filename;
extern bfd_boolean tricore_elf32_extmap_enabled;
extern bfd_boolean tricore_elf32_extmap_header;
extern bfd_boolean tricore_elf32_extmap_memory_segments;
extern int tricore_elf32_extmap_syms_by_name;
extern int tricore_elf32_extmap_syms_by_addr;
extern char *tricore_elf32_extmap_ld_name;
extern void (*tricore_elf32_extmap_ld_version) PARAMS ((FILE *));
#endif
extern void tricore_elf32_list_bit_objects (struct bfd_link_info *, FILE *);
extern int tricore_elf32_pcpmap;
extern bfd_boolean tricore_elf32_debug_pcpmap;
extern bfd_boolean tricore_elf32_check_sdata;
extern unsigned long tricore_core_arch;
extern void htc_elf32_do_export_symbols(struct bfd_link_info *info);
extern void htc_elf32_set_core_number(unsigned int);
extern unsigned int htc_elf32_check_core_name(const char *);
extern unsigned int htc_elf32_get_core_number(void);
extern unsigned int htc_elf32_get_core_number_from_name(const char *);
extern void htc_elf32_set_core_alias(const char *,const char *);
extern const char *htc_elf32_get_core_name(void);
extern void htc_elf32_add_exported_symbol(const char *, bfd_boolean , bfd_size_type , const char *);
extern void htc_elf32_add_memory_map(unsigned int ,bfd_vma ,bfd_size_type , bfd_vma );
extern void tricore_elf32_define_sda_section(struct bfd_link_info *, bfd *,
                                    const char *,const char *,const char *);




static void
parse_arch_args (args)
     char *args;
{
	if ((args == NULL) || (*args == '\0'))
      einfo (_("%P%F: error: missing argument to --mcpu=\n"));
	
	if (!strcmp(args,"tc12"))
	{
		tricore_core_arch = EF_EABI_TRICORE_V1_2;
		return;
	}
	if (!strcmp(args,"tc13"))
	{
		tricore_core_arch = EF_EABI_TRICORE_V1_3;
		return;
	}
	if (!strcmp(args,"tc131"))
	{
		tricore_core_arch = EF_EABI_TRICORE_V1_3_1;
		return;
	}
	if (!strcmp(args,"tc16"))
	{
		tricore_core_arch = EF_EABI_TRICORE_V1_6;
		return;
	}
	if (!strcmp(args,"tc161"))
	{
		tricore_core_arch = EF_EABI_TRICORE_V1_6_1;
		return;
	}
	if (!strcmp(args,"tc16p")
	    || !strcmp(args,"tc16e")
	    || !strcmp(args,"tc27xx")
	    || !strcmp(args,"tc2d5d")
	    || !strcmp(args,"aurix"))
	{
		tricore_core_arch = EF_EABI_TRICORE_V1_6_1;
		einfo (_("%P: warning: deprecated value '%s' for option '--mcpu=', use 'tc161'\n"), args);
		return;
	}
	if (!strcmp(args,"tc162"))
	{
		tricore_core_arch = EF_EABI_TRICORE_V1_6_2;
		return;
	}
  	einfo (_("%P%F: error: invalid core architecture '%s' for option --mcpu=\n"), args);
}

/* This parses the sub-options to the --pcpmap option.  */

static void
parse_pcpmap_args (args)
     char *args;
{
  if (args == NULL)
    {
      tricore_elf32_pcpmap = 0;
      return;
    }

  if (!strcmp (args, "0") || !strcasecmp (args, "tc1796"))
    {
      tricore_elf32_pcpmap = 0;
      return;
    }

  einfo (_("%P%F: error: invalid type specifier for pcpmap\n"));
}

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


/* This is called after the linker script has been parsed.  Currently
   unused.  */

static void
SDA_${EMULATION_NAME}_after_parse ()
{
}

static void
tricore_elf32_finish (void)
{
  if (config.map_file)
     tricore_elf32_list_bit_objects (&link_info, config.map_file);

  finish_default ();
}
/* This is called before sections are allocated.  */

static void
SDA_${EMULATION_NAME}_before_allocation ()
{
  /* Call main function; we're just extending it.  */
  gld${EMULATION_NAME}_before_allocation ();

#if 0
  if (multi_sdas)
    tricore_elf32_shuffle_sdata ();
  if (sort_sda)
    tricore_elf32_sort_sdata ();

  if (multi_sdas || sort_sda)
    lang_for_each_statement (tricore_elf32_find_sdas);
#endif

  if (command_line.relax)
    tricore_elf32_relax_bdata = tricore_elf32_relax_24rel = TRUE;
  else if (tricore_elf32_relax_bdata
  	   || tricore_elf32_relax_24rel)
    command_line.relax = TRUE;

#if 0
  /* extended map now in bfd/elf32-extmap.c 
     option handling in lexsup.c and ldlang.c */
  if (config.map_file)
    {
      tricore_elf32_map_file = config.map_file;
      if (tricore_elf32_extmap_enabled)
        {
	  extern memreg_t *(*tricore_elf32_extmap_get_memregs) PARAMS ((int *));
#ifdef HTC_SUPPORT
	  tricore_elf32_extmap_ld_version = htc_print_ld_version;
#else
	  tricore_elf32_extmap_ld_version = print_ld_version;
#endif
	  tricore_elf32_extmap_get_memregs = get_memregs;
          tricore_elf32_map_filename = config.map_filename;
	}
    }
  else if (tricore_elf32_extmap_enabled)
    {
      einfo (_("%P: warning: ignoring --extmap option due to missing "
      	       "-M/-Map option\n"));
      tricore_elf32_extmap_enabled = FALSE;
    }
#endif

  /* GNU ld supports relaxing in a very general sense, meaning it's
     completely up to the backend to decide what changes it wants to
     apply to a given input section, and these decisions may or may
     not be based on preliminary symbol addresses which are assigned
     during section allocation.  However, the currently implemented
     section allocation algorithm fails if the following conditions
     are met:

	- relaxing is enabled (-relax)
	- there is at least one section that can be shrunk by relaxing
	- this input section is part of an output section which is 
	  associated with a particular memory region
	- this memory region has a limited capacity which is sufficient
	  if relaxing has shrunk one (or more) input sections, but is
	  otherwise too small

     Under these circumstances the linker will issue a fatal "region full"
     error message before the relax pass even had a chance to do its
     job.  This is because the function os_region_check() in ld/ldlang.c
     can't possibly know if it is called for the "final" output section,
     i.e., if it's clear that future relax passes won't shrink any of
     its input section any further, so its current size is already the
     final size.  This is a very unfortunate situation in case of TriCore
     bit sections, because relaxing them offers a compression ratio of
     up to 8:1, and the available physical target memory for such bits is
     extremely limited, so it can't be arbitrarily expanded to hold all
     uncompressed bit sections.  Fortunately, compressing bit sections
     does not depend on any section or symbol addresses, which allows us
     to perform the compression before sections are allocated.  Doing so
     presents the allocating process with the physically needed sizes of
     the bit sections instead of their uncompressed sizes.  */

  if (tricore_elf32_relax_bdata
      && !link_info.relocatable
      && !link_info.shared)
    {
      asection *isec;
      bfd *ibfd;
      bfd_boolean again;
      struct bfd_link_info *info = &link_info;

      for (ibfd = info->input_bfds; ibfd; ibfd = ibfd->link_next)
        for (isec = ibfd->sections; isec; isec = isec->next)
	  if (isec->rawsize > 1)
	    {
	      if (isec->size == 0)
	        isec->size = isec->rawsize;
	      if ((isec->rawsize == isec->size)
	          && (!strcmp (isec->name, ".bdata")
		      || !strncmp (isec->name, ".bdata.", 7)
		      || !strcmp (isec->name, ".bbss")
		      || !strncmp (isec->name, ".bbss.", 6)))
                if (!bfd_relax_section (ibfd, isec, info, &again))
		  xexit (1);
	    }
    }
}

#if 0
static bfd_boolean
tricore_elf32_check_sda (name)
     const char *name;
{
  lang_output_section_statement_type *os;
  lang_statement_union_type *u;

  if ((os = lang_output_section_find (name)) == NULL)
    return FALSE;

  for (u = os->children.head; u != NULL; u = u->header.next)
    {
      lang_statement_union_type *u2;
      lang_input_statement_type *is;

      if (u->header.type == lang_input_section_enum)
	{
	  is = &u2->input_statement;
	}
    }
}

static void
tricore_elf32_sort_sdata ()
{
}

static void
tricore_elf32_shuffle_sdata ()
{
}

static void
tricore_elf32_find_sdas (s)
     lang_statement_union_type *s;
{
  static bfd_boolean first = TRUE;

  if (first)
    {
      lang_output_section_statement_type *os;
      lang_statement_union_type *u;

      for (u = lang_output_section_statement.head; u != NULL; u = os->next)
        {
	  lang_statement_union_type *u2;
	  lang_input_statement_type *is;

	  os = &u->output_section_statement;
	  printf ("***%p %s\n", os, os->name);
	  if (!strcmp (os->name, ".sdata"))
	    for (u2 = os->children.head; u2; u2 = u2->header.next)
	      {
	        if (u2->header.type == lang_input_section_enum)
	          {
		    is = &u2->input_statement;
		    printf ("%p\n", is);
		  }
	        printf ("%d\n", u2->header.type);
	      }
	}
      first = FALSE;
    }

  if (s->header.type == lang_output_section_statement_enum)
    {
      lang_output_section_statement_type *os;

      os = &s->output_section_statement;
#if 0
      if (!strcmp (os->name, ".sdata1") && !os->bfd_section)
        init_os (os);
#endif
      printf ("%p output section name = <%s>, bfd_section = %p\n",
              os, os->name, os->bfd_section);
    }
}
#endif



EOF

if test x"${EMULATION_NAME}" = xelf32tricore; then
cat >>e${EMULATION_NAME}.c <<EOF

/* When configuring the binutils, the macro SCRIPTDIR will be set to
   \$tooldir/lib; the linker expects this to be the directory where
   the 'ldscript' directory resides.  This is okay as long as the
   tools are actually installed in the directory passed to (or
   defaulted by) the configure script, but may cause trouble otherwise.
   For example, if SCRIPTDIR points to a drive/directory that is only
   temporarily available, access() may either block or cause a considerable
   delay.  We therefore use the function below to determine the library
   directory path at runtime, starting with the path of the executable.  */

#include "filenames.h"

#if defined(HAVE_DOS_BASED_FILE_SYSTEM) && !defined(__CYGWIN__)
#define DOSFS
#define SEPSTR "\\\\"
#define realpath(rel, abs) _fullpath ((abs), (rel), MAXPATHLEN)
#else
#define SEPSTR "/"
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 2048
#endif

static char abs_progname[MAXPATHLEN + 1];

char *
tricore_elf32_get_scriptdir (progname)
     char *progname;
{
  char *beg, *cp, *cp2, *basedir, *libpath;
  struct stat statbuf;

  if (!realpath (progname, abs_progname))
    return (".");

  cp = basedir = abs_progname;
  cp2 = NULL;
#ifdef DOSFS
  if (ISALPHA (cp[0]) && (cp[1] == ':'))
    cp += 2;
#endif
  for (beg = cp; *cp; ++cp)
    if (IS_DIR_SEPARATOR (*cp))
      cp2 = cp;
  if (cp2 != NULL)
    *cp2 = '\0';
  else
    *beg = '\0';

  libpath = concat (basedir, SEPSTR, ".."SEPSTR"tricore"SEPSTR"lib", NULL);
  if ((stat (libpath, &statbuf) != 0) || !S_ISDIR (statbuf.st_mode))
    {
      free (libpath);
      libpath = concat (basedir, SEPSTR, ".."SEPSTR"lib", NULL);
    }
  strcpy (abs_progname, libpath);
  free (libpath);

  return abs_progname;
}

static void
SDA_${EMULATION_NAME}_after_allocation (void)
{
  htc_elf32_do_export_symbols(&link_info);
  gld${EMULATION_NAME}_after_allocation();
}


EOF
fi

