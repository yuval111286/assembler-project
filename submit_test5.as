; mix

.entry START
.entry DATALABEL
.extern EXT_FUNC
.extern EXT_VAR

START:  mov #100, r0
        mov DATALABEL, r1
        add r0, r1
        cmp r1, DATALABEL
        bne LOOP
        jsr EXT_FUNC


LOOP:   mov CODELABEL, r2
        prn r2
        mov MAT[r0][r1], r3
        sub r4, r3
        jmp END


CODELABEL: mov r0, r0 
           rts

DATALABEL: .data 1, 2, 3, 4
MAT:       .mat [2][2] 10, 20, 30, 40

END:    stop
