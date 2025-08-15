; errors 

r3: mov r1, r2
mov: add r0, r1
1badlabel: sub r2, r3
dup: clr r1
dup: not r2
labelbeforeextern: .extern extlbl
longlabelnameexceedingmaxlength1234699774: mov r0, r1
invalid-char!: mov r1, r2


; Directive errors

.extern
.extern good
.extern good
.extern a, , b
.string hello
.string "bad\0char"
.data
.data 1,,2,3
.data abc
.mat [0][2] 1, 2
.mat 2,3, 1,2,3
.mat [2][3] 1,2,3
.mat [2][2],,1,2
.mat [2][2] a,b,c,d


; Instruction/operand errors


mov #, r1
mov #9999, r1
add r1
stop extratexthere
.entry notdefinedlabel
.extern bothside
.entry bothside
rts trailinggarbage
prn
lea #5, r1
mov MAT[r9][r1], r0
mov MAT[r0][r9], r0
mov MAT[r0] [r1], r0
mov MATr0][r1], r0
mov MAT[r0][r1 extra], r0



; More invalid formats

mov ##12, r0
add r0, r1, r2
.entry both_side extra
.string "ok   ;" more
cmp r0
jmp #5


; Valid lines for sanity check


main:   mov r0, r1
        add DATA_OK, r2
DATA_OK: .data 1, 2, 3
        stop
