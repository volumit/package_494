/* htc-elf.h -- architecture independend HTC extensions
   Copyright 2010 Free Software Foundation, Inc.
   Written by Horst Lehser , HighTec EDV Systeme GmbH

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */


extern void htc_check_gas_version_flags (int *ac, char ***av);
extern void htc_print_version_id (int verbose);

extern void htc_directive_extern(int ignore );


extern void htc_pop_insert(void);

#define md_pop_insert()   htc_pop_insert()

