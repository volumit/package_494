#include<stddef.h>
#include<stdlib.h>

extern int rmdir (const char *name);
extern int unlink (const char *name);
int remove(const char *path)
{
  return  (rmdir(path) < 0)?  unlink (path): 0;
}
