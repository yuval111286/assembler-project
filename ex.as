; Simple test for first pass
MAIN:   mov  #5, r1
        add  r1, r2
LOOP:   prn  r2
        stop

DATA_LABEL: .data 1, 2, 3
STR_LABEL:  .string "abc"
MAT_LABEL:  .mat 2, 2, 10, 20, 30, 40
.extern EXT_LABEL
