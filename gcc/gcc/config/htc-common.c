/* Common functionalities for HighTec backends.
   Copyright (C) 2012-2014 Free Software Foundation, Inc.
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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "rtl.h"
#include "tree.h"
#include "output.h"
#include "version.h"
#include "target.h"
#include "langhooks.h"

#include "c-family/c-common.h"
#include "diagnostic-core.h"
#include "intl.h"

extern GTY(()) htc_stat_t htc_stat;
htc_stat_t htc_stat;


void
htc_init_once (void)
{
  htc_log_init();
  htc_check_section_asm_name_required_options();
}


/* Convenience wrapper for ASM_OUTPUT_ASCII from elfos.h used below.
   All supported targets are ELF targets.  */

static void
asm_output_ascii (const char *text)
{
  ASM_OUTPUT_ASCII (asm_out_file, text, strlen (text));
}


/* Implement a `print_switch_fn_type' callback worker similar to the
   -frecord-gcc-switchs or -fverbose-asm printers.
   In the assembler file, print one option per line so that it is easier
   to diff.  If TEXT is for "options enabled", we add a new line to the
   section for the same reason.  If TEXT of for switches from the command
   line, use a blank as separator so that the text in the section resembles
   an actual command line (kind of).  */

static int
htc_print_to_version_section (print_switch_type type, const char *text)
{
  switch (type)
    {
    case SWITCH_TYPE_LINE_END:
    case SWITCH_TYPE_LINE_START:
      return 0;

    case SWITCH_TYPE_DESCRIPTIVE:
      fprintf (asm_out_file, "\n");
      asm_output_ascii (ACONCAT (("\n", text, "\n", NULL)));
      return 0;

    case SWITCH_TYPE_PASSED:
      asm_output_ascii (ACONCAT ((" ", text, NULL)));
      return 0;

    case SWITCH_TYPE_ENABLED:
      asm_output_ascii (ACONCAT ((text, "\n", NULL)));
      return 0;

    default:
      return -1;
    }
}


/* Generate a .version_info section which contains the compiler
   version information, option list collected by the SWITCH_PRINTER function,
   option list passed to the GCC driver and architecture specific options.

   Some targets like powerpc want to output additional information.
   targettm.asm_out.file_start is a good place for that.

   SWITCH_PRINTER is actually toplev.c:print_switch_values() which gets
   HTC_PRINT_TO_VERSION_SECTION as a worker callback that will print the
   stuff.  */

static void
htc_print_version_info_section (void (*values_printer)(print_switch_fn_type),
                                int (*single_printer)(print_switch_fn_type,
                                                      int,
                                                      print_switch_type,
                                                      const char*),
                                const char *target_name)
{
  const char *opt;

  (void) single_printer (htc_print_to_version_section, 0,
                         SWITCH_TYPE_DESCRIPTIVE, _("compiler version: "));

  asm_output_ascii
    (ACONCAT ((lang_hooks.name, " ", pkgversion_string, "version ",
               version_string, " (", target_name, ")\n", NULL)));
  asm_output_ascii ("\n");

  (void) single_printer (htc_print_to_version_section, 0,
                         SWITCH_TYPE_DESCRIPTIVE, _("driver options: "));

  /* HTC_GCC_OPTIONS is set in gcc.c.  Individual options are separated by \n.
     Special chars like \n in the options themselves have already been quoted
     by the driver.  */

  for (opt = getenv ("HTC_GCC_OPTIONS"); opt;)
    {
      const char *cr = strchr (opt, '\n');

      if (cr)
        {
          /* Output chunk until the next \n.  */

          char *opt1 = xstrndup (opt, (size_t) (cr - opt));
          asm_output_ascii (ACONCAT ((" ", opt1, NULL)));
          opt = cr + 1;
          free (opt1);
        }
      else
        {
          asm_output_ascii (ACONCAT ((" ", opt, NULL)));
          break;
        }
    }

  /* Print all "options passed" and "options enabled" stuff.
     This is similar to  -fverbose-asm  or  -frecord-gcc-switches.  */

  values_printer (htc_print_to_version_section);
}


/* Print out the executable's checksum which is generated by the
   by the genchecksum.c for cc1 and cc1plus.
*/
static void
htc_print_checksum(void)
{
  fprintf (asm_out_file, "%s\"", ASCII_DATA_ASM_OP);

  fputs ("Compiler executable checksum: ", asm_out_file);
  for (int i = 0; i < 16; i++)
    {
      fprintf (asm_out_file, "%02x", executable_checksum[i]);
    }
  fprintf (asm_out_file, "\\n\"\n");
}


/* Types like PRINT_SWITCH_FN_TYPE are provided by target.h but we don't want
   to include target.h in htc-common.h.  Thus, supply the prototype by hand. */

extern void
htc_asm_out_version_info (void (*)(print_switch_fn_type),
                          int (*)(print_switch_fn_type, int,
                                  print_switch_type, const char*),
                          const char*);

/* Output version information to .version* sections.

   VALUES_PRINTER passes down toplev.c:print_switch_values() which is static
   there.  Similar for SINGLE_PRINTER and toplev.c:print_single_switch().
   TARGET_NAME passes down TARGET_NAME which is -D defined by Makefile.in
   for toplev.o only.  */

void
htc_asm_out_version_info (void (*values_printer)(print_switch_fn_type),
                          int (*single_printer)(print_switch_fn_type,
                                                int, print_switch_type,
                                                const char*),
                          const char *target_name)
{
  /* .version_info section as requested by -mversion-info.  */

  if (htc_opt_version_info)
    {
      fprintf (asm_out_file, ".pushsection .version_info,\"\","
               TYPE_OPERAND_FMT "\n", "note");
      htc_print_checksum ();
      htc_print_version_info_section (values_printer, single_printer,
                                      target_name);
      fprintf (asm_out_file, ".popsection\n");
    }

  /* .version.htc user-defined section from -mversion-string=<info>.  */

  if (htc_opt_version_string)
    {
      fprintf (asm_out_file, ".pushsection .version.htc,\"\","
               TYPE_OPERAND_FMT "\n", "note");
      ASM_OUTPUT_LIMITED_STRING (asm_out_file, htc_opt_version_string);
      fprintf (asm_out_file, ".popsection\n");
    }
}

/* Set htc_opt_error_numbers variable based on command line options.
   The handling of error-numbers options should happen earlier than the
   regular option handling because its value needed for the libintl
   initialization.  */
void
htc_set_error_numbers (int argc, char **argv)
{
  int i;

  for (i = 0; i < argc; i++)
    {
      if (strcmp (argv[i], "-ferror-numbers") == 0)
        htc_opt_error_numbers = true;

      if (strcmp (argv[i], "-fno-error-numbers") == 0)
        htc_opt_error_numbers = false;
    }
}
#include "gt-htc-common.h"
