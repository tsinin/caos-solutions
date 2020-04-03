    .text
    .global main

    .extern printf
    .extern scanf

main:
    stmfd sp!, {r4-r12,lr}
    sub sp, #8

    ldr r0, =in
    add r1, sp, #8
    add r2, sp, #4
    bl scanf
    ldr r1, [sp, #4]
    ldr r2, [sp, #8]

    ldr r0, =out
    add r1, r1, r2
    bl printf

    add sp, #8
    ldmfd sp!, {r4-r12, pc}

    .data
in: .asciz "%d%d"
out: .asciz "%d\n"
