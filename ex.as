; demo: larger, valid, predictable .ob
.entry STR
.entry LEN
.extern EXT1

MAIN:   rts
        stop
AGAIN:  rts
        stop

STR:    .string "abc"
LEN:    .data 6, 9, 15
M:      .mat [2][2] 1, 2, 3, 4
