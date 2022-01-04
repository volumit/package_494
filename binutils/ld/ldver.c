/* ldver.c -- Print linker version.
   Copyright 1991, 1992, 1993, 1994, 1995, 1996, 1997, 2000, 2001, 2002,
   2003, 2007 Free Software Foundation, Inc.

   This file is part of the GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "bfdver.h"

#include "ld.h"
#include "ldver.h"
#include "ldexp.h"
#include "ldlang.h"
#include "ldfile.h"
#include "ldemul.h"
#include "ldmain.h"

#ifdef HTC_SUPPORT
#include "../htc/htc_support.h"
#include "xml.h"
#include "xml_defines.h"

void htc_ldversion(int);
void
htc_ldversion (int noisy)
{
#if defined(HTC_PRINT_TOOLVERSION)
  HTC_PRINT_TOOLVERSION("ld");
#else
  printf (_("GNU ld %s (%s)"), BFD_VERSION_STRING, HTC_ARCH_NAME);
#ifdef TOOL_VERSION_LD
  printf (" %s %s", TOOL_VERSION, TOOL_VERSION_LD);
#endif
  printf(_(" using BFD version %s\n"), BFD_BUILD_STRING);
#endif
  if (noisy & 2)
    printf (HTC_COPYRIGHT);

  if (noisy & 1)
    {
      ld_emulation_xfer_type **ptr = ld_emulations;

      printf (_("  Supported emulations:\n"));
      while (*ptr)
    {
      printf ("   %s\n", (*ptr)->emulation_name);
      ptr++;
    }
    }
}

void
ldversion (int noisy)
{

  htc_ldversion(noisy);
}

void
htc_print_ld_version (FILE *out)
{
  fprintf (out, _("GNU ld version %s (%s) using BFD version %s"),
  	   BFD_VERSION_STRING, HTC_ARCH_NAME, BFD_VERSION_STRING);
  fprintf (out, " (%s)", BFD_BUILD_STRING);
#ifdef TOOL_VERSION_LD
  fprintf (out, ", %s %s", TOOL_VERSION, TOOL_VERSION_LD);
#endif
  fprintf (out, "\n");
}

void
htc_get_ld_version_xml (p_xml_element root)
{
  p_xml_element ld_version_info = create_xml_element(XML_E_LINKER_VERSION);

  add_xml_attribute (ld_version_info, XML_A_VERSION_STRING, BFD_VERSION_STRING);
  add_xml_attribute (ld_version_info, XML_A_ARCHITECTURE_NAME, HTC_ARCH_NAME);
  add_xml_attribute (ld_version_info, XML_A_BUILD_STRING, BFD_BUILD_STRING);
 
#ifdef TOOL_VERSION_LD
  add_xml_attribute (ld_version_info, XML_A_TOOL_VERSION, TOOL_VERSION_LD);
#endif

  add_xml_child (root, ld_version_info);
}

#else /* HTC_SUPPORT */

void
ldversion (int noisy)
{
  /* Output for noisy == 2 is intended to follow the GNU standards.  */
  fprintf (stdout, _("GNU ld %s\n"), BFD_VERSION_STRING);

  if (noisy & 2)
    {
      printf (_("Copyright 2009 Free Software Foundation, Inc.\n"));
      printf (_("\
This program is free software; you may redistribute it under the terms of\n\
the GNU General Public License version 3 or (at your option) a later version.\n\
This program has absolutely no warranty.\n"));
    }

  if (noisy & 1)
    {
      ld_emulation_xfer_type **ptr = ld_emulations;

      printf (_("  Supported emulations:\n"));
      while (*ptr)
	{
	  printf ("   %s\n", (*ptr)->emulation_name);
	  ptr++;
	}
    }
}
/* Print the version of the linker to file OUT.  */

extern void print_ld_version ( FILE *out);
void
print_ld_version ( FILE *out)
{
  fprintf (out, "GNU ld version %s\n", BFD_VERSION_STRING);
}
#endif /* HTC_SUPPORT */
