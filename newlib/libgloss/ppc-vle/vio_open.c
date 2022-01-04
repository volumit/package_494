#include "vio-syscalls.h"


int
open (char *path, int flags, int mode)
{
  asm volatile ("	\n\
    li %%r12,%0	\n\
    b ___virtio_hnd	\n\
  ": : "i" (SYS_OPEN) );
}
