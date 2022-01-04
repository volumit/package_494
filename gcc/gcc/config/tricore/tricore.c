/*  Copyright (C) 2010-2014 Free Software Foundation, Inc.
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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "regs.h"
#include "output.h"
#include "insn-attr.h"
#include "tree.h"
#include "stringpool.h"
#include "stor-layout.h"
#include "c-family/c-common.h"
#include "function.h"
#include "target.h"
#include "target-def.h"
#include "common/common-target.h"
#include "expr.h"
#include "recog.h"
#include "optabs.h"
#include "ggc.h"
#include "langhooks.h"
#include "tm_p.h"
#include "basic-block.h"
#include "df.h"
#include "tm-constrs.h"
#include "tree-pass.h"
#include "version.h"
#include "opts.h"
#include "context.h"
#include "calls.h"
#include "tricore-protos.h"
#include "cgraph.h"
#include "intl.h"

/* Tokens for the machine specific attributes we support */
#define TRIC_ATTR_ABSOLUTE           "absdata"
#define TRIC_ATTR_INDIRECT           "fardata"
#define TRIC_ATTR_SMALL              "smalldata"
#define TRIC_ATTR_INTERRUPT          "interrupt"
#define TRIC_ATTR_INTERRUPT_HANDLER  "interrupt_handler"
#define TRIC_ATTR_PXHNDCALL          "pxhndcall"
#define TRIC_ATTR_LONGCALL           "longcall"

/* Addresses that have their 12 MSBs set can be loaded/stored
   by one single instruction. */
#define TRIC_ABSOLUTE_INT_ADDRESS_P(X) \
   ((0x0fffc000 & (X)) == 0)

#define TRIC_ABSOLUTE_CODE_ADDRESS_P(X) \
   ((0x0fe00001 & (X)) == 0)

/* Mask for A-regs */
#define AREGS_MASK 0xffff0000

/* Mask for D-regs */
#define DREGS_MASK 0x0000ffff

/* Mask for regs that are element of LOCAL_REGNO */
#define LOCAL_REGNO_MASK 0xf000ff00

/* Machine dependent SYMBOL_REF flags */
#define TRIC_SYMBOL_FLAG_ABSOLUTE (SYMBOL_FLAG_MACH_DEP << 0)
#define TRIC_SYMBOL_FLAG_SMALL    (SYMBOL_FLAG_MACH_DEP << 1)
#define TRIC_SYMBOL_FLAG_LONGCALL (SYMBOL_FLAG_MACH_DEP << 2)
#define TRIC_SYMBOL_FLAG_PIC      (SYMBOL_FLAG_MACH_DEP << 3)


/***********************************************************************
 ** Objects related to Section Handling
 ***********************************************************************/

/* 1st Dimension of `tric_data_section': Addressing Capability */
enum
  {
    SS_ABSOLUTE, SS_SMALL, SS_INDIRECT,
    SS1_MAX
  };

/* 2nd Dimension of `tric_data_section': Data Flavour */
enum
  {
    SS_DATA, SS_RODATA, SS_BSS,
    SS2_MAX
  };

/* 3rd Dimension of `tric_data_section': Data Alignment */
enum
  {
    SS_ALIGN_NONE = 0,
    SS_ALIGN_1, SS_ALIGN_2, SS_ALIGN_4, SS_ALIGN_8,
    SS3_MAX
  };

/* Default data sections as initialized by `tric_asm_init_sections':
   1st Dimension: Absolute/Small/Indirect-Only addressable
   2nd Dimension: .data/.rodata/.bss
   3rd Dimension: Alignment */
static GTY(()) section* tric_data_section[SS1_MAX][SS2_MAX][SS3_MAX];

#define zbss_section (tric_data_section[SS_ABSOLUTE][SS_BSS][0])

/* Data base to record default and user-defined sections with their alignments
   and flags.  To operate on the list, use `tric_insert_section' and
   `htc_lookup_section'.  The list will be initialized with the default
   sections in `tric_asm_init_sections'.  */
static GTY(()) htc_section_t *tric_text_section;

/* Points to the list of all the built-in sections from tricore-sections.def.
   This is just for swift lookup of these sections and works because
   `htc_sections' is a list and `htc_insert_section' prepends new
   sections to that list.  This pointer is also GTY'ed by that list.  */
static GTY(()) htc_section_t *tric_builtin_sections;

/* Ranges for (const) data to be put into absolute resp. small data sections
   set from -mabs* resp. -msmall* command line options. Ranges are inclusive
   the boundary. */
extern GTY(()) int tric_zdata_max;
extern GTY(()) int tric_zdata_min;
extern GTY(()) int tric_zconst_max;
extern GTY(()) int tric_zconst_min;
extern GTY(()) int tric_sdata_max;
extern GTY(()) int tric_sdata_min;
extern GTY(()) int tric_sconst_max;
extern GTY(()) int tric_sconst_min;

/***********************************************************************
 ** Miscellaneous Objects
 ***********************************************************************/

int tric_have_cost_insns = 0;
int mPos;
int mWidth;
int tric_map_combine;

extern GTY(()) struct tric_segment_trap tric_segment_trap;

/* Return true iff strings A and B are equal.  */
#define STREQ(A, B) (0 == strcmp (A, B))


/***********************************************************************
 ** Objects local to this file
 ***********************************************************************/

/* FIELD_DECLS for which we already reported that their size is
   greater than 32 bits. */

static GTY(()) tree tric_eabi_error = NULL_TREE;

/* Nodes for which we already warned as of -Winitialize-pid.  */

static GTY(()) tree tric_pid_init = NULL_TREE;

/* Enumerate .PICOFF<N> labels.  */
static GTY(()) int tric_picoff_labelno;

/* Pass number of some pass that is known be located after split1 and
   before register allocation.  */
static GTY(()) int tric_after_split1_pass_number;

/* Some rtx singletons used in rtx-cost computation */
static GTY(()) rtx fake_insn = NULL_RTX;
static GTY(()) rtx fake_jump_insn = NULL_RTX;
static GTY(()) rtx fake_pattern = NULL_RTX;
static GTY(()) rtx fake_jump_pattern = NULL_RTX;

static int tric_const_int_costs (int);
static int tric_const_double_costs (rtx);

static inline bool tric_symbol_ref_small16_p (const_rtx);

static void tric_print_operand (FILE*, rtx, int);

static rtx tric_arith_CONST_INT (rtx, rtx, rtx, rtx);

/***********************************************************************
 ** Miscellaneous Support Functions
 ***********************************************************************/

/* Return true iff STR starts with PREFIX.  */

static bool
str_prefix_p (const char *str, const char *prefix)
{
  return 0 == strncmp (str, prefix, strlen (prefix));
}


/* If code CODE represents a signed operation, return TRUE.
   If code CODE represents an unsigned operation, return FALSE.  */

bool
tric_code_signed_p (enum rtx_code code)
{
  switch (code)
    {
    default:
      gcc_unreachable();

    case SMIN:  case SMAX:
    case LT:    case LE:
    case GT:    case GE:
    case SIGN_EXTEND:
      return true;

    case UMIN:  case UMAX:
    case LTU:    case LEU:
    case GTU:    case GEU:
    case ZERO_EXTEND:
      return false;
    }

  gcc_unreachable();
  return false;
}


/* Return TRUE if CODE1 and CODE2 are operation of the same signedness
   and FALSE, otherwise.  */

bool
tric_codes_same_sign_p (enum rtx_code code1, enum rtx_code code2)
{
  return tric_code_signed_p (code1) == tric_code_signed_p (code2);
}


/* If, taken as a function of x, the expression  (x CODE OP1) ? x : OP2
   is equivalent  to  (max x OP2)  then return the respective rtx code,
   i.e. UMAX or SMAX.  Otherwise, return UNKNOWN.  */

enum rtx_code
tric_max_code (enum rtx_code code, rtx op1, rtx op2)
{
  if (GEU == code || GTU == code)
    {
      if (CONST_INT_P (op1))
        op1 = GEN_INT (USIval (op1));
      if (CONST_INT_P (op2))
        op2 = GEN_INT (USIval (op2));
    }

  if (!rtx_equal_p (op1, op2)
      && CONST_INT_P (op1)
      && CONST_INT_P (op2))
    {
      if (GEU == code || GE == code)
        // Compute as DImode to avoid SI overflow / underflow.
        op1 = plus_constant (DImode, op1, -1);
      if (GTU == code || GT == code)
        op1 = plus_constant (DImode, op1, 1);
    }

  return !rtx_equal_p (op1, op2)
    ? UNKNOWN
    : GE == code || GT == code ? SMAX
    : GEU == code || GTU == code ? UMAX
    : UNKNOWN;
}


/* If, taken as a function of x, the expression  (x CODE OP1) ? x : OP2
   is equivalent to  (min x OP2)  then return the respective rtx code,
   i.e. UMIN or SMIN.  Otherwise, return UNKNOWN.  */

extern enum rtx_code
tric_min_code (enum rtx_code code, rtx op1, rtx op2)
{
  if (LEU == code || LTU == code)
    {
      if (CONST_INT_P (op1))
        op1 = GEN_INT (USIval (op1));
      if (CONST_INT_P (op2))
        op2 = GEN_INT (USIval (op2));
    }

  if (!rtx_equal_p (op1, op2)
      && CONST_INT_P (op1)
      && CONST_INT_P (op2))
    {
      if (LEU == code || LE == code)
        // Compute as DImode to avoid SI overflow / underflow.
        op1 = plus_constant (DImode, op1, 1);
      if (LTU == code || LT == code)
        op1 = plus_constant (DImode, op1, -1);
    }

  return !rtx_equal_p (op1, op2)
    ? UNKNOWN
    : LE == code || LT == code ? SMIN
    : LEU == code || LTU == code ? UMIN
    : UNKNOWN;
}


/* Return true if the operation

      (CODE_OUTER:SI (CODE_INNER:SI (x OP_INNER))
                     OP_OUTER)

   taken as a function of x, represents a signed saturation.  If MODE is
   QImode or HImode return TRUE iff this is a saturation to exact the range
   of that machine mode.  For other modes return TRUE if the operation
   comprises a generic signed saturation.  If TRUE is returned, *LOWER and
   *UPPER are set to the boundary values of the support of the operation,
   i.e. the operation is onto [*LOWER, *UPPER] then.  LOWER and UPPER may
   be omitted or passed as NULL.  */

bool
tric_sat_p (enum machine_mode mode, enum rtx_code code_inner, rtx op_inner,
            enum rtx_code code_outer, rtx op_outer,
            HOST_WIDE_INT *lower, HOST_WIDE_INT *upper)
{
  if (!CONST_INT_P (op_inner)
      || !CONST_INT_P (op_outer))
    return false;

  HOST_WIDE_INT ldummy, udummy;
  if (!lower) lower = &ldummy;
  if (!upper) upper = &udummy;

  *lower
    = SMAX == code_inner ? INTVAL (op_inner)
    : SMAX == code_outer ? INTVAL (op_outer)
    : INT_MAX;

  *upper
    = SMIN == code_inner ? INTVAL (op_inner)
    : SMIN == code_outer ? INTVAL (op_outer)
    : INT_MIN;

  if (UMIN == code_outer
      && SMAX == code_inner
      && INTVAL (op_inner) >= 0)
    {
      *lower = INTVAL (op_inner);
      *upper = USIval (op_outer);
    }

  return QImode == mode || HImode == mode
    ? (*upper == tric_max_for_mode (mode, true)
       && *lower == tric_min_for_mode (mode, true))
    : *lower < *upper;
}


/* Return true if the operation

      (CODE_OUTER:SI (CODE_INNER:SI (x OP_INNER))
                     OP_OUTER)

   taken as a function of x, represents an unsigned saturation.  If TRUE is
   returned, *LOWER and *UPPER are set to the boundary values of the support
   of the operation, i.e. the operation is onto [*LOWER, *UPPER] then.  */

bool
tric_usat_p (enum rtx_code code_inner, rtx op_inner,
             enum rtx_code code_outer, rtx op_outer,
             HOST_WIDE_INT *lower, HOST_WIDE_INT *upper)
{
  if (!CONST_INT_P (op_inner)
      || !CONST_INT_P (op_outer))
    return false;

  *lower
    = UMAX == code_inner ? USIval (op_inner)
    : UMAX == code_outer ? USIval (op_outer)
    : UINT_MAX;

  *upper
    = UMIN == code_inner ? USIval (op_inner)
    : UMIN == code_outer ? USIval (op_outer)
    : INT_MIN;

  if ((UMIN == code_outer
       || SMIN == code_outer)
      && SMAX == code_inner
      && INTVAL (op_inner) >= 0)
    {
      *lower = INTVAL (op_inner);
      *upper = SIval (op_outer, SMIN == code_outer);
    }

  if (UMIN == code_inner
      && SMAX == code_outer
      // sic!  This means op2 can be represented as signed
      // and the result as signed is >= 0.
      && INTVAL (op_inner) >= 0)
    {
      *lower = MAX (0, INTVAL (op_outer));
      *upper = INTVAL (op_inner);
    }

  if (SMIN == code_inner
      && SMAX == code_outer
      && INTVAL (op_outer) >= 0)
    {
      *lower = INTVAL (op_outer);
      *upper = INTVAL (op_inner);
    }

  return *lower < *upper;
}


/* A class representing the range of integers from .lower to .upper.

   .lower > .upper is the (non-unique) representation of the empty set.
   The empty set will never occur as result of a computation, but handling it
   as usual makes range-arithmetic more familiar and to operate like sets.

   `range_t' mostly deals with set operations like intersection and union.
   It also implements the point-wise action of some functions like minimum
   and maximum (against a given constant), addition or subtraction of a given
   constant, negation, absolute value, modulo, etc.  All of these are straight
   forward and many act transitively.  The only one that needs more attention
   is remainder of integer division (modulo).  */

class range_t
{
 public:
  HOST_WIDE_INT lower, upper;

  static const range_t none;
  static const range_t all;

  // All possible values or (non-unique representation of) the empty set.
  range_t (bool all_or_none)
    {
      lower = all_or_none ? TRIC_INT_MIN  : +42;
      upper = all_or_none ? TRIC_UINT_MAX : -42;
    }

  // Construct interval [LO, HI] if  LO <= HI  and the empty set if LO > HI.
  range_t (HOST_WIDE_INT lo, HOST_WIDE_INT up)
    {
      lower = lo;
      upper = up;
    }

  // Construct an interval that consists of all integers which can be
  // represented by mode MODE in the specified signedness:  As signed if
  // SIGNED_P = TRUE and as unsigned if SIGNED_P = FALSE.
  range_t (enum machine_mode mode, bool signed_p)
    {
      lower = tric_min_for_mode (mode, signed_p);
      upper = tric_max_for_mode (mode, signed_p);
    }

  // Is this the empty set?
  inline bool is_empty (void) const
  {
    return lower > upper;
  }

  // Size, i.e. cardinality of the set of integers.
  HOST_WIDE_INT size (void) const
  {
    return is_empty() ? 0 : upper - lower + 1;
  }

  // Are both non-empty?
  bool operator && (const range_t &r) const
  {
    return !is_empty() && !r.is_empty();
  }

  // Intersection
  range_t operator & (const range_t &r) const
  {
    return r && *this
      ? range_t (MAX (lower, r.lower), MIN (upper, r.upper))
      : range_t::none;
  }

  // Union
  // The union of ranges is the smallest range that contains all these ranges.
  range_t operator | (const range_t &r) const
  {
    return is_empty()
      ? r
      : r.is_empty() ? *this
      : range_t (MIN (lower, r.lower),
                 MAX (upper, r.upper));
  }

  // Subset of, i.e. contained in R?
  bool operator <= (const range_t &r) const
  {
    return r && *this
      ? r.lower <= lower && upper <= r.upper
      : is_empty ();
  }

  // Superset of, i.e. contains R?
  bool operator >= (const range_t &r) const
  {
    return r <= *this;
  }

  // Equal as sets?
  bool operator == (const range_t &r) const
  {
    return (r >= *this) && (r <= *this);
  }

  // Not equal as sets?
  bool operator != (const range_t &r) const
  {
    return !(r == *this);
  }

  // Shift by an offset of K:  [a, b] -> [a, b] + k := [a+k, b+k].
  // Union of {x + K} taken over all x's in the range.
  range_t operator + (HOST_WIDE_INT k) const
  {
    return is_empty ()
      ? range_t::none
      : range_t (lower + k, upper + k);
  }

  // Shift by an offset of -K:  [a, b] -> [a, b] - K := [a-K, b-K].
  // Union of {x - K} taken over all x's in the range.
  range_t operator - (HOST_WIDE_INT k) const
  {
    return (*this) + (-k);
  }

  // Mirror the range at x = 0:  [a, b] -> -[a, b] := [-b, -a].
  // Union of {-x} taken over all x's in the range.
  range_t operator - (void) const
  {
    return is_empty ()
      ? range_t::none
      : range_t (-upper, -lower);
  }

  // Union of {min (x, K)} taken over all x's in the range.
  range_t min (HOST_WIDE_INT k) const
  {
    return is_empty ()
      ? range_t::none
      : range_t (MIN (lower, k), MIN (upper, k));
  }

  // Union of {max (x, K)} taken over all x's in the range.
  range_t max (HOST_WIDE_INT k) const
  {
    return is_empty ()
      ? range_t::none
      : range_t (MAX (lower, k), MAX (upper, k));
  }

  // Union of {|x|} taken over all x's in the range.
  range_t abs (void) const
  {
    range_t neg = (*this) & range_t (TRIC_INT_MIN, -1);
    range_t pos = (*this) & range_t (0, TRIC_UINT_MAX);

    return pos | -neg;
  }

  // Union of {min (|x|, 0x7fffffff)} taken over all x's in the range.
  range_t ss_abs (void) const
  {
    return abs().min (TRIC_INT_MAX);
  }

  range_t mod (HOST_WIDE_INT m, HOST_WIDE_INT new_max) const;
  range_t mod (enum rtx_code code, enum machine_mode mode) const;

  // Let F act on the range.
  range_t image (const_rtx f) const;
};

range_t const range_t::none = range_t (false);
range_t const range_t::all = range_t (true);


/* Consider the pointwise canonical projection

       Z --> Z / m*Z

   with Z / m*Z represented by the largest integers that are not greater than
   NEW_MAX.  Return the smallest interval containing all representatives of
   x mod M  where x is an element of the input range.

   Some notes seem to be in order:  This method returns a superset of the
   range mod M.  We could model the exact image of mod, i.e. the range mod M
   if we implemented unions of (at most two) ranges.  We prefer to avoid
   putting too much effort into (unions of) intervals' arithmetic at the
   expense of fewer optimization opportunities:  Casting back and forth
   signed / unsigned flavours of the same base type does not change the value.
   For example

       (short) (unsigned short) x  =  (short) x.

   With the present implementation, however, this is no more the case
   and casting back and forth will blow up the range and we will lose
   information and therefore miss some optimization opportunities.

   Just consider the interval [-1, 0]:  Casting this to unsigned and
   then to signed again will map  0 -> 0 -> 0  and  -1 -> 0xffffffff -> -1
   and hence map [-1, 0] to [-1, 0].  Our simplistic intervals, however,
   will map [-1, 0] -> [0, UINT_MAX] -> [INT_MIN, INT_MAX] which means we
   have lost any information on the possible values resp. lost any
   information about which values will never occur.  */

range_t
range_t::mod (HOST_WIDE_INT m, HOST_WIDE_INT new_max) const
{
  gcc_assert (m > 0);

  if (is_empty())
    return range_t::none;

  /* Step 1:   Compute .upper mod M in the requested representative system,
     ======    i.e. the largest integer not greater than NEW_MAX which is
               element of the same residue class like .upper.  */

  // With
  //
  //     u_m = upper % m    and    x_m = new_max % m
  //     q_u = upper / m           q_x = new_max / m
  //
  // we have:
  //
  //     upper   = q_u * m + u_m   with   q_u in Z   and   0 <= |u_m| < m
  //     new_max = q_x * m + x_m          q_x in Z         0 <= |x_m| < m
  //
  // with whatever convention the C/C++ implementation is using for / and %.
  // These relations simply reflect the definition of quotient and remainder.
  //
  // Let i be some integer represented in the same way:
  //
  //     i = q_i * m + i_m         with   q_i in Z   and   0 <= |i_m| < m
  //
  // and consider the value
  //
  //     a = q_x * m + i_m         with    | x_m - i_m | < m              (*)
  //                               and       x_m - i_m   = new_max - a 
  //
  // The inequality (*) holds because quotient and remainder are computed
  // according to the same convention (supplied by the C/C++ implementation).
  //
  // If  a <= new_max  then from (*) we get   new_max - m < a <= new_max
  // i.e.  a  is the representative of  i mod m  that satisfies the desired
  // maximum condition w.r.t. new_max.
  //
  // If  a > new_max  then from (*) we get   new_max - m < a - m < new_max
  // i.e.  a - m  is the representative of  i mod m  that satisfies the
  // maximum condition.
  //
  // We apply this to  i := .upper  in order to get the right representative
  // r_upper of .upper mod m:

  HOST_WIDE_INT u_m = upper % m;
  HOST_WIDE_INT q_x = new_max / m;
  HOST_WIDE_INT r_upper = q_x * m + u_m;

  if (r_upper > new_max)
    r_upper -= m;

  /* Step 2:   Shift the interval (by some multiple of m) in such a way that
     =======   .upper is mapped to its representative r_upper mod m.  */

  // Reducing the range mod m means shifting it by an offset:
  // r_upper = upper + o  -->  o = r_upper - upper = r_lower - lower:

  range_t r = (*this) + (r_upper - upper);

  /* Step 3:   Determine (the smallest) superset of the reduction which
     ======    comprises an interval (when regarded as integers).  */

  if (r.lower > new_max - m)
    // If the representative of .lower satisfies the maximum condition then
    // the projected range is the right result.
    return r;
  else
    // Otherwise the range "wraps around" and we return the full range, i.e.
    // all elements of Z / m*Z  in the desired representative system.
    return range_t (new_max + 1 - m, new_max);
}


/* Return (a superset of) the canonical projection  p: Z --> Z / m*Z  where
   Z / m*Z is represented by a representative system which is convenient
   for code CODE in mode MODE.

   This method "translates" between TriCore instructions, i.e. target specific
   RTXes and their mathematical equivalent.  We state as much as possible in
   the mathematical language and notation.  This avoids much special-casing
   which would be needed in a machine dependent formulation.  */

range_t
range_t::mod (enum rtx_code code, enum machine_mode mode) const
{
  /* As already mentioned, all machine instructions are periodic (with period
     a divisor of 2^32).  This means the instructions actually operate on
     residue classes, i.e. on  Z / m*Z  rather than on the integers.  We
     chose a representative system S of Z / m*Z in such a way that CODE
     restricted to S is the same as some well known function  f: Z --> Z
     where in the latter case we identify equivalence classes with their
     representative in Z.  Consequently, we may use f in place of CODE:MODE
     as indicated by the following diagram:

                              CODE
                   Z / m*Z    --->   Z / m*Z

                     |                 ^
                     |         #       |
                     v                 |
                               f
                     Z        --->     Z

     For all supported RTX codes there is a straight forward function f.
     For example, SS_ABS restricted to S is  x --> min (|x|, INT_MAX).

     Also notice that SIGN_EXTEND and ZERO_EXTEND are *not* extensions from
     the mathematical point of view, they are *reductions* modulo some integer:
     For example we have

         (zero_extend:SI (reg:HI))    is    f:  x  -->  x mod 2^16

     in the smallest non-negative representative system.  */

  HOST_WIDE_INT period = range_t (mode, false /* whatever */).size();

  switch (code)
    {
    case ZERO_EXTEND:
    case UMIN:
    case UMAX:
      return mod (period, tric_max_for_mode (mode, false));

    case SIGN_EXTEND:
    case SMIN:
    case SMAX:
      return mod (period, tric_max_for_mode (mode, true));

    case ABS:
      return mod (period, 1 + tric_max_for_mode (mode, true));

    case SS_ABS:
      return mod (period, 1 + tric_max_for_mode (mode, true));

    case SUBREG:
      // SUBREG just specifies how the caller (some extension in our case)
      // will access SUBREG_REG. SUBREG itself does not change its argument,
      // hence return the range unaltered.
      return *this;

    default:
      gcc_unreachable();
    }
}


/* Let the function f act on I, the input range.  Return a subset S of Im(f),
   the image of f, such that  f(I) <= S <= Im(f).  */

range_t
range_t::image (const_rtx f) const
{
  enum rtx_code code = GET_CODE (f);
  HOST_WIDE_INT k = 0;

  if (BINARY_P (f))
    {
      gcc_assert (CONST_INT_P (XEXP (f, 1)));

      /* For binary operations we define

             f (x) := f_k (x) := f (x, k)

         where k is a compile-time constant.  Non-constant k (i.e. multi
         variate functions) are not supported because we cannot determine
         the image of f then.  */

      bool signed_p = (SMIN == code || SMAX == code);

      k = SIval (XEXP (f, 1), signed_p);
    }

  range_t im_f = range_t::none;

  switch (code)
    {
    case ABS:
      im_f = abs();
      break;

    case SS_ABS:
      im_f = ss_abs();
      break;

    case UMIN:
    case SMIN:
      im_f = min (k);
      break;

    case UMAX:
    case SMAX:
      im_f = max (k);
      break;

    case SUBREG:
      if (0 == SUBREG_BYTE (f))
        // SUBREG just specifies how the caller (some extension in our case)
        // will access SUBREG_REG. SUBREG itself does not change the value,
        // hence return the range unaltered.
        im_f = *this;
      break;

    case ZERO_EXTEND:
    case SIGN_EXTEND:
      im_f = mod (code, GET_MODE (XEXP (f, 0)));
      break;

    default:
      break;
    }

  gcc_assert (im_f != range_t::none);

  return im_f;
}


/* If expression F (taken as a function of some variable x) is a projection,
   then return that x.  Otherwise, return NULL_RTX.

   A function is called a `projection' if it satisfies f o f = f.

   All the functions below are obviously projections, e.g.
   (char)(char) x = (char) x   or  abs (x) = abs (abs (x))  etc.

   Apart from being projections, it must be possible to represent the image
   of F for a given domain.  A proper superset will *not* do, c.f. the
   comments in `tric_optimize_minmax_1' w.r.t. projections.  For example,
   f = f_k(x) = x AND k is obviously a projection, but we have no means to
   represent the image of f except in the case where k can be represented as
   k = 2^n - 1.  For such f the image of a range can be representd: it is
   some range, too.  (f comprises the remainder mod 2^n in that case.) */

static rtx
tric_is_projection (const_rtx f)
{
  const char *form = GET_RTX_FORMAT (GET_CODE (f));

  if (GET_MODE (f) != SImode
      || form[0] != 'e')
    return NULL_RTX;

  rtx x = XEXP (f, 0);

  switch (GET_CODE (f))
    {
    case UMIN:   case UMAX:
    case SMIN:   case SMAX:
      if (CONST_INT_P (XEXP (f, 1)))
        return x;
      break;

    case ZERO_EXTEND:
    case SIGN_EXTEND:
      if (QImode == GET_MODE (x)
          || HImode == GET_MODE (x))
        return x;
      break;

    case ABS:
    case SS_ABS:
      return x;

    default:
      break;
    }

  return NULL_RTX;
}


/* If f(x) is opt to be optimized by `tric_optimize_minmax' then return
   that x, the argument of F.  Otherwise, return NULL_RTX.  If we support
   a binary operation F, this means we take f(x) := f(x,k) where k is a
   compile time constant, i.e. f is an univariate function parameterized
   by k.  */

static rtx
tric_supported_rtx (const_rtx f)
{
  // We support SUBREGs because they occur as wrapper of the argument of
  // sign_extend and zero_extend.  Return SUBREG_REG in that case because
  // SUBREG_REG is the effective argument sign_extend / zero_extend will
  // operate on.

  if (SUBREG == GET_CODE (f))
    return (0 == SUBREG_BYTE (f)
            && SImode == GET_MODE (SUBREG_REG (f))
            && (// (SUBREG:QI (:SI) 0)
                QImode == GET_MODE (f)
                // (SUBREG:HI (:SI) 0)
                || HImode == GET_MODE (f)))
      ? SUBREG_REG (f)
      : NULL_RTX;

  // Apart from SUBREGs we only support projections.  We could also support
  // functions like x --> x + 1 etc. and track their action on ranges.
  // However, we could not take advantage of that information at the current
  // stage of compilation (insn combination) unless we supported complex fake
  // patterns involving these expressions, and then split them later in split1.
  // For now we assume that respective optimization opportunities are rare and
  // not worth the hassle, hence we just consider projections.

  return tric_is_projection (f);
}


/* Mutually recursive part of `tric_optimize_minmax'.  Optimize expression F.
   As a side effect adjust the range R.  The returned function has the
   same effect like F, i.e. for all specified inputs the returned function
   evaluates to the same values like F.  */

static rtx
tric_optimize_minmax_1 (rtx f, range_t &r)
{
  rtx new_f = NULL_RTX;

  // Regard f as an univariate function.  Get the argument of that function.
  // If NULL_RTX is returned, the respective function (rtx) is not supported
  // by this optimization.
  rtx old_x = tric_supported_rtx (f);

  if (!old_x)
    {
      // f(x) is not supported: Return all possible values and f unaltered.
      r = range_t::all;
      return f;
    }

  // If f(x) is supported, then recursively optimize its argument x (in depth
  // first traversal) and adjust the range R.
  rtx x = tric_optimize_minmax_1 (old_x, r);

  // Get a (superset of a) convenient domain for f.
  const range_t& dom_f = r.mod (GET_CODE (f), GET_MODE (f));

  // Determine the image of the domain under f.
  const range_t& im_f = dom_f.image (f);

  // We use the following proposition with S = dom_f to determine whether we
  // may optimize out a projection f:
  //
  //     If   dom_f <= Im(f)   then   f(x) = x   for all x in dom_f.
  //
  // If this is not obvious enough:
  //
  // Proposition:
  // Let f: S --> S be a projection and S <= Im(f).  Then f is the identity.
  //
  // Proof:
  // Just pick some x in S <= Im(f).  As x in Im(f), there is some u in S so
  // that f(u) = x and hence:  x = f(u) = (fof)(u) = f(f(u)) = f(x).  QED.

  if (tric_is_projection (f)
      && dom_f <= im_f)
    {
      switch (GET_CODE (f))
        {
        case ZERO_EXTEND:
        case SIGN_EXTEND:
          if (SUBREG == GET_CODE (x))
            {
              // The very argument of SIGN/ZERO_EXTEND is SUBREG_REG (x) and
              // not x.  SUBREG just serves to adjust the machine mode, thus
              // SUBREG_REG (x) is the right new_f.

              if (0 == SUBREG_BYTE (x)
                  && SImode == GET_MODE (SUBREG_REG (x)))
                {
                  new_f = SUBREG_REG (x);
                  break;
                }
              else
                gcc_unreachable();
            }

          // FALLTHRU to default

        default:
          // f = id, thus we may replace f(x) by x.
          new_f = x;
          break;
        }
    }

  int32_t k;
  enum rtx_code code = GET_CODE (f);

  // In some cases we may use UMIN / UMAX instead of SMIN / SMAX and take
  // advantage of the larger positive immediate range (u9_operand, i.e.
  // 0..511 instead of s9_operand, i.e. 0..255).  */

  if (!new_f
      // If SMAX / SMIN always sees ...
      && (SMAX == code
          || SMIN == code)
      // ... an argument which is positive when taken as signed ...
      && r <= range_t (0, TRIC_INT_MAX)
      // ... and similar for the parameter ...
      && (k = (int32_t) INTVAL (XEXP (f, 1))) >= 0)
    {
      // ...then we may use the unsigned version of the rtx_code.  This takes
      // advantage of the extended immediate range of UMIN / UMAX w.r.t.
      // SMIN / SMAX.  The only exception is SMIN + INT16_MAX because SAT.H
      // is better than UMIN (x, 32767).
      if (SMAX == code
          || k != 32767)
      new_f = gen_rtx_fmt_ee (SMIN == code ? UMIN : UMAX,
                              SImode, x, XEXP (f, 1));
    }

  if (new_f)
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
        htc_dump ("Replaced %I:\n%r\n\nBy this instruction:\n%r\n\n",
                   r, f, new_f);
    }
  else
    {
      // If something changed, then rebuild the new rtx; patching the original
      // one, i.e. replacing XEXP (f, 0), is not appropriate.

      if (x == old_x)
        {
          new_f = f;
        }
      else
        {
          new_f = copy_rtx (f);
          XEXP (f, 0) = x;
        }

      if (r != im_f)
        if (dump_file && (dump_flags & TDF_DETAILS))
          htc_dump ("Constrained %I to %I by this instruction:\n%r\n\n",
                     r, im_f, new_f);

      r = im_f;
    }

  return new_f;
}


/* Optimize F, i.e. try to replace F by an equivalent expression
   or return F unaltered.  */

static rtx
tric_optimize_minmax (rtx f)
{
  enum rtx_code code = GET_CODE (f);
  rtx x = tric_supported_rtx (f);

  if (!x
      // f(x) is supported by this optimization.  Filter out some more RTXes
      // which are not worth applying the whole story and shortcut-return f.
      // This reduces babbling in the dump file for RTXes which cannot be
      // simplified, anyway.
      || REG_P (x)
      || MEM_P (x)
      // SUBREG is only supported as argument of sign_extend / zero_extend.
      || SUBREG == code
      // (CODE (SUBREG (REG) n)) cannot be simplified.
      || (SUBREG == GET_CODE (x)
          && REG_P (SUBREG_REG (x))))
    return f;

  // Start with the full range of possible input values.
  range_t r = range_t::all;

  // Try to replace f by an equivalent expression.
  rtx new_f = tric_optimize_minmax_1 (f, r);

  if (1 == r.size())
    // If the output range just consists of one integer, then return this
    // integer in place of f.
    new_f = gen_int_mode (r.lower, SImode);

  return new_f;
}


/* Similar to double-int.c:double_int_split_digit (it's static there).
   Splits last digit of *CST (taken as unsigned value) in BASE and returns it.
   BASE = 0 stands for 2^32.  */

unsigned
tric_double_int_pop_digit (double_int *cst, unsigned base)
{
  double_int b, rem;

  b = base
    ? double_int::from_uhwi ((unsigned HOST_WIDE_INT) base)
    : double_int_zero.set_bit (32);

  *cst = cst->udivmod (b, FLOOR_DIV_EXPR, &rem);
  
  return (unsigned) rem.to_uhwi();
}


/* Get a double_int representing the bits of a constant rtx X
   which is one of CONST_INT, CONST_DOUBLE.  */

static double_int
tric_const_double_value (rtx x)
{
  enum machine_mode mode = GET_MODE (x);
  
  if (CONST_DOUBLE_P (x)
      && mode != VOIDmode)
    {
      if (DFmode == mode)
        x = simplify_gen_subreg (DImode, x, DFmode, 0);
      else if (SFmode == mode)
        x = simplify_gen_subreg (SImode, x, SFmode, 0);
      else if (HFmode == mode)
        x = simplify_gen_subreg (HImode, x, HFmode, 0);
      else
        {
          htc_edump ("%?: %r\n", x);
          gcc_unreachable();
        }
    }

  return rtx_to_double_int (x);
}


/* Add NODE to LIST.  Return true if NODE is already contained in LIST.  */

static bool
tric_node_seen (tree *list, tree node)
{
  tree l;
  
  for (l = *list; l != NULL_TREE; l = TREE_CHAIN (l))
    {
      if (node == TREE_VALUE (l))
        return true;

      if (TREE_CHAIN (l) == NULL_TREE)
        break;
    }

  *list = chainon (build_tree_list (NULL_TREE, node), *list);

  return false;
}


/* Split X into its low 32 bit and high 32 bit parts.
   Store the low part in *LO and the high part in *HI.  */

void
tric_split_const_rtx (rtx x, unsigned *lo, unsigned *hi)
{
  double_int dval = tric_const_double_value (x);
  *lo = tric_double_int_pop_digit (&dval, 0);
  *hi = tric_double_int_pop_digit (&dval, 0);
}


/* Return -1 if X is not a CONST_INT.  Return the value of X if it is
   a CONST_INT.  Notice that HOST_WIDE_INT is always 64 bits wide, even on
   32-bit hosts.  */

static HOST_WIDE_INT
tric_rtx_to_hwi (const_rtx x)
{
  if (CONST_INT_P (x))
    return INTVAL (x) & GET_MODE_MASK (SImode);

  return -1;
}


/* Return -1 if X is not a known integer constant at compile time.
   Return the value of X if it is know at compile time.  */

static HOST_WIDE_INT
tric_tree_to_hwi (const_tree x)
{
  STRIP_NOPS (x);

  if (INTEGER_CST == TREE_CODE (x))
    return TREE_INT_CST_LOW (x) & GET_MODE_MASK (SImode);

  return -1;
}


/* Get something like `(unsigned int) VAL' for the target.  */

