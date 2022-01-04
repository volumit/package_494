#include <stdio.h>
#include "vio-syscalls.h"


int
write (int desc, void *buf, size_t len)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_WRITE) );
}
