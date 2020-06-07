# Declare constants for the multiboot header.
.set ALIGN,    		1<<0             # align loaded modules on page boundaries
.set MEMINFO,  		1<<1             # provide memory map
.set FLAGS,    		ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC, 		0x1BADB002       # 'magic number' lets bootloader find the header
.set ALIGN_MODULES, 	0x00000001
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Preallocate pages used to control paging
.section .bss, "aw", @nobits
    .align 4096
boot_page_directory:
    .skip 4096
boot_page_table1:
    .skip 4096
# If kernel grows greater than 3MiB, allocate more tables

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
    # Physical address of boot_page_table1
    movl $(boot_page_table1 - 0xC0000000), %edi
    # First address to map is 0
    movl $0, %esi
    # map 1023 pages, with 1024th being the VGA text buffer
    movl $1023, %ecx

1:
    # Map kernel only
    cmpl $(kernel_start_marker - 0xC0000000), %esi
    jl 2f
    cmpl $(kernel_end_marker - 0xC0000000), %esi
    jge 3f
    # Map physical address as present and writeable
    # .text and .rodata should not be writeable, change this
    movl %esi, %edx
    orl $0x003, %edx
    movl %edx, (%edi)

2:
    # Next page
    addl $4096, %esi
    # Next page table entry
    addl $4, %edi
    loop 1b

3:
    # Map VGA video memory to 0xC03FF000 as present and writeable
    movl $(0x000B8000 | 0x003), boot_page_table1 - 0xC0000000 + 1023*4


    movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
    movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

    # page directory's address goes in cr3
    movl $(boot_page_directory - 0xC0000000), %ecx
    movl %ecx, %cr3
    
    # Enable paging, set write-protect bit
    movl %cr0, %ecx
    orl $0x80010000, %ecx
    movl %ecx, %cr0

    # Absolute jump to higher half
    lea 4f, %ecx
    jmp *%ecx

4:
    # Paging now fully set up and enabled
    # Unmap identity mapping (now unnecessary)
    movl $0, boot_page_directory + 0
    
    # Reload cr3 to force TLB flush
    movl %cr3, %ecx
    movl %ecx, %cr3

    # Setup kernel stack
	movl $stack_top, %esp

	# Call the global constructors.
	call _init

	push %ebx
	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start
