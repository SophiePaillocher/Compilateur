%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string

section	.text
global _start
_start:
	call	fmain
	mov	eax, 1
	int	0x80
fmax:
	push	ebp
	mov	ebp, esp
	mov	eax, -1
	mov	ebx, dword [ebp + 12]
	cmp	ebx, dword [ebp + 8]
	jl	e2
	mov	eax, 0
e2:
	cmp	eax, 0
	je	e1
	mov	eax, dword [ebp + 8]
	mov	dword [ebp + 16], eax
	pop	ebp
	ret
e1:
	mov	eax, dword [ebp + 12]
	mov	dword [ebp + 16], eax
	pop	ebp
	ret
	pop	ebp
	ret
fmain:
	push	ebp
	mov	ebp, esp
	sub	esp, 4	; allocation variable locale v$v_1
	sub	esp, 4	; allocation variable locale v$v_2
	mov	eax, sinput
	call	readline
	mov	eax, sinput
	call	atoi
	mov	dword [ebp - 4], eax
	mov	eax, sinput
	call	readline
	mov	eax, sinput
	call	atoi
	mov	dword [ebp - 8], eax
	sub	esp, 4
	push	dword [ebp - 4]
	push	dword [ebp - 8]
	call	fmax
	add	esp, 8
	pop	eax
	mov	ebx, -1
	cmp	eax, dword [ebp - 4]
	je	e5
	mov	ebx, 0
e5:
	cmp	ebx, 0
	je	e3
	mov	eax, dword [ebp - 4]
	call	iprintLF
	jmp	e4
e3:
	mov	eax, dword [ebp - 8]
	call	iprintLF
e4:
	add	esp, 8
	pop	ebp
	ret
