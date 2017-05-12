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

#ifdef __cplusplus
}
#endif

#endif