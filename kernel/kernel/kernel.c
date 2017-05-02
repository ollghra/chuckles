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

void kernel_main(void) {  
  //serial_initialise();
  serial_writes(__DATE__ "\t" __TIME__ "\n");
  
  gdt_initialise();
  terminal_initialise();
  idt_init();
  isrs_install();
  irq_install();

  timer_install();
 __asm__ __volatile__ ("sti");
 
  printf("Well, Chuckles\nChuckle away\n");
  //init_paging();
}

/*
 * Old tests:
 printf("%d", 1/0);
 timer_wait(300);printf("Timer Finished");
 */
