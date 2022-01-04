/*
 * cint.c -- C interface for TriCore interrupts.
 *
 * Copyright (C) 1998 HighTec EDV-Systeme GmbH.
 *
 */

#include "cint.h"
#include "wdtcon.h"

/* This variable is set to 1 after the vectabs are initialized.  */

static int _init_vectab_initialized = 0;

/* This structure describes interrupt handlers and their arguments.  */

typedef struct _Hnd_arg
{
  void (*hnd_handler) (int);
  int hnd_arg;
} Hnd_arg;

/* This array holds the functions to be called when a trap occurs. */

void (*Tdisptab[MAX_TRAPS]) (int tin);

/* This array holds the functions to be called when an interrupt occurs.  */

Hnd_arg Cdisptab[MAX_INTRS];

void __int_handler (int arg);


/* Install TRAPHANDLER for trap TRAPNO.  */

int
_install_trap_handler (int trapno, void (*traphandler) (int))
{
  if ((trapno < 0) || (trapno >= MAX_TRAPS) || !_init_vectab_initialized)
    return 0;

  Tdisptab[trapno] = traphandler;

  return 1;
}

/*
   for use in the tricore simulator call _exit with a special exit code in
   A14 0xdead<trapnumber><tin>
*/
inline void tsim_abort(int trap, int tin)
{
  unsigned int code;

  code = 0xdead0000 + ((trap & 0xff) << 8) + (tin & 0xff);
  __asm__ volatile ("mov.a %%a14, %0;j _exit":: "d" (code): "a14");
}

/* The default handlers for class 0-7 traps; if the CPU is programmed to
   honor debug instructions, program control will return to the debugger
   and you can see which trap was triggered.  */

/* Reset  */

static void
__class_0_trap_handler (int tin)
{
  asm volatile ("debug"); /* RESET  */
  tsim_abort(0,0);
}

/* Internal Protection traps  */

static void
__class_1_trap_handler (int tin)
{
#if 0
  switch (tin)
    {
    case 1:
      asm volatile ("debug"); /* PRIV -- Privileged Instruction  */
      break;

    case 2:
      asm volatile ("debug"); /* MPR -- MemProt: Read Access  */
      break;

    case 3:
      asm volatile ("debug"); /* MPW -- MemProt: Write Access  */
      break;

    case 4:
      asm volatile ("debug"); /* MPX -- MemProt: Execution Access  */
      break;

    case 5:
      asm volatile ("debug"); /* MPP -- MemProt: Peripheral Access  */
      break;

    case 6:
      asm volatile ("debug"); /* MPN -- MemProt: Null Address  */
      break;

    case 7:
      asm volatile ("debug"); /* GRPW -- Global Register Write Prot  */
      break;
    }
#else
  asm volatile ("debug");
  tsim_abort(1,tin);
#endif
}

/* Instruction Errors  */

static void
__class_2_trap_handler (int tin)
{
#if 0
  switch (tin)
    {
    case 1:
      asm volatile ("debug"); /* IOPC -- Illegal Opcode  */
      break;

    case 2:
      asm volatile ("debug"); /* UOPC -- Unimplemented Opcode  */
      break;

    case 3:
      asm volatile ("debug"); /* OPD -- Invalid Operand Specification  */
      break;

    case 4:
      asm volatile ("debug"); /* ALN -- Data Address Alignment Error  */
      break;

    case 5:
      asm volatile ("debug"); /* MEM -- Invalid Local Memory Address  */
      break;
    }
#else
  asm volatile ("debug");
  tsim_abort(2,tin);
#endif
}

/* Context Management  */

static void
__class_3_trap_handler (int tin)
{
#if 0
  switch (tin)
    {
    case 1:
      asm volatile ("debug"); /* FCD -- Free Context List Depleted  */
      break;

    case 2:
      asm volatile ("debug"); /* CDO -- Call Depth Overflow  */
      break;

    case 3:
      asm volatile ("debug"); /* CDU -- Call Depth Underflow  */
      break;

    case 4:
      asm volatile ("debug"); /* FCU -- Free Context List Underflow  */
      break;

    case 5:
      asm volatile ("debug"); /* CSU -- Context List Underflow  */
      break;

    case 6:
      asm volatile ("debug"); /* CTYP -- Context Type Error  */
      break;

    case 7:
      asm volatile ("debug"); /* NEST -- Nesting Error (RFE)  */
      break;
    }
#else
  asm volatile ("debug");
  tsim_abort(3,tin);
#endif
}

/* System Bus and Peripheral Errors  */

static void
__class_4_trap_handler (int tin)
{
#if 0
  switch (tin)
    {
    case 1:
      asm volatile ("debug"); /* PRVP -- PrivViol on Peripheral Access  */
      break;

    case 2:
      asm volatile ("debug"); /* BUS -- Bus Error  */
      break;

    case 3:
      asm volatile ("debug"); /* PARI -- Parity/CRC Error  */
      break;

    case 4:
      asm volatile ("debug"); /* BLTO -- Bus Lock Time-Out  */
      break;

    case 5:
      asm volatile ("debug"); /* PKEY -- KeyViol for ProtPeriph (src)  */
      break;
    }
#else
  asm volatile ("debug");
  tsim_abort(4,tin);
#endif
}

/* Assertion Traps  */

static void
__class_5_trap_handler (int tin)
{
#if 0
  switch (tin)
    {
    case 1:
      asm volatile ("debug"); /* OVF -- Arithmetic Overflow  */
      break;

    case 2:
      asm volatile ("debug"); /* SOVF -- Sticky Arithmetic Overflow  */
      break;
    }
#else
  asm volatile ("debug");
  tsim_abort(5,tin);
#endif
}

/* System Call #tin  */

static void
__class_6_trap_handler (int tin)
{
  asm volatile ("debug"); /* System Call #tin  */
  tsim_abort(6,tin);
}

/* Non-maskable Interrupt  */

static void
__class_7_trap_handler (int tin)
{
  asm volatile ("debug"); /* NMI -- Non-maskable Interrupt  */
  tsim_abort(7,tin);
}


/* The default handler for interrupts.  */

void
__int_handler (int arg)
{
  /* Just ignore this interrupt.  */
}

/* Install INTHANDLER for interrupt INTNO and remember ARG for later use.  */

int
_install_int_handler (int intno, void (*inthandler)(int), int arg)
{
  if ((intno < 0) || (intno >= MAX_INTRS) || !_init_vectab_initialized)
    return 0;

  Cdisptab[intno].hnd_handler = inthandler;
  Cdisptab[intno].hnd_arg = arg;

  return 1;
}

/* This initializes the C interrupt interface.  We declare this function
   as a "constructor" so that it is automagically called by __main() if
   a program either uses c(c)inthandler or defines its own versions
   of __interrupt_1 or __trap_6 which reference a global symbol defined
   in this file.  */

void _init_vectab (void) __attribute__ ((constructor));

void
_init_vectab ()
{
  register unsigned int vptr;
  int vecno;
  extern void TriCore_trap_table(void);
  extern void TriCore_int_table(void);

  if (_init_vectab_initialized)
    return;

  /* Set BTV and BIV registers.  */
  unlock_wdtcon ();
  vptr = (unsigned int)TriCore_trap_table;
  asm volatile ("mtcr $btv,%0" : : "d" (vptr));
  vptr = (unsigned int)TriCore_int_table;
  asm volatile ("mtcr $biv,%0" : : "d" (vptr));
  lock_wdtcon ();

  /* Initialize the trap handlers.  */
  Tdisptab[0] = __class_0_trap_handler;
  Tdisptab[1] = __class_1_trap_handler;
  Tdisptab[2] = __class_2_trap_handler;
  Tdisptab[3] = __class_3_trap_handler;
  Tdisptab[4] = __class_4_trap_handler;
  Tdisptab[5] = __class_5_trap_handler;
  Tdisptab[6] = __class_6_trap_handler;
  Tdisptab[7] = __class_7_trap_handler;

  /* Initialize the interrupt handlers.  */
  for (vecno = 0; vecno < MAX_INTRS; ++vecno)
    {
      Cdisptab[vecno].hnd_handler = __int_handler;
      Cdisptab[vecno].hnd_arg = vecno;
    }

  _init_vectab_initialized = 1;
}

