
; LABEL ERRORS


r3: mov r1, r2
mov: add r0, r1
9badlabel: sub r2, r3
dup: clr r1
dup: not r2
labelbeforeextern: .extern extlbl
labelwithchar!: mov r1, r2
label withspace: mov r2, r3
longlabelnameexceeding31charactersabc: mov r0, r1
abc!: mov r1, r2


; DIRECTIVE ERRORS


.extern
.extern goodlabel
.extern goodlabel
.extern bad,label
.extern a, , b
.extern #wrongname
.string hello
.string "bad\0char"
.string ""
.string "ok" extra_text
.data
.data 1,,2,3
.data abc
.data 1, +, 2
.data 9999
.mat [0][2] 1, 2
.mat [-1][3] 1,2,3
.mat 2,3, 1,2,3
.mat [2][3] 1,2,3
.mat [2][2],,1,2
.mat [2][2] a,b,c,d
.mat [2][2] 1 2,3,4
.mat   1,2,3,4
.mat [1][1]
.mat [2][2] 1,2,3 ; missing values


; INSTRUCTION / OPERAND ERRORS


mov #, r1
mov #9999, r1
mov #abc, r2
mov #-513, r0
mov #512, r0
add r1
stop extra_text_here
.entry notdefinedlabel
.extern bothside
.entry bothside
.entry bothside extra
rts trailing_garbage
prn
lea #5, r1
lea MAT[r0][r9], r1
lea MAT[r9][r0], r1
mov MAT[r0] [r1], r0
mov MATr0][r1], r0
mov MAT[r0][r1 extra], r0
mov MAT[r0][rX], r0
mov MAT[rX][r0], r0
jmp #5
cmp r0
add r0, r1, r2
mov ##12, r0
mov #++5, r1
clr #7
red #10


;  FORMATTING / SYNTAX ERRORS


.data 1 ,  2 , , 3
.string "validstring" moretext
matmissingcolon .mat [2][2] 1,2,3,4
matwithspace [2][2]: mov r1, r2
    mov r0, r1
mov r0 r1
cmp , r1


; VALID LINES FOR SANITY CHECK


mainlabel: mov r0, r1
           add dataok, r2
dataok:    .data 1, 2, 3
           stop
