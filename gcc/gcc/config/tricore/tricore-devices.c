/* Copyright 2005-2014 Free Software Foundation, Inc.
   This file is part of GCC.

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
#include "diagnostic.h"
#include <stdlib.h>

#define STREQ(A, B) (0 == strcmp (A, B))

tric_erratum_t tric_errata[] =
{
#define DEF_ERRATA(ARG, CC, AS, DEFIN, ON, IFX_NAME)    \
  { #ARG, CC, AS, DEFIN, -1, IFX_NAME },
#include "errata.def"
#undef DEF_ERRATA
  { NULL, 0, 0, NULL, 0, NULL }
};

const tric_core_t *tric_core;

const tric_core_t tric_cores[] =
  {
    /* .name, .id, .libname  */
    {"13",    0x130,  "tc13"  },
    {"131",   0x131,  "tc131" },
    {"16",    0x160,  "tc16"  },
    {"161",   0x161,  "tc161" },
    {"162",   0x162,  "tc162" },
    { NULL, 0, NULL }
  };

const tric_device_t *tric_device;

const tric_device_t tric_devices[] =
  {
#define DEF_DEVICE(NAME, TARGET_NAME, CORE, LD_MCPU)    \
    { NAME, TARGET_NAME, CORE, LD_MCPU },
#include "devices.def"
#undef DEF_DEVICE
    { NULL, NULL, NULL, NULL }
  };


/* Set tric_core according to NAME.  NAME is something that follows -mtc
   like "13", "161", etc.  */

void
tric_set_core (const char *name)
{
  const tric_core_t *core;

  gcc_assert (name);

  for (core = tric_cores; core->name != NULL; core++)
    {
      if (!STREQ (name, core->name))
        continue;

      /* Found the core */
          
      if (tric_device
          && !STREQ (tric_device->core_mtc, name))
        {
          error ("cannot use options %<-mcpu=%s%> and %<-mtc%s%> together",
                 tric_device->name, name);
        }
      else if (tric_core
          && !STREQ (tric_core->name, name))
        {
          error ("cannot use options %<-mtc%s%> and %<-mtc%s%> together",
                 tric_core->name, name);
        }
      else
        {
          /* Get core information from tric_cores[].  */

          tric_core = core;
        }

      break;
    }
}


/* Set tric_device according to NAME.  NAME is something that follows
   -mcpu= like "tc1796" etc.  Also set tric_core accordingly.

   If NAME == NULL and either tric_core or tric_device are set, do nothing.
   If neither tric_core nor tric_device are set, do the same as
   for "tc1796".  */

void
tric_set_device (const char *name)
{
  const tric_device_t *device;

  if (name == NULL)
    {
      if (tric_core || tric_device)
        return;

      name = tric_devices[0].name;
    }

  for (device = tric_devices; device->name != NULL; device++)
    {
      if (!STREQ (name, device->name))
        continue;

      /* Found the device */
          
      if (tric_device
          && !STREQ (tric_device->name, name))
        {
          error ("cannot use options %<-mcpu=%s%> and %<-mcpu=%s%> together",
                 tric_device->name, name);
        }
      else if (tric_core
               && !STREQ (tric_core->name, device->core_mtc))
        {
          error ("cannot use options %<-mtc%s%> and %<-mcpu=%s%> together",
                 tric_core->name, name);
        }
      else
        {
          tric_device = device;

          if (!tric_core)
            tric_set_core (device->core_mtc);
        }

      break;
    }
}
