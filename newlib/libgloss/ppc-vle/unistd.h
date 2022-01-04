#ifndef _UNISTD_H
#define _UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#define __need_size_t
#define __need_ptrdiff_t
#include <stddef.h>

typedef long off_t;


void	_exit (int __status ) __attribute__ ((noreturn));
int	access(const char *__path, int __amode );
unsigned  alarm (unsigned __secs );
int     chdir (const char *__path );
int     chmod (const char *__path, mode_t __mode );
int     close (int __fildes );
int     dup (int __fildes );
int     dup2 (int __fildes, int __fildes2 );
int     isatty (int __fildes );
int     link (const char *__path1, const char *__path2 );
off_t   lseek (int __fildes, off_t __offset, int __whence );
int     rmdir (const char *__path );
void *  sbrk  (ptrdiff_t __incr);
int     unlink (const char *__path );
size_t  write (int __fd, const void *__buf, size_t __nbyte );
size_t  read (int __fd, void *__buf, size_t __nbyte );
int     truncate (const char *, off_t __length);

#define	F_OK	0
#define	R_OK	4
#define	W_OK	2
#define	X_OK	1

# define	SEEK_SET	0
# define	SEEK_CUR	1
# define	SEEK_END	2

#define STDIN_FILENO    0       /* standard input file descriptor */
#define STDOUT_FILENO   1       /* standard output file descriptor */
#define STDERR_FILENO   2       /* standard error file descriptor */

#ifdef __cplusplus
}
#endif
#endif /* _SYS_UNISTD_H */
