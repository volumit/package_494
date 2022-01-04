/* Common sections related functionalities for HighTec backends.
   Copyright (C) 2015 Free Software Foundation, Inc.
   Contributed by Peter Gal <peter.gal@hightec-rt.com>

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

#ifndef HTC_COMMON_SECTIONS_H
#define HTC_COMMON_SECTIONS_H

#define HTC_ATTR_ASECTION "asection"

/* Data addressing modes.  */
enum data_addressing_mode
  {
    DAM_absolute,
    DAM_indirect,
    DAM_small,

    DAM_invalid,
    DAM_unknown
  };

enum secspec
  {
    SECSPEC_pragma_htc_section,
    SECSPEC_attribute_htc_asection,
    SECSPEC_attribute_gcc_section,
    /**/
    SECSPEC_htc_built_in,
    SECSPEC_gcc_built_in,
    SECSPEC_gcc_named_section,

    SECSPEC_unknown
  };

enum flagstring_purpose
  {
    FLAGSTRING_for_asm_out,
    FLAGSTRING_descriptive,
    FLAGSTRING_mimic_user_input
  };

#if !defined(IN_LIBGCC2) && !defined(IN_TARGET_LIBS)

#include "hashtab.h"

typedef struct GTY(()) htc_secspec
{
  /* IDENTIFIER_NODE or NULL_TREE */
  tree id;
  const char *name;
  const char *flags;
  const char *raw;
  int align;
  unsigned int /* location_t */ location;
  enum secspec kind;
} htc_secspec_t;

typedef struct GTY((chain_next ("%h.next"))) htc_section
{
  /* Section name as ggc_alloc'ed String.  */
  const char *name;

  /* Section alignment in bytes.  */
  unsigned int align;

  /* GCC's section representation.  */
  section *sec;
  
  /* This is a single-chained list:  Pointer to the next list member.  */
  struct htc_section *next;

  /* Section flags.  */
  unsigned int flags;

  /* Declaration attached to the section if it is a named section.  */
  tree named_decl;

  /* Location to the section definition in the source file or
     BUILTINS_LOCATION if it's a default section like .zbss.
     
     FIXME: We declare the location as 'unsigned int' to match the
        type definition location_t from input.h resp. source_location
        from line-map.h.  Including these files here would lead to all
        sorts of problems, so we use the shortcut 'unsigned int' instead.  */
  unsigned int /* location_t */ location;

  /* Not `const' as `htc_secspec_t' is GTY'ed.  */
  htc_secspec_t *secspec;
} htc_section_t;

typedef struct GTY(()) htc_pragma_section
{
    /* Last code section set by #pragma section.  */
    htc_section_t *code;

    /* Last data section set by #pragma section.  */
    htc_section_t *data;
} htc_pragma_section_t;

extern bool htc_valid_section_name_p (const char *name);

extern htc_secspec_t* htc_build_secspec (enum secspec, const char*, const char*, unsigned , tree, unsigned /* location_t */);
extern tree htc_handle_asection_attribute (tree*, tree, tree, int, bool*);

extern void htc_insert_htc_attributes (tree, tree*);
extern htc_section_t* htc_insert_section (const char*, unsigned, unsigned, htc_secspec_t*, tree, unsigned int);
extern void htc_resolve_unique_section (tree, int, int);
extern enum data_addressing_mode htc_decl_addressing (const_tree);
extern htc_section_t* htc_lookup_section (const char *name);
extern bool htc_decl_gcc_builtin_p (tree decl);
extern void my_resolve_unique_section (tree, int, int);
extern section* htc_get_named_section (tree, const char*, int);
extern hashval_t htc_htab_htc_section_hash (const void *entry);
extern int htc_htab_htc_section_eq (const void *entry, const void *name);
extern htab_t htc_section_htab;
extern const char* htc_section_feature_descriptive (enum secspec);
extern void htc_set_htc_section (htc_section_t*, tree, bool);
extern htc_section_t* htc_decl_htc_section (tree);

#endif /* IN_LIBGCC2 */


/* Probably used by config/<target>/<target>.c::handle_option. */
extern int htc_register_section_asm_name (const char *arg, int value);

/* Probably used by config/<target>/<target>.c::option_override.  */
extern void htc_check_section_asm_name_required_options (void);

extern tree htc_merge_decl_attributes (tree old_decl, tree new_decl);

/* Map object's assembler name according to KEY as specified by
   -section-asm-name <key>=<map>.  Return NULL or a newly xalloc'ed string.  */
extern const char* htc_section_asm_name (tree decl, const char *key);

#endif /* !defined(HTC_COMMON_SECTIONS_H) */
