.section .text
.align 4

.global switch_task
.type switch_task, @function

switch_task:
  cli
  push $0x23            // User SS (DS in this case, no SS)
  push $0x30000         // User stack pointer
  pushfl
  popl %eax
  orl $0x200, %eax      // Set IF
  and $0xffffbfff, %eax // Unset NT
  push %eax             // EFLAGS

  push $0x1B            // User CS
  push $0               // EIP
  
  //movl $0x20000, tss.esp0 // Update tss with user stack base
  movw $0x23, %ax       // Updating data segment
  movw %ax, %dx
  
  iret  // BOOM

.global task2
task2:
  mov $0xDADAFAFA, %ecx
  
  mov $0x30022, %ebx

  mov $0xDADAFFAA, %ecx
  mov $0x01, %eax
  mov $0xDADAFFAB, %ecx
  int $0x30
  //asm volatile ("mov %0, %%ebx\n mov $0x01, %%eax\n int $0x30\n" :: "a" (0x100));
  //asm volatile ("mov $0x39000, %%ebx\n mov $0x01, %%eax\n int $0x30\n":);
  //asm("mov $0x1, %%eax\nint $0x30\n":);
  mov $0xDEADBABA, %ecx
loop:
  jmp loop 
msg:
  .byte 'D','O,'N','K',0
