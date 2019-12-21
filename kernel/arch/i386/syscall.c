#include <stdio.h>
#include <stdint.h>

#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/syscall.h>

#include <sys/debug.h>

extern void syscall_handle();

void syscall_init(void)
{
  serial_writes("SYSCALL INSTALL\n");
  idt_set_gate(0x30, (unsigned)syscall_handle, 0x08, 0xEF);
}

extern struct gdt_ptr gp;

void syscall(uint32_t number)
{
  uint32_t ds_sel_offset, ds_base;

  // TODO:change this to a map
  if(number==SYS_PRINT)
  {
      printf("PRINT SYSCALL\n");
      char * message;
      asm("   mov 44(%%ebp), %%eax    \n \
              mov %%eax, %0           \n \
              mov 24(%%ebp), %%ax     \n \
              mov %%ax, %1" : "=m" (message), "=m" (ds_sel_offset) : );
      // Find data segment corresponding to usermode task
      struct gdt_entry *ds_sel;
      ds_sel = (struct gdt_entry *) (gp.base + (ds_sel_offset & 0xF8));
      ds_base = ds_sel->base_low + (ds_sel->base_middle << 16) + (ds_sel->base_high << 24);

      // Add offset of base to message to locate char string
      puts((char *) (ds_base + message));
  }
  else
  {
      puts("SYSCALL\n");
  }
  return;
}
