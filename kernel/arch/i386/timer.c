#include <stdio.h>

#include <arch/i386/irq.h>
#include <arch/i386/timer.h>
#include <arch/i386/isr.h>

#include <sys/io.h>
#include <sys/debug.h>

volatile unsigned int timer_ticks = 0;
int seconds = 0;

/* Handles the timer. By default, the timer fires 18.222 times
 *  per second. Why 18.222Hz? Who knows? */
void timer_handler(struct regs *r)
{  
  timer_ticks++;
  
  if (timer_ticks % 100 == 0)
    {
      seconds++;
      serial_writed(seconds);
      printf("SECOND PASSED");
    }
}
 
void timer_wait(int ticks)
{
  volatile unsigned int eticks;
  
  eticks = timer_ticks + ticks;
  printf("%d", timer_ticks);
  while(timer_ticks < eticks) 
    {
      serial_writes("Timer waiting");
      for(;;);
      __asm__ __volatile__ ("sti//hlt//cli");
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
  serial_writes("timer_phase 100\n");
  irq_install_handler(0, timer_handler);
  serial_writes("timer handler installed\n");
}
