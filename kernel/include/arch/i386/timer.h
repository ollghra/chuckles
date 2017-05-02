#ifndef ARCH_I386_TIMER_H
#define ARCH_I386_TIMER_H

void timer_wait(int ticks);
void timer_phase(int hz);
void timer_install();

#endif
