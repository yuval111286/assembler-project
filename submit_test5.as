
.entry START
.entry STRLABEL
.entry MATLABEL
.entry END
.extern EXTFUNC
.extern EXTVAR
.extern OUTSIDELABEL


START:  mov #100, r0           
        mov DATALABEL, r1      
        add r0, r1              
        cmp r1, DATALABEL       
        bne LOOP                
        jsr EXTFUNC            
        lea STRLABEL, r2        
        prn r2                  
        prn #42                 
        clr r3                 
        not r3                 
        inc r3                 
        dec r3                  


        jmp LOOP                
        bne MATLABEL            
        red r4                  
        sub r4, r5               


LOOP:   mov CODELABEL, r2      
        prn r2
        mov MATLABEL[r0][r1], r3 
        sub r4, r3
        add MATLABEL[r3][r0], r5 
        cmp #7, r5               
        bne OUTSIDELABEL        
        jsr EXTFUNC             


        mov r6, r7
        add r7, r0
        sub r0, r1
        cmp r1, r2
        lea DATALABEL, r4
        lea MATLABEL, r5


        jmp END


CODELABEL: mov r0, r0
           rts


DATALABEL: .data 1, -2, 300, -511, 0, 45
STRLABEL:  .string "Hello"
MATLABEL:  .mat [2][3] 10, 20, 30, 40, 50, 60


ARR1:     .data 5, 10, 15
ARR2:     .data -1, -2, -3
TEXT:     .string "Assembler Test"


END:      stop
