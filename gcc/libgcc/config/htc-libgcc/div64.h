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

#ifndef DIV64_H
#define DIV64_H

#include <stdint.h>

extern uint32_t __udiv6432 (uint64_t, uint32_t);

extern int64_t __divdi3 (int64_t, int64_t);
extern int64_t __moddi3 (int64_t, int64_t);
extern int64_t __divmoddi4 (int64_t, int64_t, int64_t*);

extern uint64_t __udivdi3 (uint64_t, uint64_t);
extern uint64_t __umoddi3 (uint64_t, uint64_t);
extern uint64_t __udivmoddi4 (uint64_t, uint64_t, uint64_t*);

#endif /* DIV64_H */
