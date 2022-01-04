/* htc_support.h -- Generic header for HighTec support routines for binutils.
		    Tested with binutils-2.20.

   Copyright (C) 2010 HighTec EDV-Systeme GmbH.  All rights reserved.  */

#ifndef __HTC_SUPPORT_H
#define __HTC_SUPPORT_H

#include "bfdver.h"
#ifdef HTC_TRICORE
#include "../htc/tricore_versions.h"
extern void print_version (const char *);
#define PROGRAM_PREFIX "tricore-"
#elif HTC_PPCVLE
#include "../htc/ppcvle_versions.h"
extern void print_version (const char *);
#define PROGRAM_PREFIX "ppc-vle-"
#elif HTC_GTM_MCS
#include "../htc/mcs_versions.h"
extern void print_version (const char *);
#define PROGRAM_PREFIX "mcs-"
#else
#error "Unknown architecture in \"htc_support.h\"."
#endif

/* Macros to print tool version information in a special format */
#if defined(TOOL_REVISION) && defined(BUILD_DATE) && defined(PROGRAM_PREFIX)
  #define STRINGIZE(x) #x
  #define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
  #define HTC_PRINT_TOOLVERSION(toolname) \
    printf("%s%s (cosmocomp Release %s) build on %s %s\n", \
      PROGRAM_PREFIX, \
      toolname, \
      STRINGIZE_VALUE_OF(TOOL_REVISION), \
      STRINGIZE_VALUE_OF(BUILD_DATE), \
      BFD_VERSION_STRING);
#else
  #undef HTC_PRINT_TOOLVERSION
#endif

#endif /* __HTC_SUPPORT_H  */
