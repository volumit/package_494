/* This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "output.h"
#include "input.h"
#include "function.h"
#include "tm_p.h"
#include "tree-pass.h"
#include "target.h"
#include "print-tree.h"

/* This file supplies some functions for HighTec back-end developers
   with a printf-like interface.  The functions are called through
   macros htc_edump or htc_fdump from htc-target-log.h:

      htc_edump (const char *fmt, ...);

      htc_fdump (FILE *stream, const char *fmt, ...);

   htc_edump (fmt, ...) is a shortcut for htc_fdump (stderr, fmt, ...)
   htc_dump (fmt, ...) is a shortcut for htc_fdump (dump_file, fmt, ...)

  == known %-codes ==

  r: rtx
  t: tree
  T: tree (brief)
  C: enum rtx_code
  i: HOST_WIDE_INT (signed decimal)
  m: enum machine_mode
  R: enum reg_class
  D: double_int (signed decimal)
  H: location_t

  == no arguments ==

  A: call abort()
  f: current_function_name()
  F: caller (via __FUNCTION__)
  P: Pass name and number
  ?: Print caller, current function and pass info
  !: Ditto, but only print if in a pass with static pass number,
     else return.

  == same as printf ==

  %: %
  c: char
  s: string
  d: int (decimal)
  p: void*
  x: unsigned int (hex)
*/

/* Set according to -mlog= option.  */
htc_log_t htc_log;

/* The caller as of __FUNCTION__ */
static const char *htc_dump_caller = "?";

/* The worker function implementing the %-codes */
static void htc_dump_valist (FILE*, const char*, va_list);

/* As we have no variadic macros, htc_edump maps to a call to
   htc_dump_set_caller_e which saves __FUNCTION__ to htc_dump_caller and
   returns a function pointer to htc_log_fdump_e.  htc_log_fdump_e
   gets the printf-like arguments and calls htc_dump_valist, the
   worker function.  htc_dump and htc_fdump work the same way.  */

/* Provide htc_log_fdump_d/e/f so that htc_log_set_caller_d/e/f can return
   their address.  */

static int
htc_log_fdump_d (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  if (dump_file)
    htc_dump_valist (dump_file, fmt, ap);
  va_end (ap);

  return 1;
}

static int
htc_log_fdump_e (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  htc_dump_valist (stderr, fmt, ap);
  va_end (ap);

  return 1;
}

static int
htc_log_fdump_f (FILE *stream, const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  if (stream)
    htc_dump_valist (stream, fmt, ap);
  va_end (ap);

  return 1;
}

/* Macros htc_edump/htc_fdump map to calls of the following two functions,
   respectively.  You don't need to call them directly.  */

int (*
htc_log_set_caller_d (const char *caller)
     )(const char*, ...)
{
  htc_dump_caller = caller;

  return htc_log_fdump_d;
}

int (*
htc_log_set_caller_e (const char *caller)
     )(const char*, ...)
{
  htc_dump_caller = caller;

  return htc_log_fdump_e;
}

int (*
htc_log_set_caller_f (const char *caller)
     )(FILE*, const char*, ...)
{
  htc_dump_caller = caller;

  return htc_log_fdump_f;
}


/* Worker function implementing the %-codes and forwarding to
   respective print/dump function.  */

