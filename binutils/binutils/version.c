/* version.c -- binutils version information
   Copyright 1991, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
   2007 Free Software Foundation, Inc.

   This file is part of GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "bucomm.h"

#ifndef HTC_SUPPORT
/* Print the version number and copyright information, and exit.
   This implements the --version option for the various programs.  */

void
print_version (const char *name)
{
  /* This output is intended to follow the GNU standards document.  */
  /* xgettext:c-format */
  printf ("GNU %s %s\n", name, BFD_VERSION_STRING);
  printf (_("Copyright 2007 Free Software Foundation, Inc.\n"));
  printf (_("\
This program is free software; you may redistribute it under the terms of\n\
the GNU General Public License version 3 or (at your option) any later version.\n\
This program has absolutely no warranty.\n"));
  exit (0);
}
#else /* HTC_SUPPORT */
#include "../htc/htc_support.h"

/* Print the version number and copyright information for binutil NAME, and
   exit.  This implements the --version option for the various programs.  */

void
print_version (name)
     const char *name;
{
#if defined(HTC_PRINT_TOOLVERSION)
  HTC_PRINT_TOOLVERSION(name);
#else
  char *toolversion = NULL;

  if ((*name == 'a') && !strcmp (name, "as"))
    toolversion = TOOL_VERSION_GAS;
  else if ((*name == 'a') && !strcmp (name, "ar"))
    toolversion = TOOL_VERSION_AR;
  else if ((*name == 'a') && !strcmp (name, "addr2line"))
    toolversion = TOOL_VERSION_ADDR2LINE;
  else if ((*name == 'l') && !strcmp (name, "ld"))
    toolversion = TOOL_VERSION_LD;
  else if ((*name == 'n') && !strcmp (name, "nm"))
    toolversion = TOOL_VERSION_NM;
  else if ((*name == 'o') && !strcmp (name, "objcopy"))
    toolversion = TOOL_VERSION_OBJCOPY;
  else if ((*name == 'o') && !strcmp (name, "objdump"))
    toolversion = TOOL_VERSION_OBJDUMP;
  else if ((*name == 'r') && !strcmp (name, "ranlib"))
    toolversion = TOOL_VERSION_RANLIB;
  else if ((*name == 'r') && !strcmp (name, "readelf"))
    toolversion = TOOL_VERSION_READELF;
  else if ((*name == 's') && !strcmp (name, "size"))
    toolversion = TOOL_VERSION_SIZE;
  else if ((*name == 's') && !strcmp (name, "strings"))
    toolversion = TOOL_VERSION_STRINGS;
  else if ((*name == 's') && !strcmp (name, "strip"))
    toolversion = TOOL_VERSION_STRIP;

  printf (_("GNU %s %s (%s)"), name, BFD_VERSION_STRING, HTC_ARCH_NAME);
  if (toolversion)
    printf (" %s %s", TOOL_VERSION, toolversion);
  printf(_(" using BFD version %s\n"), BFD_BUILD_STRING);
#endif
  printf (HTC_COPYRIGHT);

  exit (0);
}
#endif /* HTC_SUPPORT */
