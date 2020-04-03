#include <sys/syscall.h>
    .intel_syntax noprefix
    .text
    .global _start

_start:
    mov rax, SYS_write
    mov rdi, 1
    mov rsi, hello_ptr
    mov rdx, 14
    syscall

    jmp end

end:
    mov rax, SYS_exit
    mov rdi, 0
    syscall

    .data
hello:
    .string "Hello, World!\n"
hello_ptr:
    .long hello
