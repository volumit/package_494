#include<stddef.h>
#include<errno.h>
#include<stdio.h>

void abort ()
{
#ifdef __PPC_VLE__
  asm volatile ("se_illegal");
#else
  asm volatile (".long 0");
#endif
  _exit(0xdeadbeef);
  while (1) ;
};

