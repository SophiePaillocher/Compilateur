%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string
v$a:	resd	1

section	.text
global _start
_start:
	call	fmain
	mov	eax, 1
	int	0x80
fmain:
	push	ebp
	mov	ebp, esp
	mov	dword [v$a], 100
e0:
	cmp	dword [v$a], 0
	je	e1
	mov	eax, dword [v$a]
	call	iprintLF
	mov	eax, dword [v$a]
	sub	eax, 1
	mov	dword [v$a], eax
	jmp	e0
e1:
	pop	ebp
	ret
