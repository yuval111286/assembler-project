MAIN: mov r1, r2          ; תווית תקינה
Loop1: add #5, r3         ; תווית עם מספר
END_PROGRAM: stop         ; תווית עם קו תחתון
1INVALID: mov r1, r2      ; שגיאה - תווית מתחילה במספר
mov: add r1, r2           ; שגיאה - תווית היא שם הוראה
r1: mov r2, r3            ; שגיאה - תווית היא שם רגיסטר
VeryLongLabelNameThatExceedsThirtyCharacterLimit: mov r1, r2  ; שגיאה - תווית ארוכה מדי