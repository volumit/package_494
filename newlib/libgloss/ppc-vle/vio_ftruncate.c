#include "vio-syscalls.h"

int
ftruncate (int fd, int size)
{
  asm volatile ("	\n\
    li %%r12,%0	\n\
    b ___virtio_hnd	\n\
  ": : "i" (SYS_FTRUNCATE) );
}
