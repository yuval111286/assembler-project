;  Preprocessor macro errors 

mcro                
endmcro              

mcro bad name        
mcro r1              
mcro mov             
mcro .data           
mcro 1badmacro       
mcro bad-char!       
mcro longmacroname_exceeding_length_limit_abc 

mcro DUPMACRO
mov r0, r1
endmcro

mcro DUPMACRO        
add r1, r2
endmcro

mcro NESTED
mcro INNER           
rts
endmcro
endmcro

mcro NOEND
prn r0               

; Macro usage before definition 
USE_BEFORE_DEF
mcro USE_BEFORE_DEF
add r0, r1
endmcro

; Macro invocation errors 
UNKNOWN_MACRO_CALL   
mcro EMPTYMACRO
endmcro
EMPTYMACRO extra     

; Comments and spacing issues 
    mcro SPACE_BEFORE
mov r0, r1
endmcro
mcroTAB\tNAME        
mcro
endmcro
mcro ENDSPACE        
mov r0, r1
endmcro        

; Valid macros for sanity check 
mcro GOOD1
add r0, r1
sub r1, r2
endmcro

mcro GOOD2
prn #5
stop
endmcro

;  Using valid macros 
GOOD1
GOOD2
