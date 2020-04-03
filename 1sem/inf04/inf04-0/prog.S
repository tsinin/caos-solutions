    .global summ
    .global everyday795
    .intel_syntax noprefix
    .text

summ:
    push ebp
    push ebx
    mov ecx, N

sumloop:
    // R[i] = A[i] + B[i]
    mov ebx, ecx
    dec ebx
    
    mov edx, A
    mov eax, [edx + 4*ebx]
    mov edx, B
    add eax, [edx + 4*ebx]
    mov edx, R
    mov [edx + 4*ebx], eax
    loop sumloop
    pop ebx
    pop ebp
    ret

everyday795:
    push ebp

    mov ebp, esp
    sub esp, 4
    push ebx

    mov ebx, ebp
    sub ebx, 4
    push ebx
    push scanf_format_ptr

    call scanf
    add esp, 8

    mov ebx, [ebp - 4]
    imul ebx, [ebp + 8]
    add ebx, [ebp + 12]

    push ebx
    push printf_format_ptr
    call printf
    add esp, 8

    pop ebx
    mov esp, ebp
    pop ebp
    ret
    
scanf_format_ptr:
    .long scanf_format
    .data
scanf_format:
    .string "%d"

printf_format_ptr:
    .long printf_format
    .data
printf_format:
    .string "%d\n"