static tree
tric_tree_uint (HOST_WIDE_INT val)
{
  return fold_convert (unsigned_type_node,
                       build_int_cst (unsigned_type_node,
                                      val & GET_MODE_MASK (SImode)));
}


/* Set machine_function.pic_offset.reg as singleton and return it.
   This is only used if -mcode-pic is on.  */

static rtx
tric_pic_offset_init (void)
{
  rtx *picoff = &cfun->machine->pic_offset.reg;
  rtx *symbol = &cfun->machine->pic_offset.symbol;

  if (*picoff == NULL_RTX)
    {
      rtx seq, insn;
      rtx a11 = gen_rtx_REG (SImode, REG_A11);
      basic_block entry_block = ENTRY_BLOCK_PTR_FOR_FN (cfun);
      edge entry_edge;
      char label[32];

      sprintf (label, ".PICOFF%d", ++tric_picoff_labelno);
      *symbol = gen_rtx_SYMBOL_REF (SImode, ggc_strdup (label));

      /* Can't deal with multiple successors of the entry block at the moment.
         Function should always have at least one entry point.  */

      gcc_assert (single_succ_p (entry_block));
      entry_edge = single_succ_edge (entry_block);

      start_sequence ();

      /* Load current code position of *SYMBOL to A11.  */

      emit_insn (gen_load_pc (*symbol));
      emit_move_insn (*picoff = gen_rtx_REG (SImode, REG_PIC), a11);

      /* Set pseudo *PICOFF to the offset between the current code position
         and the position determined at link time.  */

      emit_move_insn (*picoff,
                      gen_rtx_MINUS (Pmode, *picoff,
                                     force_reg (Pmode, *symbol)));

      /* Restore A11 to its original value.  */

      emit_move_insn (a11, get_hard_reg_initial_val (Pmode, REG_A11));
      
      seq = get_insns ();
      end_sequence ();

      /* Set location to prologue.  This corresponds to the
         opening '{' of the current function's body.  */

      for (insn = seq; insn != NULL_RTX; insn = NEXT_INSN (insn))
        if (INSN_P (insn))
          INSN_LOCATION (insn) = prologue_location;

      /* Emit at the very start of the function.  */

      insert_insn_on_edge (seq, entry_edge);
    }

  return *picoff;
}


/***********************************************************************
 ** tricore-specific passes.
 ***********************************************************************/

/***********************************************************************
 ** .tric-split1: Determine whether pass split1 has finished.
 ***********************************************************************/

/* FIXME: We compose insns by means of insn combine and split them in split1.
      We don't want IRA/reload to combine them to the original insns again
      because that avoids some CSE optimizations if constants are involved.
      If IRA/reload combines, the recombined insns get split again after
      reload, but then CSE does not take place.
         It appears that at present there is no other way to take away the
      insns from IRA.  Notice that split1 runs unconditionally so that all our
      insns will get split no matter of command line options.  */

bool
tric_gate_split1 (void)
{
  if (tric_after_split1_pass_number == 0
      || !current_pass
      || current_pass->static_pass_number < tric_after_split1_pass_number)
    return true;

  return false;
}


static const pass_data tric_pass_data_notice_split1 =
{
  RTL_PASS, /* type */
  "tric-split1", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  false, /* has_gate */
  false, /* has_execute */
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  0, /* todo_flags_finish */
};

class tric_pass_notice_split1 : public rtl_opt_pass
{
public:
  tric_pass_notice_split1 (gcc::context *ctxt)
    : rtl_opt_pass (tric_pass_data_notice_split1, ctxt)
  {}

  int get_next_pass_number (void)
  {
    return next->static_pass_number;
  }

}; // class tric_pass_notice_split1


/***********************************************************************
 ** .tric-anchor: Strict RTL pass to improve constant loading.
 ***********************************************************************/

/* Loading compile-time constants can be improved by (re)using known values
   held in registers.  This is addressed by `TARGET_CONST_ANCHOR' which has
   several issues:

   1) It only works with additions up to the power-of-2 provided by that
      macro.  There are, however, much more opportunities with the present
      ISA.  One prominent example is ADDIH which cannot be modelled.

   2) TARGET_CONST_ANCHOR is used by CSE on non-strict RTL which runs before
      insn combine.  This can make it harder for combine to synthesize
      instructions that support large constant.  Examples are AND with
      constant that's implemented using INSERT or 64-bit multiplications and
      multiply-add with constants.

   3) CSE might increase register pressure or introduce operations that
      turn out to be cross-pipeline after register allocation.

  This new pass addresses all of these drawbacks.

  The pass runs right after register allocation and before peephole2, hence
  prior to all post-reload split passes and before sched2.  */


/* This function is used in conditions for post-reload move splits.  */

bool
tric_anchor_completed (void)
{
  return (reload_completed
          && cfun
          && cfun->machine->anchor_completed);
}


/* Gate function for .tric anchor.  It is always TRUE, e.g. even with
   optimization turned off, because the execute method has to set
   `machine_function.anchor_completed'.  */

static bool
tric_gate_const_anchor (void)
{
  return true;
}


/* Print a neat representation of HSET to DUMP_FILE.  */

static void
tric_print_hard_reg_set (const char *prefix, HARD_REG_SET hset)
{
  if (dump_file)
    {
      fprintf (dump_file, "%s{ ", prefix);
      for (int i = 0; i < FIRST_PSEUDO_REGISTER; i++)
        {
          if (TEST_HARD_REG_BIT (hset, i))
            fprintf (dump_file, "%d ", i);
        }
      fprintf (dump_file, "}\n");
    }
}


/* Return NULL_RTX or a pattern that loads XVAL, a value known at compile time,
   to hard register XREG.  XVALS[] is an array of CONST_INTs that fit SImode
   and that are held in SImode hard register n provided HR_KNOWN[n] is set.  */

static rtx
tric_maybe_anchor_const32 (rtx xreg, rtx xval, rtx xvals[32],
                           HARD_REG_SET hr_known)
{
  enum rtx_code code = UNKNOWN;
  rtx pat = NULL_RTX;
  xval = simplify_gen_subreg (SImode, xval, GET_MODE (xreg), 0);
  xreg = simplify_gen_subreg (SImode, xreg, GET_MODE (xreg), 0);

  unsigned reg0 = D_REG_P (xreg) ? REG_D0 : REG_A0;

  for (unsigned regno = reg0; regno < reg0 + 16; regno++)
    {
      rtx pat2;
      if (!TEST_HARD_REG_BIT (hr_known, regno)
          || !(pat2 = tric_arith_CONST_INT (xreg, xval,
                                            gen_rtx_REG (SImode, regno),
                                            xvals[regno])))
        continue;

      if (SET == GET_CODE (pat2))
        {
          // We cannot get better than SET, hence stop on SET.
          pat = pat2;
          break;
        }

      if (PLUS == GET_CODE (SET_SRC (pat2))
          && (// Prefer additions over anything else (except the SET above).
              code != PLUS
              // Prefer short additions over long ones.
              || s4_operand (XEXP (SET_SRC (pat2), 1), SImode)))
        {
          code = PLUS;
          pat = pat2;
        }
    }

    return pat;
}


/* Return NULL or a sequence of insns that load XVAL to XREG.  XREG is a
   64-bit hard register and XVAL represents a constant known at compile time.
   XVALS[] is an array of CONST_INTs that fit SImode and that are held in
   SImode hard register n provided HR_KNOWN[n] is set.  If NULL is returned
   then XVAL can be loaded to XREG by a single (64-bit) MOV insn. */

static rtx
tric_anchor_const64 (rtx xreg, rtx xval, rtx xvals[32], HARD_REG_SET hr_known)
{
  int pos, width;
  rtx xreg_lo, xreg_hi, xval_lo, xval_hi, pat = NULL_RTX;
  xval = simplify_gen_subreg (DImode, xval, GET_MODE (xreg), 0);
  xreg = simplify_gen_subreg (DImode, xreg, GET_MODE (xreg), 0);
  tric_split_di (&xval, &xval_lo, &xval_hi, 1);
  tric_split_di (&xreg, &xreg_lo, &xreg_hi, 1);
  uint32_t lo = INTVAL (xval_lo);
  uint32_t hi = INTVAL (xval_hi);

  start_sequence ();

  // Part 1:  Try to find a 1-insn sequence without using XVALS.  This
  //          can be a 64-bit MOV or an IMASK with appropriate operands.

  if (E_REG_P (xreg))
    {
      if (TRIC_HAVE_MOV64
          && satisfies_constraint_Ksg (xval))
        // TC1.6 has MOV for sign-extended 16-bit values.
        pat = gen_rtx_SET (VOIDmode, xreg, xval);
      else if (0 == INTVAL (xval))
        // 0.0
        pat = gen_imaskdi (xreg, const0_rtx, const0_rtx, const0_rtx);
      else if ((pos = ones_mask (hi, &width)) >= 0
               && width < 32)
        {
          uint32_t val = (lo >> pos) & 0xf;

          if ((val << pos) == lo)
            // 1.0, 0.5, 2.0, ...
            pat = gen_imaskdi (xreg, gen_int_mode (val, SImode),
                               GEN_INT (pos), GEN_INT (width));
        }

      if (!pat
          && hi == 0
          && (pos = ones_mask (lo, &width)) >= 0
          && width <= 4)
        // Small bit cluster in the low part: 1 << n ... 15 << n
        pat = gen_imaskdi (xreg, gen_int_mode (lo >> pos, SImode),
                           GEN_INT (pos), const0_rtx);
    }

  // Part 2:  Try to find a 1-insn sequence using XVALS.  This can be a
  //          64-bit MOV or a sign- or zero-extend.

  // Test for plain 64-bit move from XVALS[] to XREG.

  if (TRIC_HAVE_MOV64
      && E_REG_P (xreg))
    for (unsigned regno = REG_D0; !pat && regno < 16; regno += 2)
      if (TEST_HARD_REG_BIT (hr_known, regno)
          && TEST_HARD_REG_BIT (hr_known, regno + 1)
          && INTVAL (xval_lo) == INTVAL (xvals[regno])
          && INTVAL (xval_hi) == INTVAL (xvals[regno + 1]))
        pat = gen_rtx_SET (VOIDmode, xreg, gen_rtx_REG (DImode, regno));

  // Test for sign- or zero-extend from a 32-bit XVALS[] to 64-bit XREG.

  if (E_REG_P (xreg)
      && (const0_rtx == xval_hi
          || constm1_rtx == xval_hi))
    for (unsigned regno = REG_D0; !pat && regno < 16; regno++)
      if (TEST_HARD_REG_BIT (hr_known, regno)
          && INTVAL (xval_lo) == INTVAL (xvals[regno]))
        {
          if (hi == 0)
            pat = gen_zero_extendsidi2 (xreg, gen_rtx_REG (SImode, regno));
          else if (lo & 0x80000000)
            pat = gen_extendsidi2 (xreg, gen_rtx_REG (SImode, regno));
        }

  // There are no more simple 1-insn sequences.

  if (pat)
    {
      // Emit the 1-insn sequence if we found one.
      emit_insn (pat);

      if (SET == GET_CODE (pat)
          && CONST_INT_P (SET_SRC (pat)))
        {
          // Avoid trivial replacements
          end_sequence();
          return NULL_RTX;
        }
    }
  else
    {
      // Part 3:  We need at least 2 instructions.

      rtx xdummy;
      int n_lo = tric_split_const_int (xval_lo, &xdummy, &xdummy, SET);
      int n_hi = tric_split_const_int (xval_hi, &xdummy, &xdummy, SET);
      bool have_lo = true;

      // Load the low part as 1-insn sequence if applicable.

      if (1 == n_lo)
        emit_move_insn (xreg_lo, xval_lo);
      else if ((pat = tric_maybe_anchor_const32 (xreg_lo, xval_lo,
                                                 xvals, hr_known)))
        emit_insn (pat);
      else
        have_lo = false;

      // Update the information that `tric_maybe_anchor_const32' for the high
      // part is about to use.

      if (have_lo)
        {
          xvals[REGNO (xreg_lo)] = xval_lo;
          SET_HARD_REG_BIT (hr_known, REGNO (xreg_lo));
        }

      // Load the high part as 1- or 2-insn sequence.

      if (1 == n_hi)
        emit_move_insn (xreg_hi, xval_hi);
      else if ((pat = tric_maybe_anchor_const32 (xreg_hi, xval_hi,
                                                 xvals, hr_known)))
        emit_insn (pat);
      else
        tric_emit_move_CONST_INT (xreg_hi, xval_hi);

      if (!have_lo)
        {
          // Update the information that `tric_maybe_anchor_const32' for
          // the low part is about to use.
          xvals[REGNO (xreg_hi)] = xval_hi;
          SET_HARD_REG_BIT (hr_known, REGNO (xreg_hi));

          if ((pat = tric_maybe_anchor_const32 (xreg_lo, xval_lo,
                                                xvals, hr_known)))
            emit_insn (pat);
          else
            tric_emit_move_CONST_INT (xreg_lo, xval_lo);
        }
    }

  rtx seq = get_insns();
  end_sequence ();

  return seq;
}


typedef struct
{
  // .VALS[] holds a const_xxx that's known to be the content of hard
  // register n iff bit n of .KNOWN is set.  0 <= n < 32.
  HARD_REG_SET known;
  // Magic "32" is the number of TriCore's general purpose registers.
  rtx vals[32];
  // Just for sanity checking.
  bool valid;
} hreg_vals_t;


/* Go through basic block BB and update *HREGS, the knowledge about the
   general purpose registers when entering BB, to respective knowledge at
   the end of BB.  Also try to replace reg-const moves by more efficient
   instructions and to replace some reg-reg moves by preferred reg-const
   moves.  */

static void
tric_const_anchor_bb (basic_block bb, hreg_vals_t *hregs)
{
  HARD_REG_SET &hr_known = hregs->known;
  rtx *xvals = hregs->vals;

  rtx insn, next;
  // Value held in general purpose hard register n.

  FOR_BB_INSNS_SAFE (bb, insn, next)
    {
      if (!NONDEBUG_INSN_P (insn))
        continue;

      // Represents all registers changed by the current insn.
      HARD_REG_SET hr_set;
      // Mode of the current move (if any).  Also indicates that the current
      // insn sets a hard reg to a known value if != VOIDmode.
      enum machine_mode mode = VOIDmode;
      rtx rsrc, src = NULL_RTX, dest = NULL_RTX;
      rtx seq = NULL_RTX;

      find_all_hard_reg_sets (insn, &hr_set);

      bool is_set = (// This sets a general purpose hard register.
                     SET == GET_CODE (PATTERN (insn))
                     && REG_P (dest = SET_DEST (PATTERN (insn)))
                     && REGNO (dest) <= REG_A15
                     // Don't mess with fixed registers.
                     && !fixed_regs[REGNO (dest)]
                     // Don't use unused regs left over from previous passes.
                     && !find_reg_note (insn, REG_UNUSED, dest));

      /* If this insn loads a compile-time constant to a register
        
             (set  reg  const_***)
        
         then try to find a better sequence than just MOVH + ADDI / LEA.  */

      if (is_set
          // The register is set to a compile time constant.
          && (CONST_INT_P (src = SET_SRC (PATTERN (insn)))
              || CONST_DOUBLE_P (src)))
        {
          mode = GET_MODE (dest);

          if (GET_MODE_SIZE (mode) <= 4)
            seq = tric_maybe_anchor_const32 (dest, src, xvals, hr_known);
          else if (GET_MODE_SIZE (mode) == 8)
            seq = tric_anchor_const64 (dest, src, xvals, hr_known);
        } // reg-const move

      /* Some passes like CSE transfrom loading of the same constant to
         reg-reg moves
         
             (set  reg_A  const_***)
             (set  reg_B  reg_A)
        
         Undo this if the value of the source register is known and can be
         loaded by 1 instruction.  Such reg-reg moves introduce additional
         scheduling dependencies or might represent cross-pipeline moves.
         Hence we try to undo such moves and resurrect the original set to
         the constant.  This also makes it easier for peephole2 to combine
         two sets into one 64-bit move like MOV %En, 0.  */

      if (is_set
          && GET_MODE_SIZE (GET_MODE (dest)) <= 4
          && REG_P (rsrc = SET_SRC (PATTERN (insn)))
          && REGNO (rsrc) <= REG_A15
          // Don't mess with fixed registers.
          && !fixed_regs[REGNO (rsrc)]
          // Is this is a reg-reg move where the source has a known value?
          && TEST_HARD_REG_BIT (hr_known, REGNO (rsrc)))
        {
          mode = GET_MODE (dest);
          src = xvals[REGNO (rsrc)];

          if (// Loading 0...15 to A-regs ...
              (A_REG_P (dest) && u4_operand (src, SImode))
              // ... and -8...7 to D-regs just costs 2 bytes.
              || (D_REG_P (dest) && s4_operand (src, SImode))
              || (!optimize_size
                  // If not optimizing for size avoid pipeline disruption.
                  && (A_REG_P (rsrc) ^ D_REG_P (dest))
                  && (high_operand (src, SImode)
                      || (D_REG_P (dest)
                          && (s16_operand (src, SImode)
                              || u16_operand (src, SImode)))
                      || (A_REG_P (dest)
                          && satisfies_constraint_KSa (src)))))
            seq = gen_rtx_SET (VOIDmode, dest,
                               simplify_gen_subreg (mode, src, SImode, 0));
        } // reg-reg move

      if (dump_file)
        {
          if (mode != VOIDmode)
            htc_dump ("\n  %r\n", insn);
          fprintf (dump_file, "\n;; Set by insn %d: ", INSN_UID (insn));
          tric_print_hard_reg_set ("", hr_set);
        }

      // Invalidate all registers set or changed or clobbered by this insn.
      AND_COMPL_HARD_REG_SET (hr_known, hr_set);

      if (mode != VOIDmode)
        {
          // If this insn sets DEST to a value known at compile time
          // then record the values of all subregs of DEST in XVALS[]
          // and set respective bits in HR_KNOWN to 1.

          unsigned regno = REGNO (dest);
          for (int i = 0; i < HARD_REGNO_NREGS (regno, mode); i++)
            {
              SET_HARD_REG_BIT (hr_known, regno + i);
              xvals[regno + i]
                = simplify_gen_subreg (SImode, src, mode,
                                       i * GET_MODE_SIZE (SImode));
              htc_dump (";; %s = %r\n", reg_names[regno + i],
                         xvals[regno + i]);
            }

          if (seq)
            {
              // If we found a sequence or a pattern that's better than
              // MOVH + ADDI / LEA then replace the current insn.
              htc_dump (";; Successfully replaced %r by:\n\n", src);
              htc_dump (INSN_P (seq) ? "%L\n" : "%r\n\n", seq);

              emit_insn_before (seq, insn);
              delete_insn (insn);
            }
        }

      tric_print_hard_reg_set (";; Known        : ", hr_known);
    } // insn
}


/* Implement execute() method of pass tric-anchor.  */

/* It works on basic blocks and implements a very simplistic simulation
   of the instructions:  When a compile-time constant is loaded to a register,
   the respective value is recorded.  If the value survives until a subsequent
   load, the compiler tries to use some arithmetic to compute the new value
   from one of the already known values -- the const anchors.  */

static unsigned int
tric_rest_of_const_anchor (void)
{
  /* Even though we currently run before the first post-reload split, some
     mov splitters use anchor_completed in their condition.  This gives us
     more freedom moving around this pass and trying to find the best
     placement.  Splitting mov insns before this pass would obviously
     disturb the optimizations we have in mind.  */

  gcc_assert (!cfun->machine->anchor_completed);
  cfun->machine->anchor_completed = true;

  if (!optimize
      || !tric_opt_const_anchor)
    return 0;

  df_note_add_problem ();
  df_analyze();

  /* Upper boundary for the basic block indices. */
  int max_bb_index = last_basic_block_for_fn (cfun);
  /* Number of basic blocks in the current function. */
  int n_bbs = n_basic_blocks_for_fn (cfun);
  /* HREG_VALS[] is indexed by the basic blocks.  Each element contains
     information about which of the 32 GPRs is holding a known value.  */
  hreg_vals_t *hreg_vals = XNEWVEC (hreg_vals_t, max_bb_index);
  /* Order BBs in such a way that single-predecessor BBs are orderd before
     their predecessor.  This enables us to regard the BBs as disjoint
     union of trees.  Roots of the trees are BBs with more than one
     predecessor.  The remaining vertices (leaves and branchings of the trees)
     are single-predecessor BBs that will inherit the hreg_vals[] knowledge
     from their predecessor.  */
  basic_block *bb_forest = single_pred_before_succ_order ();

  /* Traverse the forest's roots.  The order of the traversal does not matter.
     Each root starts with zero knowledge about the register contents.  */

  htc_dump (";; Starting bb forest\n");

  for (int i = 0; i < max_bb_index; i++)
    if (i < NUM_FIXED_BLOCKS)
      {
        CLEAR_HARD_REG_SET (hreg_vals[i].known);
        hreg_vals[i].valid = true;
      }
    else
      hreg_vals[i].valid = false;

  for (int i = 0; i < n_bbs - NUM_FIXED_BLOCKS; i++)
    if (! single_pred_p (bb_forest[i]))
      {
        basic_block bb = bb_forest[i];
        htc_dump ("\n;; bb %d\n", bb->index);

        gcc_assert (bb->index >= NUM_FIXED_BLOCKS
                    && bb->index < max_bb_index);

        /* Roots start with zero knowledge. */

        CLEAR_HARD_REG_SET (hreg_vals[bb->index].known);
        hreg_vals[bb->index].valid = true;

        tric_const_anchor_bb (bb, & hreg_vals[bb->index]);
      }

  /* Traverse the forest's leaves and branches.  The order of the traversal
     does matter and is performed in such a way that a BB Y which is the
     unique predecessor of BBs X1, X2, ... is handled before these Xs.
     A correct ordering is assured by the fact that the roots are already
     handled and by iterating the result of `single_pred_before_succ_order'
     from last to first (for Ys that are non-roots).  The incoming knowledge
     of a non-root Xn is given by the outcoming knowledge of its Y.  */

  htc_dump ("\n;; Completing bb forest\n");

  for (int i = n_bbs - NUM_FIXED_BLOCKS - 1; i >= 0; i--)
    if (single_pred_p (bb_forest[i]))
      {
        basic_block bb = bb_forest[i];
        basic_block pred = single_pred_edge (bb)->src;

        gcc_assert (bb->index >= NUM_FIXED_BLOCKS
                    && bb->index < max_bb_index
                    && pred->index < max_bb_index);

        /* Non-roots start with their predecessor's results. */

        gcc_assert (hreg_vals[pred->index].valid);
        hreg_vals[bb->index] = hreg_vals[pred->index];

        htc_dump ("\n;; bb %d\n;; Known from bb %d:", bb->index, pred->index);
        tric_print_hard_reg_set (" ", hreg_vals[bb->index].known);

        tric_const_anchor_bb (bb, & hreg_vals[bb->index]);
      }

  XDELETEVEC (bb_forest);
  XDELETEVEC (hreg_vals);

  df_analyze();

#ifdef ENABLE_CHECKING
  verify_flow_info ();
#endif

  return 0;
}


static const pass_data tric_pass_data_const_anchor =
{
  RTL_PASS,      /* type */
  "tric-anchor", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  true,          /* has_gate */
  true,          /* has_execute */
  TV_NONE,       /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  TODO_df_finish | TODO_verify_rtl_sharing | 0, /* todo_flags_finish */
};

class tric_pass_const_anchor : public rtl_opt_pass
{
public:
  tric_pass_const_anchor (gcc::context *ctxt)
    : rtl_opt_pass (tric_pass_data_const_anchor, ctxt)
  {}

  bool gate () { return tric_gate_const_anchor (); }
  unsigned int execute () { return tric_rest_of_const_anchor (); }
}; // tric_pass_const_anchor


/***********************************************************************
 ** Attributes
 ***********************************************************************/

/* A helper for the subsequent function attribute predicates to dig for
   attribute NAME in FUNC, a FUNCTION_DECL or FUNCTION_TYPE.  */

static inline int
tric_lookup_function_attribute1 (const_tree func, const char *name)
{
  tree a;

  if (FUNCTION_DECL == TREE_CODE (func))
    func = TREE_TYPE (func);
    
  gcc_assert (TREE_CODE (func) == FUNCTION_TYPE
              || TREE_CODE (func) == METHOD_TYPE);

  a = lookup_attribute (name, TYPE_ATTRIBUTES (func));
  return a != NULL_TREE;
}


/* Return nonzero if FUNC is an __interrupt__ function */

int
tric_interrupt_function_p (const_tree func)
{
  return tric_lookup_function_attribute1 (func, TRIC_ATTR_INTERRUPT);
}


/* Return nonzero if FUNC is an __interrupt_handler__ function */

int
tric_interrupt_handler_function_p (const_tree func)
{
  return tric_lookup_function_attribute1 (func, TRIC_ATTR_INTERRUPT_HANDLER);
}


/* Return nonzero if FUNC is a __pxhndcall__ PXROS handler call */

int
tric_pxhndcall_function_p (const_tree func)
{
  return tric_lookup_function_attribute1 (func, TRIC_ATTR_PXHNDCALL);
}


/* Return nonzero if FUNC is a __longcall__ function */

int
tric_longcall_function_p (const_tree func)
{
  return tric_lookup_function_attribute1 (func, TRIC_ATTR_LONGCALL);
}


/* Handle a function attribute.
   This is a hook in TARGET_ATTRIBUTE_TABLE, i.e.
   struct attribute_spec.handler.  */

