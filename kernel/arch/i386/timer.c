#include <stdio.h>
#include <stdint.h>

#include <arch/i386/irq.h>
#include <arch/i386/timer.h>
#include <arch/i386/isr.h>

#include <sys/io.h>
#include <sys/debug.h>

uint32_t timer_ticks = 0;
int seconds = 0;

extern 
/* Handles the timer. By default, the timer fires 18.222 times
 *  per second. Why 18.222Hz? Who knows? */
void timer_handler(struct regs *r)
{  
  timer_ticks++;
  if(timer_ticks % 10 == 0)
    printf(".");
  if (timer_ticks % 100 == 0)
    {
      printf("|");
      seconds++;
    }
}

/**
   Waits for (ticks * 10) ms
*/
void timer_wait(int ticks)
{
  volatile uint32_t eticks;
  
  eticks = timer_ticks + ticks;
  int n = 0;
  while(timer_ticks < eticks) 
    {
      serial_writed(n++);
      serial_writes(": Timer waiting. timer_ticks: ");
      serial_writed(timer_ticks);
      serial_writes(" | eticks: ");
      serial_writed(eticks);
      serial_writes("\n");
      //      printf("Timer waiting. timer_ticks: %d, eticks: %d\n", timer_ticks++, eticks);
       __asm__ __volatile__ ("hlt");
    }
}
 
void timer_phase(int hz)
{
  int divisor = 1193180 / hz;
  outb(0x43, 0x36);             /* Set command byte 0x36 */
  outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
  outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

void timer_install()
{
  serial_writes("Timer Install\n");
  timer_phase(100);
  irq_install_handler(0, timer_handler);
  serial_writes("timer handler installed\n");
}
