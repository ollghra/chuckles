#include <string.h>

#include <sys/debug.h>

/* Reverse str in place */
char *strrev(char *str)
{
  char *p1, *p2;

  if (! str || ! *str)
    {
      serial_writes("!str || !*str");
      return str;
    }
  serial_writes("12\n");
  for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
      serial_writec(*p1);
      serial_writec(*p2);
      serial_writec('\n');
      *p1 ^= *p2;
      *p2 ^= *p1;
      *p1 ^= *p2;
    }
  return str;
}
