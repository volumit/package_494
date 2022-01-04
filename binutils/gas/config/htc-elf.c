/* htc-elf.c -- architecture independend HTC extensions
   Copyright 2010 Free Software Foundation, Inc.
   Written by Horst Lehser , HighTec EDV Systeme GmbH

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */


#include "as.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include "dw2gencfi.h"

#ifdef OBJ_ELF
#include "dwarf2dbg.h"
#endif
#include "config/htc-elf.h"
#include "htc/htc_support.h"

extern const pseudo_typeS md_pseudo_table[];

const pseudo_typeS htc_pseudo_table[] = {
  {"extern", htc_directive_extern,0},
  {NULL, 0, 0}
};

extern void pop_insert(const pseudo_typeS *);

void
htc_pop_insert(void)
{
  pop_insert(md_pseudo_table);
  pop_insert(htc_pseudo_table);
}

/* *AC and *AV are pointers to the assembler's argc and argv variables
   as passed to its main function.  Filter out any flags that cause the
   assembler's version to be printed (and handle them here).  Build and
   return a new instance of *{AC,AV} that doesn't contain these flags.  */

void
htc_check_gas_version_flags ( int *ac, char ***av)
{
  int i, j;
  char **nargv = NULL;

  for (i = 1; i < *ac; ++i)
    {
      if (!strcmp ((*av)[i], "-v") || !strcmp ((*av)[i], "-V"))
        {
	  htc_print_version_id (0);
rebuild_ac_av:
	  if (nargv == NULL)
	    {
	      xmalloc_set_program_name ((*av)[0]);
	      nargv = (char **) xmalloc (sizeof (char *) * (*ac));
	      for (j = 0; j < i; ++j)
	        nargv[j] = (*av)[j];
	      for (j = i; j < (*ac - 1); ++j)
	        nargv[j] = (*av)[j + 1];
	      nargv[*ac - 1] = NULL;
	      *av = nargv;
	    }
	  else
	    {
	      for (j = i; j < *ac; ++j)
	        nargv[j] = nargv[j + 1];
	    }
	  *ac = *ac - 1;
	  --i;
	}
      else if ((*(*av)[i] == '-') && (strlen ((*av)[i]) > 5)
      	       && !strncmp ((*av)[i], "--version", strlen ((*av)[i])))
	{
	  htc_print_version_id (1);
	  goto rebuild_ac_av;
	}
    }
}

/* Print the assembler's release and version numbers; if VERBOSE is non-zero,
   also tell that this particular version of the assembler is supported.  */

void
htc_print_version_id (int verbose)
{
  static int printed = 0;
  FILE *stream = verbose ? stdout : stderr;

  if (printed && !verbose)
    return;

  printed = 1;

#if defined(HTC_PRINT_TOOLVERSION)
  HTC_PRINT_TOOLVERSION("as");
#else
  fprintf (stream, _("GNU assembler %s (%s)"),
  	   BFD_VERSION_STRING, HTC_ARCH_NAME);
# ifdef TOOL_VERSION_GAS
  fprintf (stream, " %s %s", TOOL_VERSION, TOOL_VERSION_GAS);
# endif 
  fprintf(stream,_(" using BFD version %s\n"), BFD_BUILD_STRING);
#endif
  if (!verbose)
    return;

  fprintf (stream, HTC_COPYRIGHT);
  fprintf (stream, _("This assembler was configured for a target of `%s'.\n"),
  	   HTC_ARCH_NAME);
  exit (0);
}

