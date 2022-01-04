#include "vio-syscalls.h"

int
close (int desc)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_CLOSE) );
}
