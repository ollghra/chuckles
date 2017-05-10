#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <arch/i386/idt.h>

#include <sys/debug.h>

struct IDT_Gate {
  uint16_t offset_lo;
  uint16_t selector;
  uint8_t ZERO;
  uint8_t type_attr;
  uint16_t offset_hi;
} __attribute__((packed));

struct IDT_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

struct IDT_Gate IDT[256];
struct IDT_ptr IDTp;

extern void IDT_load();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    IDT[num].offset_lo = (base & 0xFFFF);
    IDT[num].offset_hi = (base >> 16) & 0xFFFF;

    IDT[num].selector = sel;
    IDT[num].ZERO = 0;
    IDT[num].type_attr = flags;
}


void idt_init()
{
  IDTp.limit = (sizeof (struct IDT_Gate) * 256) - 1;
  IDTp.base = (uint32_t) &(IDT[0]);
  memset(&IDT, 0, sizeof (struct IDT_Gate) * 256);

  serial_writes("&IDT: ");
  serial_writed((uint32_t) &IDT[0]);
  serial_writec('\n');
  
  IDT_load();
}
