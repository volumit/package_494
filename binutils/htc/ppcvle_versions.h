/* tricore_versions.h -- Definitions for release number and tool versions.

   Copyright (C) 2010 HighTec EDV-Systeme GmbH.  All rights reserved.  */

#ifndef __PPCVLE_VERSIONS_H
#define __PPCVLE_VERSIONS_H

#define HTC_ARCH_NAME "ppc-vle"

/* Define intl'd strings for copyright notice.  */
#define HTC_COPYRIGHT _("\
Copyright (C) Free Software Foundation, Inc.\n\
This program is free software; you may redistribute it under the terms of\n\
the GNU General Public License version 3 or (at your option) any later version.\n\
This program has absolutely no warranty.\n\
The package was build based on Source Code from HighTec EDV-Systeme GmbH.\n")

/* Version for building unique tag names like PPCLVE_VN_M */
#define TOOL_VERSION_BINUTILS "Version 4.1"

/* Define intl'ed string for tool version.  */
#define TOOL_VERSION _("Tool Version")

/* Define the BFD build number.  Whenever it is changed, all binutils need
   to be re-built: rm gas/tc-tricore.o ld/ldver.o binutils/version.o; make.  */

#define BFD_BUILD_STRING "v2.4"

/* Define the current tool version numbers.
   If any of these numbers are changed, cd to the configuration/build
   directory, delete the files gas/tc-ppcvle.o (assembler), ld/ldver.o
   (linker) and/or binutils/version.o (all other binutils), then start
   make (or make install) to re-build (and install) the tools.  */

#define TOOL_VERSION_ADDR2LINE	"v2.0"
#define TOOL_VERSION_AR		"v2.0"
#define TOOL_VERSION_GAS	"v2.1"
#define TOOL_VERSION_LD		"v2.1"
#define TOOL_VERSION_NM		"v2.0"
#define TOOL_VERSION_OBJCOPY	"v2.0"
#define TOOL_VERSION_OBJDUMP	"v2.1"
#define TOOL_VERSION_RANLIB	"v2.0"
#define TOOL_VERSION_READELF	"v2.0"
#define TOOL_VERSION_SIZE	"v2.0"
#define TOOL_VERSION_STRINGS	"v2.0"
#define TOOL_VERSION_STRIP	"v2.0"

/* These aren't necessary for the PPCVLE.  */
#define TOOL_VERSION_COFFDUMP	"v1.0"
#define TOOL_VERSION_DLLTOOL	"v1.0"
#define TOOL_VERSION_DLLWRAP	"v1.0"
#define TOOL_VERSION_NLMCONV	"v1.0"
#define TOOL_VERSION_SRCONV	"v1.0"
#define TOOL_VERSION_SYSDUMP	"v1.0"
#define TOOL_VERSION_WINDRES	"v1.0"

#endif /* __PPCVLE_VERSIONS_H  */
