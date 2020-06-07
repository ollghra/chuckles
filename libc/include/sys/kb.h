#ifndef _SYS_KB_H
#define _SYS_KB_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    KEY_NONE         =0x00,

    KEY_UP_ARROW     =0x75,
    KEY_DOWN_ARROW   =0x72,
    KEY_LEFT_ARROW   =0x6B,
    KEY_RIGHT_ARROW  =0x74,

    KEY_PAGE_UP      =0x7D,
    KEY_PAGE_DOWN    =0x7A,
} KEYCODE;

struct key_event {
  uint8_t utf8; // Printable representation, 0 for special keys
  KEYCODE code; // keycode as defined below
  uint8_t flags; // LSB[special, control, shift, alt, 0,0,0, downMSB]
};

#define KB_FLAG_SPECIAL (1<<0)
#define KB_FLAG_CONTROL (1<<1)
#define KB_FLAG_SHIFT   (1<<2)
#define KB_FLAG_ALT     (1<<3)
#define KB_FLAG_MOD     (1<<4)
#define KB_FLAG_DOWN    (1<<7)

#endif