static void
htc_dump_valist (FILE *file, const char *fmt, va_list ap)
{
  const char *pending_quote = NULL;
  char bs[3] = {'\\', '?', '\0'};

  while (*fmt)
    {
      switch (*fmt++)
        {
        default:
          fputc (*(fmt - 1), file);
          break;

        case '\\':
          bs[1] = *fmt++;
          fputs (bs, file);
          break;

        case '%':

          if ('q' == fmt[0] && '\0' != fmt[1])
            {
              fputs ("'", file);
              pending_quote = "'";
              fmt++;
            }
          else if ('Q' == fmt[0] && '\0' != fmt[1])
            {
              fputs ("\"", file);
              pending_quote = "\"";
              fmt++;
            }

          switch (*fmt++)
            {
            case '%':
              fputc ('%', file);
              break;

            case 't':
              {
                tree t = va_arg (ap, tree);
                if (NULL_TREE == t)
                  fprintf (file, "<NULL-TREE>");
                else
                  {
                    if (stderr == file)
                      debug_tree (t);
                    else
                      {
                        print_node (file, "", t, 0);
                        putc ('\n', file);
                      }
                  }
                break;
              }

            case 'T':
              print_node_brief (file, "", va_arg (ap, tree), 3);
              break;

            case 'p':
              fprintf (file, "%p", va_arg (ap, const void*));
              break;

            case 'd':
              fprintf (file, "%d", va_arg (ap, int));
              break;

            case 'D':
              dump_double_int (file, va_arg (ap, double_int), false);
              break;

            case 'x':
              fprintf (file, "%x", va_arg (ap, unsigned));
              break;

            case 'i':
              fprintf (file, HOST_WIDE_INT_PRINT_DEC,
		       va_arg (ap, HOST_WIDE_INT));
              break;

            case 'c':
              fputc (va_arg (ap, int), file);
              break;

            case 'r':
              print_inline_rtx (file, va_arg (ap, rtx), 0);
              break;

            case 'f':
              if (cfun && cfun->decl)
                fputs (current_function_name(), file);
              break;

            case 's':
              {
                const char *str = va_arg (ap, char*);
                fputs (str ? str : "(null)", file);
              }
              break;

            case 'm':
              fputs (GET_MODE_NAME (va_arg (ap, int)), file);
              break;

            case 'C':
              fputs (rtx_name[va_arg (ap, int)], file);
              break;

            case 'R':
              fputs (reg_class_names[va_arg (ap, int)], file);
              break;

            case 'F':
              fputs (htc_dump_caller, file);
              break;

            case 'H':
              {
                location_t loc = va_arg (ap, location_t);

                if (BUILTINS_LOCATION == loc)
                  fprintf (file, "<BUILTIN-LOCATION>");
                else if (UNKNOWN_LOCATION == loc)
                  fprintf (file, "<UNKNOWN-LOCATION>");
                else
                  {
                    const char *s_file = LOCATION_FILE (loc);
                    if (strrchr (s_file, DIR_SEPARATOR))
                      s_file = 1 + strrchr (s_file, DIR_SEPARATOR);
                    fprintf (file, "%s:%d",
                             s_file, LOCATION_LINE (loc));
                  }

                break;
              }

            case '!':
              if (!current_pass)
                return;
              /* FALLTHRU */

            case '?':
              htc_log_fdump_f (file, "%F[%f:%P]");
              break;

            case 'P':
              if (current_pass)
                fprintf (file, "%s(%d)",
                         current_pass->name,
                         current_pass->static_pass_number);
              else
                fprintf (file, "pass=?");

              break;

            case 'A':
              fflush (file);
              abort();

            default:
              {
                /* Unknown %-code: Dispatch to back end.  */

                const char *const pos = fmt - 1;
                int n_consumed = targetm.htc.dump_valist (file, pos, ap);

                if (n_consumed > 0)
                  {
                    fmt = pos + n_consumed;
                  }
                else
                  {
                    /* If the back end is also clueless:  */
                    fprintf (file, "??? %%%c ???%s\n", *pos, fmt);
                    pending_quote = NULL;
                    /* Stop output */
                    fmt = "";
                  }
              }

              break;
            } /* *fmt++ */

          if (pending_quote)
            {
              fputs (pending_quote, file);
              pending_quote = NULL;
            }

          break; /* % */
        }
    }

  fflush (file);
}


/* Called from htc.c:htc_option_override().
   Parse argument of -mlog= and set respective fields in htc_log.  */

void
htc_log_init (void)
{
  if (htc_opt_log)
    {
      /* Adding , at beginning and end of string makes searching easier.  */

      const char *str = ACONCAT ((",", htc_opt_log, ",", NULL));
      bool all = NULL != strstr (str, ",all,");
      bool info = NULL != strstr (str, ",?,");

      if (info)
        fprintf (stderr, "\n-mlog=?");

#define DEF_HTC_LOG(KEY)                                              \
      htc_log.KEY = (all                                              \
                     || NULL != strstr (str, "," #KEY ","));          \
      if (info)                                                       \
        fprintf (stderr, "," #KEY);
#include "htc-target-log.def"
#undef DEF_HTC_LOG

      if (info)
        fprintf (stderr, "\n\n");
    }
}
