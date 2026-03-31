section .data
    macierz dd 1,2,3,4,5,6,7,8,9
    msg_suma db "Suma elementow: ", 0
    msg_diag db 0xA, "Suma przekatnej: ", 0
section .bss
    bufor resb 20
section .text
    global _start

_start:
    mov rcx, 9
    mov rbx, macierz
    mov rax, 0
    mov rdx, 0
petla:
    mov esi, [rbx]
    add rax, rsi
    add rbx, 4
    loop petla
mov edx, [macierz]
add edx, [macierz + 16]
add edx, [macierz +32]
push rax
push rdx

mov rax, 1
mov rdi, 1
mov rsi, msg_suma
mov rdx, 16
syscall

pop rbx
pop rax
mov rcx, 10
mov r8, 0

konwersja:
    mov rdx, 0
    div rcx
    add rdx, '0'
    push rdx
    add r8, 1
    cmp rax, 0
    jnz konwersja
wypisz: 
    pop rax
    mov [bufor], al
    mov rax, 1
    mov rdi, 1
    mov rsi, bufor
    mov rdx, 1
    syscall
    dec r8
    jnz wypisz

mov rax, 1
mov rdi, 1
mov rsi, msg_diag
mov rdx, 18
syscall
mov rax, rbx
mov rcx, 10
xor r8, r8

konwersja_2:
    mov rdx, 0
    div rcx
    add rdx, '0'
    push rdx
    add r8, 1
    cmp rax, 0
    jnz konwersja_2

wypisz_2:
    pop rax
    mov [bufor], al
    
    push r8
    mov rax, 1
    mov rdi, 1
    mov rsi, bufor
    mov rdx, 1
    syscall
    pop r8  
    dec r8
    jnz wypisz_2
mov rax, 60
mov rdi, 0
syscall
