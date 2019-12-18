.globl gdt_flush
.globl setGdt
.globl tss_flush

setGdt:
	movl 4(%esp), %eax
	movl gp, %ebx
	movl %eax, 2(%ebx)
	movw 8(%esp), %ax
	movw %ax, gp
	lgdt gp
	ret

	/*
	   .globl gdt_flush

	   gdt_flush:
	   lgdt gp
	   mov $0x10, %ax
	   mov %ax, %ds
	   mov %ax, %es
	   mov %ax, %fs
	   mov %ax, %gs
	   mov %ax, %ss
	   jmp $0x08, $flush2
	//	movw $0x0F01, (0x000B8000)
	flush2:
	ret
	 */

gdt_flush:
	ljmp $0x08, $flush
flush:	
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

  // Load (index|RPL of 3) of TSS descriptor
  // index is 5*8=0x28 and RPL is 3, 0x28|0x03 = 0x2B
  movw $0x2B, %ax
  ltr %ax // Load tss state register

	ret
/*
tss_flush:
  // Load (index|RPL of 3) of TSS descriptor
  // index is 5*8=0x28 and RPL is 3, 0x28|0x03 = 0x2B
  movw $0x2B, %ax
  ltr %ax // Load tss state register
  ret
*/
