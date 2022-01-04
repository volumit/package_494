/* Copyright (C) 2012 Free Software Foundation, Inc.

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

#define __umulsidi3(u,v) ((UDItype)(USItype)u*(USItype)v)
#define count_leading_zeros(COUNT,X)    ((COUNT) = __builtin_clz (X))
#define COUNT_LEADING_ZEROS_0 32

#define count_trailing_zeros(count, x)          \
 do {                                           \
   UWtype __ctz_x = (x);                        \
   (count) = W_TYPE_SIZE - 1 - __builtin_clz(__ctz_x & -__ctz_x); \
 } while (0)


#ifndef UDIV_NEEDS_NORMALIZATION
#define UDIV_NEEDS_NORMALIZATION 0
#endif
