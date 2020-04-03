    .text
    .global summ

summ:
    cmp r1, #0
    ble end
    sub r1, #1
    ldr r3, [r2]
    add r0, r0, r3
    add r2, r2, #4
    b summ

end:
    bx lr
