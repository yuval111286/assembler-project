




; This is a full test for the assembler

<<<<<<< HEAD
.extern externalfunc
    .extern externalvar

main:   mov #5, r1
cmp r1, #10
jmp end

loop:   add #1, r1
cmp r1, limit
bne loop
red r2

end:    prn r1
jsr externalfunc
mov externalvar, r3


    .data 10, 20, 30
limit:  .data 15

message: .string "Hello World"

    .entry main
    .entry limit
=======
    MAIN:   mov     r3, LENGTH        ; direct addressing
cmp     #-5, r1           ; immediate + register
add     r2, r3            ; register + register
lea     STR, r6           ; direct + register
prn     #48               ; immediate
sub     r4, MATRIX[1][2]  ; register + matrix
clr     r7
not     r2
inc     r1
dec     r0
jmp     END
bne     LOOP
red     VALUE
jsr     FUNC
rts
stop

STR:    .string "hello"
LENGTH: .data 6, -9, 15
MATRIX: .mat [2][2] 1, 2, 3, 4

    .extern VALUE
    .entry MAIN
    .entry STR
    .entry LENGTH

LOOP:   add     r1, r2
sub     MATRIX[0][0], r4

FUNC:   prn     LENGTH

; Invalid lines for error checking:
    BAD1:   .mat [2][2], 4, 5, 6, 7     ; illegal comma before 4
BAD2:   move    r3, r4             ; invalid opcode 'move'
BAD3:   .string hello              ; missing quotes
BAD4:   .data 5,,7                 ; double comma
BAD5:   .extern LENGTH             ; LENGTH already defined

END: stop
>>>>>>> 13b4476 (Update first_pass.c and parser.c)

