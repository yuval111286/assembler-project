;obj_and_ext

.extern EXTFUNC
.extern EXTVAL

MAIN:   mov #5, r1
        jsr EXTFUNC
        add EXTVAL, r1
        cmp r1, EXTVAL
        bne LOOP
        stop

LOOP:   clr r2
        inc r2
        dec r2
        jmp MAIN

DATA1:  .data 10, -20, 30, 40
DATA2:  .data 100, -50
STR1:   .string "testing extern"
MAT1:   .mat [2][2] 1, 2, 3, 4