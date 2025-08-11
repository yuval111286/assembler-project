; file t1_macros.as

    .entry START
    .extern EXT1

mcro BUMP
inc r2
mov M[r2][r3], r4
mcroend

START:      lea     MSG, r0
mov     COUNT, r1

mov     r4, EXT1
prn     #10
stop


DOSOMETHING:      add     #3, r1
mov     M[r1][r2], r3
rts

MSG:        .string "hello world"
COUNT:      .data   5
M:          .mat    [2][2]  9,8,7,6