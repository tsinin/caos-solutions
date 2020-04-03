    .text
    .global main

main:
    stmfd sp!, {r4-r12,lr}

loop:
    ldr r0, in
    ldr r0, [r0]
    bl fgetc
    cmp r0, #-1
    beq end
    cmp r0, #47
    ble loop
    cmp r0, #57
    bhi loop
    b print

print:
    ldr r1, out
    ldr r1, [r1]
    bl fputc
    b loop

end:
    ldmfd sp!, {r4-r12, pc}

in: .word stdin
out: .word stdout
