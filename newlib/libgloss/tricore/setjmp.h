/*
	setjmp.h
	stubs for future use.
*/

#ifndef _SETJMP_H_
#define _SETJMP_H_

#ifdef __cplusplus
#if !(defined(_BEGIN_STD_C) && defined(_END_STD_C))
#ifdef _HAVE_STD_CXX
#define _BEGIN_STD_C namespace std { extern "C" {
#define _END_STD_C  } }
#else
#define _BEGIN_STD_C extern "C" {
#define _END_STD_C  }
#endif
#endif
#else
#define _BEGIN_STD_C
#define _END_STD_C
#endif

_BEGIN_STD_C

typedef struct _jmp_buf { unsigned long return_address; unsigned long upper_ctx[16]; } jmp_buf[1];

extern void	longjmp(jmp_buf __jmpb, int __retval);
extern int	setjmp(jmp_buf __jmpb);

_END_STD_C

#endif /* _SETJMP_H_ */

