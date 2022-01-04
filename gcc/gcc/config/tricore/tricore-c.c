/* Copyright (C) 2012-2014 Free Software Foundation, Inc.
   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

/* Subroutines for the C front end of TriCore */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "input.h"
#include "tm.h"
#include "c-family/c-pragma.h"
#include "target.h"
#include "c-family/c-common.h"

/* Transorm LO into uppercase and write the result to UP and return UP.
   You must provide enough space for UP.
   Static helper for function below.  */

static char*
tric_toupper (char *up, const char *lo)
{
  char *up0 = up;
  
  for (; *lo; lo++, up++)
    *up = TOUPPER (*lo);

  *up = '\0';

  return up0;
}


/* Worker function for `TARGET_CPU_CPP_BUILTINS' */

void
tric_cpu_cpp_builtins (struct cpp_reader *pfile)
{
  static const char *const tric_builtin_names[] =
    {
#define DEF_BUILTIN(NAME, N_ARGS, ID, TYPE, INSN, TCODE)    \
      "__builtin_tricore_" NAME,
#include "builtins.def"
#undef DEF_BUILTIN
      NULL
    };

  const char *const *pname;
  const tric_erratum_t *erratum;

#ifdef HTC_CPU_CPP_BUILTINS
  HTC_CPU_CPP_BUILTINS (pfile);
#endif

  builtin_define_std ("tricore");

  cpp_define (pfile, "__TRICORE__");
  
  cpp_define_formatted (pfile, "__TC%s__", tric_core->name);
  cpp_define_formatted (pfile, "__TRICORE_CORE__=0x%3x", tric_core->id);

  if (tric_device)
    {
      char *Name = (char*) alloca (1 + strlen (tric_device->name));
      Name = tric_toupper (Name, tric_device->name);

      cpp_define_formatted (pfile, "__%s__", Name);
      cpp_define_formatted (pfile, "__TRICORE_NAME__=%s",
                            tric_device->target_name);
    }

  if (TRIC_HAVE_DIV)
    cpp_define (pfile, "__TRICORE_HAVE_DIV__");
  
  if (TRIC_HAVE_FTOIZ)
    cpp_define (pfile, "__TRICORE_HAVE_FTOIZ__");

  if (TRIC_HAVE_MOV64)
    cpp_define (pfile, "__TRICORE_HAVE_MOV64__");

  if (TRIC_HAVE_POPCNT)
    cpp_define (pfile, "__TRICORE_HAVE_POPCNT__");

  if (TRIC_HAVE_LHA)
    cpp_define (pfile, "__TRICORE_HAVE_LHA__");

  if (TRIC_HAVE_CRCN)
    cpp_define (pfile, "__TRICORE_HAVE_CRCN__");

  if (TRIC_HAVE_SHUFFLE)
    cpp_define (pfile, "__TRICORE_HAVE_SHUFFLE__");

  if (TRIC_HAVE_HP_CONVERT)
    {
      cpp_define (pfile, "__TRICORE_HAVE_FTOHP__");
      cpp_define (pfile, "__TRICORE_HAVE_HPTOF__");
    }

  cpp_define (pfile, "__TRICORE_HAVE_FLOAT16__");

  for (erratum = tric_errata; erratum->macro != NULL; erratum++)
    {
      if (erratum->fixit)
        cpp_define_formatted (pfile, "ERRATA_%s", erratum->macro);
    }

  if (TRIC_ERRATA_048)
    {
      /* For legacy code.  */

      cpp_define (pfile, "ERRATA_CPU48_1");
      cpp_define (pfile, "ERRATA_CPU48_2");
    }

  /* Define builtin macros so that the user can easily query if or
     if not a specific builtin is available.  Don't provide built-in
     defines for builtins that are used internally only.  */

  for (pname = tric_builtin_names; *pname != NULL; pname++)
    {
      if (strncmp  (*pname, "__builtin_tricore__",
                    strlen ("__builtin_tricore__")))
        {
          char *Name = (char*) alloca (1 + strlen (*pname));
          cpp_define (pfile, tric_toupper (Name, *pname));
        }
    }
}


/* IDs for all the TriCore builtins.  */

enum tric_builtin_id
  {
#define DEF_BUILTIN(NAME, N_ARGS, ID, TYPE, INSN, TCODE)        \
    TRIC_BUILTIN_ ## ID,
#include "builtins.def"
#undef DEF_BUILTIN

    TRIC_BUILTIN_COUNT
  };


/* Implement `TARGET_RESOLVE_OVERLOADED_BUILTIN' */

static tree
tric_resolve_overloaded_builtin (location_t loc, tree fndecl, void *vargs)
{
  vec<tree, va_gc> *args = static_cast<vec<tree, va_gc> *> (vargs);
  unsigned int nargs = vec_safe_length (args);
  enum built_in_function fcode = DECL_FUNCTION_CODE (fndecl);
  tree ret = NULL_TREE;
  tree fndeclx = NULL_TREE;

  switch (fcode)
    {
    default:
      break;

    case TRIC_BUILTIN_LDMST:
      
      if (3 == nargs)
        fndeclx = targetm.builtin_decl (TRIC_BUILTIN_LDMST3, true);
      else if (4 == nargs)
        fndeclx = targetm.builtin_decl (TRIC_BUILTIN_LDMST4, true);
      else
        {
          error_at (loc, "function %qE expects 3 or 4 arguments but %d given",
                    fndecl, nargs);
          ret = error_mark_node;
        }

      break;

    case TRIC_BUILTIN_CMPSWAPW:
      if (!TRIC_HAVE_CMPSWAP_W)
        {
          error_at (loc, "instruction %qs is not supported by this device",
                    "cmpswap.w");
          ret = error_mark_node;
        }
      break;

    case TRIC_BUILTIN_SWAPMSKW:
      if (!TRIC_HAVE_SWAPMSK_W)
        {
          error_at (loc, "instruction %qs is not supported by this device",
                    "swapmsk.w");
          ret = error_mark_node;
        }
      break;
    }

  if (fndeclx)
    {
      tree tnamex = DECL_NAME (fndeclx);

      /* Use name of the overloaded plugin in diagnostics, not the name
         of the builtin it is mapped to.  */

      DECL_NAME (fndeclx) = DECL_NAME (fndecl);
      ret = build_function_call_vec (loc, vNULL, fndeclx, args, NULL);
      DECL_NAME (fndeclx) = tnamex;
    }

  return ret;
}


/* Implement `REGISTER_TARGET_PRAGMAS' */

void
tric_register_pragmas (void)
{
  c_register_pragma (0, "section", htc_handle_pragma_section);
  targetm.resolve_overloaded_builtin = tric_resolve_overloaded_builtin;
}
