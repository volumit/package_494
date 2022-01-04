/* Definitions of target machine for GNU compiler, for Infineon TriCore
   Copyright (C) 2012-2014 Free Software Foundation, Inc.

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
<http://www.gnu.org/licenses/>.  */


typedef struct
{
  /* Argument of the -merrata= option, e.g. "cpu048" */
  const char *option;

  /* This erratum is handled by the compiler, i.e. it influences the
     code generation by the compiler and it does more than just setting
     built-in defines.  */
  int by_compiler;

  /* The assembler implements the respective -merrata= command switch.
     Used to filter options to pass through to the assembler.  */
  int by_assembler;

  /* Used to set a built-in macro like ERRATA_CPU48 provided the
     erratum is turned on.  */
  const char *macro;

  /* Non-zero if this erratum is active, i.e. enabled per command line
     or per default for this core / derivative.  */
  int fixit;

  /* Full name of the artifact as given in the IFX errata sheet.  */
  const char *ifx_name;
} tric_erratum_t;

enum
  {
#define DEF_ERRATA(ARG, CC1, AS, DEFIN, ON, IFX_NAME)   \
    tric_errata_ ## ARG,
#include "errata.def"
#undef DEF_ERRATA
    tric_errata_max
  };

/* Parameterize the various cores */

typedef struct
{
  /* Core name, the variable part of -mtc* */
  const char *name;
  
  /* Core id, used to -D__TRICORE_CORE__=0x... */
  int id;
  
  /* Name of device library.  */
  const char *libname;
} tric_core_t;

/* Parameterize the various cores */

typedef struct
{
  /* Device name from -mcpu=<name> e.g. "tc1796".  */
  const char *name;
  
  /* Core id, used to -D__TRICORE_NAME__=..., e.g. "0x1796".  */
  const char *target_name;

  /* The core as specified with -mtc<core> e.g. "13" for multilib selection. */
  const char *core_mtc;

  /* The core for tricore-ld --mcpu=<ld_mcpu>  */
  const char *ld_mcpu;
} tric_device_t;

extern tric_erratum_t tric_errata[];
extern const tric_core_t tric_cores[];
extern const tric_core_t *tric_core;
extern const tric_device_t tric_devices[];
extern const tric_device_t *tric_device;

extern void tric_set_core (const char*);
extern void tric_set_device (const char*);

/***********************************************************************/
/* Convenient macros for core / device specific stuff.
   Used in machine description etc.  */

#define TRIC_13    (tric_core->id == 0x130)
#define TRIC_131   (tric_core->id == 0x131)
#define TRIC_131UP (tric_core->id >= 0x131)
#define TRIC_13X   (tric_core->id <= 0x131)
#define TRIC_16    (tric_core->id == 0x160)
#define TRIC_161   (tric_core->id == 0x161)
#define TRIC_162   (tric_core->id == 0x162)
#define TRIC_16UP  (tric_core->id >= 0x160)
#define TRIC_161UP (tric_core->id >= 0x161)
#define TRIC_162UP (tric_core->id >= 0x162)

#define TRIC_HAVE_DIV   (TRIC_16UP)
#define TRIC_HAVE_FTOIZ (TRIC_131UP)
#define TRIC_HAVE_MOV64 (TRIC_16UP)
#define TRIC_HAVE_FCALL (TRIC_16UP)
#define TRIC_HAVE_CMPSWAP_W (TRIC_161UP)
#define TRIC_HAVE_SWAPMSK_W (TRIC_161UP)

#define TRIC_HAVE_CRCN        (TRIC_162UP)
#define TRIC_HAVE_LHA         (TRIC_162UP)
#define TRIC_HAVE_SHUFFLE     (TRIC_162UP)
#define TRIC_HAVE_POPCNT      (TRIC_162UP)
#define TRIC_HAVE_HP_CONVERT  (TRIC_162UP)

#define TRIC_ERRATA_048 (tric_errata[tric_errata_cpu048].fixit)
#define TRIC_ERRATA_076 (tric_errata[tric_errata_cpu076].fixit)
#define TRIC_ERRATA_081 (tric_errata[tric_errata_cpu081].fixit)
#define TRIC_ERRATA_101 (tric_errata[tric_errata_cpu101].fixit)

