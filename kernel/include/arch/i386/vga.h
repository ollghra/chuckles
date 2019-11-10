#ifndef ARCH_I386_VGA_H
#define ARCH_I386_VGA_H

#include <stdint.h>

enum vga_colour {
	VGA_COLOUR_BLACK = 0,
	VGA_COLOUR_BLUE = 1,
	VGA_COLOUR_GREEN = 2,
	VGA_COLOUR_CYAN = 3,
	VGA_COLOUR_RED = 4,
	VGA_COLOUR_MAGENTA = 5,
	VGA_COLOUR_BROWN = 6,
	VGA_COLOUR_LIGHT_GREY = 7,
	VGA_COLOUR_DARK_GREY = 8,
	VGA_COLOUR_LIGHT_BLUE = 9,
	VGA_COLOUR_LIGHT_GREEN = 10,
	VGA_COLOUR_LIGHT_CYAN = 11,
	VGA_COLOUR_LIGHT_RED = 12,
	VGA_COLOUR_LIGHT_MAGENTA = 13,
	VGA_COLOUR_LIGHT_BROWN = 14,
	VGA_COLOUR_WHITE = 15,
};

#define VGA_CTL_NOBLINK 0
#define VGA_CTL_BLINK   1

#define VGA_CTL_INTHIGH 1
#define VGA_CTL_INTLOW  0

#define vga_attr(blink,bg,intensity,fg) ((blink<<7)|(bg<<4)|(intensity<<3)|(fg))
#define vga_word(c,blink,bg,intensity,fg) ((c)|(blink<<15)|(bg<<12)|(intensity<<11)|(fg<<8))

static inline uint8_t vga_entry_colour(enum vga_colour fg, enum vga_colour bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t colour) {
	return (uint16_t) uc | (uint16_t) colour << 8;
}

#endif
