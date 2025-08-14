; ==================== ENTRY DIRECTIVE ERRORS ====================

; Erro 1: Entry label that was never defined  the file
    .entry NEVERDEFINED

; Erro 2: Entry label pointing to an external symbol (forbidden)
    .extern EXTERNALFUNC
    .entry EXTERNALFUNC

; Erro 3: Same symbol declared as both entry and extern (conflict)
    .extern CONFLICTSYM
    .entry CONFLICTSYM

; ==================== UNDEFINED SYMBOL REFERENCES ====================

; Erro 4: Direct addressing to undefine symbol
MAIN: mov UNDEFINEDVAR, r1

; Erro 5: Jump instruction to undefine label
jmp MISSINGLABEL

; Erro 6: Branch instruction to undefine label
bne UNDEFINEDBRANCH

; Erro 7: JSR to undefine subroutine
jsr MISSINGSUBROUTINE

; Erro 8: LEA instruction wit undefine address
lea UNDEFINEDADDRESS, r2

; Errr 9: Two-operand instruction wit undefine source
add UNDEFINEDSRC, r3

; Errr 10: Two-operand instruction wih undefine destination
sub r4, UNDEFINEDDEST

; Erro 11: Compare instruction wih undefine operands
cmp UNDEFINEDLEFT, UNDEFINEDRIGHT

; ==================== MATRIX ADDRESSING ERRORS ====================

; Erro 12: Matrix wih undefine base address - single operand instruction
inc UNDEFINEDMATRIX[r1][r2]

; Erro 13: Matrix wih undefine base address - two operand instruction source
mov UNDEFINEDMAT[r3][r4], r5

; Errr 14: Matrix wih undefine base address - two operand instruction destination
add r6, MISSINGMAT[r0][r1]

; Erro 15: Matrix i LEA instruction wit undefine base
lea UNDEFINEDLEAMAT[r2][r3], r7

; ==================== MIXED VALID/INVALID REFERENCES ====================

; Erro 16: Valid symbol mixed wth invalid i same instruction
VALIDSYMBOL: .data 100
mov VALIDSYMBOL INVALIDSYMBOL

; Erro 17: Multiple undefind symbols i same instruction
cmp UNDEFA, UNDEFB

; ==================== SINGLE OPERAND INSTRUCTION ERRORS ====================

; Erro 18: CLR wit undefned symbol
clr UNDEFINEDCLR

; Errr 19: NOT wit undefine symbol
not UNDEFINEDNOT

; Errr 20: DEC wit undefine symbol
dec UNDEFINEDDEC

; Erro 21: RED wit undefine symbol
red UNDEFINEDRED

; Erro 22: PRN wit undefine symbol
prn UNDEFINEDPRN

; ==================== COMPLEX ADDRESSING MODE ERRORS ====================

; Erro 23: Immediate addressing is valid, but testing undefind i direct mode
mov #5, UNDEFINEDIMMEDIATEDEST

; Errr 24: Register addressing is valid, testing wih undefind direct
add r1, UNDEFINEDREGDEST

; ==================== ENTRY DECLARATIONS FOR TESTING ====================

; These should generate "entry label not defined" errors
    .entry ANOTHERMISSING
    .entry YETANOTHERUNDEFINED
    .entry FINALMISSINGLABEL

; ==================== VALID CODE TO PASS FIRST PASS ====================

; Valid symbols to ensure first pass succeeds
REALLABEL: mov r1, r2
stop

VALIDDATA: .data 42, -17, 0
VALIDSTR: .string "test"
VALIDMAT: .mat [2][2] 1, 2, 3, 4

; Valid extern declaration (should not cause errors)
.extern REALEXTERNAL

; Using valid externl symbol (should not cause errors n second pass)
mov r0, REALEXTERNAL