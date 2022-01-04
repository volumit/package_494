/* Common functionalities for HighTec backends.
   Copyright (C) 2012 Free Software Foundation, Inc.
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

#ifndef HTC_COMMON_C_H
#define HTC_COMMON_C_H

struct cpp_reader;

/* Handle #pragma section.  */

extern void htc_handle_pragma_section (struct cpp_reader *pfile);

/* Define HighTec specific language independent macros.
   As enabled with --with-hightec.
   This can be used in the TARGET_{CPU,OS,OBJFMT}_CPP_BUILTINS macros, or
   integrate it into a backend's existing TARGET_{CPP,OS,OBJFMT}_CPP_BUILTINS
   macro definition.  */

#define HTC_CPU_CPP_BUILTINS(pfile) htc_cpu_cpp_builtins (pfile)
extern void htc_cpu_cpp_builtins (struct cpp_reader *pfile);

#endif /* HTC_COMMON_C_H */
