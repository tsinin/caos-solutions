    .text
    .global sort_by_age

sort_by_age:
    stmfd sp!, {r4-r12}
    mov r8, #-1        // i
    mov r7, #0         // j
    mov r10, #48       // 48 = size of a person structure in bytes
    b first_loop
    
first_loop:
    add r8, #1    
    mov r7, r8
    add r7, #1

    cmp r8, r0
    beq end

    b second_loop

second_loop:
    cmp r7, r0
    beq first_loop

    mul r2, r10, r8
    add r2, r1
    ldrb r5, [r2, #4]   // r5 = persons[i].age
    mul r2, r10, r7
    add r2, r1
    ldrb r6, [r2, #4]   // r6 = persons[j].age

    cmp r5, r6
    bgt swap

    add r7, #1
    b second_loop

swap:
    mul r5, r10, r7
    add r5, r1
    mul r6, r10, r8
    add r6, r1
    add r4, r6, #48
    b loop_for_swap

loop_for_swap:
    cmp r4, r6
    beq second_loop

    ldr r2, [r5]
    ldr r3, [r6]
    str r2, [r6]
    str r3, [r5]
    add r5, #4
    add r6, #4
    b loop_for_swap

end:
    ldmfd sp!, {r4-r12}
    bx lr
