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
	/*
	uint16_t ds, ss;
	asm("	movw %%ds, %0\n\
			movw %%ss, %1\n" :
			"=r" (ds), "=r" (ss));
	serial_writed(__LINE__);
	serial_writes(":KERNEL DS:");
	serial_writed(ds);
	serial_writes(", KERNEL SS:");
	serial_writed(ss);
	serial_writes("\n");
	*/
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
	//ps2kb_init();
	__asm__ __volatile__ ("sti");

  serial_writes("\nKERNEL_EARLY FINISHED\n");
}

void kernel_main(unsigned int ebx)
{
	kernel_early();
  /*
	uint32_t ds, ss;
	asm("	movl %%ds, %0\n\
			movl %%ss, %1\n" :
			"=r" (ds), "=r" (ss));
	serial_writed(__LINE__);
	serial_writes(":KERNEL DS:");
	serial_writed(ds);
	serial_writes(", KERNEL SS:");
	serial_writed(ss);
	serial_writes("\n");
  */
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
  asm("mov $0xDEADBABA, %ecx\n");
  while(1);
  return;
}

extern void switch_task();

void current_test(void)
{
  memcpy((uint8_t)0x30000,&task1,100);
  kinfo("READY TO TASK\n");
  switch_task();

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
