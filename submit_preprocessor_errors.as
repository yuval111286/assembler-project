mcro mov
inc r1
mcroend
mcro r3
add r1, r2
mcroend
mcro data
mov r1, r2
mcroend
mcro mcroend
sub r1, r2
mcroend
mcro valid_name extra text here
clr r1
mcroend
mcro
not r1
mcroend
mcro another_macro
inc r2
mcroend extra text
mcroend
mcro invalid-name
dec r1
mcroend
    mcro 123invalid
prn r1
mcroend
mcro .extern
red r1
mcroend
mcro entry
stop
mcroend
mcroend
MAIN: mov r1, r2
stop