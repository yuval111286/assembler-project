CODE1:  mov r0, r0
rts

CODE2:  mov r1, r1
rts

INTERNAL_FUNC: inc r7
rts

INTERNAL_FUNC2: dec r7
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


mov LISTA, r1
add r0, r1
cmp r1, LISTB
bne LOOP1
jsr INTERNAL_FUNC
lea STRING1, r2
prn r2
prn #99
clr r3
not r3
inc r3
dec r3
jmp LOOP1

LOOP1:  mov CODE1, r2
prn r2
mov MATRIXB[r0][r1], r3
sub r4, r3
add MATRIXA[r3][r0], r5
cmp #15, r5
bne LOOP2
jsr INTERNAL_FUNC
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
bne FINISH
jsr INTERNAL_FUNC2
mov r6, r7
add r7, r0
sub r0, r1
cmp r1, r2
lea LISTA, r4
lea MATRIXA, r5
jmp FINISH
