#include "vio-syscalls.h"


int
unlink (const char *name)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_UNLINK) );
}
