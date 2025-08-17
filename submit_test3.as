; only_obj

MAIN:   mov #5, r1  
        add r1, r2
        sub r2, r3
        clr r4
        not r4
        stop

DATA1:  .data 7, -3, 15, 42
STR1:   .string "hello"
MAT1:   .mat [2][2] 1, 2, 3, 4
