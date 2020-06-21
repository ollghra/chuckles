#ifndef _LIBC_MEMORY_H
#define _LIBC_MEMORY_H

void * physical(void * virtual);
void * virtual_kaddr(void * physical);

#endif
