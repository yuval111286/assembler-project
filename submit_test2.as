.entry START
.entry DATA1
.extern EXTFUNC
.extern EXTVAR

START:  mov #5, r0
        mov DATA1, r1
        add r0, r1
        cmp r1, DATA2
        bne LOOP
        jsr EXTFUNC
        lea STR1, r2
        prn r2
        prn #123
        clr r3
        not r3
        inc r3
        dec r3
        mov r3, r4
        cmp r4, r5
        lea MAT1, r6
        add r6, r7
        sub r7, r0
        jmp END

LOOP:   mov CODELBL, r2
        prn r2
        mov MAT1[r0][r1], r3
        sub r4, r3
        cmp #42, r5
        bne EXTVAR
        jsr EXTVAR
        mov r6, r7
        add r7, r0
        sub r0, r1
        cmp r1, r2

CODELBL: mov r0, r0
         rts

DATA1:  .data 1, -2, 300
DATA2:  .data 0, 5, -5
STR1:   .string "Hi"
MAT1:   .mat [2][2], 10, 20, 30, 40

END:    stop
