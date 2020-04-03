    .intel_syntax noprefix
    .text
    .global _start

_start:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov rax, 12
    mov rdi, 0
    syscall
// begin of residue
    mov r10, rax
// residue
    mov r11, 0
// begin
    mov r12, rax

    jmp allocate_before_scan

allocate_before_scan:
    cmp r11, 0
    jg scan
    mov r11, 2050

    mov rax, 12
    mov rdi, r10
    add rdi, r11
    syscall
    
    jmp scan

scan:
    mov rax, 0
    mov rdi, 0
    mov rsi, r10
    mov rdx, r11
    syscall

    cmp rax, 0
    je print

    add r10, rax
    sub r11, rax
    jmp allocate_before_scan

print:
    mov rax, r10
    cmp rax, r12
    je end
    // r12 - begin, not including, r10 - end
    // r9 - end of the line (not including)
    // r8 - ptr from the end
    mov al, [r10 - 1]
    cmpb al, '\n'
    jne additional_line_break
    jmp continue

additional_line_break:
    mov al, '\n'
    mov [r10], al
    inc r10
    jmp continue

continue:

    dec r12
    mov r9, r10
    mov r8, r10
    sub r8, 1
    jmp print_loop

print_loop:
    cmp r8, r12
    je print_line

    mov al, [r8]
    mov bl, '\n'
    cmpb al, bl
    je print_line
    
    dec r8
    jmp print_loop

print_line:
    mov rax, 1
    mov rdi, 1
    mov rsi, r8
    inc rsi
    mov rdx, r9
    sub rdx, rsi
    syscall

    cmp r8, r12
    je end

    mov r9, r8
    inc r9
    dec r8
    jmp print_loop
    

end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    mov rax, 60
    mov rdi, 0
    syscall
