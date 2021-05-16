; generated assembly file [May 16 2021 & 17:20:32]
; link using "ld object_file.o - o exe_file" command

%define program _start

section .text
	global program

program:
	push  rbp
	mov   rbp, rsp
	mov   QWORD [rbp-8], rdi
	mov   QWORD [rbp-16], rsi
	mov   QWORD [rbp-24], 10
	mov   QWORD [rbp-32], 20
	mov   BYTE [rbp-33], 97
	mov   BYTE [rbp-34], 66
	mov   QWORD [rbp-42], LBSTR.1
	mov   QWORD [rbp-50], LBSTR.2
	mov   r10, 5
	mov   r11, 10
	cmp   r10, r11
	jg    .L0
	mov   QWORD [rbp-58], 50
	jmp   .L1
.L0:
	mov   QWORD [rbp-66], 50
.L1:
	jmp   .L9
.L10:
	mov   r10, 10
	mov   rax, r10
.L9:
	mov   r10, 10
	mov   r11, 10
	cmp   r10, r11
	je    .L10
	pop   rbp
	syscall

section .data
	LBSTR.1: DB 76,97,115,97,110,0
	LBSTR.2: DB 78,97,110,111,71,0

; nano_g compiler developed by lasan nishshanka (@las_nish)
; May 16 2021 & 17:20:32