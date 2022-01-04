/* Common functionalities for HighTec backends.
   Copyright (C) 2015 Free Software Foundation, Inc.

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

#ifndef HTC_TARGHOOKS_H
#define HTC_TARGHOOKS_H

#if !defined(IN_LIBGCC2) && !defined(IN_TARGET_LIBS)

extern bool default_htc_bss_initializer_p (const_tree);
extern void default_htc_unnamed_section_callback (const void*);
extern bool default_htc_in_named_section (const_tree);
extern unsigned long default_htc_section_flags_from_string (const char*);
extern bool default_htc_section_flags_to_string (char*, unsigned int, enum flagstring_purpose);
extern void default_htc_asm_out_section (const char*, unsigned int, tree);
extern void htc_post_pch_load (void);
extern int default_htc_dump_valist (FILE*, const char*, va_list);

#endif /* IN_LIBGCC2 */

#endif /* HTC_TARGHOOKS_H */
