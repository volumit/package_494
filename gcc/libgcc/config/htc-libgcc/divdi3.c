/* Copyright (C) 2010, 2011, 2012 Free Software Foundation, Inc.

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

#include "div64.h"


/* Divides signed 64-bit N by signed 64-bit D and returns the
   quotient. */

int64_t
__divdi3 (int64_t n, int64_t d)
{
  uint64_t n_abs = n >= 0 ? (uint64_t) n : -(uint64_t) n;
  uint64_t d_abs = d >= 0 ? (uint64_t) d : -(uint64_t) d;
  uint64_t q_abs;

  if (d == 0)
    return (n >= 0) ? INT64_MAX : INT64_MIN;

  if (d == -1 && n == INT64_MIN)
    return INT64_MAX;

  q_abs = __udivdi3 (n_abs, d_abs);

  return (n < 0) == (d < 0) ? (int64_t) q_abs : -(int64_t) q_abs;
}
