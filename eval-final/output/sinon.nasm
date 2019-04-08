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
	mov	eax, sinput
	call	readline
	mov	eax, sinput
	call	atoi
	mov	dword [v$a], eax
	cmp	dword [v$a], 0
	je	e0
	mov	eax, 1
	call	iprintLF
	jmp	e1
e0:
	mov	eax, 0
	call	iprintLF
e1:
	pop	ebp
	ret
