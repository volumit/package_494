#include "vio-syscalls.h"

int
close (int desc)
{
  asm volatile ("	\n\
    li %%r12,%0	\n\
    b ___virtio_hnd	\n\
  ": : "i" (SYS_CLOSE) );
}
