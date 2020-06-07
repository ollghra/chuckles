#ifndef _SYS_DEBUG_H
#define _SYS_DEBUG_H

#include <stdlib.h>
#include <stdarg.h>

#include <sys/io.h>

void serial_initialise(void);

int serial_printf(const char* restrict format, ...);

static inline char serial_writec(char c)
{
#define PORT 0x3f8    // COM1
  while((inb(PORT + 5) & 0x20) == 0);  
  outb(PORT, c);
  return c;
}

static inline void
serial_writes(char *s)
{
  for(;*s != '\0';s++)
    serial_writec(*s);
}

static inline void
serial_writed(int i)
{
  serial_writes(itoa(i));
}
#endif
