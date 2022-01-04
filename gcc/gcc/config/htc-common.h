/* Common functionalities for HighTec backends.
   Copyright (C) 2012-2014 Free Software Foundation, Inc.
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

#ifndef _HTC_COMMON_H
#define _HTC_COMMON_H 1

void htc_set_error_numbers (int argc, char **argv);

extern void htc_init_once (void);

typedef struct GTY(()) htc_stat
{
  struct
  {
    struct
    {
      unsigned n_htc_sections;
      unsigned n_htc_sections_reused;
      unsigned n_htc_sections_named;
      unsigned n_unnamed_sections;
      unsigned n_pch_section_fixups;

      unsigned n_secspecs;
      unsigned n_secspecs_deserialized;
      unsigned n_secspecs_applied;
      unsigned n_secspecs_void;

      unsigned n_dams;
      unsigned n_dams_deserialized;

      struct
      {
        unsigned n_calls;
        unsigned n_hashes_computed;
        unsigned n_compares;
      } lookup;
    } section;
  } htc;
} htc_stat_t;

extern htc_stat_t htc_stat;

#endif /* _HTC_COMMON_H */
