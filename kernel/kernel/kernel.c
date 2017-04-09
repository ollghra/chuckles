#include <stdio.h>

#include <kernel/tty.h>

#include <arch/i386/paging.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/isr.h>
#include <arch/i386/irq.h>

void kernel_main(void) {
  terminal_initialise();
  gdt_initialise();
  idt_init();
  isrs_install();
  irq_install();
  //  __asm__ __volatile__ ("sti");
  printf("Well, Chuckles\nChuckle away");
  //init_paging();
}
