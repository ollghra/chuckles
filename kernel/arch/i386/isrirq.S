.globl isr0
.globl isr1
.globl isr2
.globl isr3
.globl isr4
.globl isr5
.globl isr6
.globl isr7
.globl isr8
.globl isr9
.globl isr10
.globl isr11
.globl isr12
.globl isr13
.globl isr14
.globl isr15
.globl isr16
.globl isr17
.globl isr18
.globl isr19
.globl isr20
.globl isr21
.globl isr22
.globl isr23
.globl isr24
.globl isr25
.globl isr26
.globl isr27
.globl isr28
.globl isr29
.globl isr30
.globl isr31

//  0: Divide By Zero Exception
isr0:
    cli
    pushw $0
    pushw $0
    jmp isr_common_stub

//  1: Debug Exception
isr1:
    cli
    pushw $0
    pushw $1
    jmp isr_common_stub

//  2: Non Maskable Interrupt Exception
isr2:
    cli
    pushw $0
    pushw 2
    jmp isr_common_stub

//  3: Int 3 Exception
isr3:
    cli
    pushw $0
    pushw 3
    jmp isr_common_stub

//  4: INTO Exception
isr4:
    cli
    pushw $0
    pushw 4
    jmp isr_common_stub

//  5: Out of Bounds Exception
isr5:
    cli
    pushw $0
    pushw 5
    jmp isr_common_stub

//  6: Invalid Opcode Exception
isr6:
    cli
    pushw $0
    pushw 6
    jmp isr_common_stub

//  7: Coprocessor Not Available Exception
isr7:
    cli
    pushw $0
    pushw 7
    jmp isr_common_stub

//  8: Double Fault Exception (With Error Code!)
isr8:
    cli
    pushw 8
    jmp isr_common_stub

//  9: Coprocessor Segment Overrun Exception
isr9:
    cli
    pushw $0
    pushw 9
    jmp isr_common_stub

// 10: Bad TSS Exception (With Error Code!)
isr10:
    cli
    pushw $10
    jmp isr_common_stub

// 11: Segment Not Present Exception (With Error Code!)
isr11:
    cli
    pushw $11
    jmp isr_common_stub

// 12: Stack Fault Exception (With Error Code!)
isr12:
    cli
    pushw $12
    jmp isr_common_stub

// 13: General Protection Fault Exception (With Error Code!)
isr13:
    cli
    pushw $13
    jmp isr_common_stub

// 14: Page Fault Exception (With Error Code!)
isr14:
    cli
    pushw $14
    jmp isr_common_stub

// 15: Reserved Exception
isr15:
    cli
    pushw $0
    pushw $15
    jmp isr_common_stub

// 16: Floating Point Exception
isr16:
    cli
    pushw $0
    pushw $16
    jmp isr_common_stub

// 17: Alignment Check Exception
isr17:
    cli
    pushw $0
    pushw $17
    jmp isr_common_stub

// 18: Machine Check Exception
isr18:
    cli
    pushw $0
    pushw $18
    jmp isr_common_stub

// 19: Reserved
isr19:
    cli
    pushw $0
    pushw $19
    jmp isr_common_stub

// 20: Reserved
isr20:
    cli
    pushw $0
    pushw $20
    jmp isr_common_stub

// 21: Reserved
isr21:
    cli
    pushw $0
    pushw $21
    jmp isr_common_stub

// 22: Reserved
isr22:
    cli
    pushw $0
    pushw $22
    jmp isr_common_stub

// 23: Reserved
isr23:
    cli
    pushw $0
    pushw $23
    jmp isr_common_stub

// 24: Reserved
isr24:
    cli
    pushw $0
    pushw $24
    jmp isr_common_stub

// 25: Reserved
isr25:
    cli
    pushw $0
    pushw $25
    jmp isr_common_stub

// 26: Reserved
isr26:
    cli
    pushw $0
    pushw $26
    jmp isr_common_stub

// 27: Reserved
isr27:
    cli
    pushw $0
    pushw $27
    jmp isr_common_stub

// 28: Reserved
isr28:
    cli
    pushw $0
    pushw $28
    jmp isr_common_stub

// 29: Reserved
isr29:
    cli
    pushw $0
    pushw $29
    jmp isr_common_stub

// 30: Reserved
isr30:
    cli
    pushw $0
    pushw $30
    jmp isr_common_stub

// 31: Reserved
isr31:
    cli
    pushw $0
    pushw $31
    jmp isr_common_stub

// This is our common ISR stub. It saves the processor state, sets
// up for kernel mode segments, calls the C-level fault handler,
// and finally restores the stack frame.
isr_common_stub:
    pusha
    pushw %ds
    pushw %es
    pushw %fs
    pushw %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax
    pushl %eax
    mov fault_handler, %eax
    call fault_handler
    popl %eax
    popw %gs
    popw %fs
    popw %es
    popw %ds
    popa
    add $8, %esp
    iret

.globl irq0
.globl irq1
.globl irq2
.globl irq3
.globl irq4
.globl irq5
.globl irq6
.globl irq7
.globl irq8
.globl irq9
.globl irq10
.globl irq11
.globl irq12
.globl irq13
.globl irq14
.globl irq15

// 32: IRQ0
irq0:
    cli
    pushw $0
    pushw $32
    jmp irq_common_stub

// 33: IRQ1
irq1:
    cli
    pushw $0
    pushw $33
    jmp irq_common_stub

// 34: IRQ2
irq2:
    cli
    pushw $0
    pushw $34
    jmp irq_common_stub

// 35: IRQ3
irq3:
    cli
    pushw $0
    pushw $35
    jmp irq_common_stub

// 36: IRQ4
irq4:
    cli
    pushw $0
    pushw $36
    jmp irq_common_stub

// 37: IRQ5
irq5:
    cli
    pushw $0
    pushw $37
    jmp irq_common_stub

// 38: IRQ6
irq6:
    cli
    pushw $0
    pushw $38
    jmp irq_common_stub

// 39: IRQ7
irq7:
    cli
    pushw $0
    pushw $39
    jmp irq_common_stub

// 40: IRQ8
irq8:
    cli
    pushw $0
    pushw $40
    jmp irq_common_stub

// 41: IRQ9
irq9:
    cli
    pushw $0
    pushw $41
    jmp irq_common_stub

// 42: IRQ10
irq10:
    cli
    pushw $0
    pushw $42
    jmp irq_common_stub

// 43: IRQ11
irq11:
    cli
    pushw $0
    pushw $43
    jmp irq_common_stub

// 44: IRQ12
irq12:
    cli
    pushw $0
    pushw $44
    jmp irq_common_stub

// 45: IRQ13
irq13:
    cli
    pushw $0
    pushw $45
    jmp irq_common_stub

// 46: IRQ14
irq14:
    cli
    pushw $0
    pushw $46
    jmp irq_common_stub

// 47: IRQ15
irq15:
    cli
    pushw $0
    pushw $47
    jmp irq_common_stub

irq_common_stub:
    pusha
    pushw %ds
    pushw %es
    pushw %fs
    pushw %gs

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax

    pushl %eax
    mov irq_handler, %eax
    call irq_handler
    pop %eax

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp
    iret
