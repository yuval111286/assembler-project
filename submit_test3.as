; only_obj

MAIN:   mov #5, r1
        add r1, r2
        sub r2, r3
        clr r4
        not r4
        inc r1
        dec r2
        cmp r1, r3
        bne LOOP
        stop

LOOP:   lea DATA1, r6
        add r6, r5
        prn r5
        jmp END


END:    rts

DATA1:  .data 7, -3, 15, 42, 99, -256, 0
DATA2:  .data 100, 200, -300

STR1:   .string "hello"
STR2:   .string "assembler test"

MAT1:   .mat [2][2] 1, 2, 3, 4
MAT2:   .mat [3][3] 9, 8, 7, 6, 5, 4, 3, 2, 1

