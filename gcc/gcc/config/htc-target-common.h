/* Features provided by all HighTec targets, and that are common to driver
   and compiler proper.  Header used to communicate between htc-common*.c,
   target-common.c, target.c on the one side and htc-target-common-c on the
   other side.

   Copyright (C) 2012-2015 Free Software Foundation, Inc.

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

#ifndef HTC_TARGET_COMMON_H
#define HTC_TARGET_COMMON_H

#if defined (__cplusplus)

/* Describes properties of option -msection-asm-name <key>=<map> and points
   to respective results, i.e. maps keys to mappings (assembler name
   transform rules).  */

typedef struct
{
  /* The key as in "-msection-asm-name <key>=<map>".  Valid keys are:
     pragma-data-sections, pragma-function-sections, asection-data-sections,
     asection-function-sections, data-sections, function-sections.  */
  const char *const key;

  /* ID of an option that needs also to be turned on.  If there is no
     such dependency, use OPT_SPECIAL_ignore.  E.g. -fdata-sections must
     be set if -mmap-section data-sections=<map> is specified.  */
  const int need_OPT;

  /* Payload: Mapping attached to <key>.  '+' will be replaced by the object's
     assembler name, i.e. as would be used by vanilla named sections.
     NULL means to use the object's assembler name unaltered.  */
  char *map;

  /* Payload: Parts before / after the '+'.  */
  char *map_prefix;
  char *map_suffix;
  bool named_p;

  /* Payload: The command option that triggered this mapping.  */
  const struct cl_decoded_option *decoded;

  /* Payload: The command option (as string) that triggered this mapping.  */
  const char *fulltext;
} htc_asm_name_map_t;

/* Identity maps the object's assembler name to itself. */
extern const char htc_asm_name_map_id;

extern htc_asm_name_map_t htc_asm_name_map[];
extern bool htc_section_asm_name_strict_p;

/* Probably used by common/config/<target>/<target>-common.c::handle_option. */
extern bool htc_register_section_asm_name (const struct cl_decoded_option*);

#endif /* __cplusplus */

#endif /* HTC_TARGET_COMMON_H */
