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
	sub	esp, 4	; allocation variable locale v$a
	mov	dword [ebp - 4], 1
	mov	eax, dword [ebp - 4]
	call	iprintLF
	add	esp, 4
	pop	ebp
	ret
fmain:
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	call	fprocedure
	add	esp, 4
	pop	ebp
	ret
