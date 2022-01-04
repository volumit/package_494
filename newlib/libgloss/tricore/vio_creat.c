#include "vio-syscalls.h"


int
creat (char *path, int mode)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_CREAT) );
}
