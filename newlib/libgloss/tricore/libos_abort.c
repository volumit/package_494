#include<stddef.h>
#include<errno.h>
#include<stdio.h>

void abort ()
{
  asm volatile ("debug");
  __asm__ volatile ("mov.a %%a14, %0;j _exit":: "d" (0xdeadbeef));
  while (1) ;
};

