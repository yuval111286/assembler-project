

; file t3_forward_lea.as

    .entry START
    .extern EXTZ

START:      lea     STR, r6
lea     FORW[r3][r5], r7
add     EXTZ, r1
prn     r6
bne     L1
jmp     END

L1:         mov     FORW[r1][r2], r0
sub     #5, r0
prn     r0
END:        stop

STR:        .string "forward-ref test"
VALS:       .data   6,-9,15,22
FORW:       .mat    [3][3]  1,2,3, 4,5,6, 7,8,9