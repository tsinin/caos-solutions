    .text
    .global solve

solve:
    stmfd sp!, {r4}
    stmfd sp!, {r5}
    mov r4, #0
    b loop

loop:
    mov r5, r0

    mul r5, r4, r5
    add r5, r1, r5
    mul r5, r4, r5
    add r5, r2, r5
    mul r5, r4, r5
    add r5, r3, r5
    
    cmp r5, #0
    beq end
    add r4, r4, #1
    b loop

end:
    mov r0, r4
    ldmfd sp!, {r5}
    ldmfd sp!, {r4}
    bx lr
