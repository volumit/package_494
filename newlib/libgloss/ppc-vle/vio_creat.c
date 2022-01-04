#include "vio-syscalls.h"


int
creat (char *path, int mode)
{
  asm volatile ("	\n\
    li %%r12,%0	\n\
    b ___virtio_hnd	\n\
  ": : "i" (SYS_CREAT) );
}
