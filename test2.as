; file t2_io_flow.as

    .entry MAIN
    .extern INP
    .extern OUT

MAIN:       red     r1
mov     A[r1][r0], r2
cmp     r2, #0
bne     LOOP
jmp     DONE

LOOP:       add     #1, r2
mov     r2, OUT
jsr     SUMROW
prn     r2
jmp     DONE

SUMROW:     mov     A[r0][r0], r3
add     A[r0][r1], r3
mov     r3, r2
rts

DONE:       stop

TXT:        .string "I/O & flow!"
A:          .mat    [2][4]  1,2,3,4, 5,6,7,8
NUMS:       .data   12,-3,0,44