#ifndef _SYS_KB_H
#define _SYS_KB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

struct key_event {
  uint8_t utf8;
  //bool down;
  uint8_t flags; // LSB[special, control, shift, alt, 0,0,0, downMSB]
};

#define KB_FLAG_SPECIAL (1<<0)
#define KB_FLAG_CONTROL (1<<1)
#define KB_FLAG_SHIFT   (1<<2)
#define KB_FLAG_ALT     (1<<3)
#define KB_FLAG_DOWN    (1<<7)

#ifdef __cplusplus
}
#endif

#endif
