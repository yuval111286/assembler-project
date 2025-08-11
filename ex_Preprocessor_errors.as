

mcro bad%name
mov r1, r2
mcroend
; ^ ILLEGAL_MACRO_NAME (Illegal characters in macro name)

mcro good extra text after name
mov r0, r1
mcroend
; ^ EXTRA_TEXT_AFTER_MCRO_START

mcro same
add r1, r2
mcroend

mcro same
sub r3, r4
mcroend
; ^ MACRO_MULTI_DEF (Two different definitions for the same macro name)

mcro badend
inc r5
mcroend trailing
; ^ EXTRA_TEXT_AFTER_MCROEND

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
; ^ LONG_LINE (Line exceeds 80 characters)

