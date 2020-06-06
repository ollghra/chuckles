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
#include <arch/i386/syscall.h>

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
  syscall_init();
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
  asm("mov $0xDADAFAFA, %%ecx\n":);
  
  asm("mov %0, %%eax\n\
      mov %1, (%%eax)\n\
      inc %%eax\n\
      mov %2, (%%eax)\n\
      inc %%eax\n\
      mov %3, (%%eax)\n\
      inc %%eax\n\
      mov %4, (%%eax)\n\
      inc %%eax\n\
      mov %5, (%%eax)\n":: "a" (0x300a0), "r" ('U'), "r" ('S'), "r" ('E'), "r" ('R') , "r" (0));
  /*
  char *msg = (char *) 0x300a0;
  msg[0] = 'U';
  msg[1] = 'S';
  msg[2] = 'E';
  msg[3] = 'R';
  msg[4] = 0;
  */
  //memcpy((uint8_t*));

  asm("mov $0xDADAFFAA, %%eax\n":);
  //asm volatile ("mov %0, %%ebx\n mov $0x01, %%eax\n int $0x30\n" :: "a" (0x100));
  //asm volatile ("mov $0x39000, %%ebx\n mov $0x01, %%eax\n int $0x30\n":);
  //asm("mov $0x1, %%eax\nint $0x30\n":);
  asm("mov $0xDEADBABA, %%ebx\n":);
  while(1);
  return;
}

extern void switch_task();

extern void task2();
extern tss_entry_t tss;
void current_test(void)
{
  memcpy((uint8_t*)0x30000,&task2,100);
  kinfo("READY TO TASK\n");
  switch_task();
  /*
  asm("  \n\
  movb $84, %%ax\n\
  outb %%ax, $0x3f8 \n\ 
  cli\n\
  push $0x23             // User SS (DS in this case, no SS)\n\
  push $0x30000         // User stack pointer\n\
  pushfl\n\
  popl %%eax\n\
  orl $0x200, %%eax      // Set IF\n\
  and $0xffffbfff, %%eax // Unset NT\n\
  push %%eax             // EFLAGS\n\
  \
  push $0x1B             // User CS\n\
  push $0x30000                // EIP\n\
  \
  movl $0x00000, %0      // Update tss with user stack base\n\
  movw $0x23, %%ax       // Updating data segment\n\
  movw %%ax, %%dx\n\
  \
  iret  // BOOM\n": "=m" (tss.esp0));
*/
}
