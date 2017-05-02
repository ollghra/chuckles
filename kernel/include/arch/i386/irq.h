#ifndef ARCH_I386_IRQ_H
#define ARCH_I386_IRQ_H

#include <arch/i386/isr.h>

void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);
void irq_install();

#endif
