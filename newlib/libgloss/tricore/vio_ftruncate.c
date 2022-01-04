#include "vio-syscalls.h"

int
ftruncate (int fd, int size)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_FTRUNCATE) );
}
