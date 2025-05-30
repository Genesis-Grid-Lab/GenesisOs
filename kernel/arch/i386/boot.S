;; # Declare constants for the multiboot header.
MBALIGN equ    1<<0              ;; align loaded modules on page boundaries
MEMINFO equ  1<<1             ;; # provide memory map
;; MBFLAGS equ    MBALIGN | MEMINFO
	;; # this is the Multiboot 'flag' field
MBFLAGS 	equ 0x00000003
MAGIC 		equ 0x1BADB002       ;; # 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + MBFLAGS) ;; # checksum of above, to prove we are multiboot

;; # Declare a header as in the Multiboot Standard.
section .multiboot
align 4
dd MAGIC
dd MBFLAGS
dd CHECKSUM

;; # Reserve a stack for the initial thread.
section .bss
align 16
stack_bottom:
resb 16384 ;; # 16 KiB
stack_top:

;; # The kernel entry point.
section .text
global _start:function (_start.end - _start)
_start:
	cli
	mov esp, stack_top

	;; # Call the global constructors.
	extern _init
	call _init

	;; # Transfer control to the main kernel.
	extern kernel_main
	call kernel_main

	;; # Hang if kernel_main unexpectedly returns.
	hlt
	jmp $
.end:
