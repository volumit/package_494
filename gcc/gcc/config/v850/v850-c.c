/* v850 specific, C compiler specific functions.
   Copyright (C) 2000-2014 Free Software Foundation, Inc.
   Contributed by Jeff Law (law@cygnus.com).

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "cpplib.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "c-family/c-pragma.h"
#include "diagnostic-core.h"
#include "ggc.h"
#include "tm_p.h"
#include "output.h"

#ifndef streq
#define streq(a,b) (strcmp (a, b) == 0)
#endif

static int  pop_data_area          (v850_data_area);
static int  push_data_area         (v850_data_area);
static void mark_current_function_as_interrupt (void);

/* Push a data area onto the stack.  */

static int
push_data_area (v850_data_area data_area)
{
  data_area_stack_element * elem;

  elem = (data_area_stack_element *) xmalloc (sizeof (* elem));

  if (elem == NULL)
    return 0;

  elem->prev      = data_area_stack;
  elem->data_area = data_area;

  data_area_stack = elem;

  return 1;
}

/* Remove a data area from the stack.  */

static int
pop_data_area (v850_data_area data_area)
{
  if (data_area_stack == NULL)
    warning (OPT_Wpragmas, "#pragma GHS endXXXX found without "
	     "previous startXXX");
  else if (data_area != data_area_stack->data_area)
    warning (OPT_Wpragmas, "#pragma GHS endXXX does not match "
	     "previous startXXX");
  else
    {
      data_area_stack_element * elem;

      elem = data_area_stack;
      data_area_stack = data_area_stack->prev;

      free (elem);

      return 1;
    }

  return 0;
}

/* Set the machine specific 'interrupt' attribute on the current function.  */

static void
mark_current_function_as_interrupt (void)
{
  tree name;
  
  if (current_function_decl ==  NULL_TREE)
    {
      warning (0, "cannot set interrupt attribute: no current function");
      return;
    }

  name = get_identifier ("interrupt");

  if (name == NULL_TREE || TREE_CODE (name) != IDENTIFIER_NODE)
    {
      warning (0, "cannot set interrupt attribute: no such identifier");
      return;
    }
  
  decl_attributes (&current_function_decl,
		   tree_cons (name, NULL_TREE, NULL_TREE), 0);
}


/* Support for GHS pragmata.  */

void
ghs_pragma_section (cpp_reader * pfile ATTRIBUTE_UNUSED)
{
  int repeat = 0;
  v850_used_pragma = GHS_PRAGMA;

  /* #pragma ghs section [name = alias [, name = alias [, ...]]] */
  do
    {
      tree x;
      enum cpp_ttype type;
      tree sect_ident;
      const char *sect, *alias;
      enum GHS_section_kind kind;
      
      type = pragma_lex (&x);
      
      if (type == CPP_EOF && !repeat)
	goto reset;
      else if (type == CPP_NAME)
	{
	  sect_ident = x;
	  sect = IDENTIFIER_POINTER (sect_ident);
	}
      else
	goto bad;
      repeat = 0;
      
      if (pragma_lex (&x) != CPP_EQ)
	goto bad;
      if (pragma_lex (&x) != CPP_NAME)
	goto bad;
      
      alias = IDENTIFIER_POINTER (x);
      
      type = pragma_lex (&x);
      if (type == CPP_COMMA)
	repeat = 1;
      else if (type != CPP_EOF)
	warning (OPT_Wpragmas, "junk at end of #pragma ghs section");
      
      if      (streq (sect, "data"))    kind = GHS_SECTION_KIND_DATA;
      else if (streq (sect, "text"))    kind = GHS_SECTION_KIND_TEXT;
      else if (streq (sect, "rodata"))  kind = GHS_SECTION_KIND_RODATA;
      else if (streq (sect, "const"))   kind = GHS_SECTION_KIND_RODATA;
      else if (streq (sect, "rosdata")) kind = GHS_SECTION_KIND_ROSDATA;
      else if (streq (sect, "rozdata")) kind = GHS_SECTION_KIND_ROZDATA;
      else if (streq (sect, "sdata"))   kind = GHS_SECTION_KIND_SDATA;
      else if (streq (sect, "tdata"))   kind = GHS_SECTION_KIND_TDATA;
      else if (streq (sect, "zdata"))   kind = GHS_SECTION_KIND_ZDATA;
      /* According to GHS beta documentation, the following should not be
	 allowed!  */
      else if (streq (sect, "bss"))     kind = GHS_SECTION_KIND_BSS;
      else if (streq (sect, "zbss"))    kind = GHS_SECTION_KIND_ZDATA;
      else
	{
	  warning (0, "unrecognized section name %qE", sect_ident);
	  return;
	}
      
      if (streq (alias, "default"))
	GHS_current_section_names [kind] = NULL;
      else
	GHS_current_section_names [kind] =
	  build_string (strlen (alias) + 1, alias);
    }
  while (repeat);

  return;

 bad:
  warning (OPT_Wpragmas, "malformed #pragma ghs section");
  return;

 reset:
  /* #pragma ghs section \n: Reset all section names back to their defaults.  */
  {
    int i;
    
    for (i = COUNT_OF_GHS_SECTION_KINDS; i--;)
      GHS_current_section_names [i] = NULL;
  }
}

