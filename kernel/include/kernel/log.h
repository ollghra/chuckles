#ifndef _KERNEL_LOG_H
#define _KERNEL_LOG_H

#include <stdint.h>

#include <arch/i386/vga.h>

typedef enum {
	KERNEL_LOGLEVEL_LOG = 0,
	KERNEL_LOGLEVEL_INFO = 1,
	KERNEL_LOGLEVEL_WARN = 2,
	KERNEL_LOGLEVEL_ERROR = 3,
	KERNEL_LOGLEVEL_NUM_LEVELS
}_KERNEL_LOG_LEVEL;

struct log_level_details{
	const char * prefix;
	uint8_t blink:1;
	uint8_t intensity:1;
	uint8_t fg:4;
	uint8_t bg:4;
};

int _klogs(_KERNEL_LOG_LEVEL level, const char * str);

int klog(const char * str);
int kinfo(const char * str);
int kwarn(const char * str);
int kerror(const char * str);

#endif
