#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <sys/io.h>

#include <kernel/tty.h>

#include <arch/i386/vga.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xC03FF000; // Virtual address of 0xB8000;
static const size_t VGA_MEM_SIZE = 2000;	// 80*25 

static uint8_t terminal_background;
static uint8_t terminal_foreground;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_colour;
static uint8_t terminal_blank_colour;
static uint16_t* terminal_buffer;

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outw(0x3D4, 0x0A);
	outw(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outw(0x3D4, 0x0B);
	outw(0x3D5, (inb(0x3E0) & 0xE0) | cursor_end);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}
void update_cursor(uint8_t x, uint8_t y)
{
	uint16_t pos = y * VGA_WIDTH + x;

	outw(0x3D4, 0x0F);
	outw(0x3D5, (uint8_t) (pos & 0xFF));
	outw(0x3D4, 0x0E);
	outw(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
void terminal_initialise(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_foreground = VGA_COLOUR_GREEN;
	terminal_background = VGA_COLOUR_BLACK;
	terminal_blank_colour = vga_entry_colour(terminal_foreground,terminal_background);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ',
					vga_attr(0,terminal_background, 1, terminal_foreground));
					//vga_entry_colour(terminal_foreground,terminal_background));
		}
	}
	enable_cursor(0, 15);
}

void terminal_setcolour(uint8_t fg, uint8_t bg) {
	terminal_foreground = fg;
	terminal_background = bg;
}

void terminal_putentryat(unsigned char c, uint8_t vga_attr, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, vga_attr);
}

void terminal_cls()
{
	int i = VGA_WIDTH * VGA_HEIGHT;
	for(; i >= 0; i--)
		terminal_buffer[i] = vga_entry(' ', terminal_blank_colour);
}

void terminal_putchar_attr(char c, uint8_t vga_attr) {
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
			terminal_putentryat(uc, vga_attr, terminal_column, terminal_row);
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
	update_cursor(terminal_column, terminal_row);
}
size_t terminal_write_attr(const char* data, size_t size, uint8_t vga_attr) {
	size_t i = 0;
	for (; i < size; i++)
		terminal_putchar_attr(data[i], vga_attr);
	return i;
}

size_t terminal_writestring_attr(const char* data, uint8_t vga_attr) {
	return terminal_write_attr(data, strlen(data), vga_attr);
}

void terminal_putchar(char c)
{
	terminal_putchar_attr(c,
			vga_attr(VGA_CTL_NOBLINK,terminal_foreground,VGA_CTL_INTHIGH,terminal_background));
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
