/*
 *
 * Copyright (C) 2012 HighTec EDV-Systeme GmbH.
 *
 */

#include <stddef.h>
#include <errno.h>
#include <stdio.h>

#include <sys/stat.h>

int isatty(int desc)
{
	if (desc == 0 || desc == 1 || desc == 2) return 1;
	return 0;
}

int fstat(int fd, struct stat *buf)
{
	if (fd < 0 || fd > 2)
	{
		errno = EBADF;
		return -1;
	}
	buf->st_mode = S_IFCHR;
	buf->st_blksize = S_BLKSIZE;
	return 0;
}
