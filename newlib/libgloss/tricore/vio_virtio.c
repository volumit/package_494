

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
    .code16 			\n\
    debug			\n\
    call __errno		\n\
    st.w [%a2],%d12		\n\
    mov %d2,%d11		\n\
  ");
}
