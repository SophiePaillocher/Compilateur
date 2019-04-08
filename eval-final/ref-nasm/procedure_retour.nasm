%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string

section	.text
global _start
_start:
	call	fmain
	mov	eax, 1
	int	0x80
fprocedure:
	push	ebp
	mov	ebp, esp
	mov	dword [ebp + 8], 1
	pop	ebp
	ret
	pop	ebp
	ret
fmain:
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	call	fprocedure
	pop	eax
	call	iprintLF
	pop	ebp
	ret
