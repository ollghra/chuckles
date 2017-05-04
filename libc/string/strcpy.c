#include <string.h>

#include <sys/debug.h>

char *strcpy(char *to, char *from)
{
  serial_writes("strcpy() used. Please switch to strdup once working malloc() implemented.\n");
  memmove(to, from, 1+strlen(from));
  return to;
}