/* handle the .extern directive like
   .extern symbol,type,size
   symbol is a symbol name
   type can be 
   - STT_OBJECT for a data declaration or
   - STT_FUNC  for a function declaration
   size is the size of this object in bytes
   function have always unkown size of 0 
*/
void
htc_directive_extern(int ignore ATTRIBUTE_UNUSED)
{
  char *name;
  char *typename;
  int endc;
  unsigned int type;
  symbolS *sym;
  valueT size = 0;
  elf_symbol_type *elfsym;
  int new_sym = 0;

  name = input_line_pointer;
  endc = get_symbol_end ();
  sym = symbol_find (name);

#if 1
  if (sym == NULL)
    {
      sym = symbol_make(name);
      if (sym == NULL)
        {
          as_bad(_("defining external symbol %s failed"),name);
          *input_line_pointer = endc;
          ignore_rest_of_line();
          return;
        }
      symbol_table_insert(sym);
      new_sym = 1;
    }
  *input_line_pointer = endc;

#else 
  if ((sym != NULL) && !bfd_is_und_section(symbol_get_bfdsym(sym)->section))
    {
      as_bad(_("external symbol %s is defined"), name);
      *input_line_pointer = endc;
      ignore_rest_of_line();
      return;
    }

  sym = symbol_make(name);
  if (sym == NULL)
    {
      as_bad(_("defining external symbol %s failed"),name);
      *input_line_pointer = endc;
      ignore_rest_of_line();
      return;
    }
  symbol_table_insert(sym);
  *input_line_pointer = endc;
#endif

  elfsym = (elf_symbol_type *)symbol_get_bfdsym(sym);
  SKIP_WHITESPACE();
  if (*input_line_pointer == ',')
    ++input_line_pointer;
  SKIP_WHITESPACE();
  if (*input_line_pointer == '#'
      || *input_line_pointer == '@'
      || *input_line_pointer == '%')
    ++input_line_pointer;

  typename = input_line_pointer;
  endc = get_symbol_end();

  type = BSF_GLOBAL;
  if ((strcmp(typename,"STT_OBJECT") == 0)
      || (strcmp(typename,"object") == 0))
    type |= BSF_OBJECT;
  else
  if ((strcmp(typename,"STT_FUNC") == 0)
      || (strcmp(typename,"function") == 0))
    type |= BSF_FUNCTION;
  else
    as_bad(_("unrecognized external symbol type \"%s\" must be STT_OBJECT/STT_FUNC"),typename);

  *input_line_pointer = endc;
  if (type & BSF_OBJECT)
    {
      SKIP_WHITESPACE();
      if (*input_line_pointer != ',')
        {
          as_bad(_("expect comma after symbol type in .extern directive"));
          ignore_rest_of_line();
          return;
        }
        ++input_line_pointer;
        size = get_absolute_expression ();
        demand_empty_rest_of_line();
    }
  else
    ignore_rest_of_line();

  if (new_sym || (elfsym->symbol.flags == 0))
    {
        elfsym->symbol.flags = type;
        S_SET_SIZE(sym,size);
    }
  else
    {
      /* check type and size */
      unsigned int sym_type = (elfsym->symbol.flags & (BSF_FUNCTION|BSF_OBJECT));

      if (sym_type && (sym_type != (type & (BSF_FUNCTION | BSF_OBJECT))))
        {
          as_bad(_("symbol %s already defined with different type 0x%x <-> 0x%x"),
                 elfsym->symbol.name,elfsym->symbol.flags,type);
          return;
        }
      else
        {
          elfsym->symbol.flags &= ~BSF_LOCAL;
          elfsym->symbol.flags |= type;
        }

      if (!(elfsym->symbol.flags & BSF_GLOBAL))
        {
          as_bad(_("symbol %s already defined with local scope type"),elfsym->symbol.name);
          return;
        }
      if (type & BSF_OBJECT)
        {
          if ((S_GET_SIZE(sym) != size) 
              && (S_GET_SIZE(sym) != 0)  
              && (S_GET_SIZE(sym) != -1u) ) 
            {
              as_bad(_("symbol already defined with different size %d <=> %d"),
                     (unsigned int)S_GET_SIZE(sym),(unsigned int)size);
              return;
            }
          else
            S_SET_SIZE(sym,size);
        }
    }
}
