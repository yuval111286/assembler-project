; ===== LEGAL INSTRUCTIONS =====

MAIN:   mov r3, r1          ; 2 words: base + registers in same word
        cmp #5, r2          ; 3 words: base + immediate + reg
        add LABEL1, LABEL2  ; 3 words: base + label1 + label2
        lea MAT[1][2], r7   ; 3 words: base + matrix + reg
        clr r4              ; 2 words: base + reg
        stop                ; 1 word

; ===== DIRECTIVES =====

DATA1:  .data 5, -3, 99      ; 3 values
STR1:   .string "Hello"    
MAT1:   .mat [2][2] 1, 2, 3, 4 

; Dummy labels for testing direct addressing
LABEL1: .data 10
LABEL2: .data 20
MAT:    .mat [2][2] 9,8,7,6
