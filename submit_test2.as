; addressing modes

.entry RESULT
.extern EXT_LABEL

START:  mov #5, r0       
        mov DATA, r1     
        mov MAT[r1][r2], r2 
        mov r3, r4       
        cmp #10, DATA    
        add r5, r6
        sub MAT[r3][r4], r7
        mov STR, r0
        stop

DATA:   .data 10, 20, 30
STR:    .string "Hello"
MAT:    .mat [2][3] 1, 2, 3, 4, 5, 6
RESULT: .data 0
