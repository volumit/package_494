#include<stddef.h>
#include<errno.h>
#include<stdio.h>

#define FREQ 100000000	/* clock rate of processor in Hz */

char *__progname;


void _exit (int status)
{
#ifdef __PPC_VLE__
  asm volatile ("se_illegal");
#else
  asm volatile (".long 0");
#endif
  while (1) ;
};


