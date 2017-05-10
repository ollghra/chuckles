#ifndef ARCH_I386_CPUID_H
#define ARCH_I386_CPUID_H

static inline int cpuid_string(int code, int where[4]) {
  __asm__ volatile ("cpuid":"=a"(*where),"=b"(*(where+0)),
               "=d"(*(where+1)),"=c"(*(where+2)):"a"(code));
  return (int)where[0];
}
 
const char * const cpu_string();

#endif