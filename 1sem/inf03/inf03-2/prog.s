    .text
    .global main

main:
    stmfd sp!, {r4-r12,lr}
    mov r8, #4             // array size (bytes)
    mov r7, #0             // number of characters (bytes)
    mov r6, #0             // pointer to array

    b reallocArray

reallocArray:
    mov r2, #2
    mul r1, r8, r2
    mov r8, r1
    mov r0, r6
    bl realloc
    mov r6, r0
    b loop

loop:
    ldr r0, in
	ldr r0, [r0]
    bl fgetc
    cmp r0, #-1
    subeq r7, #4
    moveq r1, r7
    beq print

    // push to array
    str r0, [r6, r7]
    add r7, #4
    cmp r7, r8
    beq reallocArray

    b loop

print:
    ldr r0, [r6, r7]
    ldr r1, out
    ldr r1, [r1]
    bl fputc
    sub r7, #4
    
    cmp r7, #-4
    beq end
    b print

end:
    mov r0, r6
    bl free
    ldmfd sp!, {r4-r12, pc}

in: .word stdin
out: .word stdout
//null: .word nullptr
