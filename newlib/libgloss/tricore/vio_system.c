#include<stddef.h>
#include<stdlib.h>

int system(const char *cmd)
{
  return (cmd == NULL)? 0: -1;
}