static tree
tric_handle_fntype_attribute (tree *node, tree name,
                              tree args ATTRIBUTE_UNUSED,
                              int flags ATTRIBUTE_UNUSED,
                              bool *no_add_attrs)
{
  if (TREE_CODE (*node) != FUNCTION_TYPE)
    {
      warning (OPT_Wattributes, "%qs attribute only applies to functions",
               IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }

  return NULL_TREE;
}


/* Handle a variable attribute.
   This is a hook in TARGET_ATTRIBUTE_TABLE, i.e.
   struct attribute_spec.handler.  */

static tree
tric_handle_decl_attribute (tree *node, tree name,
                            tree args ATTRIBUTE_UNUSED,
                            int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
  if (!DECL_P (*node))
    {
      warning (OPT_Wattributes, "%qs attribute only applies to variables",
               IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }

  return NULL_TREE;
}



/* Implement `TARGET_ATTRIBUTE_TABLE' */

const struct attribute_spec tric_attribute_table[] =
  {
    /* { name, min_len, max_len,
       decl_req, type_req, fn_type_req, handler, affects_type_identity } */
    { TRIC_ATTR_INTERRUPT,          0, 0,
      false, true,  true,  tric_handle_fntype_attribute, false },
    { TRIC_ATTR_INTERRUPT_HANDLER,  0, 0,
      false, true,  true,  tric_handle_fntype_attribute, false },
    { TRIC_ATTR_PXHNDCALL,          0, 0,
      false, true,  true,  tric_handle_fntype_attribute, false },
    { TRIC_ATTR_LONGCALL,           0, 0,
      false, true,  true,  tric_handle_fntype_attribute, false },
    { TRIC_ATTR_ABSOLUTE,           0, 0,
      true,  false, false, tric_handle_decl_attribute, false },
    { TRIC_ATTR_INDIRECT,           0, 0,
      true,  false, false, tric_handle_decl_attribute, false },
    { TRIC_ATTR_SMALL,              0, 0,
      true,  false, false, tric_handle_decl_attribute, false },
    { HTC_ATTR_ASECTION,            1, 3,
      true,  false,  false, htc_handle_asection_attribute, false },
    { NULL,                      0, 0, false, false, false, NULL, false }
  };


/* Implement `TARGET_HTC_SECTION_FLAGS_FROM_STRING'. */

/* Map the string FLAG_STR of section flags to unsigned long.
   If some unknown flag char is encountered return -1UL.
   No error message will be printed in that case. */

static unsigned long
tric_section_flags_from_string (const char *s_flags)
{
  unsigned long flags = 0;

  while (*s_flags)
    {
      switch (*s_flags++)
        {
        case 'a':
          break;
        case 'w':
          flags |= SECTION_WRITE;
          break;
        case 'x':
          flags |= SECTION_CODE;
          break;
        case 'M':
          flags |= SECTION_MERGE;
          break;
        case 'S':
          flags |= SECTION_STRINGS;
          break;
        case 's':
          flags |= SECTION_SMALL;
          break;
        case 'z':
          flags |= TRIC_SECTION_ABSOLUTE;
          break;
        case 'p':
          flags |= TRIC_SECTION_PCP;
          break;
        case 'B':
          flags |= SECTION_BSS;
          break;
        case 'c':
          if (IN_RANGE (*s_flags, '0', '6'))
            {
              flags = TRIC_SECTION_CORE_SET (flags,
                                             *s_flags - '0' + 1);
              s_flags++;
              break;
            }
          else if (*s_flags == 'g')
            {
              s_flags++;
              break;
            }
          /* FALLTHRU to default */
          
        default:
          /* Handle erroneous flags in some higher level so that we can
             print proper error message */
          return -1UL;
        }
    }

  if ((flags & TRIC_SECTION_ABSOLUTE)
      && (flags & SECTION_SMALL))
    return -1UL;
  
  return flags;
}


/* Implement `TARGET_HTC_SECTION_FLAGS_TO_STRING'. */

static bool
tric_section_flags_to_string (char *s_flags, unsigned int flags,
                              enum flagstring_purpose what)
{
  if (!(flags & SECTION_DEBUG))
    *s_flags++ = 'a';
  if (flags & SECTION_WRITE)
    *s_flags++ = 'w';
  if (flags & SECTION_CODE)
    *s_flags++ = 'x';
  if (flags & SECTION_MERGE)
    *s_flags++ = 'M';
  if (flags & SECTION_STRINGS)
    *s_flags++ = 'S';
  if (HAVE_COMDAT_GROUP && (flags & SECTION_LINKONCE))
    *s_flags++ = 'G';
  if (flags & SECTION_SMALL)
    *s_flags++ = 's';
  if (flags & TRIC_SECTION_ABSOLUTE)
    *s_flags++ = 'z';
  if (flags & TRIC_SECTION_PCP)
    *s_flags++ = 'p';
  if (flags & SECTION_BSS)
    {
      if (what != FLAGSTRING_for_asm_out)
        *s_flags++ = 'B';
    }
  if (flags & TRIC_SECTION_CORE_MASK)
    {
      *s_flags++ = 'c';
      *s_flags++ = '0' + (TRIC_SECTION_CORE_GET (flags) - 1);
    }
  
  *s_flags = '\0';

  return true;
}



/***********************************************************************
 ** Writing ASM file
 ***********************************************************************/

/* Implement `TARGET_ASM_FILE_END' */

static void
tric_asm_file_end (void)
{
  if (htc_log.stat)
    {
      const htc_stat_t *s = & htc_stat;

      htc_edump ("\nHTC Statistics\n"
                 " sections              : %d\n"
                 "     named             : %d\n"
                 "     reused            : %d\n"
                 "     gcc unnamed       : %d\n"
                 "     PCH fixups        : %d\n"
                 , s->htc.section.n_htc_sections
                 , s->htc.section.n_htc_sections_named
                 , s->htc.section.n_htc_sections_reused
                 , s->htc.section.n_unnamed_sections
                 , s->htc.section.n_pch_section_fixups);
      
      htc_edump (" section lookups       : %d\n"
                 "     comparisons       : %d\n"
                 "     hash computations : %d\n"
                 , s->htc.section.lookup.n_calls
                 , s->htc.section.lookup.n_compares
                 , s->htc.section.lookup.n_hashes_computed);

      htc_edump (" secspecs              : %d\n"
                 "     deserialized      : %d\n"
                 "     applied           : %d\n"
                 "     void              : %d\n"
                 , s->htc.section.n_secspecs
                 , s->htc.section.n_secspecs_deserialized
                 , s->htc.section.n_secspecs_applied
                 , s->htc.section.n_secspecs_void);

      htc_edump (" dams                  : %d\n"
                 "     deserialized      : %d\n"
                 , s->htc.section.n_dams
                 , s->htc.section.n_dams_deserialized);
    }
}


/* Implement `TARGET_ASM_TRAMPOLINE_TEMPLATE' */

static void
tric_asm_trampoline_template (FILE *f ATTRIBUTE_UNUSED)
{
  sorry ("nested functions are not supported for this machine");
}


/***********************************************************************
 ** Stack and Frame Layout
 ***********************************************************************/

/* Implement `INITIAL_ELIMINATION_OFFSET' */
/* Return the offset between two registers, one to be eliminated, and the other
   its replacement, at the start of a routine.  */

HOST_WIDE_INT
tric_initial_elimination_offset (int from, int to)
{
  HOST_WIDE_INT offset = 0;

  if (from == FRAME_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    {
      offset = get_frame_size()
        + crtl->outgoing_args_size;
    }
  else if (from == ARG_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    {
      offset = get_frame_size()
        + crtl->outgoing_args_size;
    }
  else if (from == ARG_POINTER_REGNUM && to == FRAME_POINTER_REGNUM)
    {
      offset = 0;
    }
  else
    {
      gcc_unreachable();
    }

  /* EABI 2.2.2.1:  Align to 8 bytes.  */

  offset = (offset + 7) & ~7;
  
  return offset;
}


/***********************************************************************
 ** Prologue and Epilogue
 ***********************************************************************/

/* Emit RTL to add a constant offset to SP */

static inline void
tric_emit_addto_sp (int frame_size, int frame_related_p)
{
  rtx insn;
  rtx offset = GEN_INT (frame_size);
  rtx part1, part2;

  gcc_assert (frame_size != 0);

  tric_split_const_int (offset, &part1, &part2, PLUS);

  if (const0_rtx != part1)
    {
      insn = emit_move_insn (stack_pointer_rtx,
                             gen_rtx_PLUS (Pmode,
                                           stack_pointer_rtx, part1));
      if (frame_related_p)
        RTX_FRAME_RELATED_P (insn) = 1;
    }

  if (NULL_RTX != part2)
    {
      insn = emit_move_insn (stack_pointer_rtx,
                             gen_rtx_PLUS (Pmode,
                                           stack_pointer_rtx, part2));
      if (frame_related_p)
        RTX_FRAME_RELATED_P (insn) = 1;
    }
}


/* EABI 2.2.2.1:  Align frame size of functions to 8 bytes.  */

static int
tric_current_function_frame_size (void)
{
  return (~7) & (get_frame_size() + crtl->outgoing_args_size + 7);
}


/* Emit RTL for the prologue of the current function. */

void
tric_emit_prologue (void)
{
  rtx insn;
  int frame_size = tric_current_function_frame_size();

  /* Init FP */

  if (frame_pointer_needed)
    {
      insn = emit_move_insn (frame_pointer_rtx, stack_pointer_rtx);
      RTX_FRAME_RELATED_P (insn) = 1;
    }

  /* Subtract frame size from SP to set up the frame.  */

  if (frame_size)
    {
      tric_emit_addto_sp (-frame_size, 1);

      if (frame_pointer_needed)
        {
          emit_insn (gen_frame_blockage ());
        }
    }

  if (flag_stack_usage)
    current_function_static_stack_size = frame_size;
}


/* Emit RTL for the epilogue of the current function. */

void
tric_emit_epilogue (int sibling_p)
{

  if (!sibling_p)
    {
      /* Free frame (restore SP and upper context) is done by RET/RFE.
         Expand the RET */
      
      emit_jump_insn (gen_return_insn ());
    }
  else
    {
      int frame_size = tric_current_function_frame_size();

      if (frame_size)
        {
          emit_insn (gen_frame_blockage ());

          tric_emit_addto_sp (frame_size, 0);
        }
    }
}


int
tric_simple_epilogue (void)
{
  return 1;
}


/* Implement `TARGET_ASM_FUNCTION_END_PROLOGUE' */
/* Output summary after end of function prologue.  */

static void
tric_asm_function_end_prologue (FILE *file)
{
  int mask = 0;
  int regno;

  /* Only print this info for -fverbose-asm or -dp */
  
  if (flag_verbose_asm
      || flag_print_asm_name)
    {
      fprintf (file, ASM_COMMENT_START " end prologue\n");

      fprintf (file, ASM_COMMENT_START " frame-pointer needed = %d\n",
               frame_pointer_needed);

      fprintf (file, ASM_COMMENT_START " frame size           = "
               HOST_WIDE_INT_PRINT_DEC "\n", get_frame_size ());

      fprintf (file, ASM_COMMENT_START " outgoing args size   = %d\n",
               crtl->outgoing_args_size);

      fprintf (file, ASM_COMMENT_START " incoming args        = ");

      if (crtl->args.info.args_mask)
        tric_output_reglist (file, crtl->args.info.args_mask);
      else
        fprintf (file, "(void)");

      fprintf (file, "\n");

      for (regno = REG_D0; regno <= REG_A15; regno++)
        if (df_regs_ever_live_p (regno))
          mask |= (1 << regno);

      fprintf (file, ASM_COMMENT_START " regs live            = ");

      if (mask)
        tric_output_reglist (file, mask);
      else
        fprintf (file, "-");
      fprintf (file, "\n");
        
      fprintf (file, ASM_COMMENT_START " return-rtx           = ");
      htc_fdump (file, "%r\n", crtl->return_rtx);

      fprintf (file, ASM_COMMENT_START " function is leaf     = %d\n",
               crtl->is_leaf);
    }
}


/* Implement `TARGET_ASM_FUNCTION_BEGIN_EPILOGUE' */
/* Output summary before start of function epilogue.  */

static void
tric_asm_function_begin_epilogue (FILE *file)
{
  /* Only print this info for -fverbose-asm or -dp */
  
  if (flag_verbose_asm
      || flag_print_asm_name)
    {
      fprintf (file, ASM_COMMENT_START " start epilogue: ");

      if (tric_interrupt_function_p (current_function_decl))
        fprintf (file, TRIC_ATTR_INTERRUPT " ");

      if (tric_interrupt_handler_function_p (current_function_decl))
        fprintf (file, TRIC_ATTR_INTERRUPT_HANDLER " ");

      if (tric_pxhndcall_function_p (current_function_decl))
        fprintf (file, TRIC_ATTR_PXHNDCALL " ");

      fprintf (file, "\n");
    }
}


/***********************************************************************
 ** Builtins
 ***********************************************************************/

/* Implement `RETURN_ADDR_RTX' */
/* Return the value of the return address for the frame COUNT steps up
   from the current frame, after the prologue.
   We punt for everything but the current frame by returning const0_rtx.  */

rtx
tric_return_addr_rtx (int count)
{
  if (count != 0)
    return const0_rtx;

  return get_hard_reg_initial_val (Pmode, REG_A11);
}


/***********************************************************************
 ** Print Operands
 ***********************************************************************/

/* Print the asm for a MEM rtx to file */

static void
tric_print_mem_operand (FILE *file, rtx addr, enum machine_mode mode)
{
  switch (GET_CODE (addr))
    {
    case REG:
      fputc ('[', file);
      tric_print_operand (file, addr, 0);
      fputs ("]0", file);

      break;
            
    case LO_SUM:
      {
        rtx reg = XEXP (addr, 0);
        rtx sum = XEXP (addr, 1);
        
        gcc_assert (REG_P (reg));
        gcc_assert (CONST == GET_CODE (sum) || SYMBOL_REF == GET_CODE (sum));
        fputc ('[', file);
        tric_print_operand (file, reg, 0);
        fputs ("] lo:", file);
        output_address (sum);

        break;
      }
      
    case PLUS:
      {
        rtx reg = XEXP (addr, 0);
        rtx sum = XEXP (addr, 1);

        gcc_assert (REG_P (reg));
        fputc ('[', file);
        tric_print_operand (file, reg, 0);
        fputs ("] ", file);
        output_address (sum);
      }
      break;

    case PRE_INC:
      fputs ("[+", file);
      tric_print_operand (file, XEXP (addr, 0), 0);
      fprintf (file, "]%d", GET_MODE_SIZE (mode));
      break;
            
    case POST_INC:
      fputc ('[', file);
      tric_print_operand (file, XEXP (addr, 0), 0);
      fprintf (file, "+]%d", GET_MODE_SIZE (mode));
      break;

    case PRE_DEC:
      fputs ("[+", file);
      tric_print_operand (file, XEXP (addr, 0), 0);
      fprintf (file, "]-%d", GET_MODE_SIZE (mode));
      break;
            
    case POST_DEC:
      fputc ('[', file);
      tric_print_operand (file, XEXP (addr, 0), 0);
      fprintf (file, "+]-%d", GET_MODE_SIZE (mode));
      break;
            
    case PRE_MODIFY:
      gcc_assert (PLUS == GET_CODE (XEXP (addr, 1)));
      gcc_assert (REGNO (XEXP (addr, 0)) == REGNO (XEXP (XEXP (addr, 1), 0)));
      {
        rtx op1 = XEXP (XEXP (addr, 1), 1);

        fputs ("[+", file);
        tric_print_operand (file, XEXP (addr, 0), 0);
        fprintf (file, "]" HOST_WIDE_INT_PRINT_DEC, INTVAL (op1));
        break;
      }
      
    case POST_MODIFY:
      gcc_assert (PLUS == GET_CODE (XEXP (addr, 1)));
      gcc_assert (REGNO (XEXP (addr, 0)) == REGNO (XEXP (XEXP (addr, 1), 0)));
      {
        rtx op1 = XEXP (XEXP (addr, 1), 1);

        fputc ('[', file);
        tric_print_operand (file, XEXP (addr, 0), 0);
        fprintf (file, "+]" HOST_WIDE_INT_PRINT_DEC, INTVAL (op1));
        break;
      }
      
    case SYMBOL_REF:
      if (tric_symbol_ref_small16_p (addr))
        {
          fputs ("[%A0] SM:", file);
        }

      tric_print_operand (file, addr, 0);
      break;
            
    case CONST_INT:
      fprintf (file, "0x%08x",
               (unsigned int) (GET_MODE_MASK (SImode) & UINTVAL (addr)));
      break;
      
    case CONST:
      {
        rtx sym = XEXP (XEXP (addr, 0), 0);
        if (tric_symbol_ref_small16_p (sym))
          {
            fputs ("[%A0] SM:", file);
          }
        tric_print_operand (file, XEXP (XEXP (addr, 0), 0), 0);
        fputc ('+', file);
        tric_print_operand (file, XEXP (XEXP (addr, 0), 1), 0);
        break;
      }
            
    default:
      htc_edump ("%F: ??? %r\n", addr);
      gcc_unreachable ();
      break;
    }
}


/* Implement `TARGET_PRINT_OPERAND_ADDRESS' */

static void
tric_print_operand_address (FILE *file, rtx x)
{
  tric_print_operand (file, x, '\0');
}


/* Implement `TARGET_PRINT_OPERAND' */

static void
tric_print_operand (FILE *file, rtx x, int code)
{
  if (NULL_RTX == x)
    {
      gcc_assert (targetm.asm_out.print_operand_punct_valid_p (code));
      return;
    }

  if ('I' == code)
    {
      fputs (REG_P (x) ? "i" : "", file);
      return;
    }

  switch (GET_CODE (x))
    {
    case REG:
      {
        int regno = REGNO (x);

        if (E_REGNO_P (regno)  ||  EA_REGNO_P (regno))
          switch (code)
            {
            case 'A':
              if (E_REGNO_P (regno))
                  fprintf (file, "%s%c%d", REGISTER_PREFIX, 'e', regno);
              if (EA_REGNO_P (regno))
                  fprintf (file, "%s%c%d", REGISTER_PREFIX, 'A', regno-REG_A0);
              return;
              
            case 'L':
              fprintf (file, "%s%s", REGISTER_PREFIX, reg_names[regno]);
              return;
            case 'H':
              fprintf (file, "%s%s", REGISTER_PREFIX, reg_names[regno+1]);
              return;
            }
        
        if (0 == code)
          {
            fprintf (file, "%s%s", REGISTER_PREFIX, reg_names[regno]);
            return;
          }

        htc_edump ("%?: %r\n", x);
        output_operand_lossage ("invalid register modifier '%c'", code);
      }
      break;

    case MEM:
      tric_print_mem_operand (file, XEXP (x, 0), GET_MODE (x));
      break;

    case CONST_INT:
    case CONST_DOUBLE:
      {
        unsigned lo32, hi32;
        
        tric_split_const_rtx (x, &lo32, &hi32);

        if ('p' == code)
          {
            /* exact_log2 */
            int pos = exact_log2 (lo32);
            gcc_assert (pos >= 0);
            fprintf (file, "%d", pos);
          }
        else if ('P' == code)
          {
            /* Position (offset) of a packet of ones */
            int pos = ones_mask (lo32 & GET_MODE_MASK (SImode), NULL);
            if (pos < 0)
              {
                htc_edump ("%?:%c %r\n", code, x);
                abort();
              }
              
            gcc_assert (pos >= 0);
            fprintf (file, "%d", pos);
          }
        else if ('M' == code)
          {
            tric_output_reglist (file, lo32);
            break;
          }
        else if ('W' == code)
          {
            /* Width of a packet of ones */
            int width = ones_width (lo32 & GET_MODE_MASK (SImode));

            if (width <= 0)
              {
                htc_edump ("%?:%c %r\n", code, x);
                abort();
              }
            fprintf (file, "%d", width);
          }
        else if ('A' == code)
          /* LOW part 16 Bits */
          fprintf (file, "%d", lo32 & 0xffff);
        else if ('B' == code)
          /* HIGH part 16 Bits */
          fprintf (file, "%d", (lo32 >> 16) & 0xffff);
        else if ('L' == code)
          /* LOW part 32 Bits */
          fprintf (file, "%d", lo32);
        else if ('H' == code)
          {
          /* HIGH part 32 Bits */
            if (CONST_INT_P (x))
              fprintf (file, (lo32 & 0x80000000) ? "-1" : "0");
            else
              fprintf (file, "%d", hi32);
          }
        else
          fprintf (file, "%d", lo32);
      }
      break;

    case CONST:
      if (MINUS == GET_CODE (XEXP (x, 0)))
        {
          fprintf (file, "((");
          output_addr_const (file, XEXP (XEXP (x, 0), 0));
          fprintf (file, ")-(");
          output_addr_const (file, XEXP (XEXP (x, 0), 1));
          fprintf (file, "))");
          
          break;
        }

      /* FALLTHRU */
    case SYMBOL_REF:
    case CODE_LABEL:
      output_addr_const (file, x);
      break;
      
    case LABEL_REF:
      fprintf (file, ".L%d", CODE_LABEL_NUMBER (XEXP (x, 0)));
      break;
            
    case CONST_STRING:
      fprintf (file, "%s", XSTR (x, 0));
      break;

    case HIGH:
      fprintf (file, "hi:");
      if (CONST == GET_CODE (XEXP (x, 0))
          && MINUS == GET_CODE (XEXP (XEXP (x, 0), 0)))
        tric_print_operand (file, XEXP (x, 0), '\0');
      else
        output_addr_const (file, XEXP (x, 0));
      break;

    default:
      htc_edump ("%F: ??? %r\n", x);
      gcc_unreachable ();
    }
}


/* Implement `TARGET_FUNCTION_OK_FOR_SIBCALL' */
/* Decide whether we can make a sibling call to a function.  DECL_CALLEE is
   the declaration of the function being targeted by the call and EXP_CALLEE
   is the CALL_EXPR representing the call. */

static bool
tric_function_ok_for_sibcall (tree decl_callee, tree exp_callee)
{
  tree fntype_callee, ret_callee, ret_current;
  int callee_irq_p, callee_irqh_p;
  int current_irq_p, current_irqh_p;
  
  fntype_callee = TREE_TYPE (CALL_EXPR_FN (exp_callee));

  if (decl_callee)
    {
      decl_callee = TREE_TYPE (decl_callee);
    }
  else
    {
      decl_callee = fntype_callee;
      
      while (FUNCTION_TYPE != TREE_CODE (decl_callee)
             && METHOD_TYPE != TREE_CODE (decl_callee))
        {
          decl_callee = TREE_TYPE (decl_callee);
        }
    }
 
  if (tric_pxhndcall_function_p (decl_callee))
    {
      return false;
    }

  /* Some users like to call irq-like functions like ordinary functions.
     As they never return, tailcalling them is ok without knowing
     anything about the return type. */
  
  callee_irq_p  = tric_interrupt_function_p (decl_callee);
  callee_irqh_p = tric_interrupt_handler_function_p (decl_callee);
  current_irq_p  = tric_interrupt_function_p (current_function_decl);
  current_irqh_p = tric_interrupt_handler_function_p (current_function_decl);

  /* Return type of the function that is candidate for a tail call */
  ret_callee = TREE_TYPE (decl_callee);
  
  /* Return type of the caller function that might issue the tail call */
  ret_current = TREE_TYPE (DECL_RESULT (current_function_decl));

  /* Due to TriCore ABI that requests to return pointers in A-regs and
     non-pointers in D-regs we must care for (implicit) casts of return
     value which will change from A to D or vice versa. */
  
  if (TREE_CODE (ret_current) != VOID_TYPE
      && (POINTER_TYPE_P (ret_callee) ^ POINTER_TYPE_P (ret_current)))
    return false;

  if (cfun->machine->sibcall_fails
      /* ISRs must return via RFE resp. RSLCX/RFE */
      || current_irq_p != callee_irq_p
      || current_irqh_p != callee_irqh_p)
    return false;

  return true;
}


/* Implement `TARGET_STATIC_CHAIN' */

static rtx
tric_static_chain (const_tree fndecl, bool incoming_p)
{
  unsigned int regno = STATIC_CHAIN_REGNUM;
  
  if (!incoming_p
      && tric_pxhndcall_function_p (fndecl))
    {
      regno = REG_A13;
    }
  
  return gen_rtx_REG (Pmode, regno);
}


/* The middle end does not know that the RET of our target machine will
   restore SP and therefore misses some cases where we can issue a tail call.
   We try to fix these cases in peephole2. */

bool
tric_peep2_may_sibcall_p (rtx addr ATTRIBUTE_UNUSED, rtx xcookie)
{
  int cookie = INTVAL (xcookie);

  if (! flag_optimize_sibling_calls
      || cfun->machine->sibcall_fails
      || (cookie & CALLCOOKIE_INTERRUPT_MASK)
      || (cookie & CALLCOOKIE_INTERRUPT_HANDLER_MASK)
      || (cookie & CALLCOOKIE_PXHNDCALL_MASK)
      || (cookie & CALLCOOKIE_NO_SIBCALL_MASK)
      || tric_interrupt_function_p (current_function_decl)
      || tric_interrupt_handler_function_p (current_function_decl))
    return false;

  return true;
}


/* Adapt flow info for peephole2 patterns that transform CALL+RET to
   tail call insns.  See GCC-135.  We pass an original insn BB_INSN to
   ship the basic block.  insn-emit.c includes basic-block.h after tm_p.h
   and we don't want the build warning induced by that include order.  */

void
tric_peep2_setup_sibcall (rtx bb_insn, rtx call)
{
  edge e;
  edge_iterator ei;
  basic_block bb = BLOCK_FOR_INSN (bb_insn);

  /* Mark the call insn as tail call: set insn flag /j  */

  SIBLING_CALL_P (call) = 1;

  /* Search for the edge that directs to the exit block and mark that
     edge as sibling call.  There must be exectly one edge, and its
     destination must be EXIT_BLOCK_PTR.  */

  FOR_EACH_EDGE (e, ei, bb->succs)
    {
      if (e->dest == EXIT_BLOCK_PTR_FOR_FN (cfun))
        {
          e->flags |= EDGE_SIBCALL | EDGE_ABNORMAL;
          return;
        }
    }

  gcc_unreachable();
}


/***********************************************************************
 ** Emit RTL too complex for target.md
 ***********************************************************************/

/* If X is a pattern that represents a single_set that needs one or
   several scratch registers and maybe clobbers PSW:

       (parallel [(set (...)
                       (...))
                  (clobber (scratch))
                  ...
                  (clobber (reg:REG_PSW))]) ;; optional

   then set *CLOB_PSW to TRUE iff the pattern clobbers PSW.  Return the
   respective SET rtx.  If the pattern is not of the indicated layout then
   return NULL_RTX; *CLOB_PSW is undefined in that case.  */

static rtx
tric_set_clobbers_psw (rtx x, bool *clob_psw)
{
  rtx xi, set = NULL_RTX;
  *clob_psw = false;

  if (PARALLEL != GET_CODE (x)
      || SET != GET_CODE (set = XVECEXP (x, 0, 0)))
    return NULL_RTX;

  for (int i = 1; i < XVECLEN (x, 0); i++)
    {
      if (CLOBBER == GET_CODE (xi = XVECEXP (x, 0, i)))
        {
          rtx clobber_what = XEXP (xi, 0);

          if (REG_P (clobber_what)
              && REG_PSW == REGNO (clobber_what))
            *clob_psw = true;
          else if (SCRATCH != GET_CODE (clobber_what))
            return NULL_RTX;
        }
      else
        return NULL_RTX;
    }

  return set;
}


/* Try to map SET rtx X to an other, equivalent rtx.  If a map is found,
   return the replacement for X; otherwise return NULL.  */

static rtx
tric_find_match_for_combine (rtx x)
{
  /* We make use of `split_insns' to map patterns to each other.  For most
     mappings this is much more convenient than coding in C.  All these
     splits are held in tricore-map.md and have `tric_map_combine' as their
     condition.  */

  tric_map_combine = 1;
  rtx insn = split_insns (x, NULL_RTX);
  tric_map_combine = 0;

  return (// If insn != 0 we found a match
          insn
          // The result is an insn list.  It must not have more than 1 insn.
          && !NEXT_INSN (insn)
          // The target patterns are wrapped in UNSPEC_MAP_COMBINE unspecs so
          // that we can distinguish between splits we are interested in and
          // ordinary matching and non-matching splits.  Actually it makes no
          // sense to have overlapping matches, but genrecog only complains
          // if two splits are matching exactly.
          && UNSPEC == GET_CODE (PATTERN (insn))
          && UNSPEC_MAP_COMBINE == XINT (PATTERN (insn), 1))
    ? XVECEXP (PATTERN (insn), 0, 0)
    : NULL_RTX;
}


/* Try to replace rtx X by a different, equivalent (and simpler) rtx.  */

static rtx
tric_simplify_combined_rtx (rtx f)
{
  // Handle combinations of MIN, MAX, SIGN_EXTEND, ZERO_EXTEND, ABS.
  // IF_THEN_ELSE should have olready been mapped to MIN / MAX by
  // tricore-map.md if applicalble.

  return tric_optimize_minmax (f);
}


/* Implement `TARGET_HTC_CANONICALIZE_COMBINED_RTX' */

static rtx
tric_htc_canonicalize_combined_rtx (rtx x)
{
  bool clobbers_psw = false;
  rtx set = NULL_RTX;
  rtx new_x = NULL_RTX;

  if (!tric_opt_map_combine)
    return x;

  if (SET == GET_CODE (x))
    {
      new_x = tric_find_match_for_combine (set = x);
    }
  else if (NULL_RTX != (set = tric_set_clobbers_psw (x, &clobbers_psw)))
    {
      // The original pattern clobbers some stuff.
      // Try match the plain SET with all clobbers stripped off.
      new_x = tric_find_match_for_combine (set);

      if (// If this did not match ...
          !new_x
          //... and the original pattern clobbers PSW ...
          && clobbers_psw
          // ... then search for an rtx that also clobbers PSW.
          && (new_x = tric_find_match_for_combine (x)))
        {
          // The matcher must also clobber PSW in that case.
          gcc_assert (tric_set_clobbers_psw (new_x, &clobbers_psw)
                      && clobbers_psw);
        }
    }

  rtx old_set, old_src, new_src;

  // The action is (like) a plain SET.  Run some hand-written code against
  // the expression to (further) canonicalize / simplify 'set' or 'new_x'.
  // Notice the side effects on 'old_set', 'old_src' and 'new_src'.

  if (!clobbers_psw
      && ((old_set = new_x)
          || (old_set = set))
      && SET == GET_CODE (old_set)
      && REG_P (SET_DEST (old_set))
      && SImode == GET_MODE (SET_SRC (old_set))
      && (old_src = SET_SRC (old_set))
      && (new_src = tric_simplify_combined_rtx (old_src))
      && new_src != old_src)
    {
      new_x = gen_rtx_SET (VOIDmode, SET_DEST (old_set), new_src);
    }

  if (dump_file && (dump_flags & TDF_DETAILS)
      && new_x)
    htc_dump ("Canonicalized this instruction:\n%r\n\n", x);

  return new_x ? new_x : x;
}


/***********************************************************************
 ** Emit RTL too complex for target.md
 ***********************************************************************/

void
tric_emit_move_CONST_INT (rtx dest, rtx src)
{
  gcc_assert (reload_completed);

  src = simplify_gen_subreg (SImode, src, GET_MODE (dest), 0);
  dest = simplify_gen_subreg (SImode, dest, GET_MODE (dest), 0);
  rtx xop[2] = { dest, src };

  int pos = exact_log2 ((uint32_t) (1 + UINTVAL (src)));

  if (!tric_split_mov_insn_p (xop, SImode))
    {
      emit_move_insn (dest, src);
    }
  else if (pos > 0
           && !A_REG_P (dest))
    {
      // Some not-so-uncommon constants of the form 0b000...111 it is
      // better to MOV -1 and then shift right by the number of zeros.
      emit_move_insn (dest, constm1_rtx);
      emit_insn (gen_lshrsi3 (dest, dest, gen_int_mode (32-pos, SImode)));
    }
  else
    {
      /* `tric_split_const_int' won't work here because that function is not
         aware of DEST's register class.  */

      int val = INTVAL (src);
      int lo = trunc_int_for_mode (val, HImode);

      // MOVH, MOVH.A, LEA, MOV.A
      emit_move_insn (dest, gen_int_mode (val - lo, SImode));
      // ADDI, ADD, ADD.A, LEA
      emit_insn (gen_addsi3 (dest, dest, gen_int_mode (lo, SImode)));
    }
}


/* SRC is an SImode register holding XSVAL, a value known at compile time.
   Try to emit one insn to load XDVAL into DEST which is a SImode register or
   NULL_RTX.  In the latter case, the result is moved to and returned in
   a newly created pseudo register.

   If this function returns non-NULL, then it is a pattern of an insn that
   sets DEST to XDVAL using some arithmetic involving SRC or a plain set from
   SRC to DEST.  Otherwise, no 1-insn sequence has been found.  This function
   only searches for arithmetic operations with SRC as one of its inputs and
   will never come up with a MOV instruction to load XDVAL to DVAL.  */

static rtx
tric_arith_CONST_INT (rtx dest, rtx xdval, rtx src, rtx xsval)
{
  rtx lo, hi, op = NULL_RTX, pat = NULL_RTX;
  uint32_t sval = (uint32_t) UINTVAL (xsval);
  uint32_t dval = (uint32_t) UINTVAL (xdval);

  if (!dest)
    dest = gen_reg_rtx (SImode);

  if ((A_REG_P (src) || A_REG_P (dest))
      && (D_REG_P (src) || D_REG_P (dest)))
    // Don't produce cross-pipeline MOV.A or MOV.D instructions.
    return NULL_RTX;

  if ((A_REG_P (dest) && u4_operand (xdval, SImode))
      || (D_REG_P (dest) && s4_operand (xdval, SImode)))
    // Values that can be loaded by a 2-byte instruction.  Prefer this
    // over the (2-byte) move below.
    return NULL_RTX;

  if (sval == dval)
    // DEST = SRC
    return gen_rtx_SET (VOIDmode, dest, src);

  op = gen_int_mode (dval - sval, SImode);

  if (REGNO (dest) == REGNO (src)
      && s4_operand (src, SImode))
    // A 2-byte addition can do the job.
    // DEST = SRC + OP
    return gen_rtx_SET (VOIDmode, dest, gen_rtx_PLUS (SImode, src, op));

  tric_split_const_int (xdval, &lo, &hi, SET);

  if (A_REG_P (src) || A_REG_P (dest)
      // Don't investigate in DVAL that can be loaded in 1 insn.
      || !hi)
    return NULL_RTX;

  if (add_input_operand (op, SImode))
    // DEST = SRC + OP
    pat = gen_addsi3 (dest, src, op);
  else if (s9_operand (op = gen_int_mode (dval + sval, SImode), SImode))
    // DEST = OP - SRC
    pat = gen_subsi3 (dest, op, src);

  // Number of LSBs that DEST and SRC have in common.
  int comm_lsb = ctz_hwi (sval ^ dval);
  // Masks bits that are different, aligned to the right.
  unsigned diff_b = (sval ^ dval) >> comm_lsb;
  // Starting with this bit, all higher bits are the same again.
  int eqbit = 1 + floor_log2 (diff_b);
  // Smallest mask covering support of diff_b and of the form 2^j - 1.
  unsigned HOST_WIDE_INT ne_mask = (HOST_WIDE_INT_1U << eqbit) - 1;
  // Part of DVAL that is different to SVAL, aligned to the right.
  unsigned ne_bits = ne_mask & (dval >> comm_lsb);

  for (int n = 0; !pat && n <= 31; n++)
    {
      rtx xn = GEN_INT (n);
      uint32_t lsl_n = sval << n;
      uint32_t lsr_n = sval >> n;
      uint32_t rol_n = (sval << n) | (sval >> (32-n));
      // Number of LSBs that DEST and SRC >> N have in common.
      unsigned bits = ctz_hwi (lsr_n ^ dval);
      bits = bits + n > 32 ? 32 - n : bits;
      // Masks the highest of these LSBs.
      unsigned HOST_WIDE_INT msbmask = HOST_WIDE_INT_1U << (bits-1);

      if (dval == lsl_n)
        // DEST = SRC << n
        pat = gen_ashlsi3 (dest, src, xn);
      else if (dval == lsr_n)
        // DEST = SRC >> n
        pat = gen_lshrsi3 (dest, src, xn);
      else if (n && dval == rol_n)
        // DEST = SRC <<< n
        pat = gen_rotlsi3 (dest, src, xn);
      else if (bits >= 32
               // Shifting out common bits ...
               || dval >> bits == 0)
        // ...yiels 0, hence zero-extract:
        // DEST = SRC[N + BITS - 1, ..., N]
        pat = gen_extzv (dest, src, GEN_INT (bits), xn);
      else if (bits > 1
               // High bits all 1?
               && 0 == (uint32_t) ((dval & -msbmask) + msbmask))
        // Sign-extract.  This also covers arithmetic shift right without
        // clobbering PSW:
        // DEST = SRC[N + BITS - 1, ..., N]
        pat = gen_extv (dest, src, GEN_INT (bits), xn);
      else if (n >= 1
               && comm_lsb >= n
               && ctz_hwi ((dval >> n) ^ sval) >= comm_lsb + eqbit - n)
        {
          // DVAL is SVAL with LSBs from SVAL inserted starting at N.
          unsigned HOST_WIDE_INT mask = (1ULL << (comm_lsb + eqbit - n)) - 1;
          rtx xmask = gen_int_mode (mask << n, SImode);
          rtx xnmask = gen_int_mode (~(mask << n), SImode);
          pat = gen_insv_3a_ic (dest, src, xn, xmask, src, xnmask);
        }
    }

  if (!pat
      && ne_bits <= 15)
    {
      // DVAL is SVAL with a zero-extended 4-bit value inserted somewhere.
      rtx xnmask = gen_int_mode (~(ne_mask << comm_lsb), SImode);
      rtx xval = gen_int_mode (dval & (ne_mask << comm_lsb), SImode);
      pat = gen_insert_uconst_ic (dest, src, xnmask, xval);
    }

  /* No need to test for AND, IOR, XOR with constants:  The same that could
     be achieved by these signed 10-bit constants is either covered by PLUS,
     or the absolute value of the result is so small that it can be loaded
     in one instruction.  */

  /* Make sure that we actually return a pattern which is not yet the case if
     the gen_xxx functions used above originates from a define_expand.  */

  if (pat && INSN_P (pat))
    {
      gcc_assert (!NEXT_INSN (pat));
      pat = PATTERN (pat);
    }

  return pat;
}


/* Same as above, but emit the generated pattern as an insn and return
   the destination.  If NULL is returned nothing has been emitted.  */

rtx
tric_emit_arith_CONST_INT (rtx dest, rtx xdval, rtx src, rtx xsval)
{
  rtx pat = tric_arith_CONST_INT (dest, xdval, src, xsval);
  if (pat)
    {
      rtx insn = emit_insn (pat);
      set_unique_reg_note (insn, REG_EQUAL, xdval);
      return SET_DEST (pat);
    }
  return NULL_RTX;
}


/* Emit code for  OP0 = OP1 * OP2 + OP3  all operands are SImode registers
   except OP2 and OP3 which may also be a CONST_INT.  The return value
   represents the result which is OP[0] in most cases, but it might also
   be const0_rtx.  */

rtx
tric_emit_multiply_add (rtx op0, rtx op1, rtx op2, rtx op3)
{
  rtx res = op0;

  if (const0_rtx == op3)
    {
      if (const0_rtx == op2)
        emit_move_insn (op0, res = const0_rtx);
      else if (const1_rtx == op2)
        emit_move_insn (op0, res = op1);
      else if (constm1_rtx == op2)
        emit_insn (gen_negsi2 (op0, op1));
      else if (single_one_operand (op2, SImode))
        {
          int off = exact_log2 (UINTVAL (op2) & GET_MODE_MASK (SImode));
          emit_insn (gen_ashlsi3 (op0, op1, GEN_INT (off)));
        }
      else
        emit_insn (gen_mulsi3 (op0, op1, reg_or_s9_operand (op2, SImode)
                               ? op2 : force_reg (SImode, op2)));
    }
  else if (const0_rtx == op2)
    emit_move_insn (op0, res = op3);
  else if (const1_rtx == op2)
    emit_insn (gen_addsi3 (op0, op1, op3));
  else if (constm1_rtx == op2)
    emit_insn (gen_subsi3 (op0, reg_or_s9_operand (op3, SImode)
                           ? op3 : force_reg (SImode, op3), op1));
  else
    emit_insn (gen_maddsi4 (op0, op1, reg_or_s9_operand (op2, SImode)
                            ? op2 : force_reg (SImode, op2),
                            force_reg (SImode, op3)));
  return res;
}


/* Emit widening SI->DI multiplication OP0 = OP1 * OP2 where OP1 and OP2
   are extended according to CODE.  The return value is OP2 or, if OP2
   has been loaded to a new register, it is that SImode register.  */

rtx
tric_emit_mulsidi (rtx op0, rtx op1, rtx op2, enum rtx_code code)
{
  rtx ret2 = op2;

  if (const1_rtx == op2)
    {
      emit_insn (SIGN_EXTEND == code
                 ? gen_extendsidi2 (op0, op1)
                 : gen_zero_extendsidi2 (op0, op1));
    }
  else
    {
      op1 = gen_rtx_fmt_e (code, DImode, op1);

      if ((SIGN_EXTEND == code && !s9_operand (op2, SImode))
          || (ZERO_EXTEND == code && !u9_operand (op2, SImode)))
        op2 = gen_rtx_fmt_e (code, DImode, ret2 = force_reg (SImode, op2));

      emit_move_insn (op0, gen_rtx_MULT (DImode, op1, op2));
    }

  return ret2;
}


/* Emit insns for widening signed multiply-add:

   XOP[0] = XOP[1] + XOP[2] * XOP[3]

   XOP[0] and XOP[1] are DImode registers.
   XOP[2] is a const_int of any size.
   XOP[3] is a SImode register.  */

void
tric_emit_maddsidi4_const (rtx *xop)
{
  rtx lo[3], hi[3], res = xop[0], lo2, hi2 = NULL_RTX;;

  if (reg_overlap_mentioned_p (res, xop[1])
      || reg_overlap_mentioned_p (res, xop[3]))
    xop[0] = gen_reg_rtx (DImode);

  tric_split_di (xop, lo, hi, 3);
  lo2 = lo[2];

  if (const0_rtx == lo[2])
    {
      emit_move_insn (xop[0], xop[1]);
    }
  else
    {
      if (INTVAL (lo[2]) < 0)
        {
          // lo[2] has been sign-extended.  Adjust the high part so that we
          // have  xop[2] = lo[2] + hi[2] * 2**32  again.
          hi[2] = plus_constant (SImode, hi[2], 1);
        }

      if (s9_m9_operand (lo[2], SImode))
        emit_insn (gen_maddsidi4_const (xop[0], xop[3], lo[2], xop[1]));
      else
        emit_insn (gen_maddsidi4 (xop[0], xop[3],
                                  lo2 = force_reg (SImode, lo[2]), xop[1]));
    }

  if (REG_P (lo2)
      && !s9_operand (hi[2], SImode))
    // lo2 holds some const_int.  Try loading hi[2] the smart way.
    hi2 = tric_emit_arith_CONST_INT (NULL_RTX, hi[2], lo2, lo[2]);

  tric_emit_multiply_add (hi[0], xop[3], hi2 ? hi2 : hi[2], hi[0]);
  emit_move_insn (res, xop[0]);
}


/* Emit insns for widening unsigned multiply-add:

   XOP[0] = XOP[1] + XOP[2] * XOP[3]

   XOP[0] and XOP[1] are DImode registers.
   XOP[2] is a const_int of any size.
   XOP[3] is a SImode register.

   The return value represents the low 32 bits of XOP[2]:  If that low part
   is not returned unaltered as const_int, then it is a reg containing that
   value or (neg reg) with reg holding the negated low part.  */

rtx
tric_emit_umaddsidi4_const (rtx *xop)
{
  rtx lo[3], hi[3], res = xop[0], lo2, hi2 = NULL_RTX;

  if (reg_overlap_mentioned_p (res, xop[1])
      || reg_overlap_mentioned_p (res, xop[3]))
    xop[0] = gen_reg_rtx (DImode);

  tric_split_di (xop, lo, hi, 3);
  lo2 = lo[2];

  if (const0_rtx == lo[2])
    {
      emit_move_insn (xop[0], xop[1]);
    }
  else if (s10n_operand (lo[2], SImode))
    {
      if (INTVAL (lo[2]) < 0)
        {
          // lo[2] has been sign-extended.  Adjust the high part so that we
          // have  xop[2] = lo[2] + hi[2] * 2**32  again.
          hi[2] = plus_constant (SImode, hi[2], 1);
        }
      emit_insn (gen_umaddsidi4_const (xop[0], xop[3], lo[2], xop[1]));
    }
  else if (constm1_rtx == hi[2]
           || (INTVAL (hi[2]) < 0
               && INTVAL (hi[2]) != INTVAL (lo[2])))
    {
      // Negating will turn the high part from -1 to 0 because lo[2] != 0.
      // hi[2] = -~hi[2];
      hi[2] = plus_constant (SImode, hi[2], 1);
      lo[2] = simplify_unary_operation (NEG, SImode, lo[2], SImode);
      emit_insn (gen_umsubsidi4 (xop[0],
                                 lo2 = force_reg (SImode, lo[2]), xop[3],
                                 xop[1]));
      lo2 = gen_rtx_NEG (SImode, lo2);
    }
  else
    {
      emit_insn (gen_umaddsidi4 (xop[0], xop[3],
                                 lo2 = force_reg (SImode, lo[2]), xop[1]));
    }

  if (!CONST_INT_P (lo2)
      && !s9_operand (hi[2], SImode))
    // lo2 holds some const_int.  Try loading hi[2] the smart way.
    hi2 = tric_emit_arith_CONST_INT (NULL_RTX, hi[2],
                                     REG_P (lo2) ? lo2 : XEXP (lo2, 0), lo[2]);

  tric_emit_multiply_add (hi[0], xop[3], hi2 ? hi2 : hi[2], hi[0]);
  emit_move_insn (res, xop[0]);

  return lo2;
}


static void
tric_emit_lshiftrt64_const (rtx *xop)
{
  gcc_assert (CONST_INT_P (xop[2]));

  rtx lo[2], hi[2];
  int off = UINTVAL (xop[2]) % 64;

  tric_split_di (xop, lo, hi, 2);

  if (0 == off)
    {
      emit_move_insn (xop[0], xop[1]);
    }
  else if (off < 32 && off > 0)
    {
      emit_insn (gen_dextr_const (lo[0], lo[1], xop[2], hi[1],
                                  gen_int_mode (32 - off, SImode)));
      emit_insn (gen_lshrsi3 (hi[0], hi[1], xop[2]));
    }
  else if (32 == off)
    {
      emit_move_insn (lo[0], hi[1]);
      emit_move_insn (hi[0], const0_rtx);
    }
  else if (off > 32 && off < 64)
    {
      emit_insn (gen_lshrsi3 (lo[0], hi[1], gen_int_mode (off - 32, SImode)));
      emit_move_insn (hi[0], const0_rtx);
    }
  else
    gcc_unreachable();
}


/* XOP[0] = XOP[1] >> XOP[2].  */

void
tric_emit_lshiftrt64 (rtx *xop)
{
  if (CONST_INT_P (xop[2]))
    return tric_emit_lshiftrt64_const (xop);

  rtx xresult = xop[0];
  xop[0] = gen_reg_rtx (DImode);

  rtx lo[2], hi[2];
  tric_split_di (xop, lo, hi, 2);

  rtx neg2 = gen_reg_rtx (SImode);
  rtx ge_32 = gen_reg_rtx (SImode);

  rtx off_n32 = gen_reg_rtx (SImode);
  rtx hi0 = gen_reg_rtx (SImode);
  rtx lo0 = gen_reg_rtx (SImode);
  rtx xop2 = ge_32;

  // If shift offset $2 >= 32 ...
  tric_emit_setcompare (GE, xop[2], GEN_INT (32), ge_32);

  // ... then   (hi0, lo0) = (0, hi[1]), i.e. shift right by 32.
  // Otherwise  (hi0, lo0) = (hi[1], lo[1])
  emit_insn (gen_movsicc (lo0, gen_rtx_EQ (VOIDmode, ge_32, const0_rtx),
                          lo[1], hi[1]));

  emit_insn (gen_movsicc (hi0, gen_rtx_EQ (VOIDmode, ge_32, const0_rtx),
                          hi[1], const0_rtx));

  // Kill bit #5 of $2; remaining are offsets 0...31.
  emit_insn (gen_andsi3 (xop2, xop[2], GEN_INT (31)));

  // 32 - $2 is the offset for DEXTR
  emit_insn (gen_subsi3 (off_n32, GEN_INT (32), xop2));

  // Low word is DEXTR ... 32 - $2
  emit_insn (gen_dextr_reg (lo[0], lo0, off_n32, hi0));

  // If $2 = 32 and hence offset of DEXTR is 32 then DEXTR doesn' do.
  // Use original low word in that case.
  emit_insn (gen_movsicc (lo[0], gen_rtx_NE (VOIDmode, xop2, const0_rtx),
                          lo[0], lo0));

  // High word: Shift right by $2, i.e. SH with offset -$2.
  emit_insn (gen_negsi2 (neg2, xop2));
  emit_insn (gen_lshrsi3_unspec (hi[0], hi0, neg2));

  // Write back the result
  emit_move_insn (xresult, xop[0]);
}


static void
tric_emit_ashiftrt64_const (rtx *xop)
{
  gcc_assert (CONST_INT_P (xop[2]));

  int off = UINTVAL (xop[2]) % 64;

  rtx lo[2], hi[2];
  tric_split_di (xop, lo, hi, 2);

  if (0 == off)
    {
      emit_move_insn (xop[0], xop[1]);
    }
  else if (off < 32 && off > 0)
    {
      emit_insn (gen_dextr_const (lo[0], lo[1], xop[2], hi[1],
                                  gen_int_mode (32 - off, SImode)));
      emit_insn (gen_ashrsi3 (hi[0], hi[1], xop[2]));
    }
  else if (32 == off && TRIC_HAVE_MOV64)
    {
      emit_insn (gen_ashrdi3_32 (xop[0], xop[1]));
    }
  else if (32 == off)
    {
      emit_move_insn (lo[0], hi[1]);
      emit_insn (gen_ashrsi3 (hi[0], lo[0], GEN_INT (31)));
    }
  else if (off > 32 && off < 64)
    {
      emit_insn (gen_ashrsi3 (lo[0], hi[1], gen_int_mode (off - 32, SImode)));
      emit_insn (gen_ashrsi3 (hi[0], lo[0], GEN_INT (31)));
    }
  else
    gcc_unreachable();
}


/* XOP[0] = XOP[1] >> XOP[2].  */

void
tric_emit_ashiftrt64 (rtx *xop)
{
  if (CONST_INT_P (xop[2]))
    return tric_emit_ashiftrt64_const (xop);

  rtx xresult = xop[0];
  xop[0] = gen_reg_rtx (DImode);

  rtx lo[2], hi[2];
  tric_split_di (xop, lo, hi, 2);

  rtx ge_32 = gen_reg_rtx (SImode);
  rtx lo0 = gen_reg_rtx (SImode);
  rtx off_n32 = lo0;
  rtx xop2 = ge_32;

  // If shift offset $2 >= 32 ...
  tric_emit_setcompare (GE, xop[2], GEN_INT (32), ge_32);

  // ... then   $0 = $1 >> 32
  // ... else   $0 = $1
  emit_insn (gen_movsicc (lo[0], gen_rtx_EQ (VOIDmode, ge_32, const0_rtx),
                          lo[1], hi[1]));

  emit_insn (gen_ashrsi3_unspec (hi[0], hi[1], GEN_INT (-31)));
  emit_insn (gen_movsicc (hi[0], gen_rtx_EQ (VOIDmode, ge_32, const0_rtx),
                          hi[1], hi[0]));

  // Kill bit #5 of $2; remaining offsets are 0...31.
  emit_insn (gen_andsi3 (xop2, xop[2], GEN_INT (31)));

  // 32 - $2 is the offset for DEXTR
  emit_insn (gen_subsi3 (off_n32, GEN_INT (32), xop2));

  // Low word is DEXTR ... 32 - $2
  emit_insn (gen_dextr_reg (lo0, lo[0], off_n32, hi[0]));

  // If $2 = 32 and hence offset of DEXTR is 32, then DEXTR doesn't do.
  // Use low word lo[0] in that case.
  emit_insn (gen_movsicc (lo[0], gen_rtx_NE (VOIDmode, xop2, const0_rtx),
                          lo0, lo[0]));

  // High word: Shift right by $2, i.e. SH with offset -$2.
  emit_insn (gen_negsi2 (xop2, xop2));
  emit_insn (gen_ashrsi3_unspec (hi[0], hi[0], xop2));

  // Write back the result
  emit_move_insn (xresult, xop[0]);
}


static void
tric_emit_ashift64_const (rtx *xop)
{
  gcc_assert (CONST_INT_P (xop[2]));

  rtx lo[2], hi[2];
  int off = UINTVAL (xop[2]) % 64;

  tric_split_di (xop, lo, hi, 2);

  if (0 == off)
    {
      emit_move_insn (xop[0], xop[1]);
    }
  else if (off < 32 && off > 0)
    {
      emit_insn (gen_dextr_const (hi[0], lo[1],
                                  gen_int_mode (32 - off, SImode),
                                  hi[1], xop[2]));
      emit_insn (gen_ashlsi3 (lo[0], lo[1], xop[2]));
    }
  else if (32 == off)
    {
      emit_move_insn (hi[0], lo[1]);
      emit_move_insn (lo[0], const0_rtx);
    }
  else if (off > 32 && off < 64)
    {
      emit_insn (gen_ashlsi3 (hi[0], lo[1], gen_int_mode (off - 32, SImode)));
      emit_move_insn (lo[0], const0_rtx);
    }
  else
    gcc_unreachable();
}


/* XOP[0] = XOP[1] << XOP[2].  */

void
tric_emit_ashift64 (rtx *xop)
{
  if (CONST_INT_P (xop[2]))
    return tric_emit_ashift64_const (xop);

  rtx xresult = xop[0];
  xop[0] = gen_reg_rtx (DImode);

  rtx lo[2], hi[2];
  tric_split_di (xop, lo, hi, 2);

  rtx off_mod32 = lo[0];
  rtx is_ge32 = gen_reg_rtx (SImode);

  tric_emit_setcompare (GE, xop[2], GEN_INT (32), is_ge32);
  emit_insn (gen_andsi3 (off_mod32, xop[2], GEN_INT (31)));

  // $0 = $1 << ($2 % 32)
  // This is the result if 0 <= $2 < 32
  emit_insn (gen_dextr_reg (hi[0], lo[1], off_mod32, hi[1]));
  emit_insn (gen_ashlsi3 (lo[0], lo[1], off_mod32));

  // If $2 >= 32, then perform a conditional shift left by 32.
  // If $2 < 32, the values won't change
  // (hi0, lo0)  :=  $2 < 32 ? (hi0, lo0) : (lo0, 0)
  emit_insn (gen_movsicc (hi[0], gen_rtx_EQ (VOIDmode, is_ge32, const0_rtx),
                          hi[0], lo[0]));
  emit_insn (gen_movsicc (lo[0], gen_rtx_EQ (VOIDmode, is_ge32, const0_rtx),
                          lo[0], const0_rtx));

  // Write back the result
  emit_move_insn (xresult, xop[0]);
}


/* Return true if SYM is an absolute addressable SYMBOL_REF */

static inline bool
tric_symbol_ref_absolute_p (const_rtx sym)
{
  return (SYMBOL_REF == GET_CODE (sym)
          && 0 != (SYMBOL_REF_FLAGS (sym) & TRIC_SYMBOL_FLAG_ABSOLUTE));
}


/* Return true if SYM is a small addressable SYMBOL_REF */

static inline bool
tric_symbol_ref_small16_p (const_rtx sym)
{
  return (SYMBOL_REF == GET_CODE (sym)
          && 0 != (SYMBOL_REF_FLAGS (sym) & TRIC_SYMBOL_FLAG_SMALL));
}


bool
tric_absolute_code_address_p (rtx addr)
{
  return (CONST_INT_P (addr)
          && TRIC_ABSOLUTE_CODE_ADDRESS_P (INTVAL (addr)));
}


/* Return true if ADDR is an absolute address. */

bool
tric_absolute_address_p (rtx addr)
{
  bool abs = false;

  switch (GET_CODE (addr))
    {
    default:
      break;
      
    case SYMBOL_REF:
      abs = tric_symbol_ref_absolute_p (addr);
      break;

    case CONST:
      {
        rtx addr2 = XEXP (addr, 0);

        if (PLUS == GET_CODE (addr2))
          {
            rtx op0 = XEXP (addr2, 0);
            rtx op1 = XEXP (addr2, 1);

            if (CONST_INT_P (op1))
              {
                abs = (tric_symbol_ref_absolute_p (op0)
                       && IN_RANGE (INTVAL (op1), 0, 0x3ff0));
              }
            
          } /* PLUS */
      } /* CONST */
      break;

    case CONST_INT:
      abs = TRIC_ABSOLUTE_INT_ADDRESS_P (INTVAL (addr));
      break;
    }

  return abs;
}

bool
tric_small16_address_p (rtx addr)
{
  bool small = false;

  switch (GET_CODE (addr))
    {
    default:
      break;
      
    case SYMBOL_REF:
      small = tric_symbol_ref_small16_p (addr);
      break;

    case CONST:
      {
        rtx addr2 = XEXP (addr, 0);

        if (PLUS == GET_CODE (addr2))
          {
            rtx op0 = XEXP (addr2, 0);
            rtx op1 = XEXP (addr2, 1);

            if (CONST_INT_P (op1))
              {
                small = (tric_symbol_ref_small16_p (op0)
                         && INTVAL (op1) >= 0);
              }
            
          } /* PLUS */
      } /* CONST */
      break;
    }

  return small;
}


/* Some locations must be loaded indirect. */

static bool
tric_must_load_indirect_p (rtx addr)
{
  bool ind = true;
  
  switch (GET_CODE (addr))
    {
    default:
      break;

    case CONST_INT:
      ind = !tric_absolute_address_p (addr);
      break;

    case REG:
    case PRE_MODIFY:  case POST_MODIFY:
    case PRE_INC:     case POST_INC:
    case PRE_DEC:     case POST_DEC:
      ind = false;
      break;

    case SYMBOL_REF:
    case CONST:
      ind = (!tric_absolute_address_p (addr)
             && !tric_small16_address_p (addr));
      break;

    case LO_SUM:
      {
        rtx base = XEXP (addr, 0);
        rtx off = XEXP (addr, 1);

        if (REG_P (base)
            && tric_symbol_ref_small16_p (off))
          ind = false;

        break;
      }

    case PLUS:
      {
        rtx base = XEXP (addr, 0);
        rtx off = XEXP (addr, 1);

        if (!CONST_INT_P (off))
            break;

        switch (GET_CODE (base))
          {
          case REG:
            ind = false;
            break;

          case SYMBOL_REF:
            ind = (! tric_symbol_ref_small16_p (base)
                   || INTVAL (off) < 0);
            break;

          default:
              break;
          }

        break;
      }
    }

  if (htc_log.must_load_indirect_p)
    htc_edump ("%?: %d = %r\n", ind, addr);

  return ind;
}


/* Can we move an immediate X in one instruction?
   We already handled the CONST_INT case if we come here.

   REG may be NULL_RTX. In that case we treat it like a pseudo. */

bool
tric_can_move_immediate_p (rtx reg, rtx x)
{
  bool can = false;
  bool dreg_p = reg != NULL_RTX  &&  D_REG_P (x);
  
  switch (GET_CODE (x))
    {
    default:
      break;
      
    case CONST:
    case SYMBOL_REF:
      can = (!dreg_p
             && (tric_absolute_address_p (x)
                 || tric_small16_address_p (x)));
      break;

    case HIGH:
      can = true;
      break;

    case CONST_DOUBLE:
    case LABEL_REF:
      return false;
    }

  return can;
}


/* Expand a move operation in mode MODE.  The operands are in OPERANDS.
   Returns true if we are done and no further code must be generated,
   false if the caller should generate an insn to move
   operands[1] to operands[0].  */

bool
tric_emit_move (rtx *operands, enum machine_mode mode)
{
  RTX_CODE code1;
  
  /* One of the ops has to be in a register.  */
  if (!register_operand (operands[0], mode)
      && !register_operand (operands[1], mode))
    {
      gcc_assert (can_create_pseudo_p());
      operands[1] = copy_to_mode_reg (mode, operands[1]);
    }

  /* If this is a memory move that can load/store without
     calculating the address, then emit this move.
     Otherwise, force the address into a register and
     load/store indirect.
     In the case of ST.A that can just handle 10-bit offsets
     for SI/SF-moves we use a secondary reload and emit ST.W instead.
     Constraint "Wa" will handle the remaining legal cases of ST.A */
  if (memory_operand (operands[1], mode))
    {
      rtx addr = XEXP (operands[1], 0);

      /* If we are in global alloc */
      if (!can_create_pseudo_p())
        return false;
      
      if (!tric_must_load_indirect_p (addr))
        return false;

      addr = copy_to_mode_reg (Pmode, addr);

      emit_move_insn (operands[0],
                      replace_equiv_address (operands[1], addr));
      
      return true;
    }
  
  if (memory_operand (operands[0], mode))
    {
      rtx addr = XEXP (operands[0], 0);

      /* If we are in global alloc */
      if (!can_create_pseudo_p())
        return false;
      
      if (!tric_must_load_indirect_p (addr))
        return false;

      addr = copy_to_mode_reg (Pmode, addr);

      emit_move_insn (replace_equiv_address (operands[0], addr),
                      operands[1]);
      
      return true;
    }

  code1 = GET_CODE (operands[1]);

  if (tric_opt_code_pic
      && tric_opt_dynamic_code_pic
      && (LABEL_REF == code1
          || (SYMBOL_REF == code1
              && (SYMBOL_REF_FLAGS (operands[1]) & TRIC_SYMBOL_FLAG_PIC))))
    {
      rtx pc_sym;
      
      tric_pic_offset_init();

      pc_sym = cfun->machine->pic_offset.symbol;

      if (htc_log.pic)
        {
          switch (code1)
            {
            case SYMBOL_REF:
              warning (0, "computing pc-relative offset for symbol %qs",
                       XSTR (operands[1], 0));
              break;
              
            case LABEL_REF:
              warning (0, "computing pc-relative offset for"
                       " %slocal label %<.L%d%>",
                       LABEL_REF_NONLOCAL_P (operands[1]) ? "non-" : "",
                       (int) CODE_LABEL_NUMBER (XEXP (operands[1], 0)));
              break;

            default:
              gcc_unreachable();
            }

          inform (DECL_SOURCE_LOCATION (current_function_decl),
                  "offset is against %qs initialized here", XSTR (pc_sym, 0));
        } /* htc_log.pic */

      emit_insn (gen_load_pic_symbol (operands[0], operands[1], pc_sym));
      return true;
    }

  if (CONST == code1
      || (immediate_operand (operands[1], SImode)
          && !CONST_INT_P (operands[1])
          && !CONST_DOUBLE_P (operands[1])))
    {
      if (!register_operand (operands[0], mode))
        {
          gcc_assert (can_create_pseudo_p());
          operands[0] = copy_to_mode_reg (mode, operands[0]);
        }
      
      rtx reg = (can_create_pseudo_p()) ? gen_reg_rtx (mode) : operands[0];
          
      if (tric_can_move_immediate_p (reg, operands[1]))
        return false;

      if (CONST == GET_CODE (operands[1]))
        {
          rtx plus = XEXP (operands[1], 0);
          rtx sym  = XEXP (plus, 0);
          rtx off  = XEXP (plus, 1);

          if (SYMBOL_REF == GET_CODE (sym)
              && CONST_INT_P (off)
              && INTVAL (off) < 0
              && (tric_symbol_ref_small16_p (sym)
                  || tric_symbol_ref_absolute_p (sym)))
            {
              emit_move_insn (operands[0], sym);
              emit_insn (gen_addsi3 (operands[0], operands[0], off));
              return true;
            }
        }

      emit_insn (gen_movsi_high (reg, operands[1]));
      emit_insn (gen_addsi_low (operands[0], reg, operands[1]));

      return true;
    }

  return false;
}


/* For the condition of movsi insn, e.g. This is to keep CSE from
   doing weird things. */

int
tric_mov_operands (rtx *op, enum machine_mode mode)
{
  if (memory_operand (op[0], mode)
      && memory_operand (op[1], mode))
    return 0;

  if (memory_operand (op[0], mode)
      && immediate_operand (op[1], mode))
    return 0;

  if (REG_P (op[0])
      && D_REGNO_P (REGNO (op[0])))
    {
      return 1;

      /* FIXME: This is too restrict. Pass vreg builds mov insns
         like (set (reg D4) (symbol_ref)) */
      if (SYMBOL_REF == GET_CODE (op[1])
          || CONST == GET_CODE (op[1]))
        return 0;
    }

  return 1;
}


/* Split a CONST_INT X into 1 or two parts that can be used in a SET or
   PLUS insn in a way that X = *PART1 + *PART2.
   part2 may be NULL_RTX in which case X = *PART1.
   If the split produced two parts, *PART1 will contain the low-part.
   Note that we don't know whether this will end up in an
   A-regs insn or in a D-regs insn. */

int
tric_split_const_int (rtx x, rtx *part1, rtx *part2, enum rtx_code code)
{
  HOST_WIDE_INT val;
  HOST_WIDE_INT lo;

  gcc_assert (CONST_INT_P (x));
  
  val = INTVAL (x);

  switch (code)
    {
    default:
      gcc_unreachable();

    case SET:
      /* ??? A <-> D */
      if (satisfies_constraint_Ksg (x)
          || satisfies_constraint_Kug (x)
          || satisfies_constraint_Khg (x))
        {
          *part1 = x;
          *part2 = NULL_RTX;
        }
      else
        {
          lo = trunc_int_for_mode (val, HImode);
          *part1 = gen_int_mode (lo, SImode);
          *part2 = gen_int_mode (val-lo, SImode);
        }
      break;

    case PLUS:
      if (satisfies_constraint_Ksg (x)
          || satisfies_constraint_Khg (x))
        {
          *part1 = x;
          *part2 = NULL_RTX;
        }
      else
        {
          lo = trunc_int_for_mode (val, HImode);
          *part1 = gen_int_mode (lo, SImode);
          *part2 = gen_int_mode (val-lo, SImode);
        }
      
      break;
    }

  return (NULL_RTX == *part2) ? 1 : 2;
}

int
tric_split_mov_insn_p (rtx * operands, enum machine_mode mode)
{
  if (LABEL_REF == GET_CODE (operands[1]))
      return true;

  if (HIGH == GET_CODE (operands[1]))
      return false;
      
  switch (mode)
    {
    case SImode:
      if (const_int_operand (operands[1], SImode))
        {
          if (d_reg_operand (operands[0], SImode))
            return !(satisfies_constraint_Khg (operands[1])
                     || satisfies_constraint_Kug (operands[1])
                     || satisfies_constraint_Ksg (operands[1]));

          if (a_reg_operand (operands[0], SImode))
            return !(satisfies_constraint_Khg (operands[1])
                     || satisfies_constraint_KSa (operands[1])
                     || (TRIC_HAVE_LHA
                         && satisfies_constraint_Khi (operands[1])));

          gcc_unreachable();
        }

      gcc_assert (tric_can_move_immediate_p (operands[0], operands[1]));

      return false;

    default:
      break;
    }

  return 0;
}


/***********************************************************************
 ** Casesi
 ***********************************************************************/

/*
  #0 = Index
  #1 = Lower
  #2 = Upper-Lower
  #3 = Table Label
  #4 = Default Label
*/

void
tric_emit_casesi (rtx r_index, rtx lower, rtx upper_lower, rtx table, rtx deflt)
{
  /* Note that CASE_VECTOR_MODE = SImode */
  rtx m_lower = gen_int_mode (-UINTVAL (lower), SImode);
  rtx r_table = gen_reg_rtx (SImode);
  rtx r_label = force_reg (SImode, gen_rtx_LABEL_REF (Pmode, table));

  REG_POINTER (r_table) = 1;

  /* Ensure the index is in a REG (not a stack slot etc.) */
  r_index = copy_to_mode_reg (SImode, r_index);

  /* Subtract lowest index from index */
  if (m_lower != const0_rtx)
    emit (gen_addsi3 (r_index, r_index, m_lower));

  /* Branch to default label if index-lower is not in [0..upper-lower] */
  tric_emit_branch (GTU, r_index, upper_lower, deflt);
  
  /* Multiply offset by 4 and add it to the table's address.  4-byte jump
     instructions are forced by .code32 in `ASM_OUTPUT_ADDR_VEC_ELT'.  */
  emit_insn (gen_maddsi4 (r_table, r_index, GEN_INT (4), r_label));

  /* Dispatch to table (contains jumps to respective positions) */
  emit (gen_tablejump (r_table, table));
}


void
tric_emit_insert (rtx *xop)
{
  HOST_WIDE_INT value = tric_rtx_to_hwi (xop[2]);
  HOST_WIDE_INT pos   = tric_rtx_to_hwi (xop[3]);
  HOST_WIDE_INT width = tric_rtx_to_hwi (xop[4]);

  if (width > 32)
    width = 32;

  if (width == 0 || pos >= 32)
    {
      /* Empty suppport:  return unaltered.  */

      emit_move_insn (xop[0], xop[1]);
      return;
    }

  if (width > 0)
    {
      if (pos >= 0 && pos + width > 32)
        width = 32 - pos;

      if (width == 32 && pos == 0)
        {
          /* Override result completely.  */

          emit_move_insn (xop[0], xop[2]);
          return;
        }

      if (value >= 0)
        {
          HOST_WIDE_INT support = (HOST_WIDE_INT) 1 << width;
          support = GET_MODE_MASK (SImode) & (support - 1);

          value &= support;
            
          if (support == value && pos >= 0)
            {
              rtx xbits = gen_int_mode (support << pos, SImode);
              if (s10_operand (xbits, SImode))
                {
                  emit_insn (gen_iorsi3 (xop[0], xop[1], xbits));
                  return;
                }
            }

          if (value == 0 && pos >= 0)
            {
              rtx xbits = gen_int_mode (~(support << pos), SImode);
              if (s10_operand (xbits, SImode))
                {
                  emit_insn (gen_andsi3 (xop[0], xop[1], xbits));
                  return;
                }
            }
        }
    }

  xop[2] = value < 0 ? xop[2] : gen_int_mode (value, SImode);
  xop[3] = pos   < 0 ? xop[3] : gen_int_mode (pos,   SImode);
  xop[4] = width < 0 ? xop[4] : gen_int_mode (width, SImode);

  if (!reg_or_u5_operand (xop[4], SImode))
    xop[4] = force_reg (SImode, xop[4]);

  if (!reg_or_u4_operand (xop[2], SImode))
    xop[2] = force_reg (SImode, xop[2]);

  if (u5_operand (xop[4], SImode))
    {
      emit_insn (gen_insert_const_width (xop[0], xop[1],
                                         xop[2], xop[3], xop[4]));
    }
  else
    {
      rtx xinsert = gen_reg_rtx (DImode);
      rtx lo = simplify_gen_subreg (SImode, xinsert, DImode, 0);
      rtx hi = simplify_gen_subreg (SImode, xinsert, DImode, 4);

      emit_move_insn (lo, xop[3]);
      emit_move_insn (hi, xop[4]);
      emit_insn (gen_insert64 (xop[0], xop[1], xop[2], xinsert));
    }
}


/* Expand for imask insn.
   $0 = DImode output.  High = mask, Low = value.
   $1 = value
   $2 = starting bit of the value
   $3 = width  */

void
tric_emit_imask (rtx *xop)
{
  rtx hi, lo;
  HOST_WIDE_INT pos   = tric_rtx_to_hwi (xop[2]);
  HOST_WIDE_INT width = tric_rtx_to_hwi (xop[3]);

  if (width == 0 || pos >= 32)
    {
      /* Empty support: return 0.  */

      emit_move_insn (xop[0], gen_int_mode (0, DImode));
      return;
    }

  /* Cut support to the 32 bits it represents.  */

  if (width > 32)
    width = 32;

  if (pos >= 0 && width >= 0 && pos + width > 32)
    width = 32 - pos;

  if (width > 0 && width < 32)
    {
      /* IMASK instruction need a compile-time-const width.  */

      xop[1] = force_reg (SImode, xop[1]);

      if (!reg_or_u4_operand (xop[2], SImode))
        xop[2] = force_reg (SImode, xop[2]);

      if (!reg_or_u5_operand (xop[3], SImode))
        xop[3] = force_reg (SImode, xop[3]);

      emit_insn (gen_imaskdi (xop[0], xop[1], xop[2], GEN_INT (width)));
      return;
    }

  /* Fallback:  Mimic IMASK's action.  */

  lo = simplify_gen_subreg (SImode, xop[0], DImode, 0);
  hi = simplify_gen_subreg (SImode, xop[0], DImode, 4);

  /* $0.low: Shift value into place.  */

  if (pos == 0)
    emit_move_insn (lo, xop[1]);
  else
    emit_move_insn (lo, gen_rtx_ASHIFT (SImode, xop[1], xop[2]));

  /* $0.high: The support.  */

  if (width == 0)
    {
      emit_move_insn (hi, const0_rtx);
    }
  else if (width == 32 && pos >= 0)
    {
      emit_move_insn (hi, gen_int_mode (-1u << pos, SImode));
    }
  else if (width == 32 && pos == -1)
    {
      emit_move_insn (hi, gen_rtx_ASHIFT (SImode,
                                          force_reg (SImode, constm1_rtx),
                                          xop[2]));
    }
  else if (width == -1)
    {
      emit_insn (gen_insert_mask (hi, xop[2], xop[3]));
    }
  else
    gcc_unreachable();
}


static void
tric_register_passes (void)
{

  /* Sole purpose of this machine specific pass is to determine
     `tric_after_split1_pass_number', a pass number that runs after split1
     and before register allocation.  */

  tric_pass_notice_split1 *tric_after_split1_pass
    = new tric_pass_notice_split1 (g);

  register_pass (tric_after_split1_pass,
                 PASS_POS_INSERT_AFTER, "asmcons", 1);

  tric_after_split1_pass_number
    = tric_after_split1_pass->get_next_pass_number();

  /* Try to optimize multi-instruction loads of compile time constants
     to shorter instruction sequences by reusing known values.  This pass
     runs after reload and before the first post-reload split pass
     `pass_split_after_reload'.  */

  register_pass (new tric_pass_const_anchor (g),
                 PASS_POS_INSERT_BEFORE, "split2", 1);
}



/***********************************************************************
 ** Options and Command Line Switches
 ***********************************************************************/

static void
tric_function_insertion_hook (struct cgraph_node* ARG_UNUSED (n1),
                              void* ARG_UNUSED (data))
{
  if (htc_log.clone)
    htc_edump ("%H [%P]: function_insertion:  %T\n",
               DECL_SOURCE_LOCATION (n1->decl), n1->decl);
}

static void
tric_node_duplication_hook (struct cgraph_node* ARG_UNUSED (n1),
                            struct cgraph_node* ARG_UNUSED (n2),
                            void* ARG_UNUSED (data))
{
  if (htc_log.clone)
    {
      if (n1->decl != n2->decl)
        htc_edump ("%H [%P]: node_duplication:  %T  %T\n",
                   DECL_SOURCE_LOCATION (n1->decl), n1->decl, n2->decl);
    }
}


static void
tric_init_once (void)
{
  static bool done = false;

  if (done)
    return;

  cgraph_add_node_duplication_hook (tric_node_duplication_hook, NULL);
  cgraph_add_function_insertion_hook (tric_function_insertion_hook, NULL);

  done = true;
}

/* Implement `TARGET_OPTION_OVERRIDE' */

static void
tric_option_override (void)
{
#ifdef WITH_LICENSER
  /* The licenser is included so we should do the required licensing check. */
  do_htc_license_check ();
#endif

  if (NULL == tric_opt_segment_trap)
    tric_decode_msegment_trap ("default", "");

  if (TARGET_FAST_MATH)
    {
      flag_unsafe_math_optimizations = 1;
      flag_finite_math_only = 1;
    }

  if (flag_pic)
    error ("%qs: position independent code is not supported for "
           "this machine, try %qs", "-fpic", "-mcode-pic");

  flag_schedule_insns = 0;

  /* ???: web crashed on branch_and_decrement
     gcc.c-torture/compile/20000629-1.c -mtc16 -O3 -funroll-loops
     so we disable it for now.  */

  flag_web = 0;

  /* Disable regrename because it does not check validity of insns after some
     thing changes which might break *mov64 insn.  HARD_REGNO_RENAME_OK does
     not provide enough information to return resonable results.  */
  
  flag_rename_registers = 0;
  
  if (!tric_segment_trap.do_ivopts)
    flag_ivopts = 0;

  tric_set_device (NULL);

  /* Handle -m[no-]errata=ARG:  If the option was not given on the
     command line, use the default as specified in the errata definition.
     We do this after the core is set so that ON is properly set.  */
     
#define DEF_ERRATA(ARG, CC1, AS, DEFIN, ON, IFX_NAME)     \
  if (-1 == tric_errata[tric_errata_ ## ARG].fixit)       \
    tric_errata[tric_errata_ ## ARG].fixit = (ON);
#include "errata.def"
#undef DEF_ERRATA

  /* Adjust upper and lower bounds for data to put into .zdata resp. .sdata. */

  if (tric_zconst_max <= tric_sconst_max)
    tric_sconst_min = tric_zconst_max + 1;
  else
    tric_zconst_min = tric_sconst_max + 1;

  if (tric_zdata_max <= tric_sdata_max)
    tric_sdata_min = tric_zdata_max + 1;
  else
    tric_zdata_min = tric_sdata_max + 1;

  tric_init_once();

  /* Register some tricore-specific pass(es).  There is no canonical place
     for pass registration.  This function is convenient.  */

  tric_register_passes ();

  /* Check whether prerequisite option for -msection-asm-name are set.
     Initialize -mlog.  */

  htc_init_once();
}


/* Implement `TARGET_ASM_RECORD_GCC_SWITCHES' */

static int
tric_asm_record_gcc_switches (print_switch_type type, const char *text)
{
  FILE *file = asm_out_file;
  
  static int printing = 0;
  static const char *start = NULL;
  static const char *end = NULL;
  static bool gcc_version_recorded_p = false;

  if (!gcc_version_recorded_p)
    {
      fputs (".section .version_info.ident,\"\",@progbits\n", file);
      fprintf (file, ".ascii \"%s\"\n\n", version_string);
      gcc_version_recorded_p = true;
    }
  
  switch (type)
    {
    case SWITCH_TYPE_DESCRIPTIVE:

      if (NULL == text)
        {
          gcc_assert (printing <= 1);
          printing++;
          if (2 == printing)
            fputs ("\n", file);
        }
      else
        {
          if (printing)
            {
              fputs ("\n.section .version_info.cc1_switches.", file);
              for (; *text && *text != ':'; text++)
                {
                  if (' ' == *text)
                    fputc ('_', file);
                  else if (ISALNUM (*text))
                    fputc (*text, file);
                }
              fputs (",\"\",@progbits\n", file);
            }
        }

      end = "\n";

      break;
      
    case SWITCH_TYPE_LINE_START:
      start = ".ascii \"";
      break;
      
    case SWITCH_TYPE_LINE_END:
      if (end)
        {
          fputs (end, file);
          end = NULL;
        }
      break;

    default:
      if (start)
        {
          fputs (start, file);
          start = NULL;
        }
      
      for (; *text; text++)
        {
          /* Escaping " and \  */
          if (strchr ("\"\\", *text))
            fputc ('\\', file);
          fputc (*text, file);
        }
      fputc (' ', file);
      
      end = "\"\n";
      break;
    }

  return 0;
}


/***********************************************************************
 ** Register Classes
 ***********************************************************************/

static inline int
tric_reg_ok_for_addr (rtx reg, int strict)
{
  unsigned int regno;

  if (!REG_P (reg))
    return 0;

  regno = REGNO (reg);

  /* Beware: Don't use REGNO_OK_FOR_BASE_P from here because reg_renumber
     is not yet set up properly, cf. HDP-172.  */

  return (A_REGNO_P (regno)
          || (!strict
              && (regno >= FIRST_PSEUDO_REGISTER
                  || regno == ARG_POINTER_REGNUM)));
}


/* Implement `REGNO_REG_CLASS' */

enum reg_class
tric_regno_reg_class (int r)
{
  static const enum reg_class tric_reg_class_tab[FIRST_PSEUDO_REGISTER] =
    {
      /* D0...D15: D15 has its own class */
      REGCLASS_D, REGCLASS_D, REGCLASS_D, REGCLASS_D,
      REGCLASS_D, REGCLASS_D, REGCLASS_D, REGCLASS_D,
      REGCLASS_D, REGCLASS_D, REGCLASS_D, REGCLASS_D,
      REGCLASS_D, REGCLASS_D, REGCLASS_D, REGCLASS_D15,
      
      /* A0...A15: SP and A15 have their own class */
      REGCLASS_A, REGCLASS_A, REGCLASS_A,   REGCLASS_A,
      REGCLASS_A, REGCLASS_A, REGCLASS_A,   REGCLASS_A,
      REGCLASS_A, REGCLASS_A, REGCLASS_A10, REGCLASS_A,
      REGCLASS_A, REGCLASS_A, REGCLASS_A,   REGCLASS_A15,

      /* ARGP, PSW */
      ALL_REGS, ALL_REGS
    };

  gcc_assert (r < FIRST_PSEUDO_REGISTER);

  return tric_reg_class_tab[r];
}


/* Implement `HARD_REGNO_MODE_OK' */
/* Return 1 if hard register REGNO can hold a value of machine-mode MODE.  */

int
tric_hard_regno_mode_ok (int regno, enum machine_mode mode)
{
  if (GET_MODE_SIZE (mode) > UNITS_PER_WORD)
    return (E_REGNO_P (regno) || EA_REGNO_P (regno));

  return regno != REG_SP && regno != REG_A11;
}


/* Implement `HARD_REGNO_RENAME_OK' */
/* Return nonzero if register OLD_REG can be renamed to register NEW_REG.  */

int
tric_hard_regno_rename_ok (unsigned int old_reg ATTRIBUTE_UNUSED,
                           unsigned int new_reg ATTRIBUTE_UNUSED)
{
  return 1;
}


/* Implement `TARGET_CONDITIONAL_REGISTER_USAGE' */
/* Called early in the compilation to conditionally modify
   fixed_regs/call_used_regs.  */

static void
tric_conditional_register_usage (void)
{
  if (tric_opt_code_pic
      && tric_opt_dynamic_code_pic)
    {
      fixed_regs[REG_PIC] = 1;
      call_used_regs[REG_PIC] = 1;
    }
}


/* Implement `LOCAL_REGNO' */
/* Adding call-saved regs to LOCAL_REGNO will result in better lifeness
   information after reload. Note that (return) does not mention that
   it will restore these regs and there is no explicit restore code. */

int
tric_local_regno (unsigned int regno)
{
  return (regno <= 31
          && (LOCAL_REGNO_MASK & (1 << regno)));
}

enum machine_mode
tric_mode_for_align (HOST_WIDE_INT align, rtx xlen)
{
  if (CONST_INT_P (xlen))
    {
      unsigned HOST_WIDE_INT len = UINTVAL (xlen);

      /* When optimizing for speed, make mode only dependent on align.  */
      
      if (optimize_insn_for_speed_p())
        len = 0;
      
      if (align % 4 == 0 && len % 8 == 0)
        return DImode;
      
      if (align % 4 == 0 && len % 4 == 0)
        return SImode;
      
      if (align % 2 == 0 && len % 2 == 0)
        return HImode;
    }
  
  return QImode;
}


/***********************************************************************
 ** Reload and Register Allocation
 ***********************************************************************/

/* Implement `TARGET_CLASS_LIKELY_SPILLED_P' */

static bool
tric_class_likely_spilled_p (reg_class_t c)
{
  return c == REGCLASS_A15 ||  c == REGCLASS_D15;
}


/* Implement `TARGET_SECONDARY_RELOAD' */

static reg_class_t
tric_secondary_reload (bool in_p, rtx x, reg_class_t rclass,
                       enum machine_mode mode,
                       secondary_reload_info *sri ATTRIBUTE_UNUSED)
{
  if (REGCLASS_R == rclass
      || reg_class_subset_p (rclass, REGCLASS_A))
    {
      if (HImode == mode || QImode == mode || HFmode == mode)
        {
          /* QI HI and HF may go into a-regs, but we cannot load/store
             them directly from memory */
          
          if (MEM_P (x))
            {
              return REGCLASS_D;
            }
        }

      /* On TC13x, ST.A just allows 10-bit offsets */

      if (TRIC_13X
          && ! in_p
          && MEM_P (x))
        {
          if (PLUS == GET_CODE (XEXP (x, 0))
              && REG_P (XEXP (XEXP (x, 0), 0))
              && ! satisfies_constraint_Ksa (XEXP (XEXP (x, 0), 1)))
            {
              return REGCLASS_D;
            }
      
          if (LO_SUM == GET_CODE (XEXP (x, 0)))
            {
              return REGCLASS_D;
            }
        }
    }

  if (REGCLASS_R == rclass
      || reg_class_subset_p (rclass, REGCLASS_D))
    {
      if (SYMBOL_REF == GET_CODE (x)
          || CONST == GET_CODE (x))
        {
          return REGCLASS_A;
        }
    }

  return NO_REGS;
}


/***********************************************************************
 ** Addresses' Legitimization and Address Constraints
 ***********************************************************************/

static bool
tric_offset_for_mode_p (rtx offset, enum machine_mode mode)
{
  int bitsize = BLKmode == mode ? 0 : GET_MODE_BITSIZE (mode);

  switch (bitsize)
    {
    case 8:
    case 16:
      return (TRIC_13X
              ? satisfies_constraint_Ksa (offset)
              : satisfies_constraint_Ksg (offset));
      
    case 64:
      return satisfies_constraint_Ksa (offset);

    case 32:
      return satisfies_constraint_Ksg (offset);

    case 0:
      return satisfies_constraint_Ksa (offset);

    default:
      return false;
    }
}


/* Implement `TARGET_LEGITIMATE_ADDRESS_P' */

static bool
tric_legitimate_address_p (enum machine_mode mode, rtx x, bool strict)
{
  int ok = 0;
  int bitsize = BLKmode == mode ? 0 : GET_MODE_BITSIZE (mode);
    
  switch (GET_CODE (x))
    {
    case REG:
      ok = tric_reg_ok_for_addr (x, strict);
      break;

    case PRE_INC:
      ok = (HAVE_PRE_INCREMENT
            && tric_reg_ok_for_addr (XEXP (x, 0), strict));
      break;

    case PRE_DEC:
      ok = (HAVE_PRE_DECREMENT
            && tric_reg_ok_for_addr (XEXP (x, 0), strict));
      break;

    case POST_INC:
    case POST_DEC:
      ok = tric_reg_ok_for_addr (XEXP (x, 0), strict);
      break;

    case PRE_MODIFY:
    case POST_MODIFY:
      {
        rtx reg = XEXP (x, 0);
        rtx mod = XEXP (x, 1);
        rtx op0 = XEXP (mod, 0);
        rtx op1 = XEXP (mod, 1);

        ok = PLUS == GET_CODE (mod)
          && tric_reg_ok_for_addr (reg, strict)
          && tric_reg_ok_for_addr (op0, strict)
          && satisfies_constraint_Ksa (op1);

        ok &= (POST_MODIFY == GET_CODE (x)
               || (HAVE_PRE_INCREMENT && INTVAL (op1) > 0)
               || (HAVE_PRE_DECREMENT && INTVAL (op1) < 0));
        break;
      }

    case CONST_INT:
      ok = TRIC_ABSOLUTE_INT_ADDRESS_P (INTVAL (x));
      break;

    case CONST:
    case SYMBOL_REF:
      ok = (tric_absolute_address_p (x)
            || (tric_small16_address_p (x)
                && (bitsize == 32
                    || bitsize == 0
                    || (!TRIC_13X
                        && (bitsize == 8 || bitsize == 16)))));
      break;

    case LO_SUM:
      {
        rtx op0 = XEXP (x, 0);
        rtx op1 = XEXP (x, 1);

        ok = (tric_reg_ok_for_addr (op0, strict)
              && SYMBOL_REF == GET_CODE (op1)
              && (bitsize == 32
                  || bitsize == 0
                  || (!TRIC_13X
                      && (bitsize == 8 || bitsize == 16))));
        
        break;
      }
      
    case PLUS:
      {
        rtx op0 = XEXP (x, 0);
        rtx op1 = XEXP (x, 1);
        
        if (REG_P (op0)
            && CONST_INT_P (op1))
          {
            ok = tric_reg_ok_for_addr (op0, strict)
             && tric_offset_for_mode_p (op1, mode);
          }

        break;
      }
      
    default:
      break;
    }

  if (htc_log.legitimate_address_p)
    htc_edump ("%?:%m strict=%d: %d = %r\n", mode, strict, ok, x);
    
  return ok;
}


/* Implement `TARGET_LEGITIMIZE_ADDRESS' */

static rtx
tric_legitimize_address (rtx x, rtx oldx, enum machine_mode mode)
{
  rtx reg, reg2, off;

  (void) oldx;
  
  if (TRIC_13X
      && GET_MODE_SIZE (mode) < UNITS_PER_WORD
      && PLUS == GET_CODE (x)
      && REG_P (reg = XEXP (x, 0))
      && CONST_INT_P (off = XEXP (x, 1))
      && ! satisfies_constraint_Ksa (off))
    {
      /* x is of the form

             (plus (reg const_int))

         where the offset doesn't fit in signed 10 bits as needed for
         QI/HI accesses.  We supply an intermediate address at offset
         a multiple of 1024 from reg.  This might help CSE in situations
         when there are more accesses like this around.  However, in
         cases where CSE cannot find similar accesses, code size will
         increase. */
      
      rtx reg2 = gen_reg_rtx (SImode);
      int offset = INTVAL (off);
      int o9 = offset % (1 << 10);

      /* Take care of positive/negative offsets and constrain to [-512,512) */
      o9 += (o9 >= (1<<9)) ? -(1<<10) : (o9 < -(1<<9)) ? (1<<10) : 0;

      /* The intermediate address */
      emit_insn (gen_addsi3 (reg2, reg, gen_int_mode (offset-o9, SImode)));

      /* The address: reg + offset = reg2 + o9 */
      return gen_rtx_PLUS (SImode, reg2, gen_int_mode (o9, SImode));
    }

  if (PLUS == GET_CODE (x)
      && PLUS == GET_CODE (XEXP (x, 0))
      && REG_P (reg  = XEXP (XEXP (x, 0), 0))
      && REG_P (reg2 = XEXP (XEXP (x, 0), 1))
      && CONST_INT_P (off = XEXP (x, 1))
      && INTVAL (off) > 0
      && tric_offset_for_mode_p (off, mode))
    {
      /* x is of the form
           (plus (plus (reg
                        reg)
                  const_int))
      */

      rtx reg_sum = gen_reg_rtx (SImode);

      emit_insn (gen_addsi3 (reg_sum, reg, reg2));

      return gen_rtx_PLUS (SImode, reg_sum, off);
    }
      
  if (htc_log.legitimize_address)
    htc_edump ("%?:%m\tx= %r\n\toldx= %r\n", mode, x, oldx);

  return x;
}


/* Constraint helper function for "Wa".  "Wa" is used to store A-regs to mem,
   it is a subset of "Wm".  TC13x has a limited (smaller than 16 bits)
   offset for ST.A.  */

int
extra_constraint_Wa (rtx x)
{
  rtx addr;
  enum machine_mode mode;

  gcc_assert (MEM_P (x));
  
  addr = XEXP (x, 0);
  mode = GET_MODE (x);

  if (!tric_legitimate_address_p (mode, addr, true))
    return false;

  return TRIC_13X ? tric_legitimate_address_p (HImode, addr, true) : true;
}


/* Constraint "Wl" is used for LDMST and SWAP.W.  */

int
extra_constraint_Wl (rtx x)
{
  rtx addr;
  enum machine_mode mode;

  gcc_assert (MEM_P (x));
  
  addr = XEXP (x, 0);
  mode = GET_MODE (x);

  if (!tric_legitimate_address_p (mode, addr, true))
    return false;

  return tric_legitimate_address_p (DImode, addr, true);
}


/* Constraint "Wc" is used for CMPSWAP.W.  */

int
extra_constraint_Wc (rtx x)
{
  rtx addr;
  enum machine_mode mode;

  gcc_assert (MEM_P (x));
  
  addr = XEXP (x, 0);
  mode = GET_MODE (x);

  if (!tric_legitimate_address_p (mode, addr, true))
    return false;

  return tric_legitimate_address_p (DImode, addr, true)
    && !tric_absolute_address_p (addr);
}


int
extra_constraint_Wm (rtx x)
{
  if (!memory_operand (x, GET_MODE (x)))
    return 0;

  return tric_legitimate_address_p (GET_MODE (x), XEXP (x, 0), true);
}


/* Implement `TARGET_HTC_IVOPT_BASE_COSTS_P' */
/* tree-ssa-loop-ivopts.c has a default algorithm to calculate the
   base costs. This hook allows to increase the base costs of
   addresses.  */

static bool
tric_ivopt_base_costs_p (void)
{
  gcc_assert (tric_segment_trap.do_ivopts);

  return tric_segment_trap.do_ivopts_base_costs;
}


/* Implement `TARGET_HTC_IVOPT_USE_ADDRESS_P' */
/* tree-ssa-loop-ivopts.c collects different kinds of "interesting" uses
   of induction variables, one of which is USE_ADDRESS.  This hook allows
   to skip respective uses so that they are not taken into account for
   optimization and hence the associated MEM_REFs won't be changed.  */

static bool
tric_ivopt_use_address_p (void)
{
  gcc_assert (tric_segment_trap.do_ivopts);

  return tric_segment_trap.do_ivopts_use_address;
}


/* Implement `TARGET_HTC_SCHED_MAY_CHANGE_ADDRESS_P' */
/* This is called from sched-deps.c (find_modifiable_mems).
   For TriCore, the scheduler is allowed to change addresses
   iff -muse-ivopt=all which enables the GCC original full ivopt
   optimization.  */

static bool
tric_sched_may_change_address_p (void)
{
  return tric_segment_trap.do_sched_change_address;
}



/***********************************************************************
 ** Double and Float Constraints
 ***********************************************************************/

int
extra_constraint_Ga9 (rtx x)
{
  unsigned lo, hi;
  tric_split_const_rtx (x, &lo, &hi);

  /* ADDC and ADDX allow s9 constants (9 bit signed) */
  return (satisfies_constraint_Ks9 (gen_int_mode (lo, SImode))
          && satisfies_constraint_Ks9 (gen_int_mode (hi, SImode)));
}


int
extra_constraint_Gsg (rtx x)
{
  unsigned lo, hi;
  tric_split_const_rtx (x, &lo, &hi);

  return satisfies_constraint_Ksg (gen_int_mode (lo, SImode));
}


int
extra_constraint_Ghg (rtx x)
{
  unsigned lo, hi;
  tric_split_const_rtx (x, &lo, &hi);

  return satisfies_constraint_Khg (gen_int_mode (lo, SImode));
}


int
extra_constraint_Gu4 (rtx x)
{
  unsigned lo, hi;
  tric_split_const_rtx (x, &lo, &hi);

  return satisfies_constraint_Ku4 (gen_int_mode (lo, SImode));
}


int
extra_constraint_GSa (rtx x)
{
  unsigned lo, hi;
  tric_split_const_rtx (x, &lo, &hi);

  return satisfies_constraint_KSa (gen_int_mode (lo, SImode));
}


/***********************************************************************
 ** Costs
 ***********************************************************************/

/* Implement `TARGET_REGISTER_MOVE_COST' */

static int
tric_register_move_cost (enum machine_mode mode,
                         reg_class_t class1, reg_class_t class2)
{
  if (reg_class_subset_p (class1, REGCLASS_A)
      && reg_class_subset_p (class2, REGCLASS_A))
    return 2 * HARD_REGNO_NREGS (0, mode);

  if (reg_class_subset_p (class1, REGCLASS_D)
      && reg_class_subset_p (class2, REGCLASS_D))
    {
      if (TRIC_HAVE_MOV64 && 64 == GET_MODE_BITSIZE (mode))
        {
          return HARD_REGNO_NREGS (0, mode);
        }
      
      return 2 * HARD_REGNO_NREGS (0, mode);
    }
  
  return 8 * HARD_REGNO_NREGS (0, mode);
}


/* Implement `TARGET_MEMORY_MOVE_COST' */

static int
tric_memory_move_cost (enum machine_mode mode, reg_class_t class1, bool in)
{
  (void) in;
  
  if (class1 == REGCLASS_D15
      || class1 == REGCLASS_A15)
    return 10 * HARD_REGNO_NREGS (0, mode);
  return 40 * HARD_REGNO_NREGS (0, mode);
}


static int
contains_large_mode_p (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  enum machine_mode mode = GET_MODE (*x);

  return mode != VOIDmode && GET_MODE_SIZE (mode) >= 8;
}


/* Cost of stuff that's a bit more complicated than just a binary operation.
   expmed.c might come up with nonsensical machine modes, hence we'll just
   have a look at the anatomy of the RTXes rather than strictly checking
   modes of all operands.  */

static bool
tric_arith_costs (rtx x, int *total, bool speed)
{
  bool done = false;
  enum machine_mode mode = GET_MODE (x);
  rtx op0 = BINARY_P (x) || UNARY_P (x) ? XEXP (x, 0) : NULL_RTX;
  rtx op00;

  switch (GET_CODE (x))
    {
    default:
      break;

    case MULT:
      if (DImode == mode
          && (SIGN_EXTEND == GET_CODE (op0)
              || ZERO_EXTEND == GET_CODE (op0)))
        {
          // [u]mulsidi3
          *total = speed ? 3 : 4;
          done = true;
        }
      break;

    case TRUNCATE:
      if (DImode == GET_MODE (op0)
          && LSHIFTRT == GET_CODE (op0)
          && MULT == GET_CODE (op00 = XEXP (op0, 0))
          && (ZERO_EXTEND == GET_CODE (XEXP (op00, 0))
              || SIGN_EXTEND == GET_CODE (XEXP (op00, 0))))
        {
          // [u]mulsidi3_highpart
          *total = speed ? 3+1 : 4+1;
          done = true;
        }
      break;
    }

  if (htc_log.rtx_costs
      && done)
    htc_edump ("%?: %r -> %d\n\n", x, *total);

  return done;
}


/* Get rtx cost for the binary operation CODE in mode MODE.
   X is the second operand and either a REG or a CONST_INT.
   The first operand is a REG. */

static int
tric_binary_cost (enum machine_mode mode, enum rtx_code code, rtx x, bool speed)
{
  int cost = 1000;
  int reg_p = (REG_P (x)
               || (SUBREG == GET_CODE (x)
                   && REG_P (SUBREG_REG (x))));

  /* Some values for conveniance including the number of expected,
     additional moves to get a constant. */
  
  const int cost_reg = speed ? 2 : 4;
  const int cost_const = (reg_p
                          ? 0
                          : ((satisfies_constraint_Ksg (x)
                              || satisfies_constraint_Khg (x))
                             ? (speed ? 2 : 4)
                             : (speed ? 4 : 8)));

  /* A DIV instruction costs 2*4..2*11 depending on size of operand.
     For the DIV we take an average of 5.5.  DVSTEP et al. cost 2*5. */
  
  const int cost_nodiv  = speed ? 2*6*6 : tric_opt_fast_div ? 6*4 : 6;
  const int cost_noudiv = speed ? 2*6*5 : tric_opt_fast_div ? 5*4 : 6;
  const int cost_div  = TRIC_HAVE_DIV ? (speed ? 11 : 4) : cost_nodiv;
  const int cost_udiv = TRIC_HAVE_DIV ? (speed ? 11 : 4) : cost_noudiv;

  const bool s9_p = reg_p || satisfies_constraint_Ks9 (x);
  const bool u9_p = reg_p || satisfies_constraint_Ku9 (x);

  if (SImode == mode)
    switch (code)
      {
      default:
        cost = 1000;
        break;
        
      case PLUS:
      case MINUS:
        cost = (reg_p
                || satisfies_constraint_Ksg (x)
                || satisfies_constraint_Khg (x))
          ? cost_reg
          : cost_reg + cost_const;
        
        break;

      case UMAX: case UMIN:
      case SMAX: case SMIN:
        cost = cost_reg;
        break;

      case AND:  case IOR:  case XOR:
      case LT:   case LE:
      case GT:   case GE:
      case EQ:   case NE:
      case COMPARE:
        cost = s9_p ? cost_reg : cost_reg + cost_const;
        break;
        
      case LTU:  case LEU:
      case GTU:  case GEU:
        cost = u9_p ? cost_reg : cost_reg + cost_const;
        break;
        
      case MULT:
        cost = speed ? 2 : cost_reg;
        if (!s9_p) cost += cost_const;
        break;
        
      case DIV:
      case MOD:
        cost = reg_p ? cost_div : cost_div + cost_const;
        break;
        
      case UDIV:
      case UMOD:
        cost = reg_p ? cost_udiv : cost_udiv + cost_const;
        break;

      case ASHIFT:
      case ROTATE:
        cost = cost_reg;
        break;
      
      case ASHIFTRT:
      case LSHIFTRT:
        cost = reg_p ? 2 * cost_reg : cost_reg;
        break;
    }
  
  if (DImode == mode)
    switch (code)
      {
      default:
        cost = 1000;
        break;


      case MULT:
        cost = speed ? 3*2 : 3*4;
        break;

      case PLUS:
      case MINUS:
        cost = s9_p ? 2*cost_reg : 2*cost_reg /*+ cost_const*/;
        break;

      case ASHIFTRT:
      case LSHIFTRT:
        cost = (speed && reg_p) ? 9 * cost_reg : 2 * cost_reg;
        break;

      case ASHIFT:
        cost = (speed && reg_p) ? 6 * cost_reg : 2 * cost_reg;
        break;
      }

  if (SFmode == mode)
    switch (code)
      {
      default:
        cost = 1000;
        break;

      case PLUS:
      case MINUS:
      case MULT:
      case DIV:
        cost = 6;
        break;

      case SMIN:
      case SMAX:
        cost = 12;
        break;
      }

  if (htc_log.rtx_costs)
    htc_edump ("%?:{%C:%m} %r -> %d\n", code, mode, x, cost);
  
  return cost;
}


/* Implement `TARGET_RTX_COSTS' */
/* Compute relative costs for some rtx. If optimizing for size we
   return the expected number of bytes the instruction will occupy.
   When optimizing for speed we try to compute the expected time an
   instruction needs to complete multiplied by 2, i.e. a simple MOV
   is rated as 2. According to P. Jewstafjew, this time is mainly
   given by the instruction latency which is 1 for most operations,
   3 for MUL and MADD (we rate as 5, i.e. 2.5 ticks), 4-11 for DIV
   and about 5-6 for DVSTEP. */

static bool
tric_rtx_costs (rtx x, int icode, int outer_code, int op_num ATTRIBUTE_UNUSED,
                int *total, bool speed)
{
  /* false --> recurse x */

  enum rtx_code code = (enum rtx_code) icode;
  enum machine_mode mode = GET_MODE (x);

  rtx op1;
  bool large_mode_p = for_each_rtx (&x, contains_large_mode_p, NULL);

  /* Section 0:
     Things we definitely don't support */
     
  *total = 000;

  if ((VOIDmode == mode && code != SET)
      || UNKNOWN == outer_code)
    return true;

  if (ASM_INPUT == code
      || ASM_OPERANDS == code)
    {
      return true;
    }
  
  if (TImode == mode
      || QImode == mode
      || HImode == mode)
    {
      /* ??? Guess after reload */
      
      *total = reload_completed ? 4 : 1000;
      return true;
    }
  
  /* Section 1:
     Handle some stuff by hand: most binary operations and
     constants. In the case of operations we estimate the overhead
     which is needed to load large constants. Quering the md does not
     work well here because we are supposed to get the cost even if
     the rtx is not supported in itself like (div reg const_int). */
  
  if (SET == outer_code
      && BINARY_P (x)
      && REG_P (XEXP (x, 0))
      && (REG_P (op1 = XEXP (x, 1))
          || CONST_INT_P (XEXP (x, 1))))
    {
      *total = tric_binary_cost (mode, GET_CODE (x), op1, speed);
      return true;
    }

  if (SET == outer_code
      && tric_arith_costs (x, total, speed))
    return true;

  if ((IOR == code || AND == code)
      && MEM_P (XEXP (x, 0)))
    {
      *total = speed ? 6 : 4;

      if (htc_log.rtx_costs)
        htc_edump ("%?:{%C:%m} %r -> %d\n", outer_code, mode, x, *total);

      return true;
    }

  if ((GET_RTX_CLASS (outer_code) & RTX_BINARY_MASK) == RTX_BINARY_RESULT
      && (REG_P (x)
          || CONST_INT_P (x)))
    {
      *total = tric_binary_cost (VOIDmode == mode
                                 ? SImode
                                 : mode, (enum rtx_code) outer_code, x, speed);
      return true;
    }

  if (SET == outer_code || UNKNOWN == outer_code)
    {
      if (CONST_INT_P (x))
        {
          if (const0_rtx == x)
            *total = 0;
          else if (speed)
            *total = (satisfies_constraint_Ksg (x)
                      ||  satisfies_constraint_Khg (x)
                      ? 2 : 4);
          else
            *total = tric_const_int_costs (INTVAL (x));

          if (htc_log.rtx_costs)
            htc_edump ("%?:{%C:%m} %r -> %d\n", outer_code, mode, x, *total);
          
          return true;
        }

      if (CONST_DOUBLE_P (x))
        {
          *total = tric_const_double_costs (x);

          if (htc_log.rtx_costs)
            htc_edump ("%?:{%C:%m} %r -> %d\n", outer_code, mode, x, *total);
          
          return true;
        }
    }

  /* Section 2:
     Query the machine description for rtx costs.  We find it too tedious
     to write all or most of the patterns down twice: once as fancy rtl and
     a second time as even more braindead and hard-to-maintain XEXP-orgy.
     We build a fake insn and look for insn attribute "ticks" resp. "space". */
  
  if (SET == outer_code
      && mode != BLKmode)
    {
      int num_clobbers;
      rtx pattern, dest;
      rtx insn;

      /* Some passes like if-convert-after-reload call for rtx costs after
         reload_completed.  We have no idea how the set-dest looks like,
         GCC developers once more make a mistery around information which
         is actually present. We return 'unknown', i.e. 0 in that case. */

      if (reload_completed)
        {
          /* ???: Just guess the costs; there is nothing to match against
             without DEST.  */
          
          *total = 4;
          return true;
        }
         
      /* Set up an insn to be recognized */

      if (NULL_RTX == fake_insn)
        {
          /* Doh!  We've got no wrapping insns yet.  Cook them up.  */

          rtx fake_reg = gen_rtx_REG (SImode, 99999);

          start_sequence();

          fake_pattern = gen_rtx_SET (VOIDmode, fake_reg, const0_rtx);
          fake_insn = emit_insn (fake_pattern);

          fake_jump_pattern = gen_rtx_SET (VOIDmode, pc_rtx, fake_reg);
          fake_jump_insn = emit_jump_insn (fake_jump_pattern);

          end_sequence();
        }

      if (VOIDmode == mode)
        {
          /* This is for a conditional jump */
          dest = pc_rtx;
          insn = fake_jump_insn;

          if (CONST_INT_P (x))
            {
              htc_edump ("%?: %r\n", x);
              abort();
            }
        }
      else
        {
          /* This is for ordinary insn */
          dest = gen_rtx_REG (mode, 99999);
          insn = fake_insn;
        }

      /* Open gate for COST_INSNs */
      
      tric_have_cost_insns = 1;

      pattern = gen_rtx_SET (VOIDmode, dest, x);
      PATTERN (insn) = pattern;

      /* Avoid insn caching from recog_memoized.  */

      INSN_CODE (insn) = recog (pattern, insn, &num_clobbers);
      
      *total = INSN_CODE (insn) < 0
        ? 22
        : (speed) ? get_attr_ticks (insn) : get_attr_space (insn);

      /* Close gate for COST_INSNs */
      
      tric_have_cost_insns = 0;

      /* ??? We set the pattern to a valid rtx construct because we observed
         ggc aborting for complex programs due to invalid set_dest in the
         pattern which originated in x. Up to now, the following fix works... */
      
      PATTERN (fake_insn) = fake_pattern;
      PATTERN (fake_jump_insn) = fake_jump_pattern;

      if (htc_log.rtx_costs)
        htc_edump ("%?: \n\tinsn = %r\n\n\tcost{%s} = %d\n\n", pattern,
                   INSN_CODE (insn) >= 0
                   ? get_insn_name (INSN_CODE (insn)) : "unknown",
                   *total);

      return true;
    } /* SET == outer_code && mode != BLKmode */
  else if (SET == outer_code && mode == BLKmode)
    {
      *total = 1;
      return true;
    }

  /* Section 3:
     Some remains still there. This needs cleanup. */
  
  *total = -1;

  switch (outer_code)
    {
    case LO_SUM:
      if (HIGH == code)
        {
          *total = 0;
        }

      break;
      
    case SET:
      htc_edump ("\tticks{%C} = %r\n\n", outer_code, x);
      gcc_unreachable();
          
      break; /* outer = SET */

    default:
      break;
    }

  if (*total == -1)
    {
      *total = large_mode_p ? 1000 : 20;
    }
    
  if (htc_log.rtx_costs)
    htc_edump ("%?:{%C} %r -> %d\n", outer_code, x, *total);

  return true;
}


/* Implement `TARGET_ADDRESS_COST' */

static int
tric_address_cost (rtx addr, enum machine_mode mode ATTRIBUTE_UNUSED,
                   addr_space_t as ATTRIBUTE_UNUSED, bool speed)
{
  int cost = 4;

  if (GET_CODE (addr) == PRE_INC || GET_CODE (addr) == POST_DEC)
    cost = 1000;

  if (htc_log.address_cost)
    htc_edump ("%?: %d[%s] = %r\n", cost, speed ? "speed" : "size", addr);
  
  return cost;
}


/* Get costs of CONST_INT, i.e. num bytes required to load it */

static int
tric_const_int_costs (int i)
{
  rtx x = gen_int_mode (i, SImode);
  
  if (satisfies_constraint_Ks4 (x))
    return 2;

  /* will depend on dest is D15 or not, assume prob of 50%  */
  
  if (satisfies_constraint_Ku8 (x))
    return 3;

  if (satisfies_constraint_Ksg (x)
      || satisfies_constraint_Kug (x)
      || satisfies_constraint_Khg (x))
    {
      return 4;
    }

  return 4 + tric_const_int_costs (trunc_int_for_mode (i, HImode));
}


static int
tric_const_double_costs (rtx x)
{
  int cost_lo, cost_hi;
  unsigned lo, hi;
  tric_split_const_rtx (x, &lo, &hi);
  
  gcc_assert (CONST_DOUBLE_P (x));

  cost_lo = tric_const_int_costs (lo);
  cost_hi = tric_const_int_costs (hi);

  cost_lo = (cost_lo == 3) ? 4 : cost_lo;
  cost_hi = (cost_hi == 3) ? 4 : cost_hi;

  if (SFmode == GET_MODE (x))
    cost_hi = 0;

  return cost_lo + cost_hi;
}


/* Implement `MOVE_RATIO' */

int
tric_move_ratio (int for_speed_p)
{
  return for_speed_p ? 3 : 2;
}


/* Implement `CLEAR_RATIO' */

int
tric_clear_ratio (int for_speed_p)
{
  return for_speed_p ? 4 : 3;
}


/* Implement `SET_RATIO' */

int
tric_set_ratio (int for_speed_p)
{
  return for_speed_p ? 4 : 2;
}


/* Split NUM DImode rtx's passed in OP[] into ther low and high parts,
   returned in LO[] resp. HI[].  */

void
tric_split_di (rtx op[], rtx lo[], rtx hi[], int num)
{
  int i;

  for (i = 0; i < num; i++)
    {
      lo[i] = simplify_gen_subreg (SImode, op[i], DImode, 0);
      hi[i] = simplify_gen_subreg (SImode, op[i], DImode, 4);
    }
}



/***********************************************************************
 ** Function Arguments and Return Value Passing
 ***********************************************************************/

/* Implement `INIT_CUMULATIVE_ARGS' */

void
tric_init_cumulative_args (CUMULATIVE_ARGS *cum, tree fntype,
                           rtx libname, int n_named_args)
{
  int cookie = 0;
  cum->outgoing = n_named_args != -1;
  /* Set up the number of registers to use for passing arguments.  */
  cum->args_mask = 0;
  cfun->machine->sibcall_fails = 0;

  /* Store information about callee's attributes */
  if (fntype)
    {
      if (tric_pxhndcall_function_p (fntype))
        cookie |= CALLCOOKIE_PXHNDCALL_MASK;

      if (cookie)
        cfun->machine->sibcall_fails = 1;

      if (tric_interrupt_function_p (fntype))
        cookie |= CALLCOOKIE_INTERRUPT_MASK;

      if (tric_interrupt_handler_function_p (fntype))
        cookie |= CALLCOOKIE_INTERRUPT_HANDLER_MASK;

      if (tric_longcall_function_p (fntype))
        cookie |= CALLCOOKIE_LONGCALL_MASK;
    }

  cum->call_cookie = cookie;
  cum->fntype = fntype;
  
  cum->libfunc_p = 0;
  cum->argno = 0;
  
  if (NULL_RTX != libname)
    {
      unsigned int i;
      const char *name = XSTR (libname, 0);

      static const tric_libfunc_info_t libfunc_info[] =
        {
          /* Just some functions from libfuncs.h that involve pointer arguments
                 v    = void
                 p    = void*
                 i    = int
                 NULL = don't care, SI is not a pointer  */
          
          /* { name, args, ret-val, fast? } */
          { "abort", NULL, 0, 0 }
          , { "memcpy",  "ppi", 'p', 0 }
          , { "memmove", "ppi", 'p', 0 }
          , { "memcmp",  "ppi", 'i', 0 }
          , { "memset",  "pii", 'p', 0 }
          , { "__cyg_profile_func_enter", "pp", 'v', 0 }
          , { "__cyg_profile_func_exit",  "pp", 'v', 0 }
          , { "setjmp",  "p",  'i', 0 }
          , { "longjmp", "pi", 'v', 0 }
          , { "_Unwind_SjLj_Register",   "p", 'v', 0 }
          , { "_Unwind_SjLj_Unregister", "p", 'v', 0 }
        };

      for (i = 0; i < sizeof (libfunc_info) / sizeof (*libfunc_info); i++)
        {
          if (STREQ (name, libfunc_info[i].name))
            {
              cum->libfunc = libfunc_info[i];
              cum->libfunc_p = NULL != cum->libfunc.args;
              
              return;
            }
        }
      
      if (!cum->libfunc_p && *name != '_')
        {
          internal_error ("Support function %qs not supported", name);
        }
    }
}


/* Implement `TARGET_RETURN_POPS_ARGS' */

static int
tric_return_pops_args (tree fundecl ATTRIBUTE_UNUSED,
                       tree funtype ATTRIBUTE_UNUSED,
                       int size ATTRIBUTE_UNUSED)
{
  return 0;
}


static int
tric_num_arg_regs (enum machine_mode mode, const_tree type)
{
  int bytes, words;

  switch (mode)
    {
    case VOIDmode:
      bytes = 0;
      break;

    case BLKmode:
      bytes = int_size_in_bytes (type);
      break;

    default:
      bytes = GET_MODE_SIZE (mode);
      break;
    }

  /* round to word size */
  words = (bytes + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
  
  return words;
}


/* Worker function for FUNCTION_ARG and FUNCTION_ARG_ADVANCE:
   Return the RTX where a function argument shall be passed.
   If CUM->update then update *CUM.  */

static rtx
tric_function_arg1 (CUMULATIVE_ARGS *cum, enum machine_mode mode,
                    const_tree type, bool named)
{
  int pointer_p = type && POINTER_TYPE_P (type);
  int nregs = tric_num_arg_regs (mode, type);
  int argno = cum->argno;

  cum->this_argno = 1 + cum->argno;

  if (cum->update)
    cum->argno = 1 + argno;

  if (mode == VOIDmode)
    {
      return GEN_INT (cum->call_cookie);
    }

  if (cum->call_cookie & CALLCOOKIE_PXHNDCALL_MASK)
    {
      if (nregs != 1)
        {
          error_at (DECL_SOURCE_LOCATION (current_function_decl),
                    "argument %d of %<%s%> function occupies %d registers but"
                    " only one is supported", argno +1, "__pxhndcall__", nregs);
        }
      
      if (cum->update)
        return NULL_RTX;
      
      return gen_rtx_REG (mode, argno == 0 ? REG_D4 : REG_A15);
    }

  /* Unnamed ... portion of varargs function
     and variable sized types are passed by reference. */
  if (!named
      || 0 == nregs
      || (type && TREE_CODE (TYPE_SIZE (type)) != INTEGER_CST))
    return NULL_RTX;

  /* Don't pass arguments larger than 64 bits in registers */
  if (nregs > 2)
    return NULL_RTX;

  if (cum->libfunc_p)
    {
      const char *args = cum->libfunc.args;

      gcc_assert (args && args[argno]);

      pointer_p = 'p' == args[argno];
    }

  {
    unsigned int regno = (pointer_p || PDImode == mode) ? REG_A4 : REG_D4;
    unsigned int rmask = ((1 << nregs) -1) << regno;

    for (;; regno += nregs, rmask <<= nregs)
    {
      if (rmask & ((1 << REG_A8) | (1 << REG_D8)))
        return NULL_RTX;

      if (0 == (cum->args_mask & rmask))
        {
          if (!cum->update)
            return gen_rtx_REG (mode, regno);
            
          cum->args_mask |= rmask;
          return NULL_RTX;
        }
    }
  }

  gcc_unreachable();
}


/* Implelemt `TARGET_STRICT_ARGUMENT_NAMING' */

static bool
tric_strict_argument_naming (cumulative_args_t cum_v ATTRIBUTE_UNUSED)
{
  return true;
}


/* Implement `TARGET_FUNCTION_ARG' */

static rtx
tric_function_arg (cumulative_args_t cum_v, enum machine_mode mode,
                   const_tree type, bool named)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  cum->update = 0;
  return tric_function_arg1 (cum, mode, type, named);
}


/* Implement `TARGET_FUNCTION_ARG_ADVANCE' */
/* Update the data in CUM to advance over an argument of mode MODE and data
   type TYPE.  TYPE is null for libcalls where that information may not
   be available.  */

static void
tric_function_arg_advance (cumulative_args_t cum_v, enum machine_mode mode,
                           const_tree type, bool named)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  cum->update = 1;
  tric_function_arg1 (cum, mode, type, named);
}


/* Implement `TARGET_FUNCTION_VALUE' */

static rtx
tric_function_value (const_tree ret_type,
                     const_tree fn_decl_or_type ATTRIBUTE_UNUSED,
                     bool outgoing ATTRIBUTE_UNUSED)
{
  enum machine_mode mode = TYPE_MODE (ret_type);
  int regno_d_SI = REG_D2;
  int regno_d_DI = REG_D2;
  int regno_a = REG_A2;
  int bytes = -1;

  rtx reg;

  if (POINTER_TYPE_P (ret_type))
    {
      reg = gen_rtx_REG (Pmode, regno_a);
      REG_POINTER (reg) = 1;
    }
  else if (PDImode == mode)
    {
      reg = gen_rtx_REG (PDImode, regno_a);
    }
  else if (mode == BLKmode)
    {
      bytes = int_size_in_bytes (ret_type);

      gcc_assert (bytes > 0);
        
      if (bytes < 4)
        reg = gen_rtx_REG (SImode, regno_d_SI);
      else if (bytes == 4)
        reg = gen_rtx_REG (mode, regno_d_SI);
      else if (bytes <= 8)
        reg = gen_rtx_REG (mode, regno_d_DI);
      else
        gcc_unreachable ();
    }
  else if (INTEGRAL_TYPE_P (ret_type))
    {
      bytes = GET_MODE_SIZE (mode);

      if (bytes < 4)
        reg = gen_rtx_REG (SImode, regno_d_SI);
      else if (bytes == 4)
        reg = gen_rtx_REG (mode, regno_d_SI);
      else if (bytes <= 8)
        reg = gen_rtx_REG (mode, regno_d_DI);
      else
        gcc_unreachable ();
    }
  else
    {
      /* CQImode, RECORD_TYPE, ... */
      bytes = GET_MODE_SIZE (mode);

      if (bytes < 4)
        reg = gen_rtx_REG (mode, regno_d_SI);
      else if (bytes == 4)
        reg = gen_rtx_REG (mode, regno_d_SI);
      else if (bytes <= 8)
        reg = gen_rtx_REG (mode, regno_d_DI);
      else
        gcc_unreachable ();
    }

  return reg;
}


/* Implement `TARGET_RETURN_IN_MEMORY' */
/* Decide whether a type should be returned in memory (true)
   or in a register (false). */

static bool
tric_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  int size = int_size_in_bytes (type);
  return size > 2 * UNITS_PER_WORD || size <= 0;
}


/* Implement `TARGET_PASS_BY_REFERENCE' */
/* std_gimplify_va_arg_expr passes CUM == NULL */

static bool
tric_pass_by_reference (cumulative_args_t cum_v ATTRIBUTE_UNUSED,
                        enum machine_mode mode ATTRIBUTE_UNUSED,
                        const_tree type,
                        bool named ATTRIBUTE_UNUSED)
{
  int size;

  if (type == NULL_TREE)
    return false;

  /* EABI 2.2.3.6: Pass composite with more than 8 bytes per reference.  */
    
  size = int_size_in_bytes (type);

  return size < 0 || size > 2 * UNITS_PER_WORD;
}


/* Implement `TARGET_CALLEE_COPIES' */

static bool
tric_callee_copies (cumulative_args_t cum_v ATTRIBUTE_UNUSED,
                    enum machine_mode mode ATTRIBUTE_UNUSED,
                    const_tree type,
                    bool named ATTRIBUTE_UNUSED)
{
  tree type_size;

  if (type == NULL_TREE)
    return false;
  
  type_size = TYPE_SIZE_UNIT (TYPE_MAIN_VARIANT (type));

  return (type_size == NULL_TREE
          || (mode == BLKmode
              && int_size_in_bytes (type) > 2 * UNITS_PER_WORD)
          || !really_constant_p (type_size));
}


/* Implement `TARGET_STRUCT_VALUE_RTX' */

static rtx
tric_struct_value_rtx (tree fntype ATTRIBUTE_UNUSED,
                       int incoming ATTRIBUTE_UNUSED)
{
  return NULL_RTX;
}


/* Implement `TARGET_FUNCTION_ATTRIBUTE_INLINABLE_P' */
/* This may be called with a function decl or a function type (see below).
   The tric attribute checkers are prepared for both.  */
   
static bool
tric_function_attribute_inlinable_p (const_tree func)
{
  if (tric_longcall_function_p (func))
    return true;

  return false;
}


/* Implement `TARGET_CAN_INLINE_P' */

static bool
tric_can_inline_p (tree caller, tree callee)
{
  /* Fixme: default_target_can_inline_p only cares for option attributes
     and checks their compatibility so that the default implementation
     of this hook supersedes function_attribute_inlinable_p.
     function_attribute_inlinable_p in turn calls the hook above with a
     function decl, but the attributes are found in the function type.  */

  const_tree a, ftype_callee = TREE_TYPE (callee);

  for (a = TYPE_ATTRIBUTES (ftype_callee); a; a = TREE_CHAIN (a))
    {
      const_tree name = TREE_PURPOSE (a);
      int i;
      
      for (i = 0; targetm.attribute_table[i].name != NULL; i++)
        if (is_attribute_p (targetm.attribute_table[i].name, name)
            && !targetm.function_attribute_inlinable_p (ftype_callee))
          return false;
    }

  return default_target_can_inline_p (caller, callee);
}

/* Implement TARGET_INVALID_PARAMETER_TYPE.  */

static const char *
tric_invalid_parameter_type (const_tree t)
{
  if (SCALAR_FLOAT_TYPE_P (t) && TYPE_PRECISION (t) == 16)
    return N_("function parameters cannot have __float16 type");
  return NULL;
}

/* Implement TARGET_INVALID_RETURN_TYPE.  */

static const char *
tric_invalid_return_type (const_tree t)
{
  if (SCALAR_FLOAT_TYPE_P (t) && TYPE_PRECISION (t) == 16)
    return N_("functions cannot return __float16 type");
  return NULL;
}

/* Implement TARGET_PROMOTED_TYPE.  */

static tree
tric_promoted_type (const_tree t)
{
  if (SCALAR_FLOAT_TYPE_P (t) && TYPE_PRECISION (t) == 16)
    return float_type_node;
  return NULL_TREE;
}

/* Implement TARGET_CONVERT_TO_TYPE.
   Specifically, this hook implements the peculiarity of Tricore
   half-precision floating-point C semantics that requires conversions between
   __float16 to or from double to do an intermediate conversion to float.  */

static tree
tric_convert_to_type (tree type, tree expr)
{
  tree fromtype = TREE_TYPE (expr);
  if (!SCALAR_FLOAT_TYPE_P (fromtype) || !SCALAR_FLOAT_TYPE_P (type))
    return NULL_TREE;
  if ((TYPE_PRECISION (fromtype) == 16 && TYPE_PRECISION (type) > 32)
      || (TYPE_PRECISION (type) == 16 && TYPE_PRECISION (fromtype) > 32))
    return convert (type, convert (float_type_node, expr));
  return NULL_TREE;
}


/***********************************************************************
 ** Predicate Helpers
 ***********************************************************************/


/* Used in insn attributes to map a CONST_INT to its actual value. */

int
intval (rtx x, enum machine_mode mode)
{
  (void) mode;

  gcc_assert (CONST_INT_P (x));

  return INTVAL (x);
}


/* Implement special predicate `mov_input_operand' */

int
mov_input_operand_p (rtx op, enum machine_mode mode)
{
  int ok = 0;

  if (immediate_operand (op, mode)
      && !const_int_operand (op, mode)
      && !const_double_operand (op, mode))
    {
      ok = tric_can_move_immediate_p (NULL_RTX, op);
    }
  else
    ok = general_operand (op, mode);

  if (htc_log.mov_input_operand)
    htc_edump ("%?:[%m] %d = %r\n", mode, ok, op);

  return ok;
}


/* Implement predicate `symbolic_operand' */

int
symbolic_operand_p (rtx op, enum machine_mode mode ATTRIBUTE_UNUSED)
{
  enum rtx_code code = GET_CODE (op);

  switch (code)
    {
    case LABEL_REF:
    case SYMBOL_REF:
    case CONST:
      return 1;
    default:
      break;
    }
    
  return 0;
}


/* Helper for `tric_output_call':
   Get a mask that tells which regs the callee insn will be passed */

static unsigned int
tric_call_insn_use_regs_mask (rtx insn)
{
  unsigned int mask = 0;
  rtx list = CALL_INSN_FUNCTION_USAGE (insn);

  while (list)
    {
      rtx use = XEXP (list, 0);

      if (USE == GET_CODE (use)
          && REG_P (XEXP (use, 0)))
        {
          rtx reg = XEXP (use, 0);
          unsigned int regno = REGNO (reg);

          switch (HARD_REGNO_NREGS (regno, GET_MODE (reg)))
            {
            default:
              gcc_unreachable();
              break;

            case 2:
              mask |= 1 << (1 + regno);
              /* FALLTHRU */
            case 1:
              mask |= 1 << regno;
              break;
            }
        }
      
      list = XEXP (list, 1);
    }

  return mask;
}


/* Print a CALL_INSN. The operands are:
   value_p = 0:
      $0 = address
      $1 =
      $2 = call_cookie
     
   value_p != 0:
      $0 = return register
      $1 = address
      $2 =
      $3 = call_cookie

  With the cookie containing additional information like if we are
  about to print a tail call, etc.

  Also print some additional information like stack and register usage if
  one of -dp, -dP, -fverbose-asm is turned on. */

void
tric_output_call (rtx insn, rtx *operands, int value_p)
{
  unsigned int mask = 0;
  rtx addr = value_p ? operands[1] : operands[0];
  int cookie = INTVAL (value_p ? operands[3] : operands[2]);
  int sibling_p = CALLCOOKIE_SIBLING_MASK & cookie;
  int pxhndcall_p = CALLCOOKIE_PXHNDCALL_MASK & cookie;
  int noreturn_p = NULL_RTX != find_reg_note (insn, REG_NORETURN, NULL);

  /* Compute register mask of passed regs */
  if (sibling_p || pxhndcall_p || flag_verbose_asm || flag_print_asm_name)
    {
      unsigned int regno;
      
      mask = tric_call_insn_use_regs_mask (insn);

      /* Make sure that we did not emit an erroneous sibling, i.e. a sibling
         that gets arguments in some call-safed registers */
      if (sibling_p)
        for (regno = REG_D0; regno <= REG_A15; regno++)
          if (! call_used_regs[regno])
            gcc_assert (0 == (mask & (1 << regno)));
    }

  if (flag_verbose_asm
      || flag_print_asm_name)
    {
      static char str[100];
      rtx op[3];
      op[0] = GEN_INT (mask);
      op[1] = value_p ? operands[2] : operands[1];

      str[0] = '\0';
      
      strcat (str, noreturn_p ? "{" "noreturn"        "}" : "");
      strcat (str, find_reg_note (insn, REG_NON_LOCAL_GOTO, NULL)
              ? "{" "nonlocal goto"          "}" : "");
      strcat (str, find_reg_note (insn, REG_SETJMP, NULL)
              ? "{" "setjmp"            "}" : "");
      strcat (str, (cookie & CALLCOOKIE_LONGCALL_MASK)
              ? "{" "longcall"          "}" : "");
      strcat (str, (cookie & CALLCOOKIE_INTERRUPT_MASK)
              ? "{" "interrupt"         "}" : "");
      strcat (str, (cookie & CALLCOOKIE_INTERRUPT_HANDLER_MASK)
              ? "{" "interrupt_handler" "}" : "");
      strcat (str, (pxhndcall_p)
              ? "{" "pxhndcall"         "}" : "");

      op[2] = gen_rtx_CONST_STRING (VOIDmode, ggc_strdup (str));

      if (sibling_p)
        output_asm_insn (ASM_COMMENT_START " outgoing.sibcall%2[%1]: %M0", op);
      else
        output_asm_insn (ASM_COMMENT_START " outgoing.call%2[%1]: %M0", op);
    }

  if (pxhndcall_p)
    {
      output_asm_insn ("syscall\t%0", &addr);
      return;
    }

  if (cookie & CALLCOOKIE_INTERRUPT_MASK)
    {
      if (CONST_INT_P (addr))
        output_asm_insn (sibling_p ? "ja\t%0" : "jla\t%0", &addr);
      else
        output_asm_insn (sibling_p ? "j%I0\t%0" : "jl%I0\t%0", &addr);
      return;
    }
  
  if (noreturn_p
      || (cookie & CALLCOOKIE_INTERRUPT_HANDLER_MASK))
    {
      sibling_p = 1;
    }

  if (find_reg_note (insn, REG_SETJMP, NULL)
      || (cookie & CALLCOOKIE_USE_CALL_MASK))
    {
      sibling_p = 0;
    }

  if (CONST_INT_P (addr))
    output_asm_insn (sibling_p ? "ja\t%0" : "calla\t%0", &addr);
  else
    output_asm_insn (sibling_p ? "j%I0\t%0" : "call%I0\t%0", &addr);
}


/***********************************************************************
 ** Helper Functions for Bit Stuff
 ***********************************************************************/

/* Similar to exact_log2. However, this may also take something like 00111100
   This input will yield 2 (LSB which is 1) with width = 4 (4 ones in a row).
   WIDTH may be NULL if no variable is available, e.g. in insn conditions.
   If UI is 0 or is not of the form indicated above, return -1 */

int
ones_mask (int ui, int *width)
{
  int ones, zeroes;
  uint32_t i = (uint32_t) ui;

  if (0 == i)
    return -1;

  i >>= (zeroes = ctz_hwi (i));

  ones = exact_log2 (HOST_WIDE_INT_1U + i);

  if (ones < 0)
    return -1;

  if (width)
    *width = ones;

  return zeroes;
}

/* The width of the mask above. To use it in insn conditions
   we need a function. Here it is. */

int
ones_width (int ui)
{
  int width;

  if (ones_mask (ui, & width) < 0)
    return -1;

  return width;
}



/***********************************************************************
 ** Comparing and Branching
 ***********************************************************************/

/* Implement `TARGET_CANONICALIZE_COMPARISON' */

static void
tric_canonicalize_comparison (int *code0, rtx *op0, rtx *op1,
                              bool op0_preserve_value ATTRIBUTE_UNUSED)
{
  enum rtx_code code = (enum rtx_code) *code0;
  enum machine_mode mode = GET_MODE (*op0);

  rtx arg1 = NULL_RTX;

  switch (code)
    {
    default:
      break;
      
    case GT:
      if (mode == GET_MODE (*op0)
          && CONST_INT_P (*op1))
        {
          HOST_WIDE_INT val1 = INTVAL (*op1);

          if (val1 < 0x7fffffff)
            {
              /* r > C  <=>  r >= C+1 */
              code = GE;
              arg1 = gen_int_mode (1 + val1, SImode);
            }
        }
      break;
      
    case GTU:
      if (mode == GET_MODE (*op0)
          && CONST_INT_P (*op1))
        {
          unsigned HOST_WIDE_INT val1 = UINTVAL (*op1);

          if (val1 < 0xffffffff)
            {
              /* r > C  <=>  r >= C+1 */
              code = GEU;
              arg1 = gen_int_mode (1 + val1, SImode);
            }
        }
      break;
            
    case LE:
      if (mode == GET_MODE (*op0)
          && CONST_INT_P (*op1))
        {
          HOST_WIDE_INT val1 = INTVAL (*op1);

          if (val1 < 0x7fffffff)
            {
              /* r <= C  <=>  r < C+1 */
              code = LT;
              arg1 = gen_int_mode (1 + val1, SImode);
            }
        }
      break;
            
    case LEU:
      if (mode == GET_MODE (*op0)
          && CONST_INT_P (*op1))
        {
          unsigned HOST_WIDE_INT val1 = UINTVAL (*op1);

          if (val1 < 0xffffffff)
            {
              /* r <= C  <=>  r < C+1 */
              code = LTU;
              arg1 = gen_int_mode (1 + val1, SImode);
            }
        }
      break;
    }

  if (arg1 != NULL_RTX)
    {
      if (htc_log.canonicalize_comparison)
        htc_edump ("%?:%m  %C -> %C\n\t%r -> %r\n",
                    mode, *code0, (int) code, *op1, arg1);

      *op1 = arg1;
      *code0 = (int) code;
    }
}


/* Old-style interface of the above hook.  */

static enum rtx_code
tric_canonicalize_comparison_code (enum rtx_code code, rtx *op0, rtx *op1)
{
  int code_i = (int) code;
  tric_canonicalize_comparison (&code_i, op0, op1, true);

  return (enum rtx_code) code_i;
  
}


void
tric_emit_branch (enum rtx_code code, rtx op0, rtx op1, rtx targ)
{
  enum machine_mode mode = GET_MODE (op0);

  code = tric_canonicalize_comparison_code (code, &op0, &op1);

  switch (code)
    {
    default:
      break;
            
    case GT:
    case LE:

      if (const0_rtx == op1)
        break;

      /* FALLTHRU */
    case GTU:
    case LEU:
      {
        rtx op;
        code = swap_condition (code);
        op = op0; op0 = op1; op1 = op;
        break;
      }
    }

  switch (code)
    {
    case NE:
    case EQ:
    case LT:
    case GE:
      if (!register_operand (op0, mode))
        op0 = copy_to_mode_reg (mode, op0);
            
      if (reg_or_s4_operand (op1, mode))
        break;
                
      if (tric_opt_branch_use_setcompare
          && s9_operand (op1, mode))
        {
          rtx reg = gen_reg_rtx (mode);
          tric_emit_setcompare (code, op0, op1, reg);
          op0 = reg;
          op1 = const0_rtx;
          code = NE;
          break;
        }
                
      op1 = copy_to_mode_reg (mode, op1);
      break;
            
    case LTU:
    case GEU:
      if (!register_operand (op0, mode))
        op0 = copy_to_mode_reg (mode, op0);
            
      if (reg_or_u4_operand (op1, mode))
        break;
                
      if (tric_opt_branch_use_setcompare
          && u9_operand (op1, mode))
        {
          rtx reg = gen_reg_rtx (mode);
          tric_emit_setcompare (code, op0, op1, reg);
          op0 = reg;
          op1 = const0_rtx;
          code = NE;
          break;
        }
                
      op1 = copy_to_mode_reg (mode, op1);
      break;

    case GT:
    case LE:
      gcc_assert (const0_rtx == op1);
      break;
      
    default:
      gcc_unreachable();
    }

  emit_jump_insn (gen_branch_rtx (targ,
                                  gen_rtx_fmt_ee (code, VOIDmode, op0, op1)));
}


/* if TARG != 0, emit a SImode setcompare instruction, i.e. emit insns
   whose effect is the same as
   
       TARG = (OP0 <<CODE>> OP1)

   where CODE is a comparison code.  We canonicalize the comparison so that
   just instructions/comparisons get generated that are supported by our
   hardware.

   If TARG == 0 we just emit insns that are generated by the canonicalisation
   and return the right hand side (OP0 <<CODE>> OP1) without emitting the
   setcompare instruction.  */

rtx
tric_emit_setcompare (enum rtx_code code, rtx op0, rtx op1, rtx targ)
{
  rtx rhs;
  enum machine_mode mode = GET_MODE (op0);

  code = tric_canonicalize_comparison_code (code, &op0, &op1);

  switch (code)
    {
    default:
      break;
            
    case GT:
    case GTU:
    case LE:
    case LEU:
      {
        rtx op;
        code = swap_condition (code);
        op = op0; op0 = op1; op1 = op;
        break;
      }
    }
    
  switch (code)
    {
    case NE:
    case EQ:
    case LT:
    case GE:
      if (!register_operand (op0, mode))
        op0 = copy_to_mode_reg (mode, op0);
            
      if (s9_operand (op1, mode))
        break;
                
      op1 = copy_to_mode_reg (mode, op1);
      break;
            
    case LTU:
    case GEU:
      if (!register_operand (op0, mode))
        op0 = copy_to_mode_reg (mode, op0);
            
      if (u9_operand (op1, mode))
        break;
            
      op1 = copy_to_mode_reg (mode, op1);
      break;
            
    default:
      gcc_unreachable();
    }

  rhs = gen_rtx_fmt_ee (code, mode, op0, op1);

  if (targ)
    emit_move_insn (targ, rhs);

  return rhs;
}


static void
tric_emit_accumulate_setcompare (rtx targ, enum rtx_code bitop, rtx compare)
{
  rtx rhs = gen_rtx_fmt_ee (bitop, SImode, compare, targ);
  rtx lhs = (IOR == bitop
             ? targ
             : gen_rtx_ZERO_EXTRACT (SImode, targ, const1_rtx, const0_rtx));
  
  emit_insn (gen_rtx_SET (VOIDmode, lhs, rhs));
}


void
tric_emit_setcompare_di (enum rtx_code code, rtx op1, rtx op2, rtx reg)
{
  enum rtx_code code_s = code;
  enum rtx_code code_u = code;
  rtx hi1 = simplify_gen_subreg (SImode, op1, GET_MODE (op1), 4);
  rtx lo1 = simplify_gen_subreg (SImode, op1, GET_MODE (op1), 0);
  rtx hi2 = simplify_gen_subreg (SImode, op2, GET_MODE (op1), 4);
  rtx lo2 = simplify_gen_subreg (SImode, op2, GET_MODE (op1), 0);

  /* Comparing against 0
     Use just the high part for < resp >= */

  if (const0_rtx == op2
      && (LT == code || GE == code))
    {
      tric_emit_setcompare (code, hi1, const0_rtx, reg);
      return;
    }

  /* For non-commutative comparison operators o, i.e. <, <=, >, >=
     in unsigned and signed flavour we have:

         A  o  B <=> (A.h == B.h && A.l  o.u  B.l) || A.h  o.s  B.h

     where o.s = strict   version of o
           o.u = unsigned version of o */

  /* Get unsigned version of comparison */
  
  switch (code)
    {
    default:
      break;

    case LT: code_u = LTU; break;
    case LE: code_u = LEU; break;
    case GT: code_u = GTU; break;
    case GE: code_u = GEU; break;
    }

  /* Get strict version of comparison */

  switch (code)
    {
    default:
      break;

    case LE: code_s = LT; break;
    case GE: code_s = GT; break;
    case LEU: code_s = LTU; break;
    case GEU: code_s = GTU; break;
    }

  /* For some cases the low part comparison turns out to be trivial. */

  if (CONST_INT_P (lo2))
    {
      int32_t loval = (int32_t) INTVAL (lo2);

      /* Low part comparison is always false, so just do the strict compare
         for the high parts */
      
      if ((GTU == code_u && loval == -1)
          || (LTU == code_u && loval == 0))
        {
          tric_emit_setcompare (code_s, hi1, hi2, reg);
          return;
        }

      /* Low part comparison will always be true. This is the case for
         some non-strict comparisons */
      
      if ((GEU == code_u && loval == 0)
          || (LEU == code_u && loval == -1))
        {
          tric_emit_setcompare (code, hi1, hi2, reg);
          return;
        }
    }

  switch (code)
    {
    case EQ:
      tric_emit_setcompare (EQ, hi1, hi2, reg);
      tric_emit_accumulate_setcompare (reg, AND,
                                       tric_emit_setcompare (EQ, lo1, lo2,
                                                             NULL_RTX));
      return;
      
    case NE:
      tric_emit_setcompare (NE, hi1, hi2, reg);
      tric_emit_accumulate_setcompare (reg, IOR,
                                       tric_emit_setcompare (NE, lo1, lo2,
                                                             NULL_RTX));
      return;

    case LT: case LTU: case LE: case LEU:
    case GT: case GTU: case GE: case GEU:
      {
        int val = CONST_INT_P (lo2) ? INTVAL (lo2) : 42;
        
        tric_emit_setcompare (EQ, hi1, hi2, reg);

        /* Avoid reloading of -1 in some cases where the unsigned comparison
           is true/false only for -1. */
        
        if ((val == -1 && GEU == code_u)  ||  (val == -2 && GTU == code_u))
          code_u = EQ, lo2 = constm1_rtx;
        else if ((val == -1 && LTU == code_u)  ||  (val == -2 && LEU == code_u))
          code_u = NE, lo2 = constm1_rtx;
        
        val = CONST_INT_P (hi2) ? INTVAL (hi2) : 42;
        
        if (val == -1 && code_s == LTU)
          code_s = NE, hi2 = constm1_rtx;
        else if (val == -2 && code_s == GTU)
          code_s = EQ, hi2 = constm1_rtx;

        tric_emit_accumulate_setcompare (reg, AND,
                                         tric_emit_setcompare (code_u, lo1, lo2,
                                                               NULL_RTX));
        tric_emit_accumulate_setcompare (reg, IOR,
                                         tric_emit_setcompare (code_s, hi1, hi2,
                                                               NULL_RTX));
        return;
      }

    default:
      gcc_unreachable();
      break;
    }
}

/* Worker function for "cbranchsi4" insn */

void
tric_emit_cbranchsi4 (rtx xop[])
{
  enum rtx_code code = GET_CODE (xop[0]);
  tric_emit_branch (code, xop[1], xop[2], xop[3]);
}

/* Worker function for "cbranchdi4" insn */

void
tric_emit_cbranchdi4 (rtx xop[])
{
  enum rtx_code code = GET_CODE (xop[0]);
  enum rtx_code code_s = code;
  enum rtx_code code_u = code;
  rtx hi1 = simplify_gen_subreg (SImode, xop[1], GET_MODE (xop[1]), 4);
  rtx lo1 = simplify_gen_subreg (SImode, xop[1], GET_MODE (xop[1]), 0);
  rtx hi2 = simplify_gen_subreg (SImode, xop[2], GET_MODE (xop[1]), 4);
  rtx lo2 = simplify_gen_subreg (SImode, xop[2], GET_MODE (xop[1]), 0);
  rtx reg = gen_reg_rtx (SImode);

  /* Comparing against 0 */

  if (const0_rtx == xop[2])
    {
      /* Use just the high part for < resp >= */

      if (LT == code || GE == code)
        {
          emit_jump_insn (gen_branch_rtx (xop[3],
                                          gen_rtx_fmt_ee (code, VOIDmode, hi1,
                                                          const0_rtx)));
          return;
        }

      /* For == and != test for a 1-bit by ORing high and lowpart */

      if (NE == code || EQ == code)
        {
          emit_insn (gen_iorsi3 (reg, hi1, lo1));
          emit_jump_insn (gen_branch_rtx (xop[3],
                                          gen_rtx_fmt_ee (code, VOIDmode, reg,
                                                          const0_rtx)));
          return;
        }
    }

  if (constm1_rtx == xop[2]
      && (EQ == code || NE == code))
    {
      emit_insn (gen_andsi3 (reg, hi1, lo1));
      emit_jump_insn (gen_branch_rtx (xop[3],
                                      gen_rtx_fmt_ee (code, VOIDmode, reg,
                                                      constm1_rtx)));
      return;
    }

  /* For non-commutative comparison operators o, i.e. <, <=, >, >=
     in unsigned and signed flavour we have:

         A  o  B <=> (A.h == B.h && A.l  o.u  B.l) || A.h  o.s  B.h

     where o.s = strict   version of o
           o.u = unsigned version of o */
  
  /* Get unsigned version of comparison */

  switch (code)
    {
    default:
      break;

    case LT: code_u = LTU; break;
    case LE: code_u = LEU; break;
    case GT: code_u = GTU; break;
    case GE: code_u = GEU; break;
    }

  /* Get strict version of comparison */

  switch (code)
    {
    default:
      break;

    case LE: code_s = LT; break;
    case GE: code_s = GT; break;
    case LEU: code_s = LTU; break;
    case GEU: code_s = GTU; break;
    }

  /* For some cases the low part comparison turns out to be trivial. */

  if (CONST_INT_P (lo2))
    {
      int32_t loval = (int32_t) INTVAL (lo2);

      /* Low part comparison is always false, so just do the strict compare
         for the high parts */

      if ((GTU == code_u && loval == -1)
          || (LTU == code_u && loval == 0))
        {
          tric_emit_setcompare (code_s, hi1, hi2, reg);
          emit_jump_insn (gen_branch_rtx (xop[3],
                                          gen_rtx_fmt_ee (NE, VOIDmode, reg,
                                                          const0_rtx)));
          return;
        }

      /* Low part comparison will always be true. This is the case for
         some non-strict comparisons */
      
      if ((GEU == code_u && loval == 0)
          || (LEU == code_u && loval == -1))
        {
          tric_emit_setcompare (code, hi1, hi2, reg);
          emit_jump_insn (gen_branch_rtx (xop[3],
                                          gen_rtx_fmt_ee (NE, VOIDmode, reg,
                                                          const0_rtx)));
          return;
        }
    }

  switch (code)
    {
    case EQ:
      tric_emit_setcompare (EQ, hi1, hi2, reg);
      tric_emit_accumulate_setcompare (reg, AND,
                                       tric_emit_setcompare (EQ, lo1, lo2,
                                                             NULL_RTX));
      break;
      
    case NE:
      tric_emit_setcompare (NE, hi1, hi2, reg);
      tric_emit_accumulate_setcompare (reg, IOR,
                                       tric_emit_setcompare (NE, lo1, lo2,
                                                             NULL_RTX));
      break;

    case LT: case LTU: case LE: case LEU:
    case GT: case GTU: case GE: case GEU:
      {
        int val = CONST_INT_P (lo2) ? INTVAL (lo2) : 42;
        
        tric_emit_setcompare (EQ, hi1, hi2, reg);

        /* Avoid reloading of -1 in some cases where the unsigned comparison
           is true/false only for -1. */
        
        if ((val == -1 && GEU == code_u)  ||  (val == -2 && GTU == code_u))
          code_u = EQ, lo2 = constm1_rtx;
        else if ((val == -1 && LTU == code_u)  ||  (val == -2 && LEU == code_u))
          code_u = NE, lo2 = constm1_rtx;
        
        val = CONST_INT_P (hi2) ? INTVAL (hi2) : 42;
        
        if (val == -1 && code_s == LTU)
          code_s = NE, hi2 = constm1_rtx;
        else if (val == -2 && code_s == GTU)
          code_s = EQ, hi2 = constm1_rtx;

        tric_emit_accumulate_setcompare (reg, AND,
                                         tric_emit_setcompare (code_u, lo1, lo2,
                                                               NULL_RTX));
        tric_emit_accumulate_setcompare (reg, IOR,
                                         tric_emit_setcompare (code_s, hi1, hi2,
                                                               NULL_RTX));
        break;
      }

    default:
      gcc_unreachable();
      break;
    }
  
  emit_jump_insn (gen_branch_rtx (xop[3],
                                  gen_rtx_fmt_ee (NE, VOIDmode, reg,
                                                  const0_rtx)));
}

/* Worker function for "cbranchsf4" insn
   $0 = comparison operator
   $3 = label to jump if condition $1 <$0> $2 is true.  */

void
tric_emit_cbranchsf4 (rtx xop[])
{
  int bit1 = -1;
  int bit2 = -1;
  int bit3 = -1;
  enum rtx_code code = GET_CODE (xop[0]);
  rtx reg = gen_reg_rtx (SImode);

  /* Do a CMP.F instruction on the stuff to be compared (two regs) */
  emit_insn (gen_cmp_f (reg, xop[1], xop[2]));

  /* Depending on the comparison to be performed, CMP.F will have set
     some bits. Indentify which bits to use for the jump. We jump if
     any of these bits is non-zero, i.e. we will OR the bits in the end. */

  switch (code)
    {
    default:
      gcc_unreachable();
      
    case EQ:
      bit1 = CMP_F_EQ;
      break;
      
    case NE:
      bit1 = CMP_F_UNORDERED; bit2 = CMP_F_LT; bit3 = CMP_F_GT;
      break;
      
    case LT:
      bit1 = CMP_F_LT;
      break;
      
    case GT:
      bit1 = CMP_F_GT;
      break;
      
    case LE:
      bit1 = CMP_F_LT; bit2 = CMP_F_EQ;
      break;
      
    case GE:
      bit1 = CMP_F_GT; bit2 = CMP_F_EQ;
      break;
      
    case UNORDERED:
      bit1 = CMP_F_UNORDERED;
      break;
      
    case UNEQ:
      bit1 = CMP_F_UNORDERED; bit2 = CMP_F_EQ;
      break;
      
    case UNLT:
      bit1 = CMP_F_UNORDERED; bit2 = CMP_F_LT;
      break;
      
    case UNGT:
      bit1 = CMP_F_UNORDERED; bit2 = CMP_F_GT;
      break;
      
    case UNGE:
      bit1 = CMP_F_UNORDERED; bit2 = CMP_F_GT; bit3 = CMP_F_EQ;
      break;
      
    case UNLE:
      bit1 = CMP_F_UNORDERED; bit2 = CMP_F_LT; bit3 = CMP_F_EQ;
      break;
      
    case LTGT:
      bit1 = CMP_F_LT; bit2 = CMP_F_GT;
      break;
    }

  if (bit3 != -1)
    {
      /* 3 bits: Mask the relevant bits out using AND. The highest bit set
         by CMP.F is bit 5, so ANDing will always work without loading
         the mask. */
      
      emit_insn (gen_andsi3 (reg, reg, GEN_INT ((1 << bit1) | (1 << bit2)
                                                | (1 << bit3))));
    }
  else
    {
      /* 1 or 2 bits: mask the relevant bits using OR.T.
         If just one bit is used, combine will collapse this to JNZ.T */
      
      if (bit2 == -1)
        bit2 = bit1;
      emit_insn (gen_iorsi3_zerox1 (reg,
                                    reg, GEN_INT (bit1), reg, GEN_INT (bit2)));
    }

  /* yeah! let's jump! */
  
  emit_jump_insn (gen_branch_rtx (xop[3],
                                  gen_rtx_fmt_ee (NE, VOIDmode, reg,
                                                  const0_rtx)));
}


/* Implement `TARGET_CAN_USE_DOLOOP_P' */

static bool
tric_can_use_doloop_p (double_int iterations ATTRIBUTE_UNUSED,
                       double_int iterations_max,
                       unsigned int loop_depth ATTRIBUTE_UNUSED,
                       bool entered_at_top ATTRIBUTE_UNUSED)
{
  if (!tric_opt_loop)
    return false;

  if (// If no upper bound for number of iterations is unknown or...
      iterations_max.is_zero()
      // ...that bound is greater than 0xfffffff, then fail.
      || iterations_max.ugt (double_int::from_uhwi (0xffffffff)))
    return false;

  return true;
}


/***********************************************************************
 ** Printing Operands in final ASM file
 ***********************************************************************/

static void
tric_output_reglist1 (FILE * stream, unsigned mask, int off)
{
  int n0, n1;

  mask &= 0xffffffff;
  
  while (mask)
    {
      n0 = ctz_hwi (mask);
      mask >>= n0;
      off += n0;

      n1 = ctz_hwi (~mask);
      mask >>= n1;
      fputs (REGISTER_PREFIX, stream);
      fputs (reg_names[off], stream);

      if (n1 >= 2)
        fprintf (stream, "-%s%s", REGISTER_PREFIX, reg_names[off+n1-1]);

      off += n1;

      if (mask)
        fputc (',', stream);
    }
}

/* Print the registers in MASK to STREAM.
   This is used to print verbose stuff in prologue. */

void
tric_output_reglist (FILE * stream, unsigned int mask)
{
  unsigned mask_a = (mask & AREGS_MASK) >> REG_A0;
  unsigned mask_d = (mask & DREGS_MASK) >> REG_D0;
    
  tric_output_reglist1 (stream, mask_a, REG_A0);

  if (mask_a && mask_d)
    fputc (',', stream);

  tric_output_reglist1 (stream, mask_d, REG_D0);
}


/***********************************************************************
 ** Initializing: Machine, per Function Data, ...
 ***********************************************************************/

/* Defining data structures for per-function information */
/* The usual; we set up our machine_function data. */

static struct machine_function*
tric_init_machine_status (void)
{
  return ggc_alloc_cleared_machine_function ();
}


/* Copy-paste static tree-ssa-structalias.c:count_num_arguments().  */

static unsigned int
tric_count_num_arguments (tree decl, bool *is_varargs)
{
  unsigned int num = 0;
  tree t;

  /* Capture named arguments for K&R functions.  They do not
     have a prototype and thus no TYPE_ARG_TYPES.  */
  for (t = DECL_ARGUMENTS (decl); t; t = DECL_CHAIN (t))
    ++num;

  /* Check if the function has variadic arguments.  */
  for (t = TYPE_ARG_TYPES (TREE_TYPE (decl)); t; t = TREE_CHAIN (t))
    if (TREE_VALUE (t) == void_type_node)
      break;
  if (!t)
    *is_varargs = true;

  return num;
}


/* Implement `TARGET_SET_CURRENT_FUNCTION' */

static void
tric_set_current_function (tree decl)
{
  if (decl != NULL_TREE
      && cfun->machine
      && !cfun->machine->bogus_pxhndcall
      && tric_pxhndcall_function_p (decl))
    {
      bool is_varargs = false;
      location_t loc = DECL_SOURCE_LOCATION (decl);
      unsigned int nargs = tric_count_num_arguments (decl, &is_varargs);

      if (!DECL_CONTEXT (decl)
          || TREE_CODE (DECL_CONTEXT (decl)) != FUNCTION_DECL)
        {
          error_at (loc, "attribute %<__pxhndcall__%> only applies to"
                    " local functions");
          cfun->machine->bogus_pxhndcall = 1;
        }

      if (is_varargs)
        {
          error_at (loc, "attribute %<__pxhndcall__%> used with varargs"
                    " function");
          cfun->machine->bogus_pxhndcall = 1;
        }
      else if (nargs != 2)
        {
          error_at (loc, "attribute %<__pxhndcall__%> forces function to get"
                    " 2 arguments but %d given", nargs);
          cfun->machine->bogus_pxhndcall = 1;
        }
    }
}


/* Implement `INIT_EXPANDERS' */
/* We just set up to call the above function.  */

void
tric_init_expanders (void)
{
  init_machine_status = tric_init_machine_status;
}

/* Implement TARGET_INIT_LIBFUNCS */

static void
tric_init_libfuncs (void)
{
  /* Arithmetic.  */
  set_optab_libfunc (add_optab, HFmode, NULL);
  set_optab_libfunc (sdiv_optab, HFmode, NULL);
  set_optab_libfunc (smul_optab, HFmode, NULL);
  set_optab_libfunc (neg_optab, HFmode, NULL);
  set_optab_libfunc (sub_optab, HFmode, NULL);

  /* Comparisons.  */
  set_optab_libfunc (eq_optab, HFmode, NULL);
  set_optab_libfunc (ne_optab, HFmode, NULL);
  set_optab_libfunc (lt_optab, HFmode, NULL);
  set_optab_libfunc (le_optab, HFmode, NULL);
  set_optab_libfunc (ge_optab, HFmode, NULL);
  set_optab_libfunc (gt_optab, HFmode, NULL);
  set_optab_libfunc (unord_optab, HFmode, NULL);
}

/* Implement TARGET_SCALAR_MODE_SUPPORTED_P.
   This simply adds HFmode as a supported mode; even though we don't
   implement arithmetic on this type directly, it's supported by
   optabs conversions, much the way the double-word arithmetic is
   special-cased in the default hook.  */

static bool
tric_scalar_mode_supported_p (enum machine_mode mode)
{
  if (mode == HFmode)
    return true;
  else
    return default_scalar_mode_supported_p (mode);
}


/***********************************************************************
 ** Section Handling
 ***********************************************************************/

static bool
tric_decl_volatile_const_rodata_p (const_tree decl)
{
  return (tric_opt_volatile_const_in_rodata
          && TREE_CODE (decl) == VAR_DECL
          && TREE_SIDE_EFFECTS (decl)
          && TREE_READONLY (decl));
}


/* Return 1 if variable VAR_DECL is located in a small addressable
   data section, 0 otherwise.  */

static int
tric_decl_in_smalldata (tree var_decl)
{
  int size;
  int size_min = tric_sdata_min;
  int size_max = tric_sdata_max;

  if (TREE_CODE (var_decl) == VAR_DECL)
    {
      int dam;
      tree var_sec;

      dam = htc_decl_addressing (var_decl);

      if (DAM_indirect == dam)
        return 0;
      else if (DAM_small == dam)
        return 1;
      else if (DAM_absolute == dam)
        return 0;

      /* is there an explicit section directive? */
      var_sec = DECL_SECTION_NAME (var_decl);
      if (var_sec != NULL_TREE)
        {
          const char *sec_name = TREE_STRING_POINTER (var_sec);
          const htc_section_t *tsec = htc_lookup_section (sec_name);
          /* For legacy code.  */
          if (STREQ (sec_name, ".sdata.rodata"))
            return 1;
          
          return tsec && (tsec->flags & SECTION_SMALL);
        }
      
      if (TREE_READONLY (var_decl)
          && (!TREE_SIDE_EFFECTS (var_decl)
              || tric_decl_volatile_const_rodata_p (var_decl))
          && (DECL_INITIAL (var_decl) == 0
              || DECL_INITIAL (var_decl) == error_mark_node
              || TREE_CONSTANT (DECL_INITIAL (var_decl))))
        {
          size_max = tric_sconst_max;
          size_min = tric_sconst_min;
        }
    }
  else if (TREE_CODE (var_decl) == STRING_CST
           || TREE_CODE (var_decl) == CONSTRUCTOR)
    {
      size_max = tric_sconst_max;
      size_min = tric_sconst_min;
    }

  /* No section attribute */

  if (size_max >= INT_MAX-1 && size_min <= 1)
    /* all data without explicit section directive go into sbss/sdata */
    return 1;

  /* Remaining data goes to sbss/sdata if size fits */
  size = int_size_in_bytes (TREE_TYPE (var_decl));

  if (size > 0
      && size <= size_max
      && size >= size_min)
    return 1;

  return 0;
}


/* Return 1 if variable VAR_DECL is located in an absolute addressable
   data section, 0 otherwise.  */

static int
tric_decl_in_absolute (tree var_decl)
{
  int size;
  int size_min = tric_zdata_min;
  int size_max = tric_zdata_max;

  if (TREE_CODE (var_decl) == VAR_DECL)
    {
      tree var_sec;
      
      int dam = htc_decl_addressing (var_decl);

      if (DAM_indirect == dam)
        return 0;
      else if (DAM_absolute == dam)
        return 1;
      else if (DAM_small == dam)
        return 0;
      
      /* Is there an explicit section directive? */
      var_sec = DECL_SECTION_NAME (var_decl);
      if (var_sec != NULL_TREE)
        {
          const char *sec_name = TREE_STRING_POINTER (var_sec);
          const htc_section_t *tsec = htc_lookup_section (sec_name);

          return tsec && (tsec->flags & TRIC_SECTION_ABSOLUTE);
        }
      
      if (TREE_READONLY (var_decl)
          && (!TREE_SIDE_EFFECTS (var_decl)
              || tric_decl_volatile_const_rodata_p (var_decl))
          && (DECL_INITIAL (var_decl) == 0
              || DECL_INITIAL (var_decl) == error_mark_node
              || TREE_CONSTANT (DECL_INITIAL (var_decl))))
        {
          size_min = tric_zconst_min;
          size_max = tric_zconst_max;
        }
    }
  else if (TREE_CODE (var_decl) == STRING_CST
           || TREE_CODE (var_decl) == CONSTRUCTOR)
    {
      size_min = tric_zconst_min;
      size_max = tric_zconst_max;
    }

  /* var_sec == NULL_TREE, i.e. no section attribute */

  if (size_max >= INT_MAX-1 && size_min <= 1)
    /* all data without explicit section directive go into zbss/zdata */
    return 1;

  /* Remaining data goes to zbss/zdata if size fits */
  size = int_size_in_bytes (TREE_TYPE (var_decl));

  if (size > 0
      && size <= size_max
      && size >= size_min)
    return 1;

  return 0;
}


/* Implement `TARGET_ASM_FUNCTION_RODATA_SECTION' */

static section*
tric_function_rodata_section (tree decl ATTRIBUTE_UNUSED)
{
  return readonly_data_section;
}


/* Helper for targetm.asm_out.init_sections:  Materialize one section from
   a respective DEF_SECTION in tricore-sections.def.  */

static void
tric_def_one_section (const char *name, const char *s_flags, int align,
                      bool init_lookup, section **psec)
{
  location_t loc = BUILTINS_LOCATION;

  htc_secspec_t *sspec = htc_build_secspec (SECSPEC_htc_built_in, name,
                                            s_flags, align, NULL_TREE, loc);

  unsigned bss = NULL != strchr (s_flags, 'B') ? SECTION_BSS : 0;
  unsigned flags = (bss
                    | SECTION_UNNAMED
                    | targetm.htc.section_flags_from_string (s_flags));
  htc_section_t *hsec = htc_insert_section (name, flags, align,
                                            sspec, NULL_TREE, loc);

  /* We may pass GTY'ed htc_section_t objects to sec->unnamed.callback
     as `TARGET_HTC_POST_PCH_LOAD' performs the needed fix-ups after
     PCH deserialization.  Notice that sec->unnamed.data is GTY skip'ed.
     Cf. also [GCC-145].  */

  section *sec = get_unnamed_section (hsec->flags,
                                      targetm.htc.unnamed_section_callback,
                                      hsec);
  hsec->sec = sec;
  hsec->next = tric_builtin_sections;
  tric_builtin_sections = hsec;

  /* Patch an existing default section like `bss_section' from varasm.c. */

  if (psec)
    *psec = sec;

  /* Set up tric_data_section[] with default data sections for fast lookup. */

  if (init_lookup)
    {
      unsigned ss1 = (flags & SECTION_SMALL) ? SS_SMALL
        : (flags & TRIC_SECTION_ABSOLUTE) ? SS_ABSOLUTE
        : SS_INDIRECT;

      unsigned ss2 = (flags & SECTION_BSS) ? SS_BSS
        : (flags & SECTION_WRITE) ? SS_DATA
        : SS_RODATA;

      unsigned ss3 = align == 1 ? SS_ALIGN_1
        : align == 2 ? SS_ALIGN_2
        : align == 4 ? SS_ALIGN_4
        : align == 8 ? SS_ALIGN_8
        : SS_ALIGN_NONE;

      tric_data_section[ss1][ss2][ss3] = sec;
    }
}


/* Implement `TARGET_ASM_INIT_SECTIONS' */
/* Set up the TriCore-specific default sections like .data.a4, .zbss, .pictext
   etc. and replace the respective section opjects like text_section from
   varasm.c provided there is such a section object known to varasm.  */

static void
tric_asm_init_sections (void)
{
#define DEF_SECTION(NAME, S_FLAGS, ALIGN, INIT_LOOKUP, PSEC)            \
  tric_def_one_section (NAME, S_FLAGS, ALIGN, INIT_LOOKUP, PSEC);

#include "tricore-sections.def"

#undef DEF_SECTION

  tric_text_section
    = htc_lookup_section (tric_opt_code_pic ? ".pictext" : ".text");
  text_section = tric_text_section->sec;

  ctors_section = get_unnamed_section (0, output_section_asm_op,
                                       CTORS_SECTION_ASM_OP);
  
  dtors_section = get_unnamed_section (0, output_section_asm_op,
                                       DTORS_SECTION_ASM_OP);
}


/* Implement `TARGET_ASM_FUNCTION_SECTION' */
/* This is basically a copy of varasm.c:default_function_section
   that is needed to replace the explicit ".text..." from there
   in the case when PIC shall be generated.
   Don't use `varasm.c::get_named_text_section' at all.  */

static section*
tric_asm_function_section (tree decl, enum node_frequency ARG_UNUSED (freq),
                           bool ARG_UNUSED (startup), bool ARG_UNUSED (exit))
{
  htc_section_t *hsec;

#if defined HAVE_LD_EH_GC_SECTIONS && defined HAVE_LD_EH_GC_SECTIONS_BUG
  /* Old GNU linkers have buggy --gc-section support, which sometimes
     results in .gcc_except_table* sections being garbage collected.  */
  if (decl
      && DECL_SECTION_NAME (decl)
      && DECL_HAS_IMPLICIT_SECTION_NAME_P (decl))
    {
      gcc_unreachable();
      return NULL;
    }
#endif

  if (!flag_reorder_functions
      || !targetm_common.have_named_sections)
    return NULL;

  if (!decl)
    return tric_text_section->sec;

  if (DECL_SECTION_NAME (decl)
      && (hsec = htc_decl_htc_section (decl)))
    {
      gcc_assert (hsec->sec);
      return hsec->sec;
    }
  else if (DECL_SECTION_NAME (decl))
    {
      /* we get here if the section is defined with attribute ((section(...))) */
      hsec = htc_lookup_section (TREE_STRING_POINTER (DECL_SECTION_NAME (decl)));
      gcc_assert (hsec);
      gcc_assert (hsec->sec);
      return hsec->sec;
    }

  gcc_unreachable();
  return NULL;
}


/* IMPLEMENT `TARGET_HTC_BSS_INITIALIZER_P' */
/* Return true if DECL's initializer is suitable for a BSS section.  */

static bool
tric_bss_initializer_p (const_tree decl)
{
  return tric_decl_volatile_const_rodata_p (decl)
    ? false
    : default_htc_bss_initializer_p (decl);
}


/* Implement `TARGET_SECTION_TYPE_FLAGS' */

static unsigned int
tric_section_type_flags (tree decl, const char *name, int reloc)
{
  htc_section_t *secp;
  unsigned int flags = default_section_type_flags (decl, name, reloc);

  if (decl
      && htc_decl_gcc_builtin_p (decl))
    return flags;

  /* We always already went through `resolve_unique_section' resp.
     `targetm.htc.resolve_unique_section' and hence... */

  gcc_assert (name);

  secp = htc_lookup_section (name);

  if (secp)
    return secp->flags;

  for (secp = tric_builtin_sections; secp; secp = secp->next)
    {
      /* Check if names have the same prefix to get the right flags for named
         sections like .zbss.a1.foo  */

      if ((secp->align == 0
           || tric_opt_aligned_data_sections)
          && str_prefix_p (name, ACONCAT ((secp->name, ".", NULL))))
        {
          unsigned new_flags = flags | secp->flags;
          if (!(secp->flags & SECTION_WRITE))
            new_flags &= ~SECTION_WRITE;

          if (htc_log.section)
            {
              char s_old[20], s_new[20];

              targetm.htc.section_flags_to_string (s_old, flags,
                                                   FLAGSTRING_descriptive);
              targetm.htc.section_flags_to_string (s_new, new_flags,
                                                   FLAGSTRING_descriptive);
              
              htc_edump ("%H: inherit %s flags from %s: f=%s -> f=%s\n",
                         decl ? DECL_SOURCE_LOCATION (decl) : UNKNOWN_LOCATION,
                         name, secp->name, s_old, s_new);
            }
            
          return new_flags;
        }
    }

  return flags;
}


/* Implement `TARGET_ASM_SELECT_SECTION' */
/* Assign our own sections for absolute or small addressable stuff
   or for sections that collect aligned data with -maligned-data-sections.
   varasm.c bypasses this hook for local bss objects; we handle this in the
   respective section callback, namely in tric_asm_output_aligned_block.  */

static section*
tric_asm_select_section (tree decl, int reloc,
                         unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED)
{
  int ss1, ss2, ss3;
  section *sec = data_section;

  if (TREE_CODE (decl) == VAR_DECL)
    sec = data_section;
  else if (TREE_CODE (decl) == FUNCTION_DECL)
    sec = text_section;

  if (DECL_P (decl))
    {
      if (decl_readonly_section (decl, reloc))
        sec = readonly_data_section;
    }
  else if (TREE_CODE (decl) == CONSTRUCTOR)
    {
      /* FIXME: The original code in varasm.c:default_select_section()
         also tests for TREE_READONLY.  How can this ever be false if
         TREE_CONSTANT is true?  */
      
      if (!TREE_SIDE_EFFECTS (decl)
          && TREE_CONSTANT (decl))
        sec = readonly_data_section;
    }
  else if (TREE_CODE (decl) == STRING_CST)
    sec = readonly_data_section;
  else
    sec = readonly_data_section;

  /* Up to here it's just what gcc does per default */

  if (sec == text_section)
    return sec;

  if (tric_decl_volatile_const_rodata_p (decl)
      && (sec == bss_section
          || sec == data_section))
    {
      sec = readonly_data_section;
    }
  else if (DECL_P (decl)
           && targetm.htc.bss_initializer_p (decl))
    {
      sec = bss_section;
    }

  /* We narrowed down the section to .bss/.data/.rodata.
     Get accessor IDs to read from tric_data_section[].  */

  if (tric_decl_in_absolute (decl))
    ss1 = SS_ABSOLUTE;
  else if (tric_decl_in_smalldata (decl))
    ss1 = SS_SMALL;
  else
    ss1 = SS_INDIRECT;

  if (sec == readonly_data_section)
    ss2 = SS_RODATA;
  else if (sec == bss_section)
    ss2 = SS_BSS;
  else
    ss2 = SS_DATA;

  ss3 = SS_ALIGN_NONE;

  if (tric_opt_aligned_data_sections)
    {
      int balign = 0;

      if (DECL_P (decl))
        {
          align_variable (decl, true /* dont_output_data */);

          balign = DECL_ALIGN (decl) / BITS_PER_UNIT;
        }
      else if (CONSTRUCTOR == TREE_CODE (decl)
               || STRING_CST == TREE_CODE (decl))
        {
          balign = TYPE_ALIGN (TREE_TYPE (decl)) / BITS_PER_UNIT;
        }

      if (balign == 0)
        ss3 = SS_ALIGN_1;
      else if (balign % 8 == 0)
        ss3 = SS_ALIGN_8;
      else if (balign % 4 == 0)
        ss3 = SS_ALIGN_4;
      else if (balign % 2 == 0)
        ss3 = SS_ALIGN_2;
      else
        ss3 = SS_ALIGN_1;
    }
  
  return tric_data_section[ss1][ss2][ss3];
}


/* Implement void `TARGET_ASM_UNIQUE_SECTION' */

static void
tric_asm_unique_section (tree decl, int reloc)
{
  /* We only need to use .gnu.linkonce if we don't have COMDAT groups.  */
  bool one_only = DECL_ONE_ONLY (decl) && !HAVE_COMDAT_GROUP;
  enum section_category cat;
  const char *sec_name;
  int ss1, ss2, ss3 = SS_ALIGN_NONE;
  const section *sec = NULL;
  const htc_section_t *hsec;

  if (one_only)
    {
      default_unique_section (decl, reloc);
      return;
    }

  cat = tric_decl_volatile_const_rodata_p (decl)
    ? SECCAT_RODATA
    : categorize_decl_for_section (decl, reloc);
  
  switch (cat)
    {
    default:
      default_unique_section (decl, reloc);
      return;

    case SECCAT_TEXT:
      sec = text_section;
      break;
      
    case SECCAT_RODATA:
    case SECCAT_RODATA_MERGE_STR:
    case SECCAT_RODATA_MERGE_STR_INIT:
    case SECCAT_RODATA_MERGE_CONST:
      ss2 = SS_RODATA;
      break;
      
    case SECCAT_DATA:
      ss2 = SS_DATA;
      break;
      
    case SECCAT_BSS:
      ss2 = SS_BSS;
      break;
    }

  if (cat != SECCAT_TEXT)
    {
      ss1 = (tric_decl_in_absolute (decl)    ? SS_ABSOLUTE
             : tric_decl_in_smalldata (decl) ? SS_SMALL
             : SS_INDIRECT);

      if (tric_opt_aligned_data_sections)
        {
          unsigned int align = DECL_ALIGN (decl) / BITS_PER_UNIT;

          if (align == 0)           ss3 = SS_ALIGN_1;
          else if (align % 8 == 0)  ss3 = SS_ALIGN_8;
          else if (align % 4 == 0)  ss3 = SS_ALIGN_4;
          else if (align % 2 == 0)  ss3 = SS_ALIGN_2;
          else                      ss3 = SS_ALIGN_1;
        }

      sec = tric_data_section[ss1][ss2][ss3];
    }

  gcc_assert (sec->unnamed.callback == targetm.htc.unnamed_section_callback);

  hsec = (const htc_section_t*) sec->unnamed.data;

  /* Set DECL_SECTION_NAME (decl) to section "base name" without any
     individual suffixes.  '=' will be picked up by
     targetm.htc.resolve_unique_section.  */

  sec_name = ACONCAT (("=", hsec->name, NULL));

  DECL_SECTION_NAME (decl) = build_string (strlen (sec_name), sec_name);
}


/* Implement `TARGET_ASM_NAMED_SECTION' */

static void
tric_asm_named_section (const char *name, unsigned int flags, tree decl)
{
  tree attr;

  if (decl
      && DECL_P (decl)
      && (attr = lookup_attribute ("@htc-section",
                                   DECL_ATTRIBUTES (decl))))
    {
      const char *s_name = TREE_STRING_POINTER (TREE_VALUE (TREE_VALUE (attr)));
      htc_section_t *hsec = htc_lookup_section (s_name);
      gcc_assert (hsec);
      name = hsec->name;
      flags = hsec->flags;
      decl = hsec->named_decl;
    }

  targetm.htc.asm_out_section (name, flags, decl);
}


typedef struct
{
  /* Location in initializer where (function) address is taken.  */
  location_t location;

  /* VAR_DECL of this initializer or NULL_TREE if unknown.  */
  const_tree decl;
} tric_walk_data_t;


/* Callback for tree_walk as we hunt for function addresses in initializers
   with -mcode-pic or -msmall-pid.  */

static tree
tric_walk_initializer_r (tree *node, int *n ATTRIBUTE_UNUSED, void *data)
{
  tric_walk_data_t *walk_data = (tric_walk_data_t*) data;
  location_t loc = walk_data->location;

  switch (TREE_CODE (*node))
    {
    case VAR_DECL:

      if (loc != UNKNOWN_LOCATION
          && tric_opt_msmall_pid
          && tric_decl_in_smalldata (*node))
        {
          if (walk_data->decl != NULL_TREE)
            warning_at (loc, OPT_Winitialize_pid,
                        "initializer of %qD uses address of register-relative"
                        " addressable variable %qD", walk_data->decl, *node);
          else
            warning_at (loc, OPT_Winitialize_pid,
                        "initializer uses address of register-relative"
                        " addressable variable %qD", *node);
        }
      break;

    case STRING_CST:

      if (tric_opt_msmall_pid
          && tric_decl_in_smalldata (*node)
          && !tric_node_seen (&tric_pid_init, *node))
        {
          if (walk_data->decl != NULL_TREE)
            warning_at (DECL_SOURCE_LOCATION (walk_data->decl),
                        OPT_Winitialize_pid,
                        "initializer of %qD uses address of register-relative"
                        " addressable string literal \"%s\"",
                        walk_data->decl, TREE_STRING_POINTER (*node));
          else
            warning (OPT_Winitialize_pid,
                     "initializer uses address of register-relative"
                     " addressable string literal \"%s\"",
                     TREE_STRING_POINTER (*node));
        }
      break;

    case FUNCTION_DECL:

      if (loc != UNKNOWN_LOCATION
          && tric_opt_code_pic)
        {
          if (walk_data->decl != NULL_TREE)
            {
              warning_at (loc, OPT_Winitialize_pic,
                          "initializer of %qD cannot compute address of"
                          " function %qD at compile time with %qs",
                          walk_data->decl, *node, "-mcode-pic");
            }
          else
            {
              warning_at (loc, OPT_Winitialize_pic,
                          "initializer cannot compute address of"
                          " function %qD at compile time with %qs",
                          *node, "-mcode-pic");
            }
        }
      break;
      
    case ADDR_EXPR:
      {
        /* Pass down the location where the (function) address is taken.  */

        walk_data->location = EXPR_LOCATION (*node);
        walk_tree_without_duplicates (&TREE_OPERAND (*node, 0),
                                      tric_walk_initializer_r, walk_data);
        walk_data->location = loc;
      }
      break;
      
    default:
      break;
    }

  return NULL_TREE;
}


/* Implement `TARGET_ENCODE_SECTION_INFO' */
/* Encode symbol attributes (local vs. global, tls model) of a SYMBOL_REF
   into its SYMBOL_REF_FLAGS.

   Also check if DECL's initializer takes addresses of functions.
   With -mcode-pic, such addresses are not computable at load lime.
   This handles the case of static variables.  */

static void
tric_encode_section_info (tree decl, rtx rtl, int first)
{
  unsigned int flag = 0;
  default_encode_section_info (decl, rtl, first);

  if ((tric_opt_code_pic
       || (tric_opt_msmall_pid && tric_warn_initialize_pid))
      && VAR_DECL == TREE_CODE (decl)
      && DECL_INITIAL (decl))
    {
      tric_walk_data_t walk_data = { UNKNOWN_LOCATION, NULL_TREE };

      walk_data.decl = decl;
      walk_tree_without_duplicates (& DECL_INITIAL (decl),
                                    tric_walk_initializer_r, &walk_data);
    }

  switch (TREE_CODE (decl))
    {
    default:
      break;

    case FUNCTION_DECL:
      if (tric_opt_code_pic)
        flag |= TRIC_SYMBOL_FLAG_PIC;
     
      if (tric_longcall_function_p (decl))
        flag |= TRIC_SYMBOL_FLAG_LONGCALL;

      break;

    case VAR_DECL:
    case STRING_CST:
    case CONSTRUCTOR:
      if (tric_decl_in_absolute (decl))
        flag |= TRIC_SYMBOL_FLAG_ABSOLUTE;
      
      if (tric_decl_in_smalldata (decl))
        flag |= TRIC_SYMBOL_FLAG_SMALL;

      break;
    }

  SYMBOL_REF_FLAGS (XEXP (rtl, 0)) |= flag;
}


/* Implement `TARGET_ASM_DECLARE_CONSTANT_NAME' */
/* Check EXPR as so that it does not take addresses of functions.
   With -mcode-pic, such addresses are not computable at load lime.
   This handles the case of local auto variables.  */

static void
tric_asm_declare_constant_name (FILE *file, const char *name, const_tree expr,
                                HOST_WIDE_INT size)
{
  if (tric_opt_code_pic
      || (tric_opt_msmall_pid && tric_warn_initialize_pid))
    {
      tric_walk_data_t walk_data = { UNKNOWN_LOCATION, NULL_TREE };
      tree node = CONST_CAST_TREE (expr);

      walk_tree_without_duplicates (&node, tric_walk_initializer_r, &walk_data);
    }

  default_asm_declare_constant_name (file, name, expr, size);
}


/* Implement `TARGET_ASM_OUTPUT_ANCHOR' */

static void
tric_asm_output_anchor (rtx symbol)
{
  char buffer[100];

  sprintf (buffer, ".+ " HOST_WIDE_INT_PRINT_DEC,
           SYMBOL_REF_BLOCK_OFFSET (symbol));
  ASM_OUTPUT_DEF (asm_out_file, XSTR (symbol, 0), buffer);
}


/* Implement `TARGET_USE_ANCHORS_FOR_SYMBOL_P' */

static bool
tric_use_anchors_for_symbol_p (const_rtx symbol)
{
  if (flag_data_sections
      || tric_symbol_ref_absolute_p (symbol)
      || tric_symbol_ref_small16_p (symbol))
    {
      return false;
    }
  
  return true;
}


/* Helper for tric_asm_output_aligned_var.  */

static void
tric_asm_output_aligned_block (FILE* stream, tree decl, const char *name,
                               int size, unsigned int align)
{
  int ss1 = SS1_MAX;

  /* Handle cases LTO markers injected for the linker by toplev.c.
     They have DECL == NULL.  */
      
  if (str_prefix_p (name, "__gnu_lto"))
    {
      fprintf (stream, "\t.comm\t");
      assemble_name (stream, name);
      fprintf (stream, ", %d, %d\n", size, align / BITS_PER_UNIT);
      
      return;
    }
    
  if (decl && tric_decl_in_smalldata (decl))
    {
      ss1 = SS_SMALL;
    }
  else if (decl && tric_decl_in_absolute (decl))
    {
      ss1 = SS_ABSOLUTE;
    }
  else
    {
      if (!flag_no_common)
        {
          fprintf (stream, "\t.comm\t");
          assemble_name (stream, name);
          fprintf (stream,
                   ", %d, %d\n", size, /* floor_log2 */ align / BITS_PER_UNIT);

          return;
        }
      else
        ss1 = SS_INDIRECT;
    }

  /* varasm.c:get_variable_section() bypasses targetm.asm_out.select_section
     for local bss objects so that we did not assign a section with appropriate
     suffix if -maligned-data-sections is on.  Do this now, cf. HDP-223.  */

  if (ss1 != SS1_MAX)
    {
      int ss3 = SS_ALIGN_NONE;

      if (tric_opt_aligned_data_sections)
        {
          unsigned int balign = align / BITS_PER_UNIT;

          if (balign == 0)
            ss3 = SS_ALIGN_1;
          else if (balign % 8 == 0)
            ss3 = SS_ALIGN_8;
          else if (balign % 4 == 0)
            ss3 = SS_ALIGN_4;
          else if (balign % 2 == 0)
            ss3 = SS_ALIGN_2;
          else
            ss3 = SS_ALIGN_1;
        }

      switch_to_section (tric_data_section[ss1][SS_BSS][ss3]);
    }

  ASM_OUTPUT_ALIGN (stream, floor_log2 (align / BITS_PER_UNIT));
  
  /* Output the type information.  */

  fprintf (stream, "%s\t ", TYPE_ASM_OP);
  assemble_name (stream, name);
  fprintf (stream, "," TYPE_OPERAND_FMT "\n", "object");

  size_directive_output = 0;
  if (!flag_inhibit_size_directive && size != 0)
    {
      size_directive_output = 1;
      fprintf (stream, "%s\t ", SIZE_ASM_OP);
      assemble_name (stream, name);
      fprintf (stream, ",%d\n", size);
    }
  ASM_OUTPUT_LABEL (stream, name);
  
  if (size != 0)
    fprintf (stream, "\t.space\t%d\n", size);
}


/* Implement `ASM_OUTPUT_ALIGNED_DECL_LOCAL' */
/* Implement `ASM_OUTPUT_ALIGNED_DECL_COMMON' */

void
tric_asm_output_aligned_var (FILE *stream, tree decl, const char *name,
                             int size, int align, int common)
{
  fprintf (stream, "\t%s\t", (common == 0) ? ".local" : ".global");
  assemble_name (stream, name);
  fprintf (stream, "\n");
  tric_asm_output_aligned_block (stream, decl, name, size, align);
}


/***********************************************************************
 ** Constructors, Destructors
 ***********************************************************************/


/* Implement `TARGET_ASM_CONSTRUCTOR' */

static void
tric_asm_constructor (rtx symbol, int priority)
{
  fputs ("\t.global __do_global_ctors\n", asm_out_file);
  default_ctor_section_asm_out_constructor (symbol, priority);
}


/* Implement `TARGET_ASM_DESTRUCTOR' */

static void
tric_asm_destructor (rtx symbol, int priority)
{
  fputs ("\t.global __do_global_dtors\n", asm_out_file);
  default_dtor_section_asm_out_destructor (symbol, priority);
}


/* Implement `ASM_OUTPUT_EXTERNAL' */

void
tric_asm_output_external (FILE *file, tree decl, const char *name)
{
  default_elf_asm_output_external (file, decl, name);
  
  /* We output the name if and only if TREE_SYMBOL_REFERENCED is set
     in order to avoid putting out names that are never really used. */
  
  if (TREE_SYMBOL_REFERENCED (DECL_ASSEMBLER_NAME (decl))
      && !TREE_ASM_WRITTEN (DECL_ASSEMBLER_NAME (decl)))
    {
      TREE_ASM_WRITTEN (DECL_ASSEMBLER_NAME (decl)) = 1;

      fputs ("\t.extern\t", file);
      assemble_name (file, name);
      if (TREE_CODE (decl) == FUNCTION_DECL)
        {
          fprintf (file, ",STT_FUNC,0\n");
        }
      else
        {
          fprintf (file, ",STT_OBJECT," HOST_WIDE_INT_PRINT_DEC "\n",
                   int_size_in_bytes (TREE_TYPE (decl)));
        }
    }
}



/***********************************************************************
 ** Inline Assembler
 ***********************************************************************/

/* Implement `TARGET_MD_ASM_CLOBBERS' */

static tree
tric_md_asm_clobbers (tree outputs ATTRIBUTE_UNUSED,
                      tree inputs ATTRIBUTE_UNUSED,
                      tree clobbers)
{
  tree t;

  for (t = clobbers; t != NULL_TREE; t = TREE_CHAIN (t))
    {
      const char *asmspec = TREE_STRING_POINTER (TREE_VALUE (t));
      int nregs, regno = decode_reg_name_and_count (asmspec, &nregs);

      if (tric_opt_code_pic && tric_opt_dynamic_code_pic
          && regno >= 0 && IN_RANGE (REG_PIC, regno, regno + nregs - 1))
        {
          error ("register %qs must not be clobbered with %qs",
                 reg_names[REG_PIC],
                 cl_options[OPT_mdynamic_address_calc_with_code_pic].opt_text);
        }
    }

  clobbers = tree_cons (NULL_TREE, build_string (3, "PSW"),
                        clobbers);

  return clobbers;
}


/***********************************************************************
 ** EABI 2.1.4.2:  Union and Structure Layout
 ***********************************************************************/


/* FIELD is a FIELD_DECL.  Get the previous field or NULL_TREE if FIELD
   is the first field in the component.  */

static tree
tric_get_previous_field (tree field)
{
  tree parent = DECL_CONTEXT (field);
  tree fld, prev = NULL_TREE;

  for (fld = TYPE_FIELDS (parent); fld != NULL_TREE; fld = TREE_CHAIN (fld))
    {
      if (TREE_CODE (fld) != FIELD_DECL)
        continue;

      if (fld == field)
        return prev;

      prev = fld;
    }

  gcc_unreachable();
}


/* Implement `ADJUST_FIELD_ALIGN' */
/* Adjust field alignment for EABI.  */

unsigned
tric_eabi_adjust_field_align (tree field, unsigned desired_align)
{
  /* Calculate the bitfield offset according to the EABI.  Bitfield alignment
     requires special attention. We have to use 64-bit arithmetik because
     size will measure bits and 2**32 * 8 doesn't fit in an 32-bit value.  */

  unsigned HOST_WIDE_INT dsize, field_position = 0;
  unsigned HOST_WIDE_INT halfword_start, halfword_end;
  tree size, prev;

  if (!DECL_BIT_FIELD (field) || DECL_ARTIFICIAL (field))
    return desired_align;

  size = DECL_SIZE (field);

  dsize = size != NULL_TREE && TREE_CODE (size) == INTEGER_CST
    ? tree_to_uhwi (size)
    /* A field with variable size */
    : desired_align;

  if (dsize == 0)
    /* EABI 2.1.4.3 requires byte alignment for zero-width bit fields */
    return EMPTY_FIELD_BOUNDARY;

  /* EABI 2.1.4.3 specifies that bit fields must not exceed 32 bits and must
     not cross more than one halfword boundary.  */

  if (tric_opt_eabi_bitfield_limit
      && dsize > 32
      && !tric_node_seen (&tric_eabi_error, field))
    {
      error ("EABI: width of bitfield %q+D exceeds 32", field);
    }

  /* Get offset and size of the previous field in order to compute
     the correct alignment of the current field.  */
  
  prev = tric_get_previous_field (field);

  if (prev && size && !DECL_PACKED (field))
    {
      tree offset = DECL_SIZE (prev);
      
      if (offset != NULL_TREE && TREE_CODE (offset) == INTEGER_CST)
        field_position += tree_to_uhwi (offset);

      offset = DECL_FIELD_BIT_OFFSET (prev);

      if (offset != NULL_TREE && TREE_CODE (offset) == INTEGER_CST)
        field_position += tree_to_uhwi (offset);
    }

  /* Half-word where the field starts resp. ends.  */

  halfword_start = field_position / 16;
  halfword_end = (field_position + dsize - 1) / 16;

  /* If the field would cross more than 2 half-word boundaries, align it
     so that it crosses at most one such boundary.  */

  return halfword_end > halfword_start + 1 ? 16 : 1;
}


/* Helper for the next function.
   Compute alignment of structs and unions.  */

static unsigned
tric_eabi_struct_alignment (tree type, unsigned align)
{
  tree field;

  gcc_assert (RECORD_OR_UNION_TYPE_P (type));
  
  for (field = TYPE_FIELDS (type); field; field = TREE_CHAIN (field))
    {
      unsigned field_align;
      unsigned desired_align;
      
      if (TREE_CODE (field) != FIELD_DECL
          || TREE_TYPE (field) == error_mark_node)
        continue;

      desired_align = TYPE_ALIGN (TREE_TYPE (field));

      field_align = DECL_BIT_FIELD (field)
        /* For bitfields just take the type alignment.  This will be
           calculated by tric_eabi_adjust_field_align above.  */
        ? TYPE_ALIGN (type)
        : DATA_ALIGNMENT (TREE_TYPE (field), desired_align);

      /* TYPE_ALIGN might be unknown, i.e. -1.  Thus, use signed MAX.  */

      align = MAX ((int) align, (int) field_align);
    }

  return align;
}


/* Return the HOST_WIDE_INT least significant bits of T, a sizetype kind
   INTEGER_CST.  This makes sure to properly sign-extend the constant.  */

static HOST_WIDE_INT
size_low_cst (const_tree t)
{
  double_int d = tree_to_double_int (t);
  return d.sext (TYPE_PRECISION (TREE_TYPE (t))).low;
}


/* Implement `ROUND_TYPE_ALIGN' */

unsigned
tric_eabi_round_type_align (tree type, unsigned computed, unsigned specified)
{
  /* The struct alignment required by the ABI is quite uncommon... */

  unsigned align = MAX (computed, specified);
  tree field, tsize = TYPE_SIZE (type);

  /* If nothing special is to be done, use default from stor-layout.c  */
  
  if (!RECORD_OR_UNION_TYPE_P (type)
      || TYPE_PACKED (type)
      || !(tsize && TREE_CODE (tsize) == INTEGER_CST)
      || size_low_cst (tsize) <= BITS_PER_UNIT)
    {
      return align;
    }

  /* If size > 8, EABI 2.1.4.2 requires at least halfword alignment even
     if all fields are chars.  */

  align = tric_eabi_struct_alignment (type, MAX (align, 2 * BITS_PER_UNIT));

  /* Bitfields need special treatment */

  for (field = TYPE_FIELDS (type); field; field = TREE_CHAIN (field))
    {
      /* EABI 2.1.4.3 specifies that bitfields > 16 impose word alignment on
         the struct although the bitfield itself is only halfword aligned.  */

      if (TREE_CODE (field) == FIELD_DECL
          && DECL_BIT_FIELD (field)
          && size_low_cst (DECL_SIZE (field)) > 16)
        {
          align = MAX (align, 4 * BITS_PER_UNIT);
        }
    }

  return align;
}


/* Implement `DATA_ALIGNMENT' */

unsigned
tric_eabi_data_alignment (tree type ATTRIBUTE_UNUSED, unsigned basic_align)
{
  return basic_align;
}


/***********************************************************************
 ** Instruction Lengths
 ***********************************************************************/

/* Get length of a load/store insn that loads/stores REG from/to MEM.
   If this is a load then LOAD_P is true and false, otherwise.
   SIGN_P is false if a load zero-extends.
   SIGN_P is true  if a load sign-extends.
   If this is a store or the load does not change the width of the data
   then SIGN_P is ignored.  */

static int
tric_len_load_store (enum machine_mode mode, rtx reg, rtx mem,
                     bool load_p, bool sign_p)
{
  rtx addr = XEXP (mem, 0);
  int n_bytes = GET_MODE_SIZE (mode);
  bool have_short_mov_p = true;
  RTX_CODE code = GET_CODE (addr);

  if (QImode == mode && load_p && sign_p)
    have_short_mov_p = false;
  
  if (HImode == mode && load_p && !sign_p)
    have_short_mov_p = false;

  if (!have_short_mov_p)
    {
      return 4;
    }

  if (REG_P (addr)
      || POST_INC == code)
    {
      return 2;
    }
  
  if (PLUS == code
      && REG_P (XEXP (addr, 0))
      && CONST_INT_P (XEXP (addr, 1)))
    {
      rtx base = XEXP (addr, 0);
      int off = INTVAL (XEXP (addr, 1));
        
      if (off % n_bytes == 0
          && IN_RANGE (off / n_bytes, 0, 15)
          && (REGNO (base) == REG_A15
              || REGNO (reg) == REG_D15
              || REGNO (reg) == REG_A15))
        {
          return 2;
        }
            
      if (n_bytes == 4
          && off % 4 == 0
          && IN_RANGE (off / 4, 0, 255)
          && REGNO (base) == REG_SP
          && (REGNO (reg) == REG_D15
              || REGNO (reg) == REG_A15))
        {
          return 2;
        }
    } /* PLUS */

  return 4;
}


/* Get length of a mov* insn.  MODE is the move mode.  DEST is the destination
   SRC  is the source
   SIGN_P is true  if SRC is to be sign-extended when loaded
   SIGN_P is false if SRC is to be zero-extended when loaded  */

static int
tric_len_mov (enum machine_mode mode, rtx dest, rtx src, bool sign_p)
{
  if (CONST_DOUBLE_P (src))
    {
      src = simplify_gen_subreg (SImode, src, mode, 0);
    }
  
  if (CONSTANT_P (src))
    {
      gcc_assert (REG_P (dest));

      if (A_REG_P (dest) && satisfies_constraint_Ku4 (src))
        return 2;

      if (D_REG_P (dest))
        {
          if (REGNO (dest) == REG_D15
              && satisfies_constraint_Ku8 (src))
            return 2;

          if (satisfies_constraint_Ks4 (src))
            return 2;
        }
    }

  if (REG_P (dest) && MEM_P (src))
    return tric_len_load_store (GET_MODE (src), dest, src, true, sign_p);

  if (MEM_P (dest) && REG_P (src))
    return tric_len_load_store (mode, src, dest, false, sign_p);
  
  return 4;
}


/* Get length of MOV %e, const  */

static int
tric_len_mov64 (rtx dest ATTRIBUTE_UNUSED, rtx src)
{
  if (satisfies_constraint_Ks4 (src))
    {
      return 2;
    }

  return 4;
}


/* Length of "*addsi3": ADD, ADDI, ADDIH, LEA, SUB.A, ADD.A, ADDIH.A,
   ADDSC.A.  */

static int
tric_len_add32 (rtx *xop)
{
  int reg0 = REGNO (xop[0]);
  int reg1 = REGNO (xop[1]);

  if (satisfies_constraint_Ks4 (xop[2]))
    {
      if (reg0 == reg1 || reg0 == REG_D15 || reg1 == REG_D15)
        return 2;
    }
  
  if (satisfies_constraint_Kn8 (xop[2])
      && reg0 == REG_SP
      && reg1 == REG_SP)
    return 2;

  if (REG_P (xop[2]))
    {
      int reg2 = REGNO (xop[2]);

      if (D_REGNO_P (reg0) && D_REGNO_P (reg1) && D_REGNO_P (reg2))
        {
          if (reg0 == reg1 || reg0 == reg2)
            return 2;

          if (reg0 == REG_D15 || reg1 == REG_D15 || reg2 == REG_D15)
            return 2;
        }
      
      if (A_REGNO_P (reg0) && A_REGNO_P (reg1) && A_REGNO_P (reg2))
        {
          if (reg0 == reg1 || reg0 == reg2)
            return 2;
        }
      
      if (A_REGNO_P (reg0) && A_REGNO_P (reg1) && reg2 == REG_D15)
        return 2;

      if (A_REGNO_P (reg0) && A_REGNO_P (reg2) && reg1 == REG_D15)
        return 2;
    }
 
  return 4;
}


/* Length of "subsi3" (SUB, SUB.A, RSUB).  */

static int
tric_len_sub32 (rtx *xop)
{
  if (D_REG_P (xop[0]) && REG_P (xop[1]))
    {
      int reg0 = REGNO (xop[0]);
      int reg1 = REGNO (xop[1]);

      if (reg0 == reg1 || reg0 == REG_D15 || reg1 == REG_D15)
        return 2;
    }
  
  return 4;
}


static int
tric_len_bitop (rtx insn, rtx *xop)
{
  rtx src = SET_SRC (single_set (insn));

  if (REG_P (xop[1]) && REG_P (xop[2])
      && (REGNO (xop[0]) == REGNO (xop[1])
          || REGNO (xop[0]) == REGNO (xop[2])))
    {
      return 2;
    }
    
  if ((AND == GET_CODE (src)
       || IOR == GET_CODE (src))
      && satisfies_constraint_Ku8 (xop[2])
      && REGNO (xop[0]) == REG_D15
      && REGNO (xop[1]) == REG_D15)
    {
      return 2;
    }
      
  return 4;
}


/* Length of EQ and NE instructions.  */

static int
tric_len_seq (rtx insn, rtx *xop)
{
  rtx src = SET_SRC (single_set (insn));

  if (GET_CODE (src) == EQ
      && D_REG_P (xop[0]) && D_REG_P (xop[1])
      && REGNO (xop[0]) == REG_D15
      && (D_REG_P (xop[2])
          || satisfies_constraint_Ks4 (xop[2])))
    {
      return 2;
    }
      
  return 4;
}


/* Implement `ADJUST_INSN_LENGTH' */

int
tric_adjust_insn_length (rtx insn, int len)
{
  rtx *op = recog_data.operand;
  enum attr_adjust adjust;
  enum machine_mode mode0 = VOIDmode;

  /* Some complex insns don't need length adjustment and therefore
     the length need not/must not be adjusted for these insns.
     It is easier to state this in an insn attribute "adjust" than
     to clutter up code here...  */
  
  if (!NONDEBUG_INSN_P (insn)
      || -1 == recog_memoized (insn))
    {
      return len;
    }

  /* Read from insn attribute "adjust" if/how length is to be adjusted.  */

  adjust = get_attr_adjust (insn);

  if (adjust == ADJUST_NO)
    {
      /* Nothing to adjust: The length from attribute "length" is fine.
         This is the default.  */
      
      return len;
    }
  
  /* Extract insn's operands.  */
  
  extract_constrain_insn_cached (insn);
  
  /* Dispatch to right function.  */

  if (single_set (insn))
    mode0 = GET_MODE (SET_DEST (single_set (insn)));

  switch (adjust)
    {
    default:
      gcc_unreachable();
      
    case ADJUST_MOV8S:
    case ADJUST_MOV16S:
      len = tric_len_mov (mode0, op[0], op[1], true);
      break;
      
    case ADJUST_MOV8:
    case ADJUST_MOV16:
    case ADJUST_MOV32:
      len = tric_len_mov (mode0, op[0], op[1], false);
      break;
      
    case ADJUST_MOV64: len = tric_len_mov64 (op[0], op[1]); break;
      
    case ADJUST_BITOP: len = tric_len_bitop (insn, op); break;
    case ADJUST_SEQ:   len = tric_len_seq   (insn, op); break;
      
    case ADJUST_ADD32: len = tric_len_add32 (op); break;
    case ADJUST_SUB32: len = tric_len_sub32 (op); break;

    case ADJUST_ADDSC:
      len = REGNO (op[1]) == REG_D15 ? 2 : 4;
      break;

    case ADJUST_SAT:
      len = REGNO (op[0]) == REGNO (op[1]) ? 2 : 4;
      break;
    }
  
  return len;
}



/* EABI 2.1.3.1 Circular Buffer Pointers

   Set up built-in types __circ and __circ64 to make use of
   circular addressing modes.  The types will be as if defined per

   typedef unsigned long long __circ64 __attribute__((mode(PDI)))

   typedef union
   {
       struct
       {
           void *buf;
           unsigned short index;
           unsigned short length;
       };
       __circ64 circ64;
   } __circ __attribute__((mode(PDI)));
*/

static void
tric_init_builtin__circ (void)
{
  static struct
  {
    const char *name;
    tree *type;
  }
  circ_field[] =
    {
      { "buf",    &ptr_type_node },
      { "index",  &short_unsigned_type_node },
      { "length", &short_unsigned_type_node }
    };
  
  unsigned i;
  tree circ64, field, fields = NULL_TREE;
  tree typeA = lang_hooks.types.make_type (RECORD_TYPE);
  tree typeB = lang_hooks.types.make_type (UNION_TYPE);

  /* Set up builtin type __circ64 as
     unsigned long long __attribute__((mode(PDI)))  */

  circ64 = make_unsigned_type (GET_MODE_BITSIZE (PDImode));
  SET_TYPE_MODE (circ64, PDImode);

  lang_hooks.types.register_builtin_type (circ64, "__circ64");

  /* typeA:  Set up
     struct { void *buf; unsigned short index, length; }  */
     
  for (i = 0; i < sizeof circ_field / sizeof circ_field[0]; i++)
    {
      field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                          get_identifier (circ_field[i].name),
                          *circ_field[i].type);
      DECL_CONTEXT (field) = typeA;
      DECL_CHAIN (field) = fields;
      fields = field;
    }

  TYPE_FIELDS (typeA) = nreverse (fields);
  layout_type (typeA);

  /* typeB:  Set up __circ as
     union {
       struct { void *buf; unsigned short index, length; };
       __circ64 circ64;
     }
   */

  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE, typeA);
  DECL_CONTEXT (field) = typeB;
  DECL_CHAIN (field) = NULL_TREE;
  fields = field;

  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      get_identifier ("circ64"), circ64);
  DECL_CONTEXT (field) = typeB;
  DECL_CHAIN (field) = fields;
  fields = field;

  TYPE_FIELDS (typeB) = nreverse (fields);
  layout_type (typeB);
  SET_TYPE_MODE (typeB, PDImode);

  lang_hooks.types.register_builtin_type (typeB, "__circ");
}

static void
tric_init_builtin__float16 (void)
{
  tree float16_type = make_node (REAL_TYPE);
  TYPE_PRECISION (float16_type) = 16;
  layout_type (float16_type);
  lang_hooks.types.register_builtin_type (float16_type, "__float16");
}


/* Implement `TARGET_MANGLE_TYPE'.  */

static const char*
tric_mangle_type (const_tree type)
{
  /* Half-precision float.  We cannot pass __float16 arguments to functions,
     but we can pass pointers to __float16, hence we need to mangle it in
     some way.  The TriCore EABI doesn't give any clue, so treat it like a
     user-defined type.  */

  if (TREE_CODE (type) == REAL_TYPE
      && TYPE_PRECISION (type) == 16)
    return "9__float16";

  return NULL;
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

typedef struct GTY(()) tric_builtin_struct
{
  enum insn_code icode;
  const char *name;
  int n_args;
  tree fndecl;
  enum tree_code tcode;
} tric_builtin_t;


/* Notice that tric_builtin[] and tric_builtin_id are initialized in such a
   way that a built-in's ID can be used to access the built-in by means of
   tric_builtin[ID].  */

static GTY(()) tric_builtin_t
tric_builtin[TRIC_BUILTIN_COUNT] =
{
#define DEF_BUILTIN(NAME, N_ARGS, ID, TYPE, INSN, TCODE)                \
  {                                                                     \
    (enum insn_code) CODE_FOR_ ## INSN, "__builtin_tricore_" NAME,      \
    N_ARGS, NULL_TREE, TCODE                                            \
  },
#include "builtins.def"
#undef DEF_BUILTIN
};


/* Implement `TARGET_BUILTIN_DECL' */

static tree
tric_builtin_decl (unsigned id, bool initialize_p ATTRIBUTE_UNUSED)
{
  if (id < TRIC_BUILTIN_COUNT)
    return tric_builtin[id].fndecl;

  return error_mark_node;
}


/* Implement `TARGET_INIT_BUILTINS' */
/* Set up all built-in functions and built-in types for this target.  */

static void
tric_init_builtins (void)
{
  size_t id;
  tree void_ftype_void
    = build_function_type_list (void_type_node, NULL_TREE);

  tree volatile_void_node
    = build_qualified_type (void_type_node, TYPE_QUAL_VOLATILE);

  tree volatile_voidptr_node
    = build_pointer_type_for_mode (volatile_void_node, Pmode, true);

  tree void_ftype_v_voidptr_uint64
    = build_function_type_list (void_type_node,
                                volatile_voidptr_node,
                                long_long_unsigned_type_node,
                                NULL_TREE);
  tree void_ftype_v_voidptr_2uint
    = build_function_type_list (void_type_node,
                                volatile_voidptr_node,
                                unsigned_type_node, unsigned_type_node,
                                NULL_TREE);
  tree void_ftype_v_voidptr_3uint
    = build_function_type_list (void_type_node,
                                volatile_voidptr_node,
                                unsigned_type_node, unsigned_type_node,
                                unsigned_type_node,
                                NULL_TREE);

  tree uint64_ftype_2uint
    = build_function_type_list (long_long_unsigned_type_node,
                                unsigned_type_node, unsigned_type_node,
                                NULL_TREE);
  tree uint64_ftype_3uint
    = build_function_type_list (long_long_unsigned_type_node,
                                unsigned_type_node, unsigned_type_node,
                                unsigned_type_node,
                                NULL_TREE);

  tree int_ftype_int
    = build_function_type_list (integer_type_node,
                                integer_type_node,
                                NULL_TREE);
  tree uint_ftype_uint
    = build_function_type_list (unsigned_type_node,
                                unsigned_type_node,
                                NULL_TREE);
  tree uint_ftype_2uint
    = build_function_type_list (unsigned_type_node,
                                unsigned_type_node, unsigned_type_node,
                                NULL_TREE);
  tree uint_ftype_4uint
    = build_function_type_list (unsigned_type_node,
                                unsigned_type_node, unsigned_type_node,
                                unsigned_type_node, unsigned_type_node,
                                NULL_TREE);
  tree uint_ftype_v_voidptr_uint
    = build_function_type_list (unsigned_type_node,
                                volatile_voidptr_node, unsigned_type_node,
                                NULL_TREE);
  tree uint_ftype_v_voidptr_2uint
    = build_function_type_list (unsigned_type_node,
                                volatile_voidptr_node, unsigned_type_node,
                                unsigned_type_node,
                                NULL_TREE);
  tric_init_builtin__circ ();

  tric_init_builtin__float16 ();

#define DEF_BUILTIN(NAME, N_ARGS, ID, TYPE, INSN, TCODE)                \
  id = TRIC_BUILTIN_ ## ID;                                             \
  gcc_assert (id < TRIC_BUILTIN_COUNT);                                 \
  tric_builtin[id].fndecl                                               \
    = add_builtin_function (tric_builtin[id].name, TYPE, id,            \
                            BUILT_IN_MD, NULL, NULL_TREE);
#include "builtins.def"
#undef DEF_BUILTIN

}


/* Subroutine of tric_expand_builtin to expand vanilla builtins with
   1 ... 3 arguments.  ICODE is the insn code number, EXP the call expression.
   VOID_P is true iff the built-in's return type is void.  If TARGET is
   non-null then it's the preferred place to return the result.  */

static rtx
tric_default_expand_builtin (enum insn_code icode, tree exp, rtx target,
                             bool void_p)
{
  rtx pat, xop[4];
  int n, n_args = call_expr_nargs (exp);
  enum machine_mode tmode = insn_data[icode].operand[0].mode;

  gcc_assert (n_args >= 1 && n_args <= 4);
              
  if (void_p
      || target == NULL_RTX
      || GET_MODE (target) != tmode
      || !insn_data[icode].operand[0].predicate (target, tmode))
    {
      target = gen_reg_rtx (tmode);
    }

  for (n = 0; n < n_args; n++)
    {
      tree arg = CALL_EXPR_ARG (exp, n);
      rtx op = expand_normal (arg);
      enum machine_mode opmode = GET_MODE (op);
      enum machine_mode mode = insn_data[icode].operand[n + !void_p].mode;

      if ((opmode == SImode || opmode == VOIDmode) && mode == HImode)
        {
          opmode = HImode;
          op = gen_lowpart (HImode, op);
        }

      /* In case the insn wants input operands in modes different from
         the result, abort.  */
  
      gcc_assert (opmode == mode || opmode == VOIDmode);

      if (!insn_data[icode].operand[n + !void_p].predicate (op, mode))
        op = copy_to_mode_reg (mode, op);

      xop[n] = op;
    }

  if (void_p)
    {
      switch (n_args)
        {
        case 1: pat = GEN_FCN (icode) (xop[0]); break;
        case 2: pat = GEN_FCN (icode) (xop[0], xop[1]); break;
        case 3: pat = GEN_FCN (icode) (xop[0], xop[1], xop[2]); break;
        case 4: pat = GEN_FCN (icode) (xop[0], xop[1], xop[2], xop[3]); break;

        default:
          gcc_unreachable();
        }
    }
  else
    {
      switch (n_args)
        {
        case 1: pat = GEN_FCN (icode) (target, xop[0]); break;
        case 2: pat = GEN_FCN (icode) (target, xop[0], xop[1]); break;
        case 3: pat = GEN_FCN (icode) (target, xop[0], xop[1], xop[2]); break;
        case 4: pat = GEN_FCN (icode) (target, xop[0], xop[1], xop[2], xop[3]);
          break;

        default:
          gcc_unreachable();
        }
    }
  
  if (pat == NULL_RTX)
    return NULL_RTX;

  emit_insn (pat);

  return target;
}


/* Implement `TARGET_EXPAND_BUILTIN'.  */
/* Expand an expression EXP that calls a built-in function, with result going
   to TARGET if that's convenient (and in mode MODE if that's convenient).
   SUBTARGET may be used as the target for computing one of EXP's operands.
   IGNORE is nonzero if the value is to be ignored.  */

static rtx
tric_expand_builtin (tree exp, rtx target,
                     rtx subtarget ATTRIBUTE_UNUSED,
                     enum machine_mode mode ATTRIBUTE_UNUSED,
                     int ignore ATTRIBUTE_UNUSED)
{
  tree fndecl = TREE_OPERAND (CALL_EXPR_FN (exp), 0);
  unsigned int id = DECL_FUNCTION_CODE (fndecl);
  const tric_builtin_t *d = &tric_builtin[id];
  bool void_p = VOID_TYPE_P (TREE_TYPE (TREE_TYPE (fndecl)));

  gcc_assert (id < TRIC_BUILTIN_COUNT);

  /* No special treatment needed: vanilla expand.  */

  gcc_assert (d->n_args == call_expr_nargs (exp));
  gcc_assert (d->icode != CODE_FOR_nothing);

  if (d->n_args == 0)
    {
      emit_insn ((GEN_FCN (d->icode)) (target));
      return NULL_RTX;
    }

  return tric_default_expand_builtin (d->icode, exp, target, void_p);
}


/* Fold for the INSERT instruction / builtin and alike.  */

static tree
tric_fold_insert (tree ttarget, tree tval, tree tpos, tree twidth)
{
  HOST_WIDE_INT target = tric_tree_to_hwi (ttarget);
  HOST_WIDE_INT val    = tric_tree_to_hwi (tval);
  HOST_WIDE_INT pos    = tric_tree_to_hwi (tpos);
  HOST_WIDE_INT width  = tric_tree_to_hwi (twidth);
  tree finsert = tric_builtin[TRIC_BUILTIN_INSERT].fndecl;

  if (width == 0 || pos >= 32)
    /* No effect.  Return original value.  */
    return ttarget;

  if (width > 32)
    /* Saturate width to 32.  */
    return build_call_expr (finsert, 4, ttarget, tval, tpos,
                            tric_tree_uint (32));

  if (pos == 0 && width == 32)
    /* Overrides target completely.  */
    return tval;

  if (width > 0)
    {
      HOST_WIDE_INT mask = ((HOST_WIDE_INT) 1 << width) -1;

      if (pos >= 0 && pos + width > 32)
        /* If the support exceeds the target, cut down support to 32 bits.  */
        return build_call_expr (finsert, 4, ttarget, tval, tpos,
                                tric_tree_uint (32 - pos));

      if (target >= 0 && val >= 0 && pos >= 0)
        {
          /* All deflates to a known value.  */

          mask <<= pos;
          val  <<= pos;
          return tric_tree_uint ((target & ~mask) | (val & mask));
        }

      if (val == 0 && pos >= 0)
        /* Inserting 0 is cheap: Prefer open coded arithmetic.  */
        return fold_build2 (BIT_AND_EXPR, unsigned_type_node,
                            ttarget, tric_tree_uint (~(mask << pos)));
    }

  return NULL_TREE;
}


static tree
tric_fold_sat (tree arg, int lo, int hi, bool unsigned_p)
{
  tree ttyp = unsigned_p ? unsigned_type_node : integer_type_node;

  if (!unsigned_p
      || lo != 0)
    {
      arg = fold_build2 (MAX_EXPR, ttyp, arg, build_int_cst (ttyp, lo));
    }

  return fold_build2 (MIN_EXPR, ttyp, arg, build_int_cst (ttyp, hi));
}


/* Implement `TARGET_FOLD_BUILTIN'.  */

static tree
tric_fold_builtin (tree fndecl, int n_args ATTRIBUTE_UNUSED, tree *arg,
                   bool ignore ATTRIBUTE_UNUSED)
{
  unsigned int fcode = DECL_FUNCTION_CODE (fndecl);
  tree result_type = TREE_TYPE (TREE_TYPE (fndecl));
  const tric_builtin_t *builtin = &tric_builtin[fcode];

  if (builtin->tcode == NOP_EXPR)
    /* Nothing to fold */
    return NULL_TREE;
  
  switch (fcode)
    {
    default:
      break;

    case TRIC_BUILTIN_INSERT:
      return tric_fold_insert (arg[0], arg[1], arg[2], arg[3]);

    case TRIC_BUILTIN_LDMST4:
      {
        tree timask = build_call_expr (tric_builtin[TRIC_BUILTIN_IMASK].fndecl,
                                       3, arg[1], arg[2], arg[3]);
        return build_call_expr (tric_builtin[TRIC_BUILTIN_LDMST2].fndecl,
                                2, arg[0], timask);
      }

    case TRIC_BUILTIN_LDMST3:
      {
        tree timask = build_call_expr (tric_builtin[TRIC_BUILTIN_IMASK2].fndecl,
                                       2, arg[1], arg[2]);
        return build_call_expr (tric_builtin[TRIC_BUILTIN_LDMST2].fndecl,
                                2, arg[0], timask);
      }

    case TRIC_BUILTIN_RROTATE:
      /* Patch rotation offset to get a left rotate which is more
         convenient with DEXTR.  */

      arg[1] = fold_build1 (NEGATE_EXPR, unsigned_type_node, arg[1]);
      fcode = TRIC_BUILTIN_LROTATE;

      /*** FALLTHRU ***/

    case TRIC_BUILTIN_LROTATE:
      /* Patch rotation offset to make it cyclic.  */

      arg[1] = fold_build2 (BIT_AND_EXPR, unsigned_type_node, arg[1],
                            tric_tree_uint (31));
      break;

    case TRIC_BUILTIN_SATB:
      return tric_fold_sat (arg[0], -128, 127, false);

    case TRIC_BUILTIN_SATBU:
      return tric_fold_sat (arg[0], 0, 255, true);

    case TRIC_BUILTIN_SATH:
      return tric_fold_sat (arg[0], -32768, 32767, false);

    case TRIC_BUILTIN_SATHU:
      return tric_fold_sat (arg[0], 0, 65535, true);
    }

  builtin = &tric_builtin[fcode];

  /* Mandatory folds are not allowed past this point, cf builtins.def for
     a description of TCODE.  */

  gcc_assert (ERROR_MARK != builtin->tcode);

  if (CONVERT_EXPR == builtin->tcode)
    /* Nothing todo if no fold was found for these.  */
    return NULL_TREE;
  
  /* The builtin can be represented as some tree expression.  */

  fndecl = builtin->fndecl;
  result_type = TREE_TYPE (TREE_TYPE (fndecl));

  if (1 == builtin->n_args)
    return fold_build1 (builtin->tcode, result_type, arg[0]);

  if (2 == builtin->n_args)
    return fold_build2 (builtin->tcode, result_type, arg[0], arg[1]);

  gcc_unreachable();
}


/* Implement `TARGET_LIBC_HAS_FUNCTION' */

static bool
tric_libc_has_function (enum function_class fn_class ATTRIBUTE_UNUSED)
{
  // FIXME: Work this out when adding dinkum / Newlib 2.1 support.
  return true;
}

/* Implement `TARGET_INVALID_UNARY_OP' */

static const char*
tric_invalid_unary_op (int op, const_tree type)
{
  enum tree_code code = (enum tree_code) op;
  
  if (PDImode == TYPE_MODE (type)
      && !truth_value_p (code))
    {
      return "operation not supported for circular buffer types";
    }

  return NULL;
}


/* Implement `TARGET_INVALID_BINARY_OP' */

static const char*
tric_invalid_binary_op (int op, const_tree type1, const_tree type2)
{
  enum tree_code code = (enum tree_code) op;
  
  if ((PDImode == TYPE_MODE (type1)
       || PDImode == TYPE_MODE (type2))
      && !truth_value_p (code))
    {
      return "operation not supported for circular buffer types";
    }

  return NULL;
}


/* Implement `TARGET_HTC_GUESS_BSS' */

static bool
tric_guess_bss_p (void)
{
  /* FIXME: */
  return tric_test == 'B';
}


/* Helper for `tric_dump_valist':  Dump VAL as hex value to FILE.  */

static void
tric_dump_double_int_hex (FILE *file, double_int val)
{
  unsigned digit[2];

  digit[0] = tric_double_int_pop_digit (&val, 0);
  digit[1] = tric_double_int_pop_digit (&val, 0);

  fprintf (file, "0x");

  if (digit[1])
    fprintf (file, "%08x", digit[1]);

  if (digit[1] | digit[0])
    fprintf (file, "%08x", digit[0]);
  else
    fprintf (file, "0");
}


/* Helper for `tric_dump_valist' for the '%I' case.  */

static void
tric_log_neat_hwi (FILE *file, HOST_WIDE_INT wi)
{
  if (TRIC_INT_MIN == wi)
    fprintf (file, "int_min");
  else if (TRIC_UINT_MAX == wi)
    fprintf (file, "Uint_Max");
  else if (TRIC_INT_MAX == wi)
    fprintf (file, "int_Max");
  else if (wi < 0)
    fprintf (file, HOST_WIDE_INT_PRINT_DEC, wi);
  else
    fprintf (file, HOST_WIDE_INT_PRINT_HEX, wi);
}


/* Implement `TARGET_HTC_DUMP_VALIST'.

   I: The interval represented by range_t printed like "[%i, %x]".
      The empty set is printed as "[,]".
   X: double_int (unsigned hex).
*/

static int
tric_dump_valist (FILE *stream, const char *fmt, va_list ap)
{
  int n_consumed = 0;

  switch (*fmt)
    {
    default:
      break;

    case 'I':
      {
        range_t r = va_arg (ap, range_t);

        fputc ('[', stream);

        if (r.lower > r.upper)
          {
            fputc (',', stream);
          }
        else
          {
            tric_log_neat_hwi (stream, r.lower);
            fprintf (stream, ", ");
            tric_log_neat_hwi (stream, r.upper);
          }
        fputc (']', stream);
      }
      n_consumed = 1;
      break;

    case 'X':
      tric_dump_double_int_hex (stream, va_arg (ap, double_int));
      n_consumed = 1;
      break;
    }

  return n_consumed;
}


/***********************************************************************
 ** Target Hooks
 ***********************************************************************/

#undef  TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD tric_secondary_reload

#undef  TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY tric_return_in_memory

#undef  TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE tric_function_value

#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG tric_function_arg

#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE tric_function_arg_advance

#undef  TARGET_ASM_FUNCTION_END_PROLOGUE
#define TARGET_ASM_FUNCTION_END_PROLOGUE tric_asm_function_end_prologue

#undef  TARGET_ASM_FUNCTION_BEGIN_EPILOGUE
#define TARGET_ASM_FUNCTION_BEGIN_EPILOGUE tric_asm_function_begin_epilogue

#undef  TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END tric_asm_file_end

#undef  TARGET_ASM_TRAMPOLINE_TEMPLATE
#define TARGET_ASM_TRAMPOLINE_TEMPLATE tric_asm_trampoline_template

#undef  TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE tric_attribute_table

#undef  TARGET_ASM_INTEGER
#define TARGET_ASM_INTEGER default_assemble_integer

#undef  TARGET_ASM_DECLARE_CONSTANT_NAME
#define TARGET_ASM_DECLARE_CONSTANT_NAME tric_asm_declare_constant_name

#undef  TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND tric_print_operand
#undef  TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS tric_print_operand_address

#undef  TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL tric_function_ok_for_sibcall

/* Same action as PROMOTE_MODE */
#undef  TARGET_PROMOTE_FUNCTION_MODE
#define TARGET_PROMOTE_FUNCTION_MODE            \
  default_promote_function_mode_always_promote

#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE tric_pass_by_reference
#undef  TARGET_STRICT_ARGUMENT_NAMING
#define TARGET_STRICT_ARGUMENT_NAMING tric_strict_argument_naming
#undef  TARGET_CALLEE_COPIES
#define TARGET_CALLEE_COPIES tric_callee_copies

#undef  TARGET_STRUCT_VALUE_RTX
#define TARGET_STRUCT_VALUE_RTX tric_struct_value_rtx

#undef  TARGET_RTX_COSTS
#define TARGET_RTX_COSTS tric_rtx_costs
#undef  TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST tric_address_cost

#undef  TARGET_ASM_FUNCTION_RODATA_SECTION
#define TARGET_ASM_FUNCTION_RODATA_SECTION tric_function_rodata_section

#undef  TARGET_ASM_RECORD_GCC_SWITCHES
#define TARGET_ASM_RECORD_GCC_SWITCHES tric_asm_record_gcc_switches

#undef  TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM hook_bool_mode_rtx_true

#undef  TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP "\t.short\t"
#undef  TARGET_ASM_ALIGNED_SI_OP
#define TARGET_ASM_ALIGNED_SI_OP "\t.word\t"
#undef  TARGET_ASM_UNALIGNED_HI_OP
#define TARGET_ASM_UNALIGNED_HI_OP "\t.uahalf\t"
#undef  TARGET_ASM_UNALIGNED_SI_OP
#define TARGET_ASM_UNALIGNED_SI_OP "\t.uaword\t"
#undef  TARGET_ASM_UNALIGNED_DI_OP
#define TARGET_ASM_UNALIGNED_DI_OP "\t.uaxword\t"

#undef  TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE tric_attribute_table

#undef  TARGET_ASM_INIT_SECTIONS
#define TARGET_ASM_INIT_SECTIONS tric_asm_init_sections

#undef  TARGET_ASM_FUNCTION_SECTION
#define TARGET_ASM_FUNCTION_SECTION tric_asm_function_section

#undef  TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION tric_asm_select_section

#undef  TARGET_ASM_UNIQUE_SECTION
#define TARGET_ASM_UNIQUE_SECTION tric_asm_unique_section

#undef  TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION tric_asm_named_section

#undef  TARGET_SECTION_TYPE_FLAGS
#define TARGET_SECTION_TYPE_FLAGS tric_section_type_flags

#undef  TARGET_ENCODE_SECTION_INFO
#define TARGET_ENCODE_SECTION_INFO tric_encode_section_info

#undef  TARGET_MIN_ANCHOR_OFFSET
#define TARGET_MIN_ANCHOR_OFFSET (-(1 << 15))

#undef  TARGET_MAX_ANCHOR_OFFSET
#define TARGET_MAX_ANCHOR_OFFSET ((1 << 15) -1)

#undef  TARGET_ASM_OUTPUT_ANCHOR
#define TARGET_ASM_OUTPUT_ANCHOR tric_asm_output_anchor

#undef  TARGET_USE_ANCHORS_FOR_SYMBOL_P
#define TARGET_USE_ANCHORS_FOR_SYMBOL_P tric_use_anchors_for_symbol_p

#undef  TARGET_ASM_CONSTRUCTOR
#define TARGET_ASM_CONSTRUCTOR tric_asm_constructor

#undef  TARGET_ASM_DESTRUCTOR
#define TARGET_ASM_DESTRUCTOR tric_asm_destructor

#undef  TARGET_INSERT_ATTRIBUTES
#define TARGET_INSERT_ATTRIBUTES  htc_insert_htc_attributes

#undef TARGET_MERGE_DECL_ATTRIBUTES
#define TARGET_MERGE_DECL_ATTRIBUTES htc_merge_decl_attributes

#undef  TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P tric_legitimate_address_p

#undef  TARGET_LEGITIMIZE_ADDRESS
#define TARGET_LEGITIMIZE_ADDRESS tric_legitimize_address

#undef  TARGET_REGISTER_MOVE_COST
#define TARGET_REGISTER_MOVE_COST tric_register_move_cost

#undef  TARGET_MEMORY_MOVE_COST
#define TARGET_MEMORY_MOVE_COST tric_memory_move_cost

#undef  TARGET_CAN_INLINE_P
#define TARGET_CAN_INLINE_P tric_can_inline_p
#undef  TARGET_FUNCTION_ATTRIBUTE_INLINABLE_P
#define TARGET_FUNCTION_ATTRIBUTE_INLINABLE_P   \
  tric_function_attribute_inlinable_p

#undef  TARGET_STATIC_CHAIN
#define TARGET_STATIC_CHAIN tric_static_chain

#undef  TARGET_MD_ASM_CLOBBERS
#define TARGET_MD_ASM_CLOBBERS tric_md_asm_clobbers

#undef  TARGET_CONDITIONAL_REGISTER_USAGE
#define TARGET_CONDITIONAL_REGISTER_USAGE tric_conditional_register_usage

#undef  TARGET_CLASS_LIKELY_SPILLED_P
#define TARGET_CLASS_LIKELY_SPILLED_P tric_class_likely_spilled_p

#undef  TARGET_RETURN_POPS_ARGS
#define TARGET_RETURN_POPS_ARGS tric_return_pops_args

#undef  TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE tric_option_override

#undef  TARGET_SET_CURRENT_FUNCTION
#define TARGET_SET_CURRENT_FUNCTION tric_set_current_function

#undef  TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS tric_init_builtins

#undef  TARGET_BUILTIN_DECL
#define TARGET_BUILTIN_DECL tric_builtin_decl

#undef  TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN tric_expand_builtin

#undef  TARGET_FOLD_BUILTIN
#define TARGET_FOLD_BUILTIN tric_fold_builtin

#undef  TARGET_INVALID_BINARY_OP
#define TARGET_INVALID_BINARY_OP tric_invalid_binary_op

#undef  TARGET_INVALID_UNARY_OP
#define TARGET_INVALID_UNARY_OP tric_invalid_unary_op

#undef  TARGET_HTC_SECTION_FLAGS_FROM_STRING
#define TARGET_HTC_SECTION_FLAGS_FROM_STRING tric_section_flags_from_string

#undef  TARGET_HTC_RESOLVE_UNIQUE_SECTION
#define TARGET_HTC_RESOLVE_UNIQUE_SECTION htc_resolve_unique_section

#undef  TARGET_HTC_BSS_INITIALIZER_P
#define TARGET_HTC_BSS_INITIALIZER_P tric_bss_initializer_p

#undef  TARGET_HTC_SECTION_FLAGS_TO_STRING
#define TARGET_HTC_SECTION_FLAGS_TO_STRING tric_section_flags_to_string

#undef  TARGET_HTC_POST_PCH_LOAD
#define TARGET_HTC_POST_PCH_LOAD htc_post_pch_load

#undef  TARGET_HTC_GET_NAMED_SECTION
#define TARGET_HTC_GET_NAMED_SECTION htc_get_named_section

#undef  TARGET_HTC_GUESS_BSS_P
#define TARGET_HTC_GUESS_BSS_P tric_guess_bss_p

#undef  TARGET_HTC_DUMP_VALIST
#define TARGET_HTC_DUMP_VALIST tric_dump_valist

#undef  TARGET_CANONICALIZE_COMPARISON
#define TARGET_CANONICALIZE_COMPARISON tric_canonicalize_comparison

#undef  TARGET_HTC_CANONICALIZE_COMBINED_RTX
#define TARGET_HTC_CANONICALIZE_COMBINED_RTX tric_htc_canonicalize_combined_rtx

#undef  TARGET_CAN_USE_DOLOOP_P
#define TARGET_CAN_USE_DOLOOP_P tric_can_use_doloop_p

#undef  TARGET_LIBC_HAS_FUNCTION
#define TARGET_LIBC_HAS_FUNCTION tric_libc_has_function

#undef  TARGET_INVALID_PARAMETER_TYPE
#define TARGET_INVALID_PARAMETER_TYPE tric_invalid_parameter_type

#undef  TARGET_INVALID_RETURN_TYPE
#define TARGET_INVALID_RETURN_TYPE tric_invalid_return_type

#undef  TARGET_PROMOTED_TYPE
#define TARGET_PROMOTED_TYPE tric_promoted_type

#undef  TARGET_CONVERT_TO_TYPE
#define TARGET_CONVERT_TO_TYPE tric_convert_to_type

#undef  TARGET_INIT_LIBFUNCS
#define TARGET_INIT_LIBFUNCS tric_init_libfuncs

#undef  TARGET_SCALAR_MODE_SUPPORTED_P
#define TARGET_SCALAR_MODE_SUPPORTED_P tric_scalar_mode_supported_p

#undef  TARGET_MANGLE_TYPE
#define TARGET_MANGLE_TYPE tric_mangle_type

#undef  TARGET_HTC_IVOPT_BASE_COSTS_P
#define TARGET_HTC_IVOPT_BASE_COSTS_P tric_ivopt_base_costs_p

#undef  TARGET_HTC_IVOPT_USE_ADDRESS_P
#define TARGET_HTC_IVOPT_USE_ADDRESS_P tric_ivopt_use_address_p

#undef  TARGET_HTC_SCHED_MAY_CHANGE_ADDRESS_P
#define TARGET_HTC_SCHED_MAY_CHANGE_ADDRESS_P tric_sched_may_change_address_p

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-tricore.h"
