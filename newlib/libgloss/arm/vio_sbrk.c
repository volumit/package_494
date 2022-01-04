/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

#include <stddef.h>
#include <errno.h>
#include <stdio.h>


extern char __HEAP;
extern char __HEAP_END;

void *sbrk(size_t incr)
{
	void *ret;
	static char *heap_top   = & __HEAP;
	char *new_heap_top = heap_top + incr;
	if (new_heap_top > & __HEAP_END)
	{
		errno = ENOMEM;
		return (void *)(-1);
	}
	if (new_heap_top < & __HEAP)
		abort();
	ret = (void *) heap_top;
	heap_top = new_heap_top;

	return ret;
}
