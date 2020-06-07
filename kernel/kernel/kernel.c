#include <stdio.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/log.h>

#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/isr.h>
#include <arch/i386/irq.h>
#include <arch/i386/timer.h>
#include <arch/i386/ps2.h>
#include <arch/i386/ps2kb.h>

#include <sys/info.h>
#include <sys/io.h>
#include <sys/debug.h>
#include <sys/kb.h>
#include <sys/shell.h>
#include <sys/multiboot.h>

extern unsigned long kernel_end_marker;
extern unsigned long kernel_start_marker;

void kernel_end(void);
void current_test(void);

void kernel_early(void)
{
	uint32_t esp;
	asm("movl %%esp, %0\n" : "=r" (esp));
	serial_initialise();
	serial_writes(__DATE__ "\t" __TIME__ "\n");
	serial_writed(esp);
	gdt_initialise();
	serial_writed(__LINE__);
	terminal_initialise();
	idt_init();
	isrs_install();
	irq_install();
	timer_install();
	ps2_init();
	ps2kb_init();
	__asm__ __volatile__ ("sti");

  serial_writes("\nKERNEL_EARLY FINISHED\n");
}

void kernel_main(unsigned int ebx)
{
	kernel_early();
	klog("LOG MESSAGE\n");
	printf("Kernel start: 0x%x, kernel end: 0x%x",
		   	&kernel_start_marker, &kernel_end_marker);

	printf("Well, Chuckles\nChuckle away\n");
	current_test();
	kernel_end();
}

void kernel_end()
{
	for(;;);
}

void current_test(void)
{
}
