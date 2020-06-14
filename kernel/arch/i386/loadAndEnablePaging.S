.text
.globl load_new_page_directory
# Loads argument into cr3 and enables paging
load_new_page_directory:
	push %ebp
	mov %esp, %ebp

	mov 8(%esp), %eax
	mov %eax, %cr3

	mov %cr0, %ebx
	or $0x80000000, %ebx
	mov %ebx, %cr0

	mov %ebp, %esp
	pop %ebp
	ret

.globl load_page_directory
# Loads argument into cr3
load_page_directory:
	push %ebp
	mov %esp, %ebp
	mov 8(%esp), %eax
	mov %eax, %cr3
	mov %ebp, %esp
	pop %ebp
	ret

.globl enable_paging
enable_paging:
	push %ebp
	mov %esp, %ebp

	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0

	mov %ebp, %esp
	pop %ebp
	ret
