

#ifndef ASSEMBLER_PROJECT_USER_INTERFACE_H
#define ASSEMBLER_PROJECT_USER_INTERFACE_H



/*user interface*/
#define WELCOME "\n"\
"            MAMAN 14 ASSEMBLER PROJECT - YUVAL & LIHI \n"\
"\n"\
"     Please enter input files: ./assembler <file1> <file2> ... \n"\
"     Machine language contains 8 registers (r0-r7)\n"\
"     Opcodes : mov, cmp, add, sub, lea, clr, not, inc, dec, jmp\n"\
"     bne, red, prn, jsr, rts, and stop\n"\
"     Generates output files if no errors: .am .ob .ent .ext \n"\
"     Errors shall be printed to the screen\n\n"
#define LONG_FILE_NAME "File name too long, moving to next file %s \n\n"
#define PREPROC "-- Processing PREPROCESSOR --\n"
#define FIRST_PASS  "-- Processing FIRST PASS --\n"
#define SECOND_PASS "-- Processing SECOND PASS --\n"
#define FINISH "Finish processing file %s \n"
#define GOODBYE "\nNo more files, end of assembler program. \nHope you enjoyed!\n"




#endif
