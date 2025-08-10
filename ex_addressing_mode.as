; Test file for addressing mode validation
; This file contains examples of both valid and invalid addressing modes
; to test the new validation functions

; === VALID INSTRUCTIONS ===
; Valid: mov source=direct(1), dest=register(3)
MAIN: mov LABEL1,r1
; Valid: cmp source=immediate(0), dest=register(3)
cmp #5,r2
; Valid: add source=register(3), dest=register(3)
add r1,r2
; Valid: lea source=direct(1), dest=register(3) - WRONG! lea dest can't be register
lea MATRIX1,r3
; Valid: prn dest=immediate(0) - only prn allows immediate dest
prn #-100
; Valid: jmp dest=direct(1)
jmp LABEL2

; === INVALID INSTRUCTIONS (should generate errors) ===
; INVALID: mov can't have immediate destination
ERROR1: mov #5,#6
; INVALID: add can't have immediate source
ERROR2: add #5,r1
; INVALID: sub can't have immediate source
ERROR3: sub #10,LABEL1
; INVALID: jmp can't have immediate destination
ERROR4: jmp #100
; INVALID: clr can't have immediate destination
ERROR5: clr #5
; INVALID: inc can't have immediate destination
ERROR6: inc #1
; INVALID: lea can't have register source or dest
ERROR7: lea r1,r2
; INVALID: lea can't have immediate source
ERROR8: lea #5,LABEL1

; === MATRIX ADDRESSING TESTS ===
; Valid: matrix with valid registers
VALID_MAT: mov MATRIX1[r2][r3],r1
; INVALID: r8 doesn't exist
INVALID_MAT1: mov MATRIX1[r8][r1],r2
; INVALID: xyz is not a register
INVALID_MAT2: mov MATRIX1[r1][xyz],r2
; INVALID: missing closing bracket
INVALID_MAT3: mov MATRIX1[r1,r2
; === DATA DEFINITIONS ===
LABEL1: .data 10,20,30
LABEL2: .data 50
MATRIX1: .mat [3][3] 1,2,3,4,5,6,7,8,9

; === END ===
END: stop