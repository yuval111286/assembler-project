; Comprehensive Error Test File for Assembler
    ; Each section tests different types of errors

; ========== SECTION 1: SYNTAX ERRORS ==========

; Missing comma between operands
START:  mov r1 r2

; Extra comma at the end
add r1, r2,

; Multiple commas
sub r1,, r2

; Missing operand after comma
cmp r1,

; Comma but no first operand
mov , r2

; Label without colon
LABEL1  inc r1

; Empty label
    :       mov r1, r2

; ========== SECTION 2: INVALID OPERAND TYPES ==========

; Immediate as destination (invalid for most instructions)
mov r1, #5
add r2, #10
sub r3, #-7
inc #5
clr #0

; Invalid source for lea (only direct/index allowed)
lea #5, r1
lea r1, r2

; Invalid addressing for jump instructions
jmp #100
bne r1
jsr #50

; ========== SECTION 3: WRONG NUMBER OF OPERANDS ==========

