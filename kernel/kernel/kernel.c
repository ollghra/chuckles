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
void current_test(multiboot_info_t* mbi);

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

void kernel_main(multiboot_info_t* mbi)
{
	kernel_early();
	printf("Kernel start: 0x%x, kernel end: 0x%x\n",
		   	&kernel_start_marker, &kernel_end_marker);

	current_test(mbi);
	kernel_end();
}

void kernel_end()
{
	for(;;);
}

void current_test(multiboot_info_t* mbi)
{
    if (mbi->flags & 1)
    {
        printf("MEM LOWER %dKiB;", mbi->mem_lower);
        printf("MEM UPPER %dKiB", mbi->mem_upper);
        printf("\n");
    }
    if (mbi->flags & (1<<6))
    {
        printf("MMAPS FOUND: %d\n", mbi->mmap_length/((memory_map_t*)mbi->mmap_addr)->size);
        int i = 0;
        for (memory_map_t* mm = (memory_map_t*)mbi->mmap_addr
                ; mm < (memory_map_t*)(mbi->mmap_addr + mbi->mmap_length)
                ; mm=(memory_map_t*)((unsigned int) mm + mm->size + sizeof(mm->size)))
        { 
            printf("MMAP %d: <S:%x,BASE:%x,LEN:%x,T:%d>\n", i
                    , mm->size
                    , mm->base_addr_low + (mm->base_addr_high << sizeof(unsigned int))
                    , mm->length_low + (mm->length_high << sizeof(unsigned int))
                    , mm->type);
            i++;
        }
    }
}
