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
