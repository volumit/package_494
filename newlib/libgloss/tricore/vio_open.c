#include "vio-syscalls.h"


int
open (char *path, int flags, int mode)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_OPEN) );
}
