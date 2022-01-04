#include <stdio.h>
#include "vio-syscalls.h"


int
write (int desc, void *buf, size_t len)
{
  asm volatile ("	\n\
    li %%r12,%0	\n\
    b ___virtio_hnd	\n\
  ": : "i" (SYS_WRITE) );
}
