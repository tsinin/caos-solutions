    .global summ
    .intel_syntax noprefix
    .text

summ:
    push ebp
    push ebx
    mov ecx, [esp + 4 * 3]

sumloop:
    // R[i] = A[i] + B[i]
    mov ebx, ecx
    dec ebx
    
    mov edx, [esp + 4 * 4]
    mov eax, [edx + 4 * ebx]
    mov edx, [esp + 4 * 5]
    add eax, [edx + 4 * ebx]
    mov edx, [esp + 4 * 6]
    mov [edx + 4 * ebx], eax
    loop sumloop
    pop ebx
    pop ebp
    ret
