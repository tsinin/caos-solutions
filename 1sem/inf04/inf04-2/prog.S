    .global mergesort
    .intel_syntax noprefix
    .text

mergesort:     // rax = i, rbx = j, rcx = n, rdx = tmp in second_loop and merge
               // r8 = it1, r9 = it2, rbx = left, r10 = mid, r11 = right
               // r12 = in, r13 = out, r14 = from, r15 = to
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov r12, rdx
    mov r13, rcx
    mov r14, rdi
    mov r15, rsi

    mov rcx, rsi
    sub rcx, rdi
    mov rax, 0
    jmp copy_to_out

copy_to_out:
    cmp rax, rcx
    jge mergesort2
    mov rsi, r14
    add rsi, rsi
    add rsi, rsi
    add rsi, r12
    mov ebx, [rsi + 4 * rax]
    mov [r13 + 4 * rax], ebx
    inc rax
    jmp copy_to_out

mergesort2:
    mov rax, 1
    jmp first_loop

first_loop:
    mov rbx, 0
    cmp rax, rcx
    jl merge
    jmp end

merge:     // r8 = it1, r9 = it2, rbx = left, r10 = mid, r11 = right    
    mov r8, 0
    mov r9, 0
    mov r10, rax
    add r10, rbx

    cmp r10, rcx
    jge continue

    mov r11, rax
    add r11, r11
    add r11, rbx
    cmp r11, rcx
    jg greater
    jmp merge_loop

greater:
    mov r11, rcx
    jmp merge_loop

merge_loop:
    mov rdx, rbx
    add rdx, r8
    cmp rdx, r10    
                    // left + it1 < mid
    jl branch1     
                    // left + it1 >= mid
    jmp branch4    

branch1:           // left + it1 < mid
    mov rdx, r10
    add rdx, r9
    cmp rdx, r11
                    // mid + it2 < right
    jl branch2     
    mov rdx, rbx   
    add rdx, r8
                        // push a[left + it1]
    mov edx, [r13 + 4 * rdx]
    push rdx
    inc r8
    jmp merge_loop

branch2:           // left + it1 < mid && mid + it2 < right
                   // rsi = A[left + it1], rdx = A[mid + it2]
    mov rsi, rbx
    add rsi, r8
    mov esi, [r13 + 4 * rsi]
    mov edx, [r13 + 4 * rdx]
    cmp esi, edx
    jl branch3
    push rdx
    inc r9
    jmp merge_loop

branch3:
    push rsi
    inc r8
    jmp merge_loop

branch4:         // left + it1 >= mid
    mov rdx, r10
    add rdx, r9
    cmp rdx, r11
    jge branch5
    mov edx, [r13 + 4 * rdx]
    push rdx
    inc r9
    jmp merge_loop
    
branch5:
    // copy from stack to out[left,right)
    mov r8, r11
    sub r8, rbx
    jmp copy_from_stack

copy_from_stack:
    pop rdx
    // rsi = r13 + 4 * (r8 + rbx) - 4
    mov rsi, r8
    add rsi, rbx
    add rsi, rsi    
    add rsi, rsi
    add rsi, r13
    sub rsi, 4
    mov [rsi], edx

    dec r8
    cmp r8, 0
    jg copy_from_stack
    jmp continue
    
continue:
                 // new(j)
                 // second_loop
    add rbx, rax
    add rbx, rax
    mov rdx, rcx
    sub rdx, rax
    cmp rbx, rcx
    jle merge
    add rax, rax
    jmp first_loop

end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
