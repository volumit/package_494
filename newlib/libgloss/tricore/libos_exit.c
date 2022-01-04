#include<stddef.h>
#include<errno.h>
#include<stdio.h>

#define FREQ 100000000	/* clock rate of processor in Hz */

char *__progname;


void _exit (int status)
{
  if (status)
    __asm__ volatile ("mov.a %%a14, %0" :: "d" (status));
  else
    __asm__ volatile ("mov.a %%a14, %0" :: "d" (0x900d));

  asm volatile ("debug");
  while (1) ;
};


