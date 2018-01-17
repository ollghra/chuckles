#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <sys/io.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static const size_t VGA_MEM_SIZE = 2000;	// 80*25 

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

/* This doesn't work 

   void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
   {
   outw(0x0A, 0x3D4);
   outw((inb(0x3D5) & 0xC0) | cursor_start, 0x3D5);

   outw(0x0B, 0x3D4);
   outw((inb(0x3E0) & 0xE0) | cursor_end, 0x3D5);
   }

   void disable_cursor()
   {
   outb(0x0A, 0x3D4);
   outb(0x20, 0x3D5);
   }

   void update_cursor(int x, int y)
   {
   uint16_t pos = y * VGA_WIDTH + x;

   outw(0x0F, 0x3D4);
   outw((uint8_t) (pos & 0xFF), 0x3D5);
   outw(0x0E, 0x3D4);
   outw((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
   }
   */
void terminal_initialise(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	//enable_cursor(0, 15);
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_cls()
{
	int i = VGA_WIDTH * VGA_HEIGHT;
	for(; i >= 0; i--)
		terminal_buffer[i] = vga_entry(' ', 
				vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_BLACK));
}

void terminal_putchar(char c) {
	unsigned char uc = c;
	switch(uc)
	{
		case '\n':
			terminal_column = 0;
			if(terminal_row + 1 == VGA_HEIGHT)
			{	
				terminal_row = 0;
				terminal_cls();
			} 
			else
				terminal_row++;
			break;
		default:
			terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
			terminal_column++;

			if (terminal_column == VGA_WIDTH) {
				terminal_column = 0;
				terminal_row++;

				if (terminal_row == VGA_HEIGHT)
				{
					terminal_row = 0;
					terminal_cls();
				}
			}
	}
	//update_cursor(terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
