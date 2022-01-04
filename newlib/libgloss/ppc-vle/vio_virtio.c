

void __virtio_dummy_hnd(void)
{
asm volatile ("			\n\
    .ascii \"_vio\"		\n\
    .globl ___virtio_hnd	\n\
    .type ___virtio_hnd,@function	\n\
    .globl ___virtio		\n\
    .type ___virtio,@function	\n\
___virtio_hnd:			\n\
___virtio:			\n\
   ");
#ifdef __PPC_VLE__
  asm volatile ("se_illegal");
#else
  asm volatile (".long 0");
#endif
asm volatile ("			\n\
    bl __errno		\n\
    stw %r12, 0(%r3)		\n\
    mr  %r11,%r3		\n\
  ");
}
