#include <sys/syscall.h>
    .intel_syntax noprefix
    .text
    .global _start

_start:
    sub rsp, 8

    mov rax, SYS_read
    mov rdi, 0
    mov rsi, rsp
    add rsi, 8
    mov rdx, 1
    syscall

    cmp rax, 0
    jle end

    mov rax, SYS_write
    mov rdi, 1
    mov rsi, rsp
    add rsi, 8
    mov rdx, 1
    syscall

    jmp _start

end:
    add rsp, 8

    mov rax, SYS_exit
    mov rdi, 0
    syscall
