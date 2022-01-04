/* Functionalities for HighTec licenser.
   Copyright (C) 2014 Free Software Foundation, Inc.
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
#include "diagnostic.h"
#include "intl.h"
#include <libiberty.h>

#include "tm.h"
#include "htc-licenser.h"

#ifndef LICENSER_PRG
#error "No licenser application defined (LICENSER_PRG)"
#endif

#ifndef LICENSE_PRODUCT
#error "No product defined (LICENSE_PRODUCT)"
#endif

#define X_STR(x) #x
#define STR(x) X_STR(x)

/* Align these results with the licenser counterpart */
enum Result {
  Ok = 109, /* 0b01101101 */
  License_Fail = 94 /* 0b01011110 */
};

static char *htc_licenser_dir = NULL;

/* Initialize the arguments for the subprocess call. */
static char**
arguments_prepare (int checksum)
{
  char **argv = (char**) xmalloc (8 * sizeof(char*));

  /* application */
  if (htc_licenser_dir)
    argv[0] = concat (htc_licenser_dir, "/", STR(LICENSER_PRG), NULL);
  else
    argv[0] = xstrdup (STR(LICENSER_PRG));

  argv[1] = xstrdup ("-p" STR(LICENSE_PRODUCT)); /* feature */
  argv[2] = xstrdup ("-v" LICENSE_VERSION);      /* version */
  argv[3] = xstrdup ("-b" BASEVER);              /* base version */

  argv[4] = (char*) xmalloc (6);                 /* retry count */
  snprintf (argv[4], 5, "-r%d", htc_opt_license_retries);

  argv[5] = (char*) xmalloc (7);
  snprintf (argv[5], 6, "-c%d", checksum);

  /* license dir */
  if (htc_license_dir)
    argv[6] = concat ("-l", htc_license_dir, NULL);
  else
    argv[6] = NULL;

  argv[7] = NULL;

  return argv;
}


/* Free up the arguments of the subprocess. */
static void
arguments_free (char **argv)
{
  char **arg;
  for (arg = argv; *arg; ++arg)
    free (*arg);

  free (argv);
}

/* Check the subprocess' return code to see if the license is valid. */
static bool
check_subprocess (struct pex_obj *pex, int checksum)
{
  int exit_status;
  if (!pex_get_status (pex, 1, &exit_status))
    {
      error ("could not query licenser");
      return false;
    }

  if ((WEXITSTATUS(exit_status) ^ checksum) != Ok)
    {
      error ("error in licenser");
      return false;
    }

  /* The process exited correctly. */
  return true;
}


void
set_htc_licenser (const char *argv0, const char *progname)
{
  char *licenser_path = getenv ("HTC_LICENSER_DIR");
  if (licenser_path)
    htc_licenser_dir = licenser_path;
  else
    {
      /* Get the path from the argv0.  */
      htc_licenser_dir = xstrndup(argv0, strlen (argv0) - strlen (progname));
    }
}


void
do_htc_license_check (void)
{
  const int pex_flags = PEX_SEARCH | PEX_USE_PIPES;
  int checksum;
  char **argv;
  struct pex_obj *pex;
  int err;

  srand (time (0));

  checksum = rand () % 253 + 1;

#ifdef _WIN32 || _WIN64
  if (checksum == 110)
    checksum = 111;
#endif

  argv = arguments_prepare (checksum);

  pex = pex_init (pex_flags, argv[0], NULL);
  if (!pex)
    {
      error ("could not initialize licenser");
      return;
    }

  if (pex_run (pex, pex_flags, argv[0], argv, NULL, NULL, &err) != NULL)
    {
      error ("could not execute licenser");
      pex_free (pex);
      return;
    }

  if (!check_subprocess (pex, checksum))
    error ("license check failed");

  arguments_free (argv);
  pex_free (pex);
}


