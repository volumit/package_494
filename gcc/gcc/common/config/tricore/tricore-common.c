/* TriCore common hooks.
   Copyright (C) 2014 Free Software Foundation, Inc.

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
#include "diagnostic.h"
#include "tm.h"
#include "tm_p.h"
#include "rtl.h"
#include "tree.h"
#include "langhooks.h"
#include "common/common-target.h"
#include "common/common-target-def.h"
#include "opts.h"

int tric_zdata_max;
int tric_zdata_min;
int tric_zconst_max;
int tric_zconst_min;
int tric_sdata_max;
int tric_sdata_min;
int tric_sconst_max;
int tric_sconst_min;

struct tric_segment_trap tric_segment_trap;

void
tric_decode_msegment_trap (const char *args0, const char *opt_text)
{
  static const struct
  {
    int per_default;
    const char* key;
    int *pval;
  }
  keys[] =
    {
      { 0, "all", NULL, },
      { 0, "none", NULL },
      { 0, "default", NULL },
      { 1, "pre-inc", &tric_segment_trap.have_pre_inc },
      { 1, "pre-dec", &tric_segment_trap.have_pre_dec },
      { 1, "ivopts", &tric_segment_trap.do_ivopts },
      { 0, "ivopts-base-costs", &tric_segment_trap.do_ivopts_base_costs },
      { 0, "ivopts-use-address", &tric_segment_trap.do_ivopts_use_address },
      { 0, "sched-change-address", &tric_segment_trap.do_sched_change_address }
    };

  bool bug = false;
  bool all = NULL != strstr (args0, "all");
  bool deflt = NULL != strstr (args0, "default");

  char *valid_args = xstrdup ("");

  /* Initialize TRIC_SEGMENT_TRAP by applying "all" and "default".
     Also cook up VALID_ARGS, a string of all known keys for the case
     where we have to issue a diagnostic because of unknown key(s).  */

  for (size_t i = 0; i < ARRAY_SIZE (keys); i++)
    {
      if (keys[i].pval)
        *(keys[i].pval) = all || (deflt && keys[i].per_default);

      valid_args = reconcat (valid_args, valid_args, ", ",
                             keys[i].pval ? "[no-]" : "", keys[i].key, NULL);
    }

  /* Traverse the arguments, split into comma-separated chunks.  */

  for (char *arg = xstrdup (args0); arg; )
    {
      char *end = strchr (arg, ',');

      if (end)
        *end = '\0';

      /* For each chunk, traverse KEYS[] searching for ARG.  */

      for (size_t i = 0;; i++)
        {
          if (i == ARRAY_SIZE (keys))
            {
              // Not found:  ARG is invalid.
              error_at (UNKNOWN_LOCATION, "invalid argument %qs to option %qs",
                        arg, opt_text);
              bug = true;
              break;
            }
          else if (0 == strcmp (arg, keys[i].key))
            {
              // Found: Apply ARG and finish traversing KEYS[].
              if (keys[i].pval)
                *(keys[i].pval) = 1;
              break;
            }
          else if (keys[i].pval
                   && 0 == strncmp (arg, "no-", 3)
                   && 0 == strcmp (arg + 3, keys[i].key))
            {
              // Found the no-variant of ARG.
              *(keys[i].pval) = 0;
              break;
            }
        } // for keys[]

      arg = end ? end + 1 : NULL;
    }

  if (bug)
    inform (UNKNOWN_LOCATION, "known arguments to option %qs are: %s.",
            opt_text, valid_args + 2 /* "+2" skips first ', ' */);
  free (valid_args);
}


static void
tric_opt_error (int value, const char *op1, const char *op2, location_t loc)
{
  if (value)
    error_at (loc, "Cannot use options %qs and %qs simultaneously", op1, op2);
}


/* Implement `TARGET_HANDLE_OPTION'  */

static bool
tric_handle_option (struct gcc_options *opts ATTRIBUTE_UNUSED,
                    struct gcc_options *opts_set ATTRIBUTE_UNUSED,
                    const struct cl_decoded_option *decoded, location_t loc)
{
  size_t code = decoded->opt_index;
  int value = decoded->value;
  const char *arg = decoded->arg;

  switch (code)
    {
    case OPT_mabs_:
      if (0 == value)
        tric_opt_mabs = INT_MAX;
      else
        tric_opt_error (tric_opt_msmall_pid, "-mabs=", "-msmall-pid", loc);

      tric_zdata_max = tric_zconst_max = tric_opt_mabs;
      return true;

    case OPT_mabs_data_:
      if (0 == value)
        tric_opt_mabs_data = INT_MAX;
      else
        tric_opt_error (tric_opt_msmall_pid, "-mabs-data=", "-msmall-pid", loc);

      tric_zdata_max = tric_opt_mabs_data;
      return true;

    case OPT_mabs_const_:
      if (0 == value)
        tric_opt_mabs_const = INT_MAX-1;
      else
        tric_opt_error (tric_opt_msmall_pid, "-mabs-const=", "-msmall-pid",
                        loc);

      tric_zconst_max = tric_opt_mabs_const;
      return true;

    case OPT_msmall_:
      if (0 == value)
        tric_opt_msmall = INT_MAX-1;
      else
        tric_opt_error (tric_opt_msmall_pid, "-msmall=", "-msmall-pid", loc);

      tric_sdata_max = tric_sconst_max = tric_opt_msmall;
      return true;

    case OPT_msmall_data_:
      if (0 == value)
        tric_opt_msmall_data = INT_MAX-1;
      else
        tric_opt_error (tric_opt_msmall_pid, "-msmall-data=", "-msmall-pid",
                        loc);

      tric_sdata_max = tric_opt_msmall_data;
      return true;

    case OPT_msmall_const_:
      if (0 == value)
        tric_opt_msmall_const = INT_MAX-1;
      else
        tric_opt_error (tric_opt_msmall_pid, "-msmall-const=", "-msmall-pid",
                        loc);

      tric_sconst_max = tric_opt_msmall_const;
      return true;
      
    case OPT_msmall_pid:
      tric_sdata_max = tric_sconst_max = INT_MAX-1;

      tric_opt_error (tric_opt_msmall && tric_opt_msmall != INT_MAX-1,
                      "-msmall-pid", "-msmall=", loc);
      tric_opt_error (tric_opt_msmall_data && tric_opt_msmall_data != INT_MAX-1,
                      "-msmall-pid", "-msmall-data=", loc);
      tric_opt_error (tric_opt_msmall_const
                      && tric_opt_msmall_const != INT_MAX-1,
                      "-msmall-pid", "-msmall-const=", loc);
      tric_opt_error (tric_opt_mabs,       "-msmall-pid", "-mabs=", loc);
      tric_opt_error (tric_opt_mabs_data,  "-msmall-pid", "-mabs-data=", loc);
      tric_opt_error (tric_opt_mabs_const, "-msmall-pid", "-mabs-const=", loc);
      return true;

    case OPT_msection_asm_name:
      return htc_register_section_asm_name (decoded);

    case OPT_merrata_:
      tric_errata[value].fixit = 1;
      return true;

    case OPT_mno_errata_:
      tric_errata[value].fixit = 0;
      return true;

    case OPT_mcpu_:
      tric_set_device (arg);
      return true;
      
    case OPT_mtc:
      tric_set_core (arg);
      return true;

    case OPT_msegment_trap_:
      tric_decode_msegment_trap (arg, cl_options[code].opt_text);
      return true;

    default:
      return true;
    }
}


/* Implement `TARGET_OPTION_OPTIMIZATION_TABLE' */

static const struct default_options
tric_option_optimization_table[] =
  {
    { OPT_LEVELS_2_PLUS, OPT_fomit_frame_pointer, NULL, 1 },
    { OPT_LEVELS_SIZE, OPT_mfast_div, NULL, 0 },
    { OPT_LEVELS_ALL, OPT_fira_algorithm_, NULL, IRA_ALGORITHM_PRIORITY },
    /* EABI 2.1.4.3 requires plain int bit-fields to be unsigned.
       The EABI v2.5 does not say if this also covers `char' and `short'
       bit-fields and if such bitfields are legitimate at all, see
       C99 6.7.2.1 Clause 4.  The easiest way to satisfy EABI 2.1.4.3 is
       to enable -funsigned-bitfields per default like 3.x did and
       thereby apply the same signdness rules to `char' and `short'.  */
    { OPT_LEVELS_ALL, OPT_funsigned_bitfields, NULL, 1 },
    { OPT_LEVELS_NONE, 0, NULL, 0 }
  };


/* Implement `TARGET_OPTION_INIT_STRUCT' */

static void
tric_option_init_struct (struct gcc_options *opts ATTRIBUTE_UNUSED)
{
  flag_no_common = 1;

  /* EABI 2.1.2 leaves some room for interpretation, e.g. if enums always
     were 4 bytes it would also comply with 2.1.2 because that clause does
     not force to use the /smallest/ size.  We use short enums in order to
     be compatible with 3.4.  Note, however, that GCC will use a size of
     1 byte for an enum in the range 0..255 so that it cannot be signed as
     required by the EABI.  */

  flag_short_enums = 1;

  flag_aggressive_loop_optimizations = 0;
}


#undef  TARGET_HANDLE_OPTION
#define TARGET_HANDLE_OPTION tric_handle_option

#undef  TARGET_OPTION_OPTIMIZATION_TABLE
#define TARGET_OPTION_OPTIMIZATION_TABLE tric_option_optimization_table

#undef  TARGET_OPTION_INIT_STRUCT
#define TARGET_OPTION_INIT_STRUCT tric_option_init_struct

#undef  TARGET_HAVE_NAMED_SECTIONS
#define TARGET_HAVE_NAMED_SECTIONS true

struct gcc_targetm_common targetm_common = TARGETM_COMMON_INITIALIZER;
