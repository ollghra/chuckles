.globl IDT_load
IDT_load:
	lidt (IDTp)
	ret
