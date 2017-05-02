.globl IDT_load
IDT_load:
	lidt IDTp
	ret
/*
int_handler:
	mov %ax, LINEAR_DATA_SELECTOR
	mov %ax, %gs
	//movl [gs:0xB8000],') : '
	movl %al, %gs:0xb8000 (, 1, 1)
	movb ':', %al
	movb %al, (%gs, $0xB8000, 1)
	movb '(', %al
	movb %al, (%gs, $0xB8001, 1)
	hlt

.lcomm idt, $50*$2

.lcomm idtr, ($50*$8) - $1
	LINEAR_ADDRESS(idt)
	
.globl test1
test1:
	lidt (idtr)
	mov int_handler, %eax
//	mov %ax, (idt+$49*$8)
	movb %ax, (idt, $49, $8)
//	movw CODE_SELECTOR, (idt+49*8+2)
	movw CODE_SELECTOR, (idt, $49, $8 + $2)
//	movw $0x8E00, (idt+49*8+4)
	movw $0x8E00, (idt, $49, $8+$4)
	shr $16, %eax
	mov %ax, (idt+49*8+6)
	int $49
*/
/*
int_handler:
    movw $0xAB, 0x0 // this places magic value "0x123abc" at the beginning of memory
    hlt
 
.p2align 4
idt:
    .skip 50*2
 
idtr:
    .short (50*8)-1
    .long idt
 
.globl do_test
do_test:
    lidt idtr
    movl $int_handler, %eax
    mov %ax, idt+49*16
    movw $0x20, idt+49*16+2 // replace 0x20 with your code section selector
    movw $0x8e00, idt+49*16+4
    shr $16, %eax
    mov %ax, idt+49*16+6
    shr $16, %eax
    mov %eax, idt+49*16+8
    int $49

*/
