global outb
global inb

outb:
	mov al, [esp + 8]
	mov dx, [esp + 4]
	out dx, al
	ret

inb:
	mov edx, [esp + 4]  ; Load port argument from stack
	xor eax, eax        ; Clear EAX to ensure only AL is affected
	in al, dx           ; Read from port into AL
	ret
