    .global calc
    .global vector_sum
    .intel_syntax noprefix
    .text

calc:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    cvtsi2sd xmm3, edi
    addsd xmm2, xmm3
    addsd xmm0, xmm1
    divsd xmm0, xmm2
    jmp end

vector_sum:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov r8, rcx
    mov r9, rdx
    mov rcx, rdi
    inc rcx    
    loop sum_loop

sum_loop:
    sub rcx, 4
    mov rax, rcx
    mov rbx, 4
    mul rbx
    add rax, rsi
    movaps xmm0, [rax]
    sub rax, rsi
    add rax, r9
    movaps xmm1, [rax]
    sub rax, r9
    add rax, r8
    addps xmm0, xmm1
    movaps [rax], xmm0

    add rcx, 1
    loop sum_loop
    jmp end

end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
