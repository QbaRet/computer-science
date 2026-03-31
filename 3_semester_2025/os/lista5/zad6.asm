section .data
    liczba dd 0xABCDEF01
section .bss
    bufor resb 8
section .text
    global _start
_start:
    mov eax, [liczba]
    mov r9d, 28
    mov rdi, 0
petla:
    mov ebx, eax
    mov ecx, r9d
    shr ebx, cl
    and ebx, 0xF

    cmp ebx, 10
    jb jest_cyfra
    add ebx, 'A'-10
    jmp dalej

jest_cyfra:
    add ebx, '0'

dalej:
    mov [bufor + rdi], bl
    inc rdi
    sub r9d, 4
    cmp r9d, 0
    jge petla
mov rax, 1
mov rdi, 1
mov rsi, bufor
mov rdx, 8
syscall
mov rax, 60
mov rdi, 0
syscall