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

uint32_t __udiv6432 (uint64_t n, uint32_t d)
{
  /* Number base (16 bits) */
  const uint32_t b = (1<<16);
  
  /* Norm. dividend */
  uint32_t nn1, nn0;
  
  /* Norm. divisor */
  uint32_t dn1, dn0;
  
  /* Quotient digits */
  uint32_t q1, q0;
  
  /* Dividend digit pairs */
  uint32_t nn32, nn21, nn10;

  /* remainder */
  uint32_t rhat;

  /* Shift amount for norm */
  int s;

  /* Dividend as fullwords */
  uint32_t n1, n0;

  n1 = n >> 32;
  n0 = n;

  /* If overflow, return the largest possible quotient */
  if (n1 >= d)
    return UINT32_MAX;

  /* 0 <= s <= 31 */
  s = __builtin_clz(d);

  /* Normalize divisor */
  d = d << s;

  /* Break divisor up into two 16-bit digits */
  dn1 = d >> 16;
  dn0 = d & 0xFFFF;

  nn32 = s
    ? (n1 << s) | (n0 >> (32 - s))
    : n1;

  /* Shift dividend left */
  nn10 = n0 << s;

  /* Break right half of dividend into two digits */
  nn1 = nn10 >> 16;
  nn0 = nn10 & 0xFFFF;

  /* Compute the first quotient digit, q1 */
  q1 = nn32 / dn1;
  rhat = nn32 - q1*dn1;
  
again1:
  if (q1 >= b || q1*dn0 > b*rhat + nn1)
    {
      q1 = q1 - 1;
      rhat = rhat + dn1;
      if (rhat < b)
        goto again1;
    }

  /* Multiply and subtract */
  nn21 = nn32*b + nn1 - q1*d;

  /* Compute the second quotient digit, q0 */
  q0 = nn21/dn1;
  rhat = nn21 - q0*dn1;
  
again2:
  if (q0 >= b || q0*dn0 > b*rhat + nn0)
    {
      q0 = q0 - 1;
      rhat = rhat + dn1;
      if (rhat < b)
        goto again2;
    }
  
    return q1*b + q0;
}
