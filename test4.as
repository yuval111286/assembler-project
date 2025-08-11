; file t4_registers_data.as

    .entry GO
    .extern W

GO:         mov     r1, r2
add     r3, r4
sub     r6, r6
cmp     r2, r4
prn     #65
prn     TITLE
clr     VALUE
not     r5
inc     r5
dec     VALUE
mov     MAT2[r0][r1], r7
mov     r7, W
stop

TITLE:      .string "Regs & Data"
VALUE:      .data   10, -1, 3
MAT2:       .mat    [2][3] 1,2,3,4,5,6