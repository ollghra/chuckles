#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <arch/i386/ps2.h>
#include <arch/i386/irq.h>

#include <sys/io.h>
#include <sys/debug.h>

#define LCTL 0x10
#define LSHF 0x11
#define LALT 0x12
#define RCTL LCTL
#define RSHF 0x14 
#define RALT LALT

struct {
  bool lshift;
  bool rshift;
  bool lctrl;
  bool rctrl;
  bool lalt;
  bool ralt;
} ps2kb_state;

struct key_event {
  uint8_t utf8;
  bool down;
  uint8_t flags; // LSB[special, control, shift, alt, 0,0,0,0MSB]
};

char *kb_map;

char gb[] = { // GB keyboard mapping
  /*
  0   1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  */
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,'\t', 0  , 0  , // 0
  0  ,LALT,LSHF, 0  ,LCTL, 'q', '1', 0  , 0  , 0  , 'z', 's', 'a', 'w', '2', 0  , // 1
  0  , 'c', 'x', 'd', 'e', '4', '3', 0  , 0  , ' ', 'v', 'f', 't', 'r', '5', 0  , // 2
  0  , 'n', 'b', 'h', 'g', 'y', '6', 0  , 0  , 0  , 'm', 'j', 'u', '7', '8', 0  , // 3
  0  , ',', 'k', 'i', 'o', '0', '9', 0  , 0  , '.', '/', 'l', ';', 'p', '-', 0  , // 4
  0  , 0  ,'\'', 0  , '[', '=', 0  , 0  , 0  ,RSHF,'\n', ']', 0  , '#', 0  , 0  , // 5
  0  ,'\\', 0  , 0  , 0  , 0  ,'\b', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 6
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 7
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 8
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 9
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // A
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // B
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // C
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // D
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // E
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0    // F
};

void ps2kb_handler(struct regs *r)
{
  uint8_t scancode = 0;
  scancode = ps2_read();

  struct key_event e;
  e.flags = 1; // Set special bit by default
  e.down = false;
  bool state;
  if(scancode == 0xF0)
    {
      state = false;
      scancode = ps2_read();
    }
  else if(scancode == 0xE0)
    {
      scancode = ps2_read();
      if(scancode == 0xF0)
	{
	  state = false;
	  scancode = ps2_read();
	}
      else
	{
	  state = true;
	}
      switch(kb_map[scancode])
	{
	case RCTL:
	  ps2kb_state.rctrl = state;
	  break;
	case RALT:
	  ps2kb_state.ralt  = state;
	  break;
	default:
	  printf("Unhandled E0 scancode %x", scancode);
	  break;
	}
    }
  else if(scancode == 0)
    {
      printf("WHY 0?");
    }
  else
    {
      state = true;
    }
  switch(kb_map[scancode])
    {
    case 0:
      printf("<%x:%d>", scancode,__LINE__);
    case LSHF:
      ps2kb_state.lshift = state;
      break;
    case LCTL:
      ps2kb_state.lctrl  = state;
      break;
    case LALT:
      ps2kb_state.lalt   = state;
      break;
    case RSHF:
      ps2kb_state.rshift = state;
      break;
    default:
      e.utf8 = kb_map[scancode];
      e.flags = 0 + // Special bit unset, i.e. character
	(((int)(ps2kb_state.lctrl || ps2kb_state.rctrl)) << 1) +
	(((int)(ps2kb_state.lshift || ps2kb_state.rshift)) << 2) +
	(((int)(ps2kb_state.lalt || ps2kb_state.ralt )) << 3);
      e.down = state;
      break;
    }
  
  if(!(e.flags & 1) && e.down)
    printf("%c F:[%c,%c,%c]", e.utf8,
	   (e.flags & (1<<1)) == (1<<1)? 'C':'0',
	   (e.flags & (1<<2)) == (1<<2) ? 'S': '0',
	   (e.flags & (1<<3)) == (1<<3) ? 'A': '0');

  // Fire key_event ==> >=3 }-->
  
  serial_writes("KB:");
  serial_writed(e.utf8);
  serial_writes("\n");
  
}

void ps2kb_init(void)
{
  irq_install_handler(1, ps2kb_handler);

  ps2kb_state.lshift = false;
  ps2kb_state.rshift = false;
  ps2kb_state.lctrl  = false;
  ps2kb_state.rctrl  = false;
  ps2kb_state.lalt   = false;
  ps2kb_state.ralt   = false;

  kb_map = gb;
  printf("Keyboard Installed\n");
  printf("WARNING! THIS REALLY DOESN't WORK");
  serial_writes("Keyboard install\n");
}
