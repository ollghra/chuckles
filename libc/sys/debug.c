#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <sys/io.h>
#include <sys/debug.h>

void serial_initialise(void)
{
#define PORT 0x3f8    // COM1
  outb(PORT + 1, 0x00);    // Disable all interrupts
  outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  outb(PORT + 1, 0x00);    //                  (hi byte)
  outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
  outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
  outb(PORT + 4, 0); 
}
int debug_printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);
	int written = printf("DEBUG: ");
	written += printf(format);
	printf("\n");
	va_end(parameters);
	return written;
}
