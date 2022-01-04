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

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.
   If not see <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "tm.h"
#include "diagnostic-core.h"
#include "ggc.h"
#include "opts.h"
#include "options.h"
#include "hashtab.h"
#include "target.h"
#include "langhooks.h"
#include "output.h"
#include "timevar.h"
#include "stringpool.h"
#include "attribs.h"
#include "common/common-target.h"

static htc_stat_t * const hstat = &htc_stat;
 
#define TCODE_FOR_SECSPEC IDENTIFIER_NODE
#define TCODE_FOR_DAM     IDENTIFIER_NODE

extern GTY ((param_is (htc_section_t))) htab_t htc_section_htab;
htab_t htc_section_htab;

extern GTY(()) htc_pragma_section_t htc_pragma_section;
htc_pragma_section_t htc_pragma_section;

static bool
str_eq (const char *s1, const char *s2)
{
  return 0 == strcmp (s1, s2);
}


/* Hash table callbacks for htc_section_htab.  */

hashval_t
htc_htab_htc_section_hash (const void *entry)
{
  hstat->htc.section.lookup.n_hashes_computed++;
  return htab_hash_string (((const htc_section_t*) entry)->name);
}

int
htc_htab_htc_section_eq (const void *entry, const void *name)
{
  hstat->htc.section.lookup.n_compares++;
  return str_eq (((const htc_section_t*) entry)->name,
                 (const char*) name);
}

/* Undocumented, used internally only */

#define HTC_ATTR_SECSPEC     "@section-spec"
#define HTC_ATTR_ADDRESSMODE "@addressing.mode"
#define HTC_ATTR_HTCSECTION  "@htc-section"

static const char c_secspec_kind[] = { 'P', 'A', 'S', 'H', 'G', 'N', '?', 0 };

static const char c_dam_kind[] = { 'A', 'I', 'S', 0 };

static const char* ATTRIBUTE_UNUSED
decl_name (tree decl)
{
  const char *s_decl = IDENTIFIER_POINTER (DECL_NAME (decl));
  return targetm.strip_name_encoding (s_decl);
}

static enum secspec
htc_secspec_from_letter (char c)
{
  /* Map 'P' to SECSPEC_pragma_htc_section.
     Map 'A' to SECSPEC_attribute_htc_asection.
     Map 'S' to SECSPEC_attribute_gcc_section.
     Map 'H' to SECSPEC_htc_built_in.
     Map 'G' to SECSPEC_gcc_built_in.
     Map 'N' to SECSPEC_gcc_named_section.  */

  const char *p = strchr (c_secspec_kind, c);
  enum secspec kind = (enum secspec) (p - c_secspec_kind);

  gcc_assert ((unsigned) kind < (unsigned) SECSPEC_unknown);

  return kind;
}

static enum data_addressing_mode
htc_dam_from_letter (char c)
{
  /* Map 'A' to DAM_absolute.
     Map 'I' to DAM_indirect.
     Map 'S' to DAM_small.  */

  const char *p = strchr (c_dam_kind, c);
  enum data_addressing_mode kind = (enum data_addressing_mode)(p - c_dam_kind);

  gcc_assert ((unsigned) kind < (unsigned) DAM_invalid);

  return kind;
}

typedef struct
{
  enum data_addressing_mode dam;
  const char *s_dam;
  /* Raw string as in attribute `@...' produced by (de)serializing.  */
  const char *s_raw;
  const char *s_source;
  location_t location;
  /* IDENTIFIER_NODE */
  tree id;

  /* #pragma section
     attribute asection */
  struct
  {
    location_t location;
    const char *s_section;
    const char *s_flags;
  } section;
} htc_dam_t;


/* Return true if a variable declaration DECL has some initializers that
   force the associated object into section header of type SHT_PROGBITS,
   aka "@progbits".  */

static bool
htc_vardecl_progbits_p (tree decl)
{
  return (decl
          && VAR_DECL == TREE_CODE (decl)
          && TREE_STATIC (decl)
          && DECL_INITIAL (decl)
          && DECL_INITIAL (decl) != error_mark_node
          && !initializer_zerop (DECL_INITIAL (decl)));
}

static const char*
htc_consume_upto_sep (const char **pstr)
{
  const char *tok = NULL;
  const char *sep = strchr (*pstr, ':');
  gcc_assert (sep && sep[1] != '\0');
  if (sep - *pstr)
    tok = ggc_alloc_string (*pstr, sep - *pstr);
  *pstr = 1 + sep;
  return tok;
}

/* This is only for short-lived htc_dam_t objects during serialization.  */

static htc_dam_t*
htc_build_dam (tree decl, enum data_addressing_mode mode,
               const char *s_source, location_t loc,
               const char *s_section, const char *s_flags, location_t sec_loc)
{
  htc_dam_t *dam;

  if (mode >= DAM_invalid)
    return NULL;

  hstat->htc.section.n_dams++;
  dam = (htc_dam_t*) ggc_alloc_cleared_atomic (sizeof (htc_dam_t));

  dam->dam = mode;
  dam->id = (decl)? get_identifier(decl_name(decl)): NULL;
  dam->location = loc;

  switch (mode)
    {
    case DAM_indirect:   dam->s_dam = "indirect";  break;
    case DAM_absolute:   dam->s_dam = "absolute";  break;
    case DAM_small:      dam->s_dam = "small";     break;
    default:
      return dam;
    }

  if (s_flags && strchr (s_flags, ':'))
    s_flags = NULL;
  dam->section.location = sec_loc;
  dam->section.s_flags = s_flags;
  dam->s_source = s_source;

  if (sec_loc != UNKNOWN_LOCATION
      && !strchr (s_section, ':'))
    {
      dam->s_source = ACONCAT ((dam->s_source, " \"", s_section, "\"", NULL));
      dam->section.s_section = s_section;
    }

  dam->s_source = ggc_strdup (ACONCAT ((dam->s_source, "'", NULL)));

  return dam;
}

static size_t
htc_snprintf_dam (char *str, size_t len, const htc_dam_t *dam)
{
  const char *s_flags = dam->section.s_flags ? dam->section.s_flags : "";
  const char *s_source = dam->s_source;

  gcc_assert (dam->dam < DAM_invalid);

  return snprintf (str, len, "%c:%s:%s:%s:%d:%d",
                   c_dam_kind[dam->dam], s_flags, dam->s_dam, s_source,
                   (unsigned) dam->location,
                   (unsigned) dam->section.location);
}


static size_t
htc_snprintf_secspec (char *str, size_t len, const htc_secspec_t *ss)
{
  const char *name = ss->name ? ss->name : "";
  const char *flags = ss->flags ? ss->flags : "";

  gcc_assert (ss->kind < SECSPEC_unknown);

  return snprintf (str, len, "%s:%s:%d:%c:%u",
                   name, flags, ss->align,
                   c_secspec_kind[ss->kind], (unsigned) ss->location);
}


/* As TREE_LIST (decl, STRING_CST / ) */

static tree
htc_serialize_secspec (const htc_secspec_t *ss)
{
  char *str = NULL;
  size_t len, len2;

  len = htc_snprintf_secspec (NULL, 0, ss);
  str = (char*) alloca (1 + len);
  len2 = htc_snprintf_secspec (str, 1 + len, ss);
  gcc_assert (len2 == len);

  gcc_checking_assert (TCODE_FOR_SECSPEC == STRING_CST
                       || TCODE_FOR_SECSPEC == IDENTIFIER_NODE);

  return build_tree_list (ss->id,
                          TCODE_FOR_SECSPEC == STRING_CST
                          ? build_string (len, str)
                          : get_identifier (str));
}


/* TREE_LIST
   TREE_PURPOSE = dam->id
   TREE VALUE = rest.
   set DAM->s_raw */


