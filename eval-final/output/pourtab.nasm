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
	jl	e1
	mov	eax, 0
e1:
	cmp	eax, 0
	je	e2
	mov	eax, dword [vi]
	mov	ebx, dword [vi]
	sub	ebx, 1
	add	ebx, ebx
	add	ebx, ebx
	mov	ecx, dword [vv+ebx]
	add	ecx, 5
	add	eax, eax
	add	eax, eax
	mov	dword [vv+eax], ecx
	mov	eax, dword [vi]
	add	eax, 1
	mov	dword [vi], eax
	jmp	e0
e2:
	mov	dword [vi], 0
e3:
	mov	eax, -1
	cmp	dword [vi], 20
	jl	e4
	mov	eax, 0
e4:
	cmp	eax, 0
	je	e5
	mov	eax, dword [vi]
	add	eax, eax
	add	eax, eax
	mov	eax, dword [vv+eax]
	call	iprintLF
	mov	eax, dword [vi]
	add	eax, 1
	mov	dword [vi], eax
	jmp	e3
e5:
	pop	ebp
	ret
