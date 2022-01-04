/* Common functionalities for HighTec backends.
   Copyright (C) 2012 Free Software Foundation, Inc.
   Contributed by Zoltán Ördög <zoltan.ordog@hightec-rt.com>

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
<http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "tm.h"
#include "target.h"
#include "output.h"
#include "c-family/c-pragma.h"
#include "c-family/c-common.h"
#include "cpplib.h"

/* last section set by pragma section */
extern htc_pragma_section_t htc_pragma_section;

/***********************************************************************
 ** Pragma implementation
 ***********************************************************************/

/* If RETURN is NULL then the section pragma ends, i.e. the complete line
   is "#pragma section" with no tokens thereafter.

   If RETURN is non-NULL, then
   - RETURN is a xmalloc'ed string that might serve as section name.
            There was no sanity check for reasonable section name yet.
   - *PTYPE is the type of the token after the section name.
   - *X     is the tree representing the token after the section name.  */

static char*
htc_pragma_scan_section_name (enum cpp_ttype *ptype, tree *x)
{
  char *secname;

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
      /* Old style:  Dreaded syntax tries to recover section name
         from various tokens like CPP_DOT and CPP_NAME.

         Presumably, the reason for this weird syntax was to try to be
         compatible with other compiler vendors.

         Specifying section names as identifiers/dots instead of
         as string is odd. E.g. this results in mappings like
             . foo     -> .foo
             foo .     -> foo
             foo.      -> foo
             foo. aw   -> foo.aw (instead of foo. with flags=aw)
             foo.5a    -> invalid suffix "a" on floating constant

         To fix this we would have to extend the lexer and allow dots in
         identifiers which is quite tedious and generally not wanted.
      */

      secname = xstrdup ("");

      do
        {
          if (*ptype == CPP_DOT)
            {
              secname = reconcat (secname, secname, ".", NULL);

              /* Assumption is that a section name does not end with '.'.
                 Thus, unconditionally scan the next token.  */

              *ptype = pragma_lex (x);
            }

          if (*ptype == CPP_NAME)
            {
              secname = reconcat (secname, secname,
                                  IDENTIFIER_POINTER (*x), NULL);
              *ptype = pragma_lex (x);
            }
        }
      while (*ptype == CPP_DOT);

      warning (OPT_Wdeprecated_pragma_section_name,
               "section name %qs is not a string", secname);
    }

  return secname;
}


/* Implement #pragma section

   The main outcome of this function is to set `htc_pragma_section.code'
   resp. `htc_pragma_section.data' which are used in
   `htc_insert_attributes' to attach section information to new decls.
   Formerly unknown sections are added to `htc_sections' by means of
   `htc_insert_section' which also performs additional sanity checks.  */

void
htc_handle_pragma_section (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  static htc_section_t *last_section_pragma = NULL;
  static location_t last_code_section_location = UNKNOWN_LOCATION;
  static location_t last_data_section_location = UNKNOWN_LOCATION;

  tree x = NULL_TREE;
  enum cpp_ttype type = CPP_EOF;
  char *secname;
  htc_section_t *secinfo;
  unsigned int align = 0;
  const char *s_flags = NULL;
  htc_secspec_t *sspec;

  secname = htc_pragma_scan_section_name (&type, &x);

  if (secname == NULL)
    {
      /* No section name specified: pragma section ends */

      if (htc_log.section)
        {
          if (last_section_pragma == NULL)
            {
              htc_edump ("%H: useless pragma section\n",
                         input_location);
            }
          else if (last_section_pragma->flags & SECTION_CODE)
            {
              htc_edump ("%H: closing pragma for code section %s"
                         " opened in %H\n",
                         input_location, last_section_pragma->name,
                         last_code_section_location);
            }
          else
            {
              htc_edump ("%H: closing pragma for data section %s"
                         " opened in %H\n",
                         input_location, last_section_pragma->name,
                         last_data_section_location);
            }
        }

      if (last_section_pragma == NULL)
        {
          /* pragma section ends without pragma section define */
          return;
        }

      if (last_section_pragma->flags & SECTION_CODE)
        {
          htc_pragma_section.code = NULL;
          last_code_section_location = UNKNOWN_LOCATION;
          last_section_pragma = htc_pragma_section.data;
        }
      else
        {
          htc_pragma_section.data = NULL;
          last_data_section_location = UNKNOWN_LOCATION;
          last_section_pragma = htc_pragma_section.code;
        }

      return;
    } /* secname == NULL */

  /* pragma section starts: scan arguments after section name */

  do
    {
      switch (type)
        {
        case CPP_EOF:
          break;

        case CPP_NUMBER:
          /* get the section alignment */

          if (align)
            {
              error ("section alignment specified more than once");
              free (secname);
              return;
            }

          align = TREE_INT_CST_LOW (x);

          if (exact_log2 (align) < 0)
            {
              error ("section alignment %<%d%> is not a power of 2", align);
              free (secname);
              return;
            }

          break;

        case CPP_NAME:
          /* get the section flags */

          if (s_flags)
            {
              error ("section flags specified more than once");
              free (secname);
              return;
            }

          s_flags = IDENTIFIER_POINTER (x);
          break;

        default:
          error ("unexpected token in pragma section");
          break;
        }
    }
  while (type != CPP_EOF
         && (type = pragma_lex (&x)) != CPP_EOF);

  sspec = htc_build_secspec (SECSPEC_pragma_htc_section,
                             secname, s_flags, align,
                             NULL_TREE, input_location);

  /* Find or create section named `secname' and check if everything
     is consistent. */

  secinfo = htc_insert_section (secname, -1u, align, sspec,
                                NULL_TREE /* unnamed */,
                                input_location);
  htc_set_htc_section (secinfo, NULL_TREE /* unnamed */, 0);

  /* secinfo now either points to a legal section named `secname'
     (known or newly created) or is NULL and an error message had been
     issued */

  if (NULL == secinfo)
    {
      /* htc_insert_section issued an error */

      free (secname);
      return;
    }

  if (secinfo->flags & SECTION_CODE)
    {
      if (htc_pragma_section.code != NULL)
        {
          /* Opening a code section within a code section in not allowed. */

          error ("missing pragma section end for code section %qs",
                 htc_pragma_section.code->name);
          inform (last_code_section_location,
                  "pragma section %qs started here",
                  htc_pragma_section.code->name);
        }

      htc_pragma_section.code = secinfo;
      last_code_section_location = input_location;
    }
  else
    {
      if (htc_pragma_section.data != NULL)
        {
          /* Opening a data section within a data section in not allowed. */

          error ("missing pragma section end for data section %qs",
                 htc_pragma_section.data->name);
          inform (last_data_section_location,
                  "pragma section %qs started here",
                  htc_pragma_section.data->name);
        }

      htc_pragma_section.data = secinfo;
      last_data_section_location = input_location;
    }

  last_section_pragma = secinfo;
}


/* Define HighTec specific language independent macros.

   This can be used in the TARGET_{CPU,OS,OBJFMT}_CPP_BUILTINS macros or
   integrate it into a backend's existing TARGET_{CPP,OS,OBJFMT}_CPP_BUILTINS
   macro definition.  */

void
htc_cpu_cpp_builtins (cpp_reader *pfile)
{
  cpp_define (pfile, "__HIGHTEC__");
  cpp_define (pfile, "__GNUC_AURIX__");

  if (tree_to_uhwi (TYPE_SIZE_UNIT (float_type_node)) 
      == tree_to_uhwi (TYPE_SIZE_UNIT (double_type_node)))
    cpp_define (pfile, "__HAVE_SHORT_DOUBLE__");
}