static tree
htc_serialize_dam (htc_dam_t *dam)
{
  char *str = NULL;
  size_t len, len2;

  len = htc_snprintf_dam (NULL, 0, dam);
  str = (char*) alloca (1 + len);
  len2 = htc_snprintf_dam (str, 1 + len, dam);
  gcc_assert (len2 == len);
  dam->s_raw = ggc_strdup (str);
  gcc_checking_assert (TCODE_FOR_DAM == STRING_CST
                       || TCODE_FOR_DAM == IDENTIFIER_NODE);
  return build_tree_list (dam->id, TCODE_FOR_DAM == STRING_CST
                          ? build_string (len, str)
                          : get_identifier (str));
}


static htc_dam_t*
htc_deserialize_dam (tree decl)
{
  unsigned loc, sec_loc;
  htc_dam_t *dam;
  const char *s_arg;
  tree attr, arg;

  if (!decl
      || VAR_DECL != TREE_CODE (decl)
      || ! (attr = lookup_attribute (HTC_ATTR_ADDRESSMODE,
                                     DECL_ATTRIBUTES (decl))))
    return NULL;

  arg = TREE_VALUE (attr);
  gcc_assert (arg
              && TREE_PURPOSE (arg)
              && IDENTIFIER_NODE == TREE_CODE (TREE_PURPOSE (arg))
              && TCODE_FOR_DAM == TREE_CODE (TREE_VALUE (arg)));

  gcc_checking_assert (TCODE_FOR_DAM == STRING_CST
                       || TCODE_FOR_DAM == IDENTIFIER_NODE);
  s_arg = TCODE_FOR_DAM == STRING_CST
    ? TREE_STRING_POINTER (TREE_VALUE (arg))
    : IDENTIFIER_POINTER (TREE_VALUE (arg));

  hstat->htc.section.n_dams++;
  hstat->htc.section.n_dams_deserialized++;
  dam = (htc_dam_t*) ggc_alloc_cleared_atomic (sizeof (htc_dam_t));

  dam->s_raw = s_arg;
  dam->id = TREE_PURPOSE (arg);
  dam->dam = htc_dam_from_letter (s_arg[0]);
  gcc_assert (s_arg[1] == ':');
  s_arg += 2;
  dam->section.s_flags = htc_consume_upto_sep (&s_arg);
  dam->s_dam = htc_consume_upto_sep (&s_arg);
  dam->s_source = htc_consume_upto_sep (&s_arg);
  sscanf (s_arg, "%u:%u", & loc, &sec_loc);
  dam->location = (location_t) loc;
  dam->section.location = (location_t) sec_loc;

  return dam;
}


static const char*
htc_secspec_raw (tree spec)
{
  gcc_assert (TREE_LIST == TREE_CODE (spec)
              && (!TREE_PURPOSE (spec)
                  || (IDENTIFIER_NODE == TREE_CODE (TREE_PURPOSE (spec))))
              && TCODE_FOR_SECSPEC == TREE_CODE (TREE_VALUE (spec)));
  return ggc_strdup (TCODE_FOR_SECSPEC == STRING_CST
                     ? TREE_STRING_POINTER (TREE_VALUE (spec))
                     : IDENTIFIER_POINTER (TREE_VALUE (spec)));
}


/* T is TREE_LIST or NULL_TREE.  */

static htc_secspec_t*
htc_deserialize_secspec (tree spec)
{
  char c;
  htc_secspec_t *ss;
  const char *str;

  hstat->htc.section.n_secspecs++;
  hstat->htc.section.n_secspecs_deserialized++;
  ss = (htc_secspec_t*) ggc_alloc_atomic (sizeof (htc_secspec_t));

  ss->raw = str = htc_secspec_raw (spec);
  ss->name = htc_consume_upto_sep (&str);
  ss->flags = htc_consume_upto_sep (&str);
  ss->id = TREE_PURPOSE (spec);

  sscanf (str, "%d:%c:%u", & ss->align, & c, &ss->location);

  ss->kind = htc_secspec_from_letter (c);

  return ss;
}


const char*
htc_section_feature_descriptive (enum secspec kind)
{
  switch (kind)
    {
    default:
      return "invalid";

    case SECSPEC_attribute_gcc_section:
      return "attribute section";

    case SECSPEC_pragma_htc_section:
      return "#pragma section";

    case SECSPEC_attribute_htc_asection:
      return "attribute asection";
    }

  gcc_unreachable();
  return NULL;
}

/* Constructor-like function.  */

htc_secspec_t*
htc_build_secspec (enum secspec kind, const char *s_name,
                   const char *s_flags, unsigned align, tree decl,
                   unsigned /* location_t */ location)
{
  tree tspec;
  htc_secspec_t *ss;

  gcc_assert (kind != SECSPEC_unknown);

  hstat->htc.section.n_secspecs++;
  ss = (htc_secspec_t*) ggc_alloc_cleared_atomic (sizeof (htc_secspec_t));

  ss->name = ggc_strdup (s_name);
  ss->flags = s_flags ? ggc_strdup (s_flags) : NULL;
  ss->align = (int) align;

  ss->id = (decl)? get_identifier(decl_name(decl)): NULL;
  ss->location = location;
  ss->kind = kind;

  tspec = htc_serialize_secspec (ss);

  gcc_assert (TREE_LIST == TREE_CODE (tspec)
              && kind < SECSPEC_unknown);

  ss->raw = ggc_strdup (TCODE_FOR_SECSPEC == STRING_CST
                        ? TREE_STRING_POINTER (TREE_VALUE (tspec))
                        : IDENTIFIER_POINTER (TREE_VALUE (tspec)));
  return ss;
}

/* Return TREE_LIST deserialized from DECL's attribute secspec
   or NULL.  */

static tree
htc_get_secspec (tree decl)
{
  tree attr;

  gcc_assert (DECL_P (decl));

  attr = lookup_attribute (HTC_ATTR_SECSPEC, DECL_ATTRIBUTES (decl));

  return attr ? TREE_VALUE (attr) : NULL_TREE;
}


static htc_secspec_t*
htc_get_deserialize_secspec (tree decl)
{
  tree spec = htc_get_secspec (decl);
  return spec ? htc_deserialize_secspec (spec) : NULL;
}


/* 2 * (DECL_P, TREE_LIST (DECL_P, STRING_CST)) */

static tree
htc_merge_secspec (tree decl1, tree tspec1,
                   tree decl2, tree tspec2)
{
  bool may_use1 = true;
  bool may_use2 = true;
  unsigned flags1 = 0;
  unsigned flags2 = 0;
  const htc_secspec_t *ss1, *ss2;

  if (!tspec1)
    return tspec2;

  if (!tspec2)
    return tspec1;

  if (tspec1 == tspec2)
    return tspec1;

  ss1 = htc_deserialize_secspec (tspec1);
  ss2 = htc_deserialize_secspec (tspec2);

  if (ss1->kind != ss2->kind)
    {
      /* Specified more than one kind aka. "section feature" of a secspec
         is undefined behaviour. */

      if (warning (OPT_Wsection_feature, "%q+D uses section feature %qs",
                   decl2, htc_section_feature_descriptive (ss2->kind)))
        {
          inform (ss1->location, "%qD already uses section feature %qs"
                  " from here", decl1,
                  htc_section_feature_descriptive (ss1->kind));
        }
    }

  if (!str_eq (ss1->name, ss2->name))
    {
      error ("%q+D uses conflicting section name %qs", decl2, ss2->name);
      inform (ss1->location, "%q+D used section name %qs from here",
              decl1, ss1->name);
      return NULL;
    }

  if (ss1->flags && ss2->flags)
    {
      flags1 = targetm.htc.section_flags_from_string (ss1->flags);
      flags2 = targetm.htc.section_flags_from_string (ss2->flags);
      if (flags1 != flags2)
        {
          error ("%q+D uses conflicting section flags %qs from section"
                 " %qs", decl2, ss2->flags, ss2->name);
          inform (ss1->location, "used flags %qs for section %qs here",
                  ss1->flags, ss1->name);
        }
    }

  if (ss1->align && ss2->align
      && ss1->align != ss2->align)
    {
      /* FIXME: Yet another UB, just error until it's spec'ed.
         Alignment is actually already treated while inserting
         attributes...  */

      error ("%q+D uses conflicting alignment %d for section"
             " %qs", decl2, ss2->align, ss2->name);
      inform (ss1->location, "used alignment %d for section %qs here",
              ss1->align, ss1->name);
    }

  if (seen_error())
    {
      /* Nice! The user comes up with wrong code, no need to think about
         this one :-)  */
      return tspec1;
    }

  if (str_eq (ss1->raw, ss2->raw))
    {
      /* Easy: The secspecs are litearally the same, even the location
         matches.  */
      return tspec1;
    }

  /* Now the only differences can be in alignment, location, presence
     or absence of flags and the decl.  Determine which tspec to use
     ignoring align as it has already been handled and promoted to
     DECL_USER_ALIGN.  */

  may_use1 = (flags1 == flags2) || !ss2->flags;
  may_use2 = (flags1 == flags2) || !ss1->flags;

  if (may_use1)
    return tspec1;

  if (may_use2)
    return tspec2;

  error ("cannot merge this declaration of %q+D...", decl1);
  inform (DECL_SOURCE_LOCATION (decl2), "...with this one");

  return NULL;
}


/* TREE_LIST */

static void
htc_add_secspec (tree decl, enum secspec kind,
                 const char *s_name, const char *s_flags, int align,
                 location_t loc)
{
  htc_secspec_t *ss;
  tree attr0, args;
  bool bug = false;

  if (htc_decl_gcc_builtin_p (decl))
    return;

  gcc_assert (kind < SECSPEC_htc_built_in);

  if (/* Allow any section "name" for the GCC section attribute and be not
         too restrictive with that feature.  Users might have used that for
         their hacks and work-arounds.  */
      SECSPEC_attribute_gcc_section != kind
      && !htc_valid_section_name_p (s_name))
    {
      error ("%q+D: invalid section name %qs", decl, s_name);
      bug = true;
    }

  if (s_flags
      && -1U == targetm.htc.section_flags_from_string (s_flags))
    {
      error ("%q+D: invalid section flags for section %qs: %qs", decl,
             s_name, s_flags);
      bug = true;
    }

  if (bug)
    {
      if (loc != input_location)
        inform (loc, "section %qs defined here", s_name);
      return;
    }

  if (VAR_DECL == TREE_CODE (decl)
      && !TREE_STATIC (decl)
      && !DECL_EXTERNAL (decl))
    {
      return;
    }

  ss = htc_build_secspec (kind, s_name, s_flags, align, decl, loc);

  /* NULL or from a previous secspec */
  attr0 = lookup_attribute (HTC_ATTR_SECSPEC, DECL_ATTRIBUTES (decl));

  args = htc_serialize_secspec (ss);

  if (htc_log.secspec)
    htc_edump ("%H: %s secspec for %T to \"%s\" @ %H",
               DECL_SOURCE_LOCATION (decl),
               attr0 ? "merging" : "setting", decl,
               htc_secspec_raw (args), ss->location);

  if (attr0)
    {
      tree tspec;

      if (htc_log.secspec)
        {
          const htc_secspec_t *ss0 = htc_get_deserialize_secspec (decl);
          htc_edump (" (was: \"%s\" @ %H)\n", ss0->raw, ss0->location);
        }

      if (NULL_TREE != (tspec = htc_merge_secspec (decl, args,
                                                   decl, TREE_VALUE (attr0))))
        {
          TREE_VALUE (attr0) = tspec;
        }
    }
  else
    {
      if (htc_log.secspec)
        htc_edump ("\n");

      DECL_ATTRIBUTES (decl)
        = tree_cons (get_identifier (HTC_ATTR_SECSPEC), args,
                     DECL_ATTRIBUTES (decl));
    }

  if (kind == SECSPEC_pragma_htc_section
      || kind == SECSPEC_attribute_htc_asection)
    {
      DECL_SECTION_NAME (decl) = build_string (strlen ("?"), "?");
    }
}


/* Perform a check between two decl trees.
   We check that the two decls have the same section, and if
   not report it as a warning. */

tree
htc_merge_decl_attributes (tree decl1, tree decl2)
{
  tree secspec1 = htc_get_secspec (decl1);
  tree secspec2 = htc_get_secspec (decl2);
  tree attrs12 = merge_decl_attributes (decl1, decl2);
  tree secspec12;

  const htc_dam_t *dam1 = htc_deserialize_dam (decl1);
  const htc_dam_t *dam2 = htc_deserialize_dam (decl2);

  secspec12 = htc_merge_secspec (decl1, secspec1, decl2, secspec2);

  if (secspec12)
    {
      attrs12 = remove_attribute (HTC_ATTR_SECSPEC, attrs12);
      attrs12 = tree_cons (get_identifier (HTC_ATTR_SECSPEC), secspec12,
                           attrs12);
    }

  if (dam1 && dam2
      && dam1->dam != dam2->dam)
    {
      if (warning (OPT_Waddressing_mode,
                   "using %qs addressing for %q+D",
                   dam2->s_dam, decl2))
        {

          inform (DECL_SOURCE_LOCATION (decl1),
                  "specified %qs addressing for %qD by %s here",
                  dam1->s_dam, decl1, dam1->s_source);

          if (dam1->section.location != UNKNOWN_LOCATION)
            {
              if (dam1->section.location == BUILTINS_LOCATION)
                inform (input_location, "%s defined built-in with flags %qs",
                        dam1->s_source, dam1->section.s_flags);
              else
                inform (dam1->section.location,
                        "%s defined here with flags %qs", dam1->s_source,
                        dam1->section.s_flags);
            }
        }
    }
  else
    {
      /* Relying on default attribute merging.  */
    }

  return attrs12;
}


/* Check option dependencies for -msection-asm-name <map>=<key>.  This must
   be performed after *all* command options have been parsed and pruned.  */

void
htc_check_section_asm_name_required_options (void)
{
  if (!htc_section_asm_name_strict_p)
    return;

  // Traverse <key>s allowed in -msection-asm-name <key>=<map>.

  for (const htc_asm_name_map_t *maps = htc_asm_name_map; maps->key; maps++)
    {
      if (// No mapping specified for this key: nothing to do.
          ! maps->map
          // Special OPT if there is no option dependency.
          || maps->need_OPT == OPT_SPECIAL_ignore)
        continue;

      // Check whether the needed option is also set, e.g.
      // -msection-asm-name data-sections=<map> requires -fdata-sections to be
      // turned on.

      const struct cl_option *needed_option = &cl_options[maps->need_OPT];

      gcc_assert (needed_option->var_type == CLVC_BOOLEAN);

      const int *pflag
        = (const int*) option_flag_var (maps->need_OPT, &global_options);

      if (! *pflag)
        {
          error_at (UNKNOWN_LOCATION, "option %qs requires option %qs",
                    maps->decoded->orig_option_with_args_text,
                    needed_option->opt_text);
        }
    }
}


bool
htc_decl_gcc_builtin_p (tree decl)
{
  if (!decl
      || !DECL_P (decl))
    return false;

  return (UNKNOWN_LOCATION == DECL_SOURCE_LOCATION (decl)
          || (FUNCTION_DECL == TREE_CODE (decl)
              && DECL_BUILT_IN (decl)));
}

/* Map object's assembler name according to KEY as specified by
   -section-asm-name <key>=<map>.  Return NULL or a newly xalloc'ed string.  */
/*static char* htc_section_asm_name (tree decl, const char *key);*/

/* Transform the assembler name of decl DECL according to rule KEY as specified
   by command line option "-msection-asm-name <key>=<map>" and return
   NULL or a new, ggc-able string.  */

static const char*
htc_section_asm_name (tree decl, const char *key, bool *pnamed)
{
  const htc_asm_name_map_t *maps;
  bool is_func, is_data;

  if (!decl
      || decl == error_mark_node
      || !DECL_P (decl)
      || !DECL_NAME (decl))
    return NULL;

  is_func = FUNCTION_DECL == TREE_CODE (decl);
  is_data = (VAR_DECL == TREE_CODE (decl)
             || CONST_DECL == TREE_CODE (decl));

  for (maps = htc_asm_name_map; maps->key; maps++)
    {
      const char *asm_name;

      if (0 != strcmp (key, maps->key))
        continue;

      /* Some sanity checking:  Ensure the caller did not mis-spell key.  */

      if (is_func)
        gcc_checking_assert (strstr (key, "function-sections"));

      if (is_data)
        gcc_checking_assert (strstr (key, "data-sections")
                             || (targetm.htc.guess_bss_p()
                                 && strstr (key, "-bss-sections")));

      /* FIXME: `DECL_ASSEMBLER_NAME' i.e. tree.c:decl_assembler_name()
         has the side effect of setting the assembler name if it's not
         already set.  However c-decl.c:start_function() asserts that
         the assembler name has not already been set.  That assertion
         makes sense because vanilla GCC has no means to set the (user)
         assembler name as early as targetm.insert_attributes().
         Mangling names is actually only interesting for compiling C++
         so we can get away with the following kludge.  */

      asm_name = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl));
      asm_name = targetm.strip_name_encoding (asm_name);

      gcc_checking_assert (NULL == strstr (asm_name, " *INTERNAL* "));

      asm_name = maps->named_p
        ? ACONCAT ((maps->map_prefix, asm_name, maps->map_suffix, NULL))
        : maps->map;

      asm_name = ggc_strdup (asm_name);

      if (pnamed)
        *pnamed = maps->named_p;

      if (htc_log.section)
        {
          const char *s_tree = get_tree_code_name(TREE_CODE (decl));
          const char *s_decl = decl_name (decl);

          htc_edump ("%H: mapping decl_name '%s' of %s %p to",
                     input_location, s_decl, s_tree, (void *) decl);
          if (str_eq (s_decl, asm_name))
            htc_edump (" dito as part of named section ");
          else
            htc_edump (" asm_name '%s' as part of named section ", asm_name);

          if (maps->fulltext)
            htc_edump ("(rule '%s')\n", maps->fulltext);
          else
            htc_edump ("(rule default for '%s')\n", key);
        }

      return asm_name;
    } /* for maps */

  gcc_unreachable();
  return NULL;
}




/* As asserted in langhooks::set_decl_assembler_name().  */
/* The language-independent code should never use the
   DECL_ASSEMBLER_NAME for lots of DECLs.  Only FUNCTION_DECLs and
   VAR_DECLs for variables with static storage duration need a real
   DECL_ASSEMBLER_NAME.  */
#if 0
static bool
htc_may_have_asm_name (tree decl)
{
  if (!decl)
    return false;

  return (TREE_CODE (decl) == FUNCTION_DECL
          || (TREE_CODE (decl) == VAR_DECL
              && (TREE_STATIC (decl)
                  || DECL_EXTERNAL (decl)
                  || TREE_PUBLIC (decl))));
}
#endif


/* Map attribute address.mode to DAM_* data addressing mode.  */

enum data_addressing_mode
htc_decl_addressing (const_tree decl)
{
  tree attr;
  enum data_addressing_mode dam = DAM_unknown;

  if (decl
      && TREE_CODE (decl) == VAR_DECL
      && ((attr = lookup_attribute (HTC_ATTR_ADDRESSMODE,
                                    DECL_ATTRIBUTES (decl)))))
    {
      char c_dam;
      tree arg = TREE_VALUE (attr);
      gcc_checking_assert (TCODE_FOR_DAM == STRING_CST
                           || TCODE_FOR_DAM == IDENTIFIER_NODE);
      gcc_assert (arg
                  && TREE_LIST == TREE_CODE (arg)
                  && TREE_PURPOSE (arg)
                  && IDENTIFIER_NODE == TREE_CODE (TREE_PURPOSE (arg))
                  && TREE_VALUE (arg)
                  && TCODE_FOR_DAM == TREE_CODE (TREE_VALUE (arg)));
      c_dam = * (TCODE_FOR_DAM == STRING_CST
                 ? TREE_STRING_POINTER (TREE_VALUE (arg))
                 : IDENTIFIER_POINTER (TREE_VALUE (arg)));
      dam = htc_dam_from_letter (c_dam);
    }

  return dam;
}


/* -- Section name as supplied by attribute ASECTION resp. #pragma section.
   -- Section flags.
   -- Section alignment.
   -- Kind of this entry, e.g. attribute ASECTION or #pragma section.
   -- Addressing mode for this object.
   -- Location info of the section origin, e.g. location of #pragma section.
*/


/* Search for section NAME in the htc_sections[] list. Return a pointer to
   the section with that name or NULL if such a section does not (yet) exist. */

htc_section_t*
htc_lookup_section (const char *name)
{
  htc_section_t *hsec;
  hashval_t hash;

  if (verbose_flag)
    timevar_push (TV_HTC_SECTION_LOOKUP);

  hstat->htc.section.lookup.n_calls++;

  hash = htab_hash_string (name);
  hstat->htc.section.lookup.n_hashes_computed++;

  if (!htc_section_htab)
    htc_section_htab = htab_create_ggc (37, htc_htab_htc_section_hash,
                                        htc_htab_htc_section_eq,
                                        NULL);

  hsec = (htc_section_t*) htab_find_with_hash (htc_section_htab, name, hash);

  gcc_assert (hsec == NULL
              || hsec->location == BUILTINS_LOCATION
              || hsec->next == NULL);

  if (verbose_flag)
    timevar_pop (TV_HTC_SECTION_LOOKUP);

  return hsec;
}

/* Test if NAME is a valid section name.  Valid section names are like
   valid C identifiers.  In addition, '.' might be used like a letter.  */

bool
htc_valid_section_name_p (const char *name)
{
  const char *s;

  if (*name == '\0')
    return false;

  for (s = name; *s; s++)
    {
      if (*s == '.' || ISIDST (*s))
        continue;

      /* First letter must not be a digit */

      if (s == name || !ISDIGIT (*s))
        return false;
    }

  return true;
}


/* No error will be issued. */

static unsigned int
htc_get_asection_alignment (tree attribs)
{
  tree arg, asec = lookup_attribute (HTC_ATTR_ASECTION, attribs);
  if (!asec)
    return 0;

  arg = TREE_VALUE (asec);

  for (arg = TREE_CHAIN (arg); arg; arg = TREE_CHAIN (arg))
    {
      int align;
      const char *s_arg;

      if (STRING_CST != TREE_CODE (TREE_VALUE (arg)))
        continue;

      s_arg = TREE_STRING_POINTER (TREE_VALUE (arg));

      if (s_arg[0] == 'a'
          && s_arg[1] != '\0'
          && 0 <= exact_log2 (align = atoi (s_arg + 2)))
        {
          return (unsigned) align;
        }
    }

  return 0;
}


static bool
has_smalldata_p (void)
{
  return (NULL != sbss_section
          || NULL != lookup_attribute_spec (get_identifier ("smalldata")));
}

static bool
has_absdata_p (void)
{
  return NULL != lookup_attribute_spec (get_identifier ("absdata"));
}


static bool
has_fardata_p (void)
{
  return NULL != lookup_attribute_spec (get_identifier ("fardata"));
}


/* No errors.  */

static enum data_addressing_mode
htc_addressing_mode_from_string (const char *s_flags)
{
  enum data_addressing_mode dam = DAM_unknown;

  if (has_smalldata_p()
      && strchr (s_flags, 's'))
    dam = DAM_small;

  if (has_absdata_p()
      && strchr (s_flags, 'z'))
    dam = dam == DAM_small ? DAM_invalid : DAM_absolute;

  return dam;
}


/* Return false if an error has been emit */


static bool
htc_add_dam (tree decl, htc_dam_t *dam)
{
  const htc_dam_t *dam0;
  tree tdam;

  if (!dam)
    return true;

  tdam = htc_serialize_dam (dam);

  if (htc_log.dam)
    {
      const char *s_tree = get_tree_code_name(TREE_CODE (decl));
      location_t loc = dam->section.location == UNKNOWN_LOCATION
        ? dam->location
        : dam->section.location;
      
      htc_edump ("%H: tagging %s '%s' with '%s' addressing \"%s\" @ %H\n",
                 DECL_SOURCE_LOCATION (decl), s_tree, decl_name (decl),
                 dam->s_dam, dam->s_raw, loc);
    }

  dam0 = htc_deserialize_dam (decl);

  if (dam0
      && dam->dam != dam0->dam)
    {
      if (warning (OPT_Waddressing_mode,"using %qs addressing for %q+D by %s",
                   dam->s_dam, decl, dam->s_source))
        {

          inform (dam0->location,
                  "specified %qs addressing for %qs by %s here",
                  dam0->s_dam, IDENTIFIER_POINTER(dam0->id), dam0->s_source);

          if (dam0->section.location != UNKNOWN_LOCATION)
            {
              if (dam0->section.location == BUILTINS_LOCATION)
                inform (input_location, "%s defined built-in with flags %qs",
                        dam0->s_source, dam0->section.s_flags);
              else
                inform (dam0->section.location,
                        "%s defined here with flags %qs", dam0->s_source,
                        dam0->section.s_flags);
            }
        }
    }

  DECL_ATTRIBUTES (decl)
    = tree_cons (get_identifier (HTC_ATTR_ADDRESSMODE), tdam,
                 DECL_ATTRIBUTES (decl));
  return true;
}


/* Handle "absdata", "smalldata", "fardata" */

static void
htc_add_dam_from_attributes (tree decl, tree attribs)
{
  htc_dam_t *dam;

  if (!decl
      || TREE_CODE (decl) != VAR_DECL
      || ! (TREE_STATIC (decl)
            || DECL_EXTERNAL (decl)))
    return;

  if (has_absdata_p()
      && lookup_attribute ("absdata", attribs))
    {
      /* Closing ' added by htc_build_dam().  */
      dam = htc_build_dam (decl, DAM_absolute, "attribute 'absdata",
                           input_location, NULL, NULL, UNKNOWN_LOCATION);
      htc_add_dam (decl, dam);
    }

  if (has_smalldata_p()
      && lookup_attribute ("smalldata", attribs))
    {
      /* Closing ' added by htc_build_dam().  */
      dam = htc_build_dam (decl, DAM_small, "attribute 'smalldata",
                           input_location, NULL, NULL, UNKNOWN_LOCATION);
      htc_add_dam (decl, dam);
    }

  if (has_fardata_p()
      && lookup_attribute ("fardata", attribs))
    {
      /* Closing ' added by htc_build_dam().  */
      dam = htc_build_dam (decl, DAM_indirect, "attribute 'fardata",
                           input_location, NULL, NULL, UNKNOWN_LOCATION);
      htc_add_dam (decl, dam);
    }
}


void
htc_set_htc_section (htc_section_t *hsec, tree decl, bool warn)
{
  /* Error somewhere, for simplicity in the caller, we test here for NULL.  */

  if (!hsec)
    return;

  if (!hsec->sec)
    {
      hsec->sec = get_unnamed_section (hsec->flags,
                                       targetm.htc.unnamed_section_callback,
                                       /* hsec is GTY'ed ! */
                                       hsec);
      hstat->htc.section.n_unnamed_sections++;
    }

  if (decl)
    {
      if ((hsec->flags & SECTION_BSS)
          && htc_vardecl_progbits_p (decl))
        {
          if (warning (OPT_Wnonzero_initialized_in_bss,
                       "variable %q+D with non-zero initializer is put "
                       "into @nobits section %qs with section flag %<B%>",
                       decl, hsec->name))
            {
              const char *s_here = (hsec->location == BUILTINS_LOCATION
                                    ? "built-in" : "here");

              inform (hsec->location,
                      "section %qs defined %s with flags %qs",
                      hsec->name, s_here, hsec->secspec->flags
                      ? hsec->secspec->flags : "");
            }
        }

      // Diagnose if read-onlyness of objects and sections might disagree.

      if (warn
          && TREE_CODE (decl) == VAR_DECL
          && (TREE_STATIC (decl) || DECL_EXTERNAL (decl)))
        {
          tree node0 = decl;

          /* For C++, we have to peel arrays in order to get correct
             determination of readonlyness.  */

          do
            node0 = TREE_TYPE (node0);
          while (TREE_CODE (node0) == ARRAY_TYPE);

          if (node0 != error_mark_node)
            {
              bool warn_ro_in_writable
                = (htc_warn_readonly_in_writable
                   && (hsec->flags & (SECTION_WRITE | SECTION_BSS)));

              bool warn_non_const_in_ro = !(hsec->flags & SECTION_WRITE);

              bool is_readonly = TYPE_READONLY (node0) || TREE_READONLY (decl);

              if (warn_ro_in_writable && is_readonly)
                warning (OPT_Wreadonly_in_writable,
                         "read-only variable %q+D put into writable"
                         " section %qs", decl, hsec->name);

              if (warn_non_const_in_ro && !is_readonly)
                warning (0, "non-const variable %q+D put into read-only"
                         " section %qs", decl, hsec->name);
            }
        }
    }
}


/* Handle the asection attribute.  */

tree
htc_handle_asection_attribute (tree *node, tree ARG_UNUSED (name),
                               tree args, int ARG_UNUSED (flags),
                               bool *no_add_attrs)
{
  tree decl = *node;
  const char *s_flags = NULL;
  unsigned int align = 0;

  tree t;
  const char *sec_name = TREE_STRING_POINTER (TREE_VALUE (args));

  for (t = TREE_CHAIN (args); t; t = TREE_CHAIN (t))
    {
      const char *str = TREE_STRING_POINTER (TREE_VALUE (t));

      switch (*str)
        {
        case 'a':
          {
            if (str[1] != '=')
              warning (0, "%D: incorrect separator %qc in asection alignment",
                       decl, str[1]);

            align = atoi (str + 2);

            if (exact_log2 (align) < 0)
              {
                error ("%D: illegal %qs alignment %qs (must be a power of 2)",
                       decl, HTC_ATTR_ASECTION, str + 2);
                *no_add_attrs = true;
              }
          }
          break;

        case 'f':
          {
            if (str[1] != '=')
              warning (0, "%D: incorrect separator %qc in asection flags",
                       decl, str[1]);
            s_flags = str + 2;
          }
          break;

        default:
          {
            error ("%D: illegal %qs attributes %qs",
                   decl, HTC_ATTR_ASECTION, str);
            *no_add_attrs = true;
          }
          break;
        }
    }

  if (VAR_DECL == TREE_CODE (decl)
      && current_function_decl != NULL_TREE
      && !TREE_STATIC (decl)
      && !DECL_EXTERNAL (decl))
    {
      error ("%D: attribute %qs not allowed for local variables",
             decl, HTC_ATTR_ASECTION);
      *no_add_attrs = true;
    }

  if (s_flags)
    {
      unsigned flags = targetm.htc.section_flags_from_string (s_flags);
      bool code_p = !!(flags & SECTION_CODE);

      if (!code_p
          && FUNCTION_DECL == TREE_CODE (decl))
        {
          error ("function %qD cannot be put into non-code section", decl);
          *no_add_attrs = true;
        }

      if (code_p
          && FUNCTION_DECL != TREE_CODE (decl))
        {
          error ("variable %qD cannot be put into code section", decl);
          *no_add_attrs = true;
        }

      if (VAR_DECL == TREE_CODE (decl)
          && flags != -1u)
        {
          htc_dam_t *dam
            = htc_build_dam (decl,
                             htc_addressing_mode_from_string (s_flags),
                             "attribute 'asection", input_location,
                             sec_name, s_flags, input_location);

          if (dam
              && !htc_add_dam (decl, dam))
            {
              *no_add_attrs = true;
            }
        }
    }

  /* Alignment has already been set by htc_insert_attribute_secspec which
     runs before this callback, same applied to addressing mode.  */

  if (! *no_add_attrs)
    {
      htc_secspec_t *sspec;
      unsigned flags = s_flags
        ? targetm.htc.section_flags_from_string (s_flags)
        : -1u;

      htc_add_secspec (decl, SECSPEC_attribute_htc_asection,
                       sec_name, s_flags, align, input_location);

      /* Check same asections at different decls to be consistent.  */

      sspec = htc_get_deserialize_secspec (decl);

      if (sspec)
        {
          htc_section_t *hsec;
          hsec = htc_insert_section (sec_name, flags, align, sspec,
                                     NULL_TREE, input_location);
          htc_set_htc_section (hsec, decl, true);
        }
    }

  return NULL_TREE;
}


