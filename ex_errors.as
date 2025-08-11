; =========================
; Label errors
; =========================

r3: mov r1, r2
; ^ REGISTER_NAME_AS_LABEL

mov: add r0, r1
; ^ RESERVED_WORD_AS_LABEL

1badlabel: sub r2, r3
; ^ INVALID_LABEL_NAME (Starts with a digit)

dup: clr r1
dup: not r2
; ^ DUPLICATE_LABEL

label_before_extern: .extern extlbl
; ^ ILLEGAL_EXTERN_LABEL (A label cannot precede a .extern directive)

; =========================
; Directive errors
; =========================

.extern
; ^ MISSING_OPERAND_EXTERN

.extern good
.extern good
; ^ DUPLICATE_EXTERN

.extern a, , b
; ^ EXTERN_SYNTAX_ERROR (Multiple commas / empty operand)

.string hello
; ^ STRING_MISSING_QUOTES

.data
; ^ MISSING_OPERANDS_DATA

.data 1,,2,3
; ^ MULTIPLE_COMMAS

.mat [0][2] 1, 2
; ^ INVALID_MATRIX_DIMENSIONS (Non-positive dimensions)

.mat 2,3, 1,2,3
; ^ MATRIX_DIMENSION_FORMAT (Missing [n][m] format brackets)

.mat [2][3] 1,2,3
; ^ MATRIX_VALUE_COUNT_MISMATCH (Expected 6 values)

; =========================
; Instruction/operand errors
; =========================

mov #, r1
; ^ INVALID_INSTRUCTION_OPERANDS (Invalid immediate – “#” without a number)

add r1
; ^ INVALID_INSTRUCTION_OPERANDS (Operand count does not match expectation)

stop extra_text_here
; ^ EXTRANEOUS_TEXT_AFTER_COMMAND

.entry not_defined_label
; ^ ENTRY_LABEL_NO_DEF

.extern both_side
.entry both_side
; ^ SAME_NAME_ENTRY_AND_EXTERNAL_LABEL

; Additional examples to trigger other checks:

rts trailing_garbage
; ^ EXTRANEOUS_TEXT_AFTER_COMMAND

prn
; ^ INVALID_INSTRUCTION_OPERANDS (Missing operand)

lea #5, r1
; ^ INVALID_INSTRUCTION_OPERANDS (If LEA does not support immediate as source)

; =========================
; More examples to trigger convenient error validations
; =========================

mov ##12, r0
; ^ INVALID_INSTRUCTION_OPERANDS (Illegal immediate format)

add r0, r1, r2
; ^ INVALID_INSTRUCTION_OPERANDS or EXTRANEOUS_TEXT_AFTER_COMMAND (Extra operand)

.entry both_side extra
; ^ EXTRANEOUS_TEXT_AFTER_COMMAND (Also for directives)

.string "ok   ;"  more
; ^ EXTRANEOUS_TEXT_AFTER_COMMAND (Extra text after a valid string)

; A few valid lines at the end to confirm processing doesn’t stop immediately:

main:    mov r0, r1
         stop