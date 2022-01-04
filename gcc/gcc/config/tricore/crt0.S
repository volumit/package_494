/*
 * crt0.S -- Startup code for GNU/TriCore applications.
 *
 *  Copyright (C) 1998-2014 HighTec EDV-Systeme GmbH.
 * 
 * This file is part of GCC.
 *
 * GCC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GCC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * Under Section 7 of GPL version 3, you are granted additional
 * permissions described in the GCC Runtime Library Exception, version
 * 3.1, as published by the Free Software Foundation.
 *  
 * You should have received a copy of the GNU General Public License and
 * a copy of the GCC Runtime Library Exception along with this program;
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.  */

#ifndef __TRICORE_NAME__
#error Please define __TRICOR_NAME__
#endif    

#if defined (__TRICORE_CORE__) \
	&& (__TRICORE_CORE__ == 0x161 || __TRICORE_CORE__ == 0x162)
#include "crt0-tc2x.S"
#else
#include "crt0-tc1x.S"
#endif
