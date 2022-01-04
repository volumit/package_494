//#include <unistd.h>
#include "vio-syscalls.h"


long
lseek (int desc, long offset, int whence)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_LSEEK) );
}
