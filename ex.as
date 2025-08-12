MAT3: .mat [1][4] -1, 0, +1, 2
MAT4: .mat [2][3]
; error - too many values
    .mat [2][2] 1, 2, 3, 4, 5
; error - zero dimension
    .mat [0][2] 1, 2
    .mat [a][2] 1, 2
    .mat [2[2] 1, 2, 3, 4
LABEL: .entry MAIN
mov r1, #5
; error - immediate cannot be source in MOV
mov #10, r1
; CMP instruction tests (addressing modes 0,1,2,3 -> 0,1,2,3)
; immediate to immediate
cmp #5, #10
; immediate to register
cmp #3, r1
; immediate to label
cmp #7, LABEL3
; immediate to matrix
cmp #2, MAT1[r1][r2]
; register to register
cmp r1, r2
; label to label
cmp LABEL1, LABEL2
; matrix to matrix
cmp MAT1[r1][r2], MAT2[r3][r4]

; ADD/SUB instruction tests (addressing modes 0,1,2,3 -> 0,1,2,3)
; immediate to register
add #5, r1
; register to register
add r1, r2
; label to register
add LABEL1, r3
; matrix to register
add MAT1[r1][r2], r4
; immediate to label
sub #10, LABEL1
; register to matrix
sub r1, MAT1[r2][r3]

; LEA instruction tests (addressing modes 1,2 -> 1,2)
; label to label
lea LABEL1, LABEL2
; matrix to label
lea MAT1[r1][r2], LABEL3
; label to matrix
lea LABEL1, MAT2[r3][r4]
; error - immediate not valid in LEA
lea #5, LABEL1
; error - immediate not valid as destination in LEA
lea LABEL1, #10
; Single operand instruction tests (addressing modes 1,2,3)
; register
clr r1
; label
clr LABEL1
; matrix
clr MAT1[r1][r2]
; error - immediate not valid
clr #5
; register
not r2
; label
not LABEL2
; matrix
inc MAT2[r3][r4]
; register
dec r5
; jump to matrix
jmp MAT1[r1][r2]
; subroutine call
jsr SUBROUTINE
; read input
red r1
; read to label
red LABEL1

; PRN instruction tests (addressing modes 0,1,2,3)
; print immediate
prn #42
; print register
prn r1
; print label
prn LABEL1
; print matrix
prn MAT1[r1][r2]

; No operand instruction tests
; return from subroutine
rts
; stop program
stop
; error - RTS takes no operands
rts r1

; Addressing mode tests
; Immediate addressing
; positive immediate
mov r1, #5
; negative immediate
mov r1, #-10
; immediate with explicit positive sign
mov r1, #+15
mov r1, #
; error - missing #
mov r1, 5
; Matrix addressing
; valid matrix
mov r1, MATRIX[r2][r3]
; error - too many indices
mov r1, MATRIX[r1][r2][r3]
; valid - spaces inside brackets
mov r1, MATRIX[ r1 ][ r2 ]

; Register addressing
; valid registers (r0-r7)
mov r1, r2
; error - r8 doesn't exist
mov r8, r1
; error - ra doesn't exist
mov ra, r1
; error - registers must be lowercase
mov R1, r2






