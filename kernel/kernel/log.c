#include <stdint.h>

#include <kernel/log.h>
#include <kernel/tty.h>

#include <arch/i386/vga.h>

const struct log_level_details lldetarr [KERNEL_LOGLEVEL_NUM_LEVELS] = 
{
	{"LOG"  , VGA_CTL_NOBLINK, VGA_CTL_INTHIGH, VGA_COLOUR_BLACK, VGA_COLOUR_BLACK}, /* log */
	{"INFO" , VGA_CTL_NOBLINK, VGA_CTL_INTHIGH, VGA_COLOUR_BLACK, VGA_COLOUR_BLACK}, /* info */
	{"WARN" , VGA_CTL_NOBLINK, VGA_CTL_INTHIGH, VGA_COLOUR_BLACK, VGA_COLOUR_BLACK}, /* warn */
	{"ERROR", VGA_CTL_NOBLINK, VGA_CTL_INTHIGH, VGA_COLOUR_BLACK, VGA_COLOUR_BLACK} /* error */
};

/* Kernel logging */
int _klogs(_KERNEL_LOG_LEVEL level, const char * str)
{
	return terminal_writestring_attr(
			str,
			vga_attr(
				lldetarr[level].blink,
				lldetarr[level].bg,
				lldetarr[level].intensity,
				lldetarr[level].fg
				)
			);
}

int klog(const char * str)   { return _klogs(KERNEL_LOGLEVEL_LOG  , str); }
int kinfo(const char * str)  { return _klogs(KERNEL_LOGLEVEL_INFO , str); }
int kwarn(const char * str)  { return _klogs(KERNEL_LOGLEVEL_WARN , str); }
int kerror(const char * str) { return _klogs(KERNEL_LOGLEVEL_ERROR, str); }
