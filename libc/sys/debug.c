#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

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

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (serial_writec(bytes[i]) == EOF)
			return false;
	return true;
}

int serial_printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(str, len))
				return -1;
			written += len;
		} else if (*format == 'd') {
			format++;
			int d = va_arg(parameters, int);
			if (!print(itoa(d), strlen(itoa(d))))
				return -1;
			written += strlen(itoa(d));
		} else if (*format == 'x') {
			format++;
#define INT_DIGITS 16
			char buf[INT_DIGITS + 1 /* \0 */];
			int x = va_arg(parameters, int);
			int i = 0;
			char hex[16] = {'0','1','2','3','4','5','6','7'
				,'8','9','A','B','C','D','E','F'};
			//      buf[0] = 0;
			if(x > 0)
			{
				for(i = 0; x > 0; x /= 0x10, i++)
				{
					buf[i] = hex[x%16];
				}
			}
			else if ( x < 0 )
			{
				//print("NEG", 3);
				for(i = 0, x++; x < 0; x /= 0x10, i++)
				{
					buf[i] = hex[(x % 0x10) + 15];
				}
			}
			else if ( x == 0 )
			{
				buf[0] = '0';
				i = 1;
			}
			buf[i] = 0;
			strrev(buf);
			if(!print(buf, i))
				return -1;
			written += i;
		}
		else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}
