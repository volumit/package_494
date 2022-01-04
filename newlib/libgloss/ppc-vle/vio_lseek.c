#include <stdlib.h>
#include "vio-syscalls.h"


long
lseek (int desc, long offset, int whence)
{
  asm volatile ("	\n\
    li %%r12,%0	\n\
    b ___virtio_hnd	\n\
  ": : "i" (SYS_LSEEK) );
}
