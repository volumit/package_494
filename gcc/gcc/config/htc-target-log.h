/* This file is part of GCC.

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

#ifndef _HTC_TARGET_LOG_H
#define _HTC_TARGET_LOG_H

#if !defined(IN_LIBGCC2) && !defined(IN_TARGET_LIBS)

typedef struct
{
#define DEF_HTC_LOG(aKEY) \
    unsigned aKEY:1;
#include "htc-target-log.def"
#undef DEF_HTC_LOG
} htc_log_t;

extern htc_log_t htc_log;

#define htc_dump  (htc_log_set_caller_d (__FUNCTION__))
#define htc_edump (htc_log_set_caller_e (__FUNCTION__))
#define htc_fdump (htc_log_set_caller_f (__FUNCTION__))

#ifdef HTC_DEBUG
#define htc_debug if (htc_log.debug) htc_dump
#else
#define htc_debug(...)
#endif

extern int (*htc_log_set_caller_d (const char*))(const char*, ...);
extern int (*htc_log_set_caller_e (const char*))(const char*, ...);
extern int (*htc_log_set_caller_f (const char*))(FILE*, const char*, ...);

extern void htc_log_init (void);

#endif /* ! in target lib */

#endif /* _HTC_TARGET_LOG_H */
