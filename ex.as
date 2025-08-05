
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