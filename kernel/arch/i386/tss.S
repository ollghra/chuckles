.section .text
.align 4

.global tss_flush
.type tss_flush, @function

tss_flush:
  // Load (index|RPL of 3) of TSS descriptor
  // index is 5*8=0x28 and RPL is 3, 0x28|0x03 = 0x2B
  mov $0x2B, %ax
  ltr %ax
  ret
