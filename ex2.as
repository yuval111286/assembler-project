;label: .data 10         ; שגיאה - @ לא חוקי
;STR: .string "abcdef"  ; שגיאה - @ לא חוקי

MAT1: .mat [3][2t]
;MAT2: .mat [2][2] 1, 2, 3, 4    ; מטריצה מאותחלת
;add M1[r2][r3], r4 ; test fail

    .entry LOOP
    .entry LENGTH

    .extern L3
    .extern W

