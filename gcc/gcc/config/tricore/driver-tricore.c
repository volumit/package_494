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
#include "gcc.h"
#include "diagnostic.h"
#include <stdlib.h>


/* Implement extra spec function `device_to_startfile' */
/* Device and core are already set by tric_self_specs.  */

const char*
tric_device_to_startfile (int argc ATTRIBUTE_UNUSED,
                          const char **argv ATTRIBUTE_UNUSED)
{
  const char *cpu = tric_device ? tric_device->name : "no-device-specified";
  const char *crt0 = concat ("crt0-", cpu, "%O%s", NULL);
  const char *memoryx = concat ("memory-", cpu, ".x%s", NULL);

  return
    concat ("%{!shared:%{pg:g", crt0, "}",
            "  %{!pg:%{p:%{!T*:-T ", memoryx, "} m", crt0, "}",
            "        %{!p:%{!T*:-T ", memoryx, "} %{!nocrt0:", crt0, "}}",
            "   }",
            " } ", NULL);
}


/* Implement extra spec function `device_to_ld' */
/* Device and core are already set by tric_self_specs.  */

const char*
tric_device_to_ld (int argc ATTRIBUTE_UNUSED,
                   const char **argv ATTRIBUTE_UNUSED)
{
  return tric_device
    ? concat ("--mcpu=", tric_device->ld_mcpu, NULL)
    : concat ("--mcpu=tc", tric_core->name, NULL);
}


/* Implement extra spec function `device_to_as' */
/* Device and core are already set by tric_self_specs.  */

const char*
tric_device_to_as (int argc ATTRIBUTE_UNUSED,
                   const char **argv ATTRIBUTE_UNUSED)
{
  char *opts
    = concat (" -mtc", tric_core ? tric_core->name : tric_cores[0].name, NULL);

  /* Adjust errata options for the assembler because it does not (yet)
     support -merrata=<bug> and supports less errata than the compiler.  */

#define DEF_ERRATA(ARG, CC1, AS, DEFIN, ON, IFX_NAME)                   \
  if (AS)                                                               \
    opts = ON                                                           \
      ? reconcat (opts, opts, " %{!mno-errata=" #ARG ":-m" #ARG "} ", NULL) \
      : reconcat (opts, opts, " %{merrata=" #ARG ":-m" #ARG "} ", NULL);
#include "errata.def"
#undef DEF_ERRATA

  return opts;
}


/* Implement driver self spec function `tric_self_specs' */
/* Patch the command line options in order to fake-support the
   deprecated  -m<bug>  and  -mno-<bug>  options.  Goal is that
   these options are cleaned up altogether one day.  Until then,
   they are mapped to  -merrata=<bug>  resp.  -mno-errata=<bug>.

   This function sets `tric_core' and `tric_device' which are used by:

   -  tric_device_to_startfile
   -  tric_device_to_as
   -  tric_device_to_ld

   from above so that these functions need not to receive or
   (re)evaluate any options.  */

const char*
tric_self_specs (int argc, const char **argv)
{
  char *spec = NULL;
  int i;

  /* We need device / core information for errata defaults.  */

  for (i = 0; i < argc; i++)
    {
      if (0 == strncmp (argv[i], "mcpu=", strlen ("mcpu=")))
        tric_set_device (argv[i] + strlen ("mcpu="));

      if (0 == strncmp (argv[i], "mtc", strlen ("mtc")))
        tric_set_core (argv[i] + strlen ("mtc"));
    }

  tric_set_device (NULL);

  /* Map  -m[no-]<bug>  to  -m[no-]errata=<bug>  and then clean up
     the command options from  -m[no-]<bug>.  */

#define DEF_ERRATA(ARG, CC1, AS, DEFIN, ON, IFX_NAME)                   \
  spec = ON                                                             \
    ? reconcat (spec, spec,                                             \
                " %{mno-" #ARG ":-mno-errata=" #ARG "} ", NULL)         \
    : reconcat (spec, spec,                                             \
                " %{m" #ARG ":-merrata=" #ARG "} ", NULL);              \
                                                                        \
  spec = reconcat (spec, spec, " %<m" #ARG, " %<mno-" #ARG, NULL);
#include "errata.def"
#undef DEF_ERRATA

  return spec;
}