/* Get the real section name build from section.anyfix(assembler_name(decl))
 */


static tree
htc_build_section_name (tree decl, htc_secspec_t **ss, int reloc,
                        bool *pnamed)
{
  const char *key = NULL;
  const char *s_asm_name = NULL;
  const char *s_secname = NULL;
  unsigned flags = -1u;
  int align = 0;
  bool is_bss = false;
  bool with_prefix_p = true;

  enum secspec kind = *ss ? (*ss)->kind : SECSPEC_unknown;

  gcc_assert (!*ss
              || (*ss)->kind != SECSPEC_unknown);

  *pnamed = false;

  if (kind == SECSPEC_attribute_gcc_section)
    {
      gcc_assert (DECL_SECTION_NAME (decl));
      return DECL_SECTION_NAME (decl);
    }

  (void) DECL_ASSEMBLER_NAME (decl);

  switch (TREE_CODE (decl))
    {
    default:
      gcc_unreachable();

    case FUNCTION_DECL:
      if (kind == SECSPEC_attribute_htc_asection
          && htc_opt_asection_function_sections)
        key = "asection-function-sections";

      if (kind == SECSPEC_pragma_htc_section
          && htc_opt_pragma_function_sections)
        key = "pragma-function-sections";

      if (kind == SECSPEC_unknown
          && flag_function_sections)
        key = "function-sections";

      if (key)
        s_asm_name = htc_section_asm_name (decl, key, pnamed);

      break;

    case VAR_DECL:
    case CONST_DECL:

      if (targetm.htc.guess_bss_p()
          && (*ss)
          && VAR_DECL == TREE_CODE (decl)
          && targetm.htc.bss_initializer_p (decl))
        {
          if ((*ss)->kind == SECSPEC_pragma_htc_section
              && htc_opt_pragma_data_sections)
            key = "pragma-bss-sections";

          if ((*ss)->kind == SECSPEC_attribute_htc_asection
              && htc_opt_asection_data_sections)
            key = "asection-bss-sections";

          is_bss = key != NULL;
        }

      if (key == NULL)
        {
          if (kind == SECSPEC_attribute_htc_asection
              && htc_opt_asection_data_sections)
            key = "asection-data-sections";

          if (kind == SECSPEC_pragma_htc_section
              && htc_opt_pragma_data_sections)
            key = "pragma-data-sections";

          if (kind == SECSPEC_unknown
              && flag_data_sections)
            key = "data-sections";
        }

      if (key)
        s_asm_name = htc_section_asm_name (decl, key, pnamed);

      break;
    } /* switch TREE_CODE (decl) */

  if (kind != SECSPEC_unknown)
    {
      s_secname = (*ss)->name;
    }
  else
    {
      tree t_secname, decl_section_name;

      /* Draw the name of default sections from `TARGET_ASM_UNIQUE_SECTION'.
         That hook will prepend '=' to the section named to signal when
         we shall append a pimped assembler name.  */

      decl_section_name = DECL_SECTION_NAME (decl);
      targetm.asm_out.unique_section (decl, reloc);
      t_secname = DECL_SECTION_NAME (decl);
      if (!s_asm_name)
        DECL_SECTION_NAME (decl) = decl_section_name;

      gcc_assert (t_secname);

      s_secname = TREE_STRING_POINTER (t_secname);

      if (s_secname[0] == '=')
        {
          htc_section_t *hsec;

          s_secname++;

          kind = SECSPEC_htc_built_in;
          hsec = htc_lookup_section (s_secname);
          gcc_assert (hsec);
          flags = hsec->flags;
          align = hsec->align;
        }
      else
        {
          /* Use unaltered as provided by `TARGET_ASM_UNIQUE_SECTION'
             of there is no '=' prefix.  */

          s_asm_name = NULL;
          kind = SECSPEC_gcc_built_in;
        }
    }

  if (s_asm_name)
    {
      with_prefix_p = '!' != s_asm_name[0];
      /* Finally, we reached the grale of section pimp!  */

      s_secname = with_prefix_p
        ? ACONCAT ((s_secname, ".", s_asm_name, NULL))
        : 1 + s_asm_name;

      if (kind == SECSPEC_htc_built_in)
        kind = SECSPEC_gcc_named_section;
    }

  if (*pnamed
      && is_bss
      && (NULL == (*ss)->flags
          || !strchr ((*ss)->flags, 'B')))
    {
      char flagchars[20];
      const char *s_flags = (*ss)->flags ? (*ss)->flags : "aw";
      unsigned flags = (SECTION_BSS
                        | targetm.htc.section_flags_from_string (s_flags));

      s_flags = flagchars;
      targetm.htc.section_flags_to_string (flagchars, flags,
                                           FLAGSTRING_mimic_user_input);

      *ss = htc_build_secspec (kind, s_secname,
                               s_flags, (*ss)->align, decl,
                               DECL_SOURCE_LOCATION (decl));
      if (htc_log.secspec)
        htc_edump ("%H: guessing secspec for %T to \"%s\" @ %H\n",
                   DECL_SOURCE_LOCATION (decl), decl,
                   (*ss)->raw, (*ss)->location);
    }

  if (kind >= SECSPEC_htc_built_in
      && s_asm_name)
    {
      location_t loc = kind == SECSPEC_htc_built_in
        ? BUILTINS_LOCATION
        : DECL_SOURCE_LOCATION (decl);
      char s_flags[20];

      if (verbose_flag)
        timevar_push (TV_HTC_SECSPEC_HATCH);

      if (-1u != flags)
        targetm.htc.section_flags_to_string (s_flags, flags,
                                             FLAGSTRING_mimic_user_input);

      *ss = htc_build_secspec (kind, s_secname,
                               flags == -1u ? NULL : s_flags, align,
                               (*pnamed) ? decl : NULL, loc);

      if (verbose_flag)
        timevar_pop (TV_HTC_SECSPEC_HATCH);

      if (htc_log.secspec
          && !htc_decl_gcc_builtin_p (decl))
        htc_edump ("%H: hatching secspec for %T to \"%s\" @ %H\n",
                    DECL_SOURCE_LOCATION (decl), decl,
                    (*ss)->raw, (*ss)->location);
    }

  return build_string (strlen (s_secname), s_secname);
}


htc_section_t*
htc_decl_htc_section (tree decl)
{
  htc_section_t *hsec = NULL;
  tree attr, arg;

  if (decl
      && CODE_CONTAINS_STRUCT (TREE_CODE (decl), TS_DECL_WITH_VIS)
      && (attr = lookup_attribute (HTC_ATTR_HTCSECTION,
                                   DECL_ATTRIBUTES (decl))))
    {
      const char *s_name;
      gcc_assert ((arg = TREE_VALUE (attr))
                  && TREE_LIST == TREE_CODE (arg)
                  && NULL == TREE_PURPOSE (arg)
                  && STRING_CST == TREE_CODE (TREE_VALUE (arg)));

      s_name = TREE_STRING_POINTER (TREE_VALUE (arg));
      hsec = htc_lookup_section (s_name);
      gcc_assert (hsec);
      gcc_assert (hsec->sec);
    }

  return hsec;
}



/* Introduces to have a means to process secspecs.  */

void
my_resolve_unique_section (tree decl, int reloc,
                           int ARG_UNUSED (flag_code_or_data_sections))
{
  htc_section_t *hsec;
  bool named_p;
  unsigned flags;
  htc_secspec_t *sspec;
  tree secname;
  const char *s_name;

  gcc_assert (targetm_common.have_named_sections);

  /* If the section name has already been set we have nothing more
     to do here except... */

  if (DECL_SECTION_NAME (decl)
      && NULL != htc_decl_htc_section (decl))
    {
      return;
    }

  if (/* If the section name has already been set we have nothing more
         to do here except... */
      DECL_SECTION_NAME (decl)
      /* ... in the case when the section name comes from `htc_add_secspec'.
         For rationale see there.  */
      && !str_eq ("?", TREE_STRING_POINTER (DECL_SECTION_NAME (decl))))
    {
      return;
    }

  sspec = htc_get_deserialize_secspec (decl);

  if (DECL_COMMON (decl))
    {
      /* Whether or not an object is .comm or .lcomm isn't known when
         the decl is being parsed: in `TARGET_INSERT_ATTRIBUTES', the
         initializer might not yet be parsed or in might be just a plain
         declaration.  Moreover, whether an initializer has been parsed
         might depend on how the front-end works.  */

      if (sspec)
        {
          gcc_assert (sspec->kind != SECSPEC_pragma_htc_section
                      && sspec->kind != SECSPEC_attribute_htc_asection);

          if (htc_log.secspec)
            htc_edump ("%H: deleting secspec \"%s\" of %T\n",
                        DECL_SOURCE_LOCATION (decl), sspec->raw, decl);

          /* Just clean up tags that turned out to be not needed.  */

          DECL_ATTRIBUTES (decl)
            = remove_attribute (HTC_ATTR_SECSPEC,
                                DECL_ATTRIBUTES (decl));

          hstat->htc.section.n_secspecs_void++;
        }

      return;
    }

  /* The common case is -fno-common  */

  secname = htc_build_section_name (decl, &sspec, reloc, &named_p);

  if (!sspec)
    {
      /* Options + secspec revealed that the decl is plain vanilla.
         Simply switch to GCC's default behaviour.  */

      return;
    }

  if (htc_log.secspec
      && !htc_decl_gcc_builtin_p (decl))
    htc_edump ("%H: applying secspec \"%s\" to %T @ %H\n",
                DECL_SOURCE_LOCATION (decl), sspec->raw, decl,
                sspec->location);

  if (htc_log.section
      && !htc_decl_gcc_builtin_p (decl))
    htc_edump ("%H: sectioning %s '%s' into %s @ %H\n",
               DECL_SOURCE_LOCATION (decl),
               get_tree_code_name(TREE_CODE (decl)), decl_name (decl),
               TREE_STRING_POINTER (secname), sspec->location);

  /* Transform the information drawn from the secspec into a section name
     prefixed by "htc:" so we can easily pick up the bound `htc_section_t'
     object again.  */

  s_name = TREE_STRING_POINTER (secname);

  gcc_assert (DECL_P (decl));
  DECL_SECTION_NAME (decl) = secname;

  DECL_ATTRIBUTES (decl)
    = tree_cons (get_identifier (HTC_ATTR_HTCSECTION),
                 build_tree_list (NULL_TREE, secname),
                 remove_attribute (HTC_ATTR_SECSPEC,
                                   DECL_ATTRIBUTES (decl)));

  hstat->htc.section.n_secspecs_applied++;

  /* Check whether we can find any stupidity in the sources and
     diagnose as many of them as possible.  */

  hsec = htc_lookup_section (s_name);

  if (hsec
      && secname)
    {
      htc_section_t *hsec2;
      /* FIXME: Section type flags? */
      hsec2 = htc_insert_section (TREE_STRING_POINTER (secname),
                                  -1u, 0, sspec,
                                  named_p ? decl : NULL_TREE,
                                  DECL_SOURCE_LOCATION (decl));
      htc_set_htc_section (hsec2, decl, false);
    }

  if (!hsec
      && secname)
    {
      location_t loc = sspec
        ? sspec->location
        : DECL_SOURCE_LOCATION (decl);

      unsigned align = sspec ? sspec->align : 0;
      /* Default flags in case sspec->flags = NULL.  */
      flags = targetm.section_type_flags (decl,
                                          TREE_STRING_POINTER (secname),
                                          reloc);
      if (sspec && sspec->flags)
        {
          unsigned flag_G = HAVE_COMDAT_GROUP
            ? (flags & SECTION_LINKONCE)
            : 0;

          flags = (flag_G
                   | targetm.htc.section_flags_from_string (sspec->flags));
        }

      hsec = htc_insert_section (TREE_STRING_POINTER (secname),
                                 flags, align, sspec,
                                 named_p ? decl : NULL_TREE,
                                 loc);
      htc_set_htc_section (hsec, decl, true);

    }
}


/* Implement `TARGET_INSERT_ATTRIBUTES' */

void
htc_insert_htc_attributes (tree decl, tree *attribs)
{
  const htc_secspec_t *pspec = NULL;
  htc_section_t *pragma;
  unsigned align, asection_align, pragma_align = 0;
  tree attr;

  if (!decl
      || error_mark_node == decl
      || !DECL_P (decl))
    return;

  switch (TREE_CODE (decl))
    {
    default:
      return;

    case FUNCTION_DECL:
      pragma = htc_pragma_section.code;
      if (DECL_BUILT_IN (decl))
        return;
      break;

    case VAR_DECL:
    case CONST_DECL:
      pragma = htc_pragma_section.data;
      if (!TREE_STATIC (decl)
          && !DECL_EXTERNAL (decl))
        return;
      break;
    } /* TREE_CODE (decl) */

  if (pragma)
    {
      htc_section_t *hsec;
      pspec = pragma->secspec;
      htc_add_secspec (decl, SECSPEC_pragma_htc_section,
                       pspec->name, pspec->flags, pspec->align,
                       pspec->location);
      pragma_align = pspec->align;

      hsec = htc_lookup_section (pspec->name);
      /* check for readonly section assignment */
      htc_set_htc_section (hsec, decl, true);

      /* FIXME: This is inefficient! */
      if (pspec->flags
          && VAR_DECL == TREE_CODE (decl))
        {
          htc_dam_t *dam
            = htc_build_dam (decl,
                             htc_addressing_mode_from_string (pspec->flags),
                             /* Closing ' added by htc_build_dam().  */
                             "'#pragma section", input_location,
                             pspec->name, pspec->flags, pspec->location);
          htc_add_dam (decl, dam);
        }
    }

  asection_align = htc_get_asection_alignment (*attribs);

  align = MAX (asection_align, pragma_align);

  if (align > 0)
    {
      DECL_ATTRIBUTES (decl)
        = tree_cons (get_identifier ("aligned"),
                     tree_cons (NULL_TREE, size_int (align), NULL_TREE),
                     DECL_ATTRIBUTES (decl));

      DECL_ALIGN (decl) = align * BITS_PER_UNIT;
      DECL_USER_ALIGN (decl) = 1;
    }

  /* Attribute handler for attribute "asection" will be called after this
     hook.  Therefore use similar procedure for attribute "section":  If there
     are multiple secspecs, e.g. #pragma section and a section specifying
     attribute at the same time, the #pragma occurs first in the source code.
     Specifying multiple secspecs of different kind at the same time
     is undefined behaviour:  Currently it raises an error, hence the
     placing of the code which adds a "section" secspec will just have a
     marginal impact for now, i.e. what the diagnostics look like.  */

  if ((attr = lookup_attribute ("section", *attribs)))
    {
      htc_secspec_t *sspec;
      tree args = TREE_VALUE (attr);
      gcc_assert (TREE_LIST == TREE_CODE (args)
                  && STRING_CST == TREE_CODE (TREE_VALUE (args)));

      htc_add_secspec (decl, SECSPEC_attribute_gcc_section,
                       TREE_STRING_POINTER (TREE_VALUE (args)), NULL, 0,
                       input_location);

      sspec = htc_get_deserialize_secspec (decl);
      if (sspec)
        {
          htc_section_t *hsec;
          hsec = htc_insert_section (sspec->name, -1u, 0, sspec,
                                     NULL_TREE, input_location);
          htc_set_htc_section (hsec, decl, true);
        }
    }

  if (VAR_DECL == TREE_CODE (decl))
    {
      htc_add_dam_from_attributes (decl, *attribs);
    }
}

/* Helper function for `htc_insert_section'. */

/* Construct a new GTY-ed `htc_section_t' object, initialize it and
   insert it to `htc_section_htab' by NAME.  */

static htc_section_t*
htc_insert_section_to_htab (const char *name, unsigned flags, unsigned align,
                            htc_secspec_t *sspec, tree named_decl,
                            unsigned int ulocation)
{
  hashval_t hash;
  void **slot;
  htc_section_t *hsec;

  if (verbose_flag)
    timevar_push (TV_HTC_SECTION_LOOKUP);

  hash = htab_hash_string (name);
  htc_stat.htc.section.lookup.n_hashes_computed++;

  slot = htab_find_slot_with_hash (htc_section_htab, name, hash, INSERT);

  hsec = (htc_section_t*) *slot;
  if (hsec)
    gcc_unreachable();

  htc_stat.htc.section.n_htc_sections++;
  htc_stat.htc.section.n_htc_sections_named += named_decl != NULL_TREE;

  hsec = (htc_section_t*) ggc_alloc_cleared_atomic (sizeof (htc_section_t));

  hsec->name = ggc_strdup (name);
  hsec->flags = flags;
  hsec->align = align;
  hsec->next = NULL;
  hsec->location = ulocation;
  hsec->named_decl = named_decl;

  hsec->secspec = sspec;

  *slot = hsec;

  if (verbose_flag)
    timevar_pop (TV_HTC_SECTION_LOOKUP);
  return hsec;
}

/* Insert a new section definition to our section list.
   If an equally named section already exists, check for compatibility of flags
   and alignment.

   If ALIGN=0 no alignment was specified, we use default alignment in that case.
   Otherwise, we have already tested that ALIGN is a power of 2.

   If SSPEC->FLAGS is NULL and FLAGS is -1u no flags were specified,
   we use default flags "aw" in that case.

   If everything is fine return the pointer to the newly created or already
   existing section. Otherwise, print error message and return NULL. */

htc_section_t*
htc_insert_section (const char *name, unsigned flags, unsigned align,
                     htc_secspec_t *sspec, tree named_decl,
                     unsigned int ulocation)
{
  bool bug = false;
  bool flags_known = flags != -1u;
  location_t location = (location_t) ulocation;
  htc_section_t *secp;

  gcc_assert (sspec);

  /* Section name validity checks already performed by `htc_add_secspec'.  */

  /* flags = -1u:  Flags are unknown. */

  if (sspec->flags)
    {
      unsigned flags_sspec
        = targetm.htc.section_flags_from_string (sspec->flags);

      if (flags_sspec == -1u)
        {
          error_at (location, "illegal section flags for section %qs: %qs",
                    name, sspec->flags);
          bug = true;
        }
      else if (-1u == flags)
        {
          flags = flags_sspec;
          flags_known = flags != -1u;
        }
    }

  secp = htc_lookup_section (name);

  if (align == 0)
    {
      /* Use known resp. default alignment */
      align = secp ? secp->align
                   : (flags & SECTION_CODE) ? (FUNCTION_BOUNDARY / BITS_PER_UNIT)
                                            : 0;
    }
  else if (align < (FUNCTION_BOUNDARY / BITS_PER_UNIT)
           && (flags != -1u
               && (flags & SECTION_CODE)))
    {
      error_at (location,
                "minimum alignment for a code section is %u",
                (FUNCTION_BOUNDARY / BITS_PER_UNIT));
      bug = true;
    }

  if (secp)
    {
      const char *s_here = (secp->location == BUILTINS_LOCATION
                            ? "built-in" : "here");

      location_t loc = (secp->location == BUILTINS_LOCATION
                        ? location : secp->location);

      if (!flags_known)
        {
          flags = secp->flags;
          flags_known = true;
        }

      if (secp->flags != flags)
        {
          char old_flags[20];
          char new_flags[20];

          targetm.htc.section_flags_to_string (old_flags, secp->flags,
                                               FLAGSTRING_descriptive);
          targetm.htc.section_flags_to_string (new_flags, flags,
                                               FLAGSTRING_descriptive);

          error_at (location, "section %qs redefined with different flags %qs",
                    name, new_flags);
          inform (loc, "section %qs defined %s with flags %qs",
                  name, s_here, old_flags);
          bug = true;
        }

      if (align != secp->align)
        {
          error_at (location, "section %qs redefined with different"
                    " alignment %d", name, align);

          if (secp->align)
            inform (loc, "section %qs defined %s with alignment %d",
                    name, s_here, secp->align);
          else
            inform (loc, "section %qs defined %s with no alignment",
                    name, s_here);
          bug = true;
        }
    }

  if (bug)
    {
      return NULL;
    }

  if (!flags_known)
    flags = SECTION_WRITE;

  if (secp
      && location != UNKNOWN_LOCATION)
    {
      if (named_decl
          && secp->named_decl
          && named_decl != secp->named_decl)
        {
          warning_at (location, OPT_Wsection_multiple_data,
                      "%qD and %qD assigned to same section %qs",
                      named_decl, secp->named_decl, secp->name);
        }
      else if (named_decl
               && !secp->named_decl)
        {
          warning_at (location, OPT_Wsection_multiple_data,
                      "data/function section %qs for %qD already defined",
                       secp->name, named_decl);
        }
      else if (!named_decl
               && secp->named_decl)
        {
          warning_at (location, OPT_Wsection_multiple_data,
                      "section %qs already defined by %qD",
                      secp->name, secp->named_decl);
        }
    }

  if (htc_log.section
      && location != UNKNOWN_LOCATION)
    {
      char new_flags[20];

      const char *s_code = (flags & SECTION_CODE) ? "code" : "data";

      targetm.htc.section_flags_to_string (new_flags, flags,
                                           FLAGSTRING_descriptive);

      if (secp)
        {
          const char *s_named = secp->named_decl ? "named" : "unnamed";

          if (secp->location == BUILTINS_LOCATION)
            htc_edump ("%H: using built-in %s section %s, f=%s, a=%d\n",
                       location, s_code, name, new_flags, align);
          else
            htc_edump ("%H: using %s %s section %s, f=%s, a=%d @ %H\n",
                       location, s_named, s_code, name, new_flags, align,
                       secp->location);
        }
      else
        {
          const char *s_named = named_decl ? "named" : "unnamed";

          htc_edump ("%H: defining %s %s section %s, f=%s, a=%d",
                     location, s_named, s_code, name, new_flags, align);
          if (named_decl)
            htc_edump (" (for: %T @ %H)",
                       named_decl, DECL_SOURCE_LOCATION (named_decl));
          htc_edump ("\n");
        }
    } /* htc_log.section */

  if (secp)
    {
      htc_stat.htc.section.n_htc_sections_reused++;
      return secp;
    }

  secp = htc_insert_section_to_htab (name, flags, align,
                                     sspec, named_decl,
                                     ulocation);

  return secp;
}


#include "gt-htc-common-sections.h"
