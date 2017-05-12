#ifndef _SYS_SHELL_H
#define _SYS_SHELL_H
#define __chuckles_libc 1

#include <sys/kb.h>

int shell_input_key_event(struct key_event e);
int shell_key(struct key_event e);

#endif