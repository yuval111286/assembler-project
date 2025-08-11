; directives

.entry TEXT
.extern EXT_DATA

LABEL1: .data 1, -1, 327, 0
TEXT:   .string "assembler project"
MATRIX: .mat [3][2] 1, 2, 3, 4, 5, 6
        mov #7, r1
        jsr EXT_DATA
        stop
