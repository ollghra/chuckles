#ifndef _SYS_DEBUG_H
#define _SYS_DEBUG_H

#include <stdlib.h>

#include <sys/io.h>

inline void
serial_writec(char c)
{
#define PORT 0x3f8    // COM1
  while(inb(PORT + 5) & 0x20 == 0);  
  outb(PORT, c);
}

inline void
serial_writes(char *s)
{
  for(;*s != '\0';s++)
    serial_writec(*s);
}

inline void
serial_writed(int i)
{
  serial_writes(itoa(i));
}
#endif