#include <stdio.h>

#include <kernel/tty.h>

#include <arch/i386/paging.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/isr.h>
#include <arch/i386/irq.h>
#include <arch/i386/timer.h>

#include <sys/io.h>
#include <sys/debug.h>

void serial_initialise()
{
#define PORT 0x3f8    // COM1
  outb(PORT + 1, 0x00);    // Disable all interrupts
  outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  outb(PORT + 1, 0x00);    //                  (hi byte)
  outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
  outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
  //outb(PORT + 4, 0);
  
}

void kernel_main(void) {  
  //serial_initialise();
  serial_writes(__DATE__ "\t" __TIME__ "\n");
  
  gdt_initialise();
  terminal_initialise();
  idt_init();
  isrs_install();
  irq_install();
 
  //timer_install();
  __asm__ __volatile__ ("sti");
  printf("%d", 1/0);
  printf("Well, Chuckles\nChuckle away\n");
  //timer_wait(5);
  printf("Timer Finished");
  //init_paging();
}
