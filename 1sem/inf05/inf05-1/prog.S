    .global my_sin
    .intel_syntax noprefix
    .text

my_sin:     // xmm0 = ans, xmm2 = -x^2, xmm3 = temporary
            // xmm1 = add = (-1)^n * x^(2n + 1) / (2n + 1)!
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15


    movsd xmm2, xmm0
    mulsd xmm2, xmm2
    mov rax, -1
    cvtsi2sd xmm3, rax
    mulsd xmm2, xmm3

    movsd xmm1, xmm0
    movsd xmm0, xmm0
    mov rax, 1

loop:
    mov rcx, 0
    cvtsi2sd xmm3, rcx
    comisd xmm1, xmm3
    jz end

    inc rax
    cvtsi2sd xmm3, rax
    divsd xmm1, xmm3

    inc rax
    cvtsi2sd xmm3, rax
    divsd xmm1, xmm3

    mulsd xmm1, xmm2
    
    addsd xmm0, xmm1

    jmp loop

end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
