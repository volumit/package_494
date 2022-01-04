#include<stddef.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>
#include<sys/stat.h>

#define FREQ 100000000	/* clock rate of processor in Hz */

int __attribute__ ((weak)) _my_errno;
__attribute__ ((weak)) int *
__errno ()
{
    return &_my_errno;
}

/* define an empty environment */
__attribute__ ((weak)) char *__my_environ[2] = {"",0};
__attribute__ ((weak)) char **environ = __my_environ;

/*
 * implementation of not supported system calls
 */

int getpid ()
{
	return 0;
};


int isatty (int desc)
{
	if (desc==0 || desc==1 || desc==2) return 1;
	return 0;
};

int fork ()
{
	abort();
};


int wait (int *status)
{
	abort();
};


int kill (int pid, int sig)
{
	abort();
};


int link (const char *old, const char *new)
{
	abort();
};


#if 1
int stat (const char *file, struct stat *buf)
{
	abort();
};


int fstat (int fd, struct stat *buf)
{
	if (fd < 0 || fd > 2)
	{
		errno = EBADF;
		return -1;
	}
	buf->st_mode = S_IFCHR;
	buf->st_blksize = S_BLKSIZE;
	return 0;
};



int gettimeofday (struct timeval *tv, void *tz)
{
	abort();
};
#endif

#if 0
#include "dirent.h"

int getdents(unsigned int fd, struct dirent *dirp, unsigned int count)
{
	abort();
}
#endif
