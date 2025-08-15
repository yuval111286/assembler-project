.entry BEGIN
.entry STRING1
.entry MATRIXA
.entry FINISH
.extern EXT1
.extern EXT2
.extern LABELX

BEGIN:  mov #200, r0
        mov LISTA, r1
        add r0, r1
        cmp r1, LISTB
        bne LOOP1
        jsr EXT1
        lea STRING1, r2
        prn r2
        prn #99
        clr r3
        not r3
        inc r3
        dec r3
        jmp LOOP1
        bne MATRIXA
        red r4
        sub r4, r5
        mov MATRIXA[r0][r1], r6
        cmp #45, r6
        bne LABELX
        jsr EXT2
        mov r7, r0
        add r0, r1
        sub r1, r2
        cmp r2, r3
        lea LISTB, r4
        lea MATRIXB, r5
        jmp LOOP2

LOOP1:  mov CODE1, r2
        prn r2
        mov MATRIXB[r0][r1], r3
        sub r4, r3
        add MATRIXA[r3][r0], r5
        cmp #15, r5
        bne LABELX
        jsr EXT1
        mov r6, r7
        add r7, r0
        sub r0, r1
        cmp r1, r2
        lea LISTC, r4
        lea MATRIXB, r5
        jmp FINISH

LOOP2:  mov CODE2, r2
        prn r2
        mov MATRIXB[r0][r1], r3
        sub r4, r3
        add MATRIXA[r3][r0], r5
        cmp #8, r5
        bne LABELX
        jsr EXT2
        mov r6, r7
        add r7, r0
        sub r0, r1
        cmp r1, r2
        lea LISTA, r4
        lea MATRIXA, r5

CODE1:  mov r0, r0
        rts

CODE2:  mov r1, r1
        rts

LISTA:   .data 5, 10, 15, -20, 35
LISTB:   .data -1, -2, -3, -4, -5
LISTC:   .data 100, 200, 300, 400
STRING1: .string "Test File"
STRING2: .string "Example"
MATRIXA: .mat [2][3] 1, 2, 3, 4, 5, 6
MATRIXB: .mat [3][2] 7, 8, 9, 10, 11, 12
TEXT1:   .string "Assembler Data"
TEXT2:   .string "Final Check"

FINISH:  stop
