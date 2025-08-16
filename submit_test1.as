

.entry STR
.entry LENGTH
.extern EXT1

MAIN:   mov r3, LENGTH
        add r2, r3
        jsr EXT1
        stop

STR:    .string "abcdef"
LENGTH: .data 6, -9, 15
M:      .mat [2][2] 1, 2, 3, 4
