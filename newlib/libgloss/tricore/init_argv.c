#include <stdio.h>
#include <ctype.h>


#pragma section .rodata.argbuf a 4
/* the debugger will save the argument string into the buffer __init_arg_buf */
#define __INIT_ARG_BUF_SIZE  256
char __init_arg_buf[__INIT_ARG_BUF_SIZE] = {0,'a','b'};
char *__init_arg_buf_len = __INIT_ARG_BUF_SIZE;
#pragma section
/* 
 * int _init_args(char *args)
 * scan the string in __init_arg_buf for argument strings seperated by white 
 * space characters
 * create the argv vector with adresses to the arguments on the stack
 * the modified stack pointer is returned in register a2
 * the number of found argumnets is returnd in d2
 *
 */
int
_init_args(char **stackp)
{
	int n = 0;
	char *cp;
	char **p;

	if (__init_arg_buf[0] == 0) {
/*
		delete for compatibility with 3.4.2.x
		stackp -= 2;
		*stackp = 0;
*/
		asm ("mov.aa %%a2,%0"::"a" (stackp) : "a2");
		return 0;
	}
	/* count arguments */
	cp = __init_arg_buf;
	while (*cp && isspace(*cp)) cp++;
	if (*cp) n++;
	for (; *cp; cp++) {
		if (isspace(*cp)) {
			while (*cp && isspace(*cp)) cp++;
			if (*cp) n++;
		}
	}


	stackp -= (n & 1)? n+1:n+2;

	p = stackp;
	
	cp = __init_arg_buf;
	while (*cp && isspace(*cp)) cp++;
	*p++ = cp;
	for (; *cp; cp++) {
		if (isspace(*cp)) {
			*cp++ = 0;
			while (*cp && isspace(*cp)) cp++;
			if (*cp) *p++ = cp;
		}
	}
	*p = 0;
	asm ("mov.aa %%a2, %0" : : "a" (stackp): "a2");
	return n;
}


