loop:
	mov $0xDEADBABA, %eax
	jmp loop

mov $0xDEADBEEF, %eax
jmp loop
