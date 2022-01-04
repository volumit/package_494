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

/* Signed 64-bit division. */

uint64_t
__udivdi3 (uint64_t n, uint64_t d)
{
  if (d == 0)
    return UINT64_MAX;
  
  if (d >> 32 == 0)
    {
      /* Proof of correctness: (the quoient will fit into 32 Bit)

         Let n, d, b, n1, and n0 be defined as in this function.
         Let [x] be the "floor" of x.  Let T = b[n1/d].
         Assume d nonzero.  Then:
           [n/d] = [n/d] - T + T
                 = [n/d - T] + T                         by (1) below
                 = [(b*n1 + n0)/d - T] + T               by definition of n
                 = [(b*n1 + n0)/d - dT/d] + T
                 = [(b(n1 - d[n1/d]) + n0)/d] + T
                 = [(b[n1 % d] + n0)/d] + T,             by definition of %
         which is the expression calculated below.

         (1) Note that for any real x, integer i: [x] + i = [x + i].

         To prevent udiv6432() from trapping, [(b[n1 % d] + n0)/d] must
         be less than b.  Assume that [n1 % d] and n0 take their
         respective maximum values of d - 1 and b - 1:
               [(b(d - 1) + (b - 1))/d] < b
           <=> [(bd - 1)/d] < b
           <=> [b - 1/d] < b
         which is a tautology.

         Therefore, this code is correct and the quotient fits into 32 Bit */
      
      uint64_t b = 1ULL << 32;
      uint32_t n1 = n >> 32;
      uint32_t n0 = n;
      uint32_t d0 = d;

      return __udiv6432 (b * (n1 % d0) + n0, d0) + b * (n1 / d0);
    }
  else
    {
      /* Based on the algorithm and proof available from
         http://www.hackersdelight.org/revisions.pdf
      */

      if (n < d)
        return 0;
      else
        {
          uint32_t d1 = d >> 32;
          int s = __builtin_clz (d1);
          uint64_t q = __udiv6432 (n >> 1, (d << s) >> 32) >> (31 - s);

          return n - (q - 1) * d < d ? q - 1 : q;
        }
    }
}
