#include <stdio.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/log.h>

#include <arch/i386/paging.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/isr.h>
#include <arch/i386/irq.h>
#include <arch/i386/timer.h>
#include <arch/i386/tss.h>
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
	serial_initialise();
	serial_writes(__DATE__ "\t" __TIME__ "\n");
	gdt_initialise();
	terminal_initialise();
	idt_init();
	isrs_install();
	irq_install();
	timer_install();
	ps2_init();
	ps2kb_init();
	__asm__ __volatile__ ("sti");
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

void task1(void)
{
#define PORT 0x3f8    // COM1
	while(1){
	  while((inb(PORT + 5) & 0x20) == 0);  
	  outb(PORT, '.');
	}
	return;
}

extern struct tss default_tss;

void current_test(void)
{
	memcpy((char*) 0x30000, &task1,100);	
	asm("	cli\n\
			push $0x2B\n\
			push $0x30000\n\
			pushfl\n\
			popl %%eax\n\
			orl $0x200,%%eax\n\
			and $0xffffbfff, %%eax\n\
			push %%eax\n\
			push $0x23\n\
			push $0x0\n\
			movl $0x20000, %0\n\
			movw $0x2B, %%ax\n\
			movw %%ax, %%ds\n\
			iret" : "=m" (default_tss.esp0) :);

	/*multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
	unsigned int address_of_module = mbinfo->mods_addr;
	printf("ebx: %x\nmodule address: %x\nflags: %x\nmods_count: %x\n", ebx, address_of_module, mbinfo->flags, mbinfo->mods_count);
	typedef void (*call_module_t)(void);
	call_module_t start_program = (call_module_t) address_of_module;
	//start_program();
	*/
	//debug_shell();
	/*
	   printf("kernel start = %x\n", &kernel_start_marker);
	   printf("kernel end Magic! = %x\n", kernel_end_marker);
	   printf("kernel end   = %x\n", (int)&kernel_end_marker);
	   printf("kernel size  = %d B, %d kB\n", &kernel_end_marker - &kernel_start_marker, (&kernel_end_marker - &kernel_start_marker)/1024);
	   printf("CPUID: %s\n", cpu_string());
	   */
	// init_paging();
	// printf("JKIBJONJ\n");

	/*
	 * Old tests:
	 printf("%d", 1/0);
	 timer_wait(300);printf("Timer Finished");
	 */
}
