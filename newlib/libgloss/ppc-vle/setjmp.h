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

#define _JBLEN 32
#define _JBTYPE unsigned long

typedef	_JBTYPE jmp_buf[_JBLEN];

extern void	longjmp(jmp_buf __jmpb, int __retval);
extern int	setjmp(jmp_buf __jmpb);

_END_STD_C

#endif /* _SETJMP_H_ */