void
ghs_pragma_interrupt (cpp_reader * pfile ATTRIBUTE_UNUSED)
{
  tree x;
  
  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of #pragma ghs interrupt");
  
  mark_current_function_as_interrupt ();
}

void
ghs_pragma_starttda (cpp_reader * pfile ATTRIBUTE_UNUSED)
{
  tree x;
  
  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of #pragma ghs starttda");
  
  push_data_area (DATA_AREA_TDA);
}

void
ghs_pragma_startsda (cpp_reader * pfile ATTRIBUTE_UNUSED)
{
  tree x;
  
  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of #pragma ghs startsda");
  
  push_data_area (DATA_AREA_SDA);
}

void
ghs_pragma_startzda (cpp_reader * pfile ATTRIBUTE_UNUSED)
{
  tree x;
  
  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of #pragma ghs startzda");
  
  push_data_area (DATA_AREA_ZDA);
}

void
ghs_pragma_endtda (cpp_reader * pfile ATTRIBUTE_UNUSED)
{
  tree x;
  
  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of #pragma ghs endtda");
  
  pop_data_area (DATA_AREA_TDA);
}

void
ghs_pragma_endsda (cpp_reader * pfile ATTRIBUTE_UNUSED)
{
  tree x;
  
  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of #pragma ghs endsda");
  
  pop_data_area (DATA_AREA_SDA);
}

void
ghs_pragma_endzda (cpp_reader * pfile ATTRIBUTE_UNUSED)
{
  tree x;
  
  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of #pragma ghs endzda");
  
  pop_data_area (DATA_AREA_ZDA);
}

/***********************************************************************
 ** Pragma implementation
 ***********************************************************************/

/* If RETURN is NULL then the section pragma ends, i.e. the complete line
   is "#pragma section" with no tokens thereafter.

   If RETURN is non-NULL, then
   - RETURN is a xmalloc'ed string that might serve as section name.
            There was no sanity check for reasonable section name yet.
   - *PTYPE is the type of the token after the section name.
   - *X     is the tree representing the token after the section name.

   If the code flag 'x' is set in flags the section assignment is only valid
   for functions. Otherwise, the assignment is only for data variables. */
static char*
v850_pragma_scan_section_name (enum cpp_ttype *ptype, tree *x)
{
  char *secname = NULL;

  *ptype = pragma_lex (x);

  if (*ptype == CPP_EOF)
    {
      /* pragma section ends: switch to default section */
      return NULL;
    }
  else if (*ptype == CPP_STRING)
    {
      /* New style: Specify section name as string.  */

      secname = xstrdup (TREE_STRING_POINTER (*x));
      *ptype = pragma_lex (x);
    }
  else
    {
      /* Old style pragma section ignored */
      warning (0, "incorrect format for #pragma section, ignoring");
    }

  return secname;
}


