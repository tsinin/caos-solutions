    .global dot_product
    .intel_syntax noprefix
    .text

dot_product:  // rdi = N, rsi = A, r9 = B, ymm0 = ans, ymm1-ymm3 = temporary
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov r9, rdx
    
    mov rax, 0
    cvtsi2ss xmm0, rax

    cmp rdi, 8
    jl second_loop

    jmp first_loop

first_loop:
    sub rdi, 8
    mov rax, rdi
    mov rbx, 4
    mul rbx
    add rax, rsi
    vmovups ymm1, [rax]
    sub rax, rsi
    add rax, r9
    vmovups ymm2, [rax]

    vmulps ymm1, ymm1, ymm2
    vhaddps ymm1, ymm1, ymm1
    vhaddps ymm1, ymm1, ymm1
    vextractf128 xmm3, ymm1, 1
    addss xmm1, xmm3
    addss xmm0, xmm1

    cmp rdi, 8
    jl second_loop 

    jmp first_loop

second_loop:
    cmp rdi, 0
    jz end
    dec rdi

    mov rax, rdi
    mov rbx, 4
    mul rbx
    add rax, rsi
    movups xmm1, [rax]
    sub rax, rsi
    add rax, r9
    movups xmm2, [rax]
    mulss xmm1, xmm2

    addss xmm0, xmm1
    
    jmp second_loop

end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
