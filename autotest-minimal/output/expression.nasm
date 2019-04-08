%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string
v$a:	resd	1
v$b:	resd	1

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
	mov	eax, sinput
	call	readline
	mov	eax, sinput
	call	atoi
	mov	dword [v$b], eax
	mov	eax, dword [v$a]
	add	eax, dword [v$b]
	call	iprintLF
	pop	ebp
	ret
