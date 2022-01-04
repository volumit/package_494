/* Common sections related functionalities for HighTec backends.
   Copyright (C) 2015 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "hashtab.h"
#include "target.h"
#include "output.h"
#include "timevar.h"
#include "diagnostic-core.h"
#include "version.h"

/* Default for `TARGET_HTC_SECTION_FLAGS_FROM_STRING'.  */

unsigned long
default_htc_section_flags_from_string (const char* ARG_UNUSED (s_flags))
{
  gcc_unreachable();
  return -1UL;
}


bool
default_htc_section_flags_to_string (char *f, unsigned int flags,
                                     enum flagstring_purpose ARG_UNUSED (what))
{
#ifndef TLS_SECTION_ASM_FLAG
#define TLS_SECTION_ASM_FLAG 'T'
#endif

  if (!(flags & SECTION_DEBUG))
    *f++ = 'a';
  if (flags & SECTION_WRITE)
    *f++ = 'w';
  if (flags & SECTION_CODE)
    *f++ = 'x';
  if (flags & SECTION_SMALL)
    *f++ = 's';
  if (flags & SECTION_MERGE)
    *f++ = 'M';
  if (flags & SECTION_STRINGS)
    *f++ = 'S';
  if (flags & SECTION_TLS)
    *f++ = TLS_SECTION_ASM_FLAG;
  if (HAVE_COMDAT_GROUP && (flags & SECTION_LINKONCE))
    *f++ = 'G';
  *f = '\0';

  return true;
}



void
default_htc_asm_out_section (const char *name, unsigned int flags, tree decl)
{
  char s_flags[20];

  gcc_assert (0 == strchr (name, '?'));

  if (!(HAVE_COMDAT_GROUP && (flags & SECTION_LINKONCE))
      && (flags & SECTION_DECLARED))
    {
      fprintf (asm_out_file, ".section %s\n", name);
      return;
    }

  targetm.htc.section_flags_to_string (s_flags, flags,
                                       FLAGSTRING_for_asm_out);
  
  fprintf (asm_out_file, ".section %s,\"%s\"", name, s_flags);

  if (!(flags & SECTION_NOTYPE))
    {
      const char *type;
      const char *format;

      if (flags & SECTION_BSS)
        type = "nobits";
      else
        type = "progbits";

      format = ",@%s";
#ifdef ASM_COMMENT_START
      /* On platforms that use "@" as the assembly comment character,
         use "%" instead.  */
      if (strcmp (ASM_COMMENT_START, "@") == 0)
        format = ",%%%s";
#endif
      fprintf (asm_out_file, format, type);

      if (flags & SECTION_ENTSIZE)
        fprintf (asm_out_file, ",%d", flags & SECTION_ENTSIZE);

      if (HAVE_COMDAT_GROUP && (flags & SECTION_LINKONCE))
        {
          if (TREE_CODE (decl) == IDENTIFIER_NODE)
            fprintf (asm_out_file, ",%s,comdat", IDENTIFIER_POINTER (decl));
          else
            {
              fprintf (asm_out_file, ",%s,comdat",
                       IDENTIFIER_POINTER (DECL_COMDAT_GROUP (decl)));
            }
        }
    }

  putc ('\n', asm_out_file);
}

/* Original version is varasm.c:output_section_asm_op().  */

void
default_htc_unnamed_section_callback (const void *data)
{
  const htc_section_t *hsec = (const htc_section_t*) data;

  targetm.htc.asm_out_section (hsec->name, hsec->flags, hsec->named_decl);
}



void
htc_resolve_unique_section (tree decl, int reloc,
                            int ARG_UNUSED (flag_code_or_data_sections))
{
  if (verbose_flag)
    timevar_push (TV_HTC_SECTION_RESOLUTION);

  my_resolve_unique_section (decl, reloc, flag_code_or_data_sections);

  if (verbose_flag)
    timevar_pop (TV_HTC_SECTION_RESOLUTION);
}


/* A reasonable implementation of `TARGET_HTC_GET_NAMED_SECTION':
   If we have a HTC_ATTR_HTCSECTION attribute, then we also have a
   `htc_section_t' object which will provide all the information that
   we need.  If DECL is NULL or there is no HTC_ATTR_HTCSECTION, then
   just default to the original implementation of get_named_section().
   from varasm.c.  */

section*
htc_get_named_section (tree decl, const char *name, int reloc)
{
  htc_section_t *hsec;

  if (decl
      && CODE_CONTAINS_STRUCT (TREE_CODE (decl), TS_DECL_WITH_VIS)
      && (hsec = htc_decl_htc_section (decl)))
    {
      gcc_assert (hsec->sec);
      return hsec->sec;
    }

  return default_htc_get_named_section (decl, name, reloc);
}


static int
htc_post_pch_fixup_one_section (void **slot, void* ARG_UNUSED (info))
{
  htc_section_t *hsec = (htc_section_t*) *slot;

  gcc_assert (hsec);

  if (hsec->sec)
    {
      gcc_assert (SECTION_UNNAMED == SECTION_STYLE (hsec->sec));
      gcc_assert (hsec->sec->unnamed.callback
                  == targetm.htc.unnamed_section_callback);

      /* section.unnamed.data is GTY'ed "skip" and hence won't be fixed-up
         during deserialization.  As we want .data to hold an allocated object,
         we have to perform that fixup by hand.  */

      hsec->sec->unnamed.data = hsec;
      htc_stat.htc.section.n_pch_section_fixups++;
    }
  else
    {
      error_at (hsec->secspec->location, "section %qs defined in"
                " precompiled header", hsec->name);
      sorry ("precompiled headers + section features like %qs is not"
             " supported.  please contact %s if you actually need this.",
             htc_section_feature_descriptive (hsec->secspec->kind),
             bug_report_url);
    }

  return 1 /* continue traversal */;
}


void
htc_post_pch_load (void)
{
  htab_traverse_noresize (htc_section_htab,
                          htc_post_pch_fixup_one_section, NULL);
}


int
default_htc_dump_valist (FILE* ARG_UNUSED(stream), const char* ARG_UNUSED(fmt),
                         va_list ARG_UNUSED (ap))
{
  return 0;
}