/* Function to handle the arguments for the #pragma section.

   If RETURN is true, then
   - s_flags  contains the flags for the section.
   - algin    contains the alignment for the section.
   - ptype    is advanced further by the  `pragma_lex` function.
*/
static bool
v850_pragma_section_arguments (enum cpp_ttype *ptype, tree *x, char **s_flags,  unsigned int *align)
{
  do
    {
      switch (*ptype)
        {
        case CPP_EOF:
          break;

        case CPP_NUMBER:
          /* get the section alignment */

          if (*align)
            {
              error ("section alignment specified more than once");
              return false;
            }

          *align = TREE_INT_CST_LOW (*x);

          if (exact_log2 (*align) < 0)
            {
              error ("section alignment %d is not a power of 2", *align);
              return false;
            }
          break;

        case CPP_NAME:
          /* get the section flags */

          if (*s_flags)
            {
              error ("section flags specified more than once");
              return false;
            }

          *s_flags = const_cast<char *>(IDENTIFIER_POINTER (*x));
          break;

        default:
          error ("unexpected token in pragma section");
          break;
        }
    }
  while (*ptype != CPP_EOF && (*ptype = pragma_lex (x)) != CPP_EOF);

  return true;
}


/* Implement #pragma section

   The main outcome of this function is to set `v850_pragma_section_code'
   resp. `v850_pragma_section_data' which are used in
   `v850_insert_attributes' to attach section information to new decls.
   Formerly unknown sections are added to `v850_sections' by means of
   `v850_insert_section' which also performs additional sanity checks.  */

void
v850_pragma_section (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  v850_used_pragma = HTC_PRAGMA;
  static v850_section_t *last_section_pragma = NULL;
  static location_t last_code_section_location = UNKNOWN_LOCATION;
  static location_t last_data_section_location = UNKNOWN_LOCATION;

  tree x = NULL_TREE;
  enum cpp_ttype type = CPP_EOF;
  char *secname;
  v850_section_t *secinfo;
  unsigned int align = 0;
  char *s_flags = NULL;

  secname = v850_pragma_scan_section_name (&type, &x);

  if (secname == NULL)
    {
      /* No section name specified: pragma section ends */
      if (last_section_pragma == NULL)
        {
          /* pragma section ends without pragma section define */
          return;
        }

      if (last_section_pragma->flags & SECTION_CODE)
        {
          /* last pragma section was a code section, switch back to it */
          v850_pragma_section_code = NULL;
          last_code_section_location = UNKNOWN_LOCATION;
          last_section_pragma = v850_pragma_section_data;
        }
      else
        {
          v850_pragma_section_data = NULL;
          last_data_section_location = UNKNOWN_LOCATION;
          last_section_pragma = v850_pragma_section_code;
        }

      return;
    } /* secname == NULL */

  /* pragma section starts: scan arguments after section name */
  if (!v850_pragma_section_arguments (&type, &x, &s_flags, &align))
    {
      free (secname);
      return;
    }

  /* Find or create section named `secname' and check if everything
     is consistent. */
  secinfo = v850_insert_section (secname, s_flags, align, input_location);

  /* secinfo now either points to a legal section named `secname'
     (known or newly created) or is NULL and an error message had been
     issued */
  if (NULL == secinfo)
    {
      /* arm_insert_section issued an error */
      free (secname);
      return;
    }

  if (secinfo->flags & SECTION_CODE)
    {
      if (v850_pragma_section_code != NULL)
        {
          /* Opening a code section within a code section in not allowed. */
          error ("missing pragma section end for code section %qs",
                 v850_pragma_section_code->name);
          inform (last_code_section_location, "pragma section %qs started here",
                  v850_pragma_section_code->name);
        }

      v850_pragma_section_code = secinfo;
      last_code_section_location = input_location;
    }
  else
    {
      if (v850_pragma_section_data != NULL)
        {
          /* Opening a data section within a data section in not allowed. */
          error ("missing pragma section end for data section %qs",
                 v850_pragma_section_data->name);
          inform (last_data_section_location, "pragma section %qs started here",
                  v850_pragma_section_data->name);
        }

      v850_pragma_section_data = secinfo;
      last_data_section_location = input_location;
    }

  last_section_pragma = secinfo;
}
