#include <stdio.h>
#include <string.h>

#include <kernel/tty.h>

#include <arch/i386/paging.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/isr.h>
#include <arch/i386/irq.h>
#include <arch/i386/timer.h>

#include <sys/io.h>
#include <sys/debug.h>

void kernel_end();

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
  // init_paging();
  printf("JKIBJONJ\n");
  kernel_end();
}

extern unsigned long kernel_end_marker;
extern unsigned long kernel_start_marker;
void kernel_end()
{
  printf("kernel start = %x\n", &kernel_start_marker); 
  printf("kernel end Magic! = %x\n",  kernel_end_marker);
  printf("kernel end   = %x\n", &kernel_end_marker);
  printf("kernel size  = %d B, %d kB\n", &kernel_end_marker - &kernel_start_marker, (&kernel_end_marker - &kernel_start_marker)/1024);
  char a[] = {'P','o','o',0};
  char *b = "Big\0";

  printf("a = %s, b = %s\n", a, b);
  printf("strcpy(b,a) : %s, a = %s, b = %s\n", strcpy(b, a), a, b);
  serial_writes(b);
}
/*
 * Old tests:
 printf("%d", 1/0);
 timer_wait(300);printf("Timer Finished");
 */
