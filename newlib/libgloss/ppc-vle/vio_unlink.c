#include "vio-syscalls.h"


int
unlink (const char *name)
{
  asm volatile ("	\n\
    li %%r12,%0	\n\
    b ___virtio_hnd	\n\
  ": : "i" (SYS_UNLINK) );
}
