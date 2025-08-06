
mov: .data 5, 6


r1: .data 1, 2


1Label: .data 3, 4


GOOD_LABEL: .data 7, 8


GOOD_LABEL: .string "duplicate"


mov r1, r2, r3


EMPTY_DATA: .data


BAD_DATA: .data 1,,2


BAD_STRING: .string "Unclosed string


BAD_STRING2: .string missing closing"


MAT1: .mat [2][2] 1, 2, 3


MAT2: .mat [2,2] 1, 2, 3, 4


MAT3: .mat [2][2], 1, 2, 3, 4


MAT4: .mat [2][2] 1,,2,3,4


MAT5: .mat [2][2] 1, 2, 3, 4,


MAT6: .mat [1][3] 1, 2


.extern


.extern EXT1


.extern EXT1
