%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string
vv:	resd	20
vi:	resd	1

section	.text
global _start
_start:
	call	fmain
	mov	eax, 1
	int	0x80
fmain:
	push	ebp
	mov	ebp, esp
	mov	dword [vv+0], 5
	mov	dword [vi], 0
e0:
	mov	eax, -1
	cmp	dword [vi], 20
	jl	e2
	mov	eax, 0
e2:
	cmp	eax, 0
	je	e1
	mov	eax, dword [vi]
	sub	eax, 1
	add	eax, eax
	add	eax, eax
	mov	ebx, dword [vv+eax]
	add	ebx, 5
	mov	eax, dword [vi]
	add	eax, eax
	add	eax, eax
	mov	dword [vv+eax], ebx
	mov	eax, dword [vi]
	add	eax, 1
	mov	dword [vi], eax
	jmp	e0
e1:
	mov	dword [vi], 0
e3:
	mov	eax, -1
	cmp	dword [vi], 20
	jl	e5
	mov	eax, 0
e5:
	cmp	eax, 0
	je	e4
	mov	eax, dword [vi]
	add	eax, eax
	add	eax, eax
	mov	eax, dword [vv+eax]
	call	iprintLF
	mov	eax, dword [vi]
	add	eax, 1
	mov	dword [vi], eax
	jmp	e3
e4:
	pop	ebp
	ret
