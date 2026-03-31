section .bss
    tekst resb 64

section .text
global _start
_start:
    mov rax, 0
    mov rdi, 0
    mov rsi, tekst
    mov rdx, 64
    syscall
    mov rbx, 0
petla:
    movzx rax, byte[rsi]
    cmp al, 0xA
    je koniec
    cmp al,'0'
    jl nastepny_znak
    cmp al,'9'
    jg nastepny_znak
    sub al,'0'
    add rbx, rax
nastepny_znak:
    add rsi,1
    jmp petla
koniec:
    mov rax, rbx      
    mov rbx, 10       
    mov rdx, 0
mov rcx,0
dzielenie:
    mov rdx, 0
    div rbx
    add rdx, '0'
    push rdx
    add rcx,1
    cmp rax,0
    jnz dzielenie
wypisz:
    pop rax
    mov [tekst],al
    mov rax,1
    mov rdi,1
    mov rsi,tekst
    mov rdx,1
    push rcx
    syscall
    pop rcx
    sub rcx,1
    jnz wypisz
mov rax, 60
mov rdi, 0
syscall