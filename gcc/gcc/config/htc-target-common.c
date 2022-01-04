/* Features provided by all HighTec targets, and that are common to driver
   and compiler proper, used by common/config/<target>/<target>-common.c.

   Copyright (C) 2012-2015 Free Software Foundation, Inc.

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
   <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "diagnostic.h"

#include "config/htc-target-common.h"

#include "opts.h"
#include "options.h"

/* Refers to the assembler name of the object / function as would be used
   to compose the name of vanilla named sections.  Specifying no mapping to
   pimp the assembler name is basically the same as specifying just '+'.
   There is not much choice for this character because many punctuations
   like '*' or '%' are special to shells and it's nice not to force quoting
   of options or usage of @opt file.  */
const char htc_asm_name_map_id = '+';

// Can be unset by "-mno-section-asm-name strict".
bool htc_section_asm_name_strict_p = true;

/* Describes results of specifying -msection-asm-name <key>=<map>
   and also properties of allowed arguments to that option.  */

// key: valid key in -msection-asm-name <key>=<map>
// opt: OPT_SPECIAL_ignore or the index of a command line option that must
//      also be set if a mapping for key is specified.
#define MAP(key, opt)                           \
  { key, opt, NULL, empty, empty, true, NULL, NULL }

static char empty[] = "";

htc_asm_name_map_t htc_asm_name_map[] =
  {
    MAP ("data-sections",          OPT_fdata_sections),
    MAP ("pragma-data-sections",   OPT_mpragma_data_sections),
    MAP ("asection-data-sections", OPT_masection_data_sections),

    MAP ("function-sections",          OPT_ffunction_sections),
    MAP ("pragma-function-sections",   OPT_mpragma_function_sections),
    MAP ("asection-function-sections", OPT_masection_function_sections),

    // Undocumented
    MAP ("pragma-bss-sections",   OPT_mpragma_data_sections),
    MAP ("asection-bss-sections", OPT_masection_data_sections),

    // End of list
    MAP (NULL, 0)
  };

#undef MAP

/* Return true iff STR starts with PREFIX.  */

static bool
str_prefix_p (const char *str, const char *prefix)
{
  return 0 == strncmp (str, prefix, strlen (prefix));
}


/* "Register" a map for key, the payload of "-msection-asm-name <key>=<map>".
   This function doesn't check whether prerequisite options (e.g. like
   -fdata-sections for key "data-sections") is also set.  It checks only
   whether the syntax of "-msection-asm-name <key>=<map>" is correct, i.e.

   -- The key is valid, i.e. one of "data-sections", "function-sections", ...

   -- The mapping contains exactly one `htc_asm_name_map_id' ('+').
      ('+' is to be replaced by the object / function name whenever a named
      section of kind <key> is generated).

   -- The result of replacing '+' with any object's assembler name will yield
      a valid section name, i.e. <map> is a letter, a digit, '.', '_' or '+'.

   *DECODED is an instance of command line option -msection-asm-name.

   Return TRUE if the option is valid, which is always the case.  If the
   <key>=<map> part is actually not valid, then this function will issue an
   error and callers need not to report (a less specific) error again.  */

bool
htc_register_section_asm_name (const struct cl_decoded_option *decoded)
{
  const location_t loc = UNKNOWN_LOCATION;
  const int opt_index = (int) decoded->opt_index;
  const int opt_on = decoded->value;
  const char *const opt_arg = decoded->arg;
  const char *const opt_fulltext = decoded->orig_option_with_args_text;

  gcc_assert (opt_index == OPT_msection_asm_name);

  if (0 == strcmp (opt_arg, "strict"))
    {
      htc_section_asm_name_strict_p = opt_on;
      return true;
    }

  bool handled = false;

  for (htc_asm_name_map_t *maps = htc_asm_name_map; maps->key != NULL; maps++)
    {
      const char *key = NULL;

      // Besides the keys given in htc_asm_name_map[], also allow "all",
      // "all-code" and "all-data" for convenience.  Semantics as obvious.

      if (str_prefix_p (opt_arg, maps->key))
        key = maps->key;
      else if (str_prefix_p (opt_arg, "all-code")
               && strstr (maps->key, "function"))
        key = "all-code";
      else if (str_prefix_p (opt_arg, "all-data")
               && strstr (maps->key, "data"))
        key = "all-data";
      else if (str_prefix_p (opt_arg, "all")
               && !str_prefix_p (opt_arg, "all-code")
               && !str_prefix_p (opt_arg, "all-data"))
        key = "all";
      else
        continue;

      handled = true;

      // Store this option for later processing and diagnostics, e.g. in
      // htc-common-sections.c::htc_check_section_asm_name_required_options.

      maps->decoded = decoded;

      // Where '=' is expected in the option's argument, i.e. in <key>=<map>.

      size_t eq = strlen (key);
      int n_wildcards = 0;

      if (opt_arg[eq] != '=')
        {
          error_at (loc, "%qs: missing %qc after %qs", opt_fulltext, '=', key);
          return true;
        }

      maps->map = opt_on ? xstrdup (opt_arg + eq + 1) : NULL;

      // Allow -mno-section-asm-name <key>=* for convenience.  This will
      // install the default rule (unchanged asm name for named sections).

      if (!maps->map)
        {
          maps->map_prefix = empty;
          maps->map_suffix = empty;
          continue;
        }

      // Character-wise traverse the mapping:
      // -- Must contain exactly one asm name ('+' aka. htc_asm_name_map_id).
      // -- After replacing '+' by any valid assembler name, the resuling
      //    string must be a valid section name.  We don't allow '$' even
      //    if dollars_in_ident is set.

      for (const char *m = maps->map; *m; m++)
        {
          if (*m == htc_asm_name_map_id
              || *m == '*')
            {
              n_wildcards++;
              maps->map_prefix = xstrndup (maps->map, m - maps->map);
              maps->map_suffix = xstrdup (m + 1);
            }
          else if (htc_section_asm_name_strict_p
                   && *m != '.'
                   && !ISIDNUM (*m))
            {
              error_at (loc, "%qs: invalid mapping %qs: %qc renders resulting"
                        " section names illegal", opt_fulltext, maps->map, *m);
              return true;
            }
        }

      maps->named_p = n_wildcards != 0;

      if (n_wildcards == 0)
        {
          if (htc_section_asm_name_strict_p)
            {
              error_at (loc, "%qs: invalid mapping %qs: missing %qc",
                        opt_fulltext, maps->map, htc_asm_name_map_id);
              return true;
            }
        }
      else if (n_wildcards > 1)
        {
          error_at (loc, "%qs: invalid mapping %qs: %qc specified more than"
                    " once", opt_fulltext, maps->map, htc_asm_name_map_id);
          return true;
        }
    }

  if (handled)
    // This means we found at least one matching key in htc_asm_name_map[].
    return true;

  // No valid key has been specified:  Verbose error citing all allowed keys.

  error_at (loc, "%qs: invalid key in %qs", opt_fulltext, opt_arg);

  const char *s_keys = "";

  for (htc_asm_name_map_t *maps = htc_asm_name_map;
       maps->key != NULL && !strstr (maps->key, "bss");
       maps++)
    s_keys = ACONCAT ((s_keys, *s_keys ? ", '" : "'", maps->key, "'", NULL));

  inform (loc, "valid keys are: %s", s_keys);

  return true;
} // handle OPT_msection_asm_name
