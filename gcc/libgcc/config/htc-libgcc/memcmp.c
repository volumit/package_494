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


extern int memcmp (const void *str1, const void *str2, __SIZE_TYPE__ count);

int
memcmp (const void *str1, const void *str2, __SIZE_TYPE__ count)
{
  const unsigned char *s1 = str1;
  const unsigned char *s2 = str2;
  unsigned char c1;
  unsigned char c2;

  while (count-- > 0)
    {
      c1 = *s1++;
      c2 = *s2++;
      if (c1 != c2)
	  return c1 < c2 ? -1 : 1;
    }
  return 0;
}
