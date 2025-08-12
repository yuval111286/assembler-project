; test
; valid
MAIN: mov r1, r2
; label with number
    Loop1: add #5, r3
; label with underscore
    END_PROGRAM: stop
    ; error - label starts with number
    1INVALID: mov r1, r2
; error - label is instruction name
mov: add r1, r2
; error - label is register name
r1: mov r2, r3
; error - empty after label
LABEL:

    ; .data directive tests
; valid
DATA1: .data 10
; numbers with signs
    DATA2: .data 5, -3, +15
; long list
DATA3: .data 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
; error - empty after .data
    .data
; error - double comma
    .data 1,, 2
; error - comma at end
    .data 1, 2,
; error - not a number
    .data abc
; error - missing comma
    .data 1 2

; .string directive tests
; valid string
STR1: .string "hello"
; string with spaces
    STR2: .string "Hello World!"
; string with numbers
    STR3: .string "123"
; empty string
STR4: .string ""
; error - no quotes
    .string hello
; error - missing closing quote
    .string "hello
; error - missing opening quote
    .string hello"
; error - empty after .string
    .string

; .mat directive tests
; valid initialized matrix
MAT1: .mat [2][2] 1, 2, 3, 4
; 3x1 matrix
MAT2: .mat [3][1] 5, 10, 15
; matrix with signed numbers
MAT3: .mat [1][4] -1, 0, +1, 2
; uninitialized matrix
MAT4: .mat [2][3]
; error - not enough values
    .mat [2][2] 1, 2, 3
; error - too many values
    .mat [2][2] 1, 2, 3, 4, 5
; error - zero dimension
    .mat [0][2] 1, 2
; error - negative dimension
    .mat [-1][2] 1, 2
; error - non-numeric dimension
    .mat [a][2] 1, 2
; error - missing opening bracket
    .mat 2][2] 1, 2, 3, 4
; error - missing closing bracket
    .mat [2[2] 1, 2, 3, 4

; .entry and .extern directive tests
; valid
    .entry MAIN
; valid
    .entry LOOP
; valid
    .entry END_PROGRAM
; valid
    .extern EXTERNAL_FUNC
; valid
    .extern OTHER_LABEL
; error - empty after .entry
    .entry
; error - empty after .extern
    .extern
; error - instruction name
    .entry mov
; error - register name
    .extern r1
; warning - label before .entry
LABEL: .entry MAIN

; MOV instruction tests (addressing modes 1,2,3 -> 0,1,2,3)
; register to register
mov r1, r2
; label to register
mov LABEL1, r1
; matrix to register
mov MAT1[r1][r2], r3
; register to immediate (error - immediate cannot be destination)
mov r1, #5
; register to label
mov r1, LABEL2
; register to matrix
mov r1, MAT2[r3][r4]
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
; error - register not valid in LEA
lea r1, r2
; error - immediate not valid as destination in LEA
lea LABEL1, #10
; error - register not valid as destination in LEA
lea LABEL1, r1

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
; jump to label
jmp LOOP_LABEL
; jump to matrix
jmp MAT1[r1][r2]
; conditional branch
bne END_LABEL
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
; error - STOP takes no operands
stop LABEL1

; Addressing mode tests
; Immediate addressing
; positive immediate
mov r1, #5
; negative immediate
mov r1, #-10
; immediate with explicit positive sign
mov r1, #+15
; error - not a number
mov r1, #abc
; error - empty after #
mov r1, #
; error - missing #
mov r1, 5

; Direct addressing (labels)
; valid label
mov r1, SOME_LABEL
; error - invalid label
mov r1, 1INVALID

; Matrix addressing
; valid matrix
mov r1, MATRIX[r2][r3]
; error - r8 doesn't exist
mov r1, MATRIX[r8][r1]
; error - abc is not a register
mov r1, MATRIX[abc][r1]
; error - missing second index
mov r1, MATRIX[r1]
; error - too many indices
mov r1, MATRIX[r1][r2][r3]
; valid - spaces inside brackets
mov r1, MATRIX[ r1 ][ r2 ]
; error - space between brackets
mov r1, MATRIX[r1] [r2]

; Register addressing
; valid registers (r0-r7)
mov r1, r2
; error - r8 doesn't exist
mov r8, r1
; error - ra doesn't exist
mov ra, r1
; error - registers must be lowercase
mov R1, r2

; General error tests
; error - too many operands
mov r1, r2, r3
; error - not enough operands
mov r1
; error - no operands
mov
; error - instruction doesn't exist
invalid_opcode r1, r2
; error - double comma
mov r1,, r2
; error - comma at end
mov r1, r2,
; error - empty operand
mov , r2

; Additional syntax tests
; leading spaces - valid
mov r1, r2
; multiple spaces - valid
mov    r1,    r2
; tabs - valid
mov	r1,	r2