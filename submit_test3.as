.entry START
.entry DATASET1
.entry MSG1
.entry BIGMAT1
.entry FINISH
.extern OUTLABEL
.extern EXTCODE
.extern EXTNUM

START:  mov #100, r0
        mov DATASET1, r1
        add r0, r1
        cmp r1, DATASET2
        bne LOOPA
        jsr OUTLABEL
        lea MSG1, r2
        prn #123
        clr r3
        not r3
        mov r3, r4
        jmp LOOPB

LOOPA: mov CODEA, r2
        prn r2
        mov BIGMAT1[r0][r1], r3
        sub r4, r3
        cmp #50, r3
        bne EXTCODE
        jsr EXTCODE
        lea DATASET3, r4
        jmp LOOPC

LOOPB: mov CODEB, r2
        prn r2
        mov BIGMAT2[r0][r1], r3
        sub r4, r3
        cmp #42, r3
        bne OUTLABEL
        jsr OUTLABEL
        lea DATASET4, r4
        jmp LOOPD

LOOPC: mov CODEC, r2
        prn r2
        mov BIGMAT3[r0][r1], r3
        cmp #99, r3
        bne EXTNUM
        jsr EXTNUM
        jmp FINISH

LOOPD: mov CODED, r2
        prn r2
        mov BIGMAT4[r0][r1], r3
        cmp #7, r3
        bne EXTNUM
        jsr OUTLABEL

CODEA:  rts
CODEB:  rts
CODEC:  rts
CODED:  rts

DATASET1: .data 10, 20, 30
DATASET2: .data -1, -2, -3
DATASET3: .data 100, 200
DATASET4: .data 0, 1

MSG1:    .string "Extended Test"

BIGMAT1: .mat [2][2], 1, 2, 3, 4
BIGMAT2: .mat [2][2], 5, 6, 7, 8
BIGMAT3: .mat [2][2], 9, 10, 11, 12
BIGMAT4: .mat [2][2], 13, 14, 15, 16

FINISH:  stop
