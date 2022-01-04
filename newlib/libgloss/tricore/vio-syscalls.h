/*	vio-syscall.h -- Supported system calls of virtual IO
	
	Copyright (C) 2011 HighTec EDV-Systeme GmbH.
	
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


/* the following syscalls are supported by virtual IO */
#define SYS_OPEN        0x01
#define SYS_CLOSE       0x02
#define SYS_LSEEK       0x03
#define SYS_READ        0x04
#define SYS_WRITE       0x05
#define SYS_CREAT       0x06
#define SYS_UNLINK      0x07
#define SYS_STAT        0x08
#define SYS_FSTAT       0x09
#define SYS_GETTIME     0x0a
#define SYS_RENAME      0x0d

/* #define SYS_FTRUNCATE   0x0b */

