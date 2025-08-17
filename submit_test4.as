; obj_and_ent

.entry MAIN 
.entry STR

MAIN:   mov #10, r1
        add r1, r2
        cmp #5, r2
        bne LOOP
        stop

LOOP:   inc r3
        dec r3
        jmp MAIN

STR:    .string "assembler"
NUMS:   .data 1, 2, 3, 4, 5
MAT2:   .mat [2][3] 1, 2, 3, 4, 5, 6
