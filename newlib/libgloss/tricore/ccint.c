/*
 * ccint.c -- Chained C interrupt handlers for TriCore.
 *
 * Copyright (C) 1998 HighTec EDV-Systeme GmbH.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "cint.h"

/* This variable is set to 1 after the chained handlers are initialized.  */

static int _init_hnd_chain_initialized = 0;

/* This structure describes chained interrupt handlers.  */

typedef struct _Hnd_Chain
{
  struct _Hnd_Chain *hnd_next, *hnd_last;
  void (*hnd_handler) (int);
  int hnd_arg;
} *Hnd_Chain;

/* This array holds chained interrupt handlers.  */

static Hnd_Chain Hnd_chain[MAX_INTRS];

extern void __int_handler (int arg);
static void chain_hnd (int intno);

void *
_install_chained_int_handler (int intno, void (*inthandler) (int), int arg)
{
  Hnd_Chain chain, first;

  if ((intno < 0) || (intno >= MAX_INTRS) || !_init_hnd_chain_initialized)
    return NULL;

  if ((chain = (Hnd_Chain) malloc (sizeof (*chain))) == NULL)
    return NULL;

  chain->hnd_handler = inthandler;
  chain->hnd_arg = arg;

  if ((first = Hnd_chain[intno]) == NULL)
    {
      chain->hnd_next = chain->hnd_last = chain;
      Hnd_chain[intno] = chain;
    }
  else
    {
      chain->hnd_next = first;
      chain->hnd_last = first->hnd_last;
      first->hnd_last->hnd_next = chain;
      first->hnd_last = chain;
    }
  
  _install_int_handler (intno, chain_hnd, intno);

  return chain;
}

/* Helper function for _install_chained_int_handler -- calls all handlers
   installed for INTNO.  */

static void
chain_hnd (int intno)
{
  register Hnd_Chain chain, first;

  if ((chain = first = Hnd_chain[intno]) != NULL)
    do
      {
        (*chain->hnd_handler) (chain->hnd_arg);
	chain = chain->hnd_next;
      } while (chain != first);
}

/* Remove the chained handler (pointed to by ACHAIN) installed for interrupt
   INTNO be dequeuing it and freeing the queue's memory.  */

int
_remove_chained_int_handler (int intno, void *achain)
{
  register Hnd_Chain first, fel;
  Hnd_Chain chain = (Hnd_Chain) achain;

  if ((intno < 0) || (intno >= MAX_INTRS) || !_init_hnd_chain_initialized)
    return 1;  /* Wrong logic, but necessary for compatibility.  */

  if ((fel = first = Hnd_chain[intno]) == NULL)
    return 1;

  for (; (first != chain) && (fel != first->hnd_next); first = first->hnd_next)
    ;
  if (first == chain)
    {
      if (first->hnd_next == first)
        {
	  Hnd_chain[intno] = NULL;
	  _install_int_handler (intno, __int_handler, intno);
	}
      else
        {
	  first->hnd_last->hnd_next = first->hnd_next;
	  first->hnd_next->hnd_last = first->hnd_last;
	  if (Hnd_chain[intno] == first)
	    Hnd_chain[intno] = first->hnd_next;
	}
      free (chain);
    }

  return 0;
}

/* Initialize Hnd_chain.  Called automagically by __main().  */

void _init_hnd_chain (void) __attribute__ ((constructor));

void
_init_hnd_chain ()
{
  int i;

  if (_init_hnd_chain_initialized)
    return;

  for (i = 0; i < MAX_INTRS; ++i)
    Hnd_chain[i] = NULL;

  _init_hnd_chain_initialized = 1;
}
