#ifndef PROJECT_ERRORS_HANDLER_H
#define PROJECT_ERRORS_HANDLER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


/**
 * @brief Prints an external error message.
 *
 * Prints to stdout only errors in the source file
 * Prints the name of the file the error and the line number
 * @param error_msg error message
 * @param file pointer to the source file name and line number
 * in case the line is irrelevant will be printed -1
 */
void error_log(char *file_name, int line, char *error_msg);

#define ERROR_STRUCT "--ERROR--: In file %s at line: %d\n          The error is: %s\n"

/*Preprocessor errors*/
#define LONG_LINE "Line is too long, more than 80 characters\n"
#define MACRO_MULTI_DEF "Multi definitions for the same MACRO\n"
#define ILLEGAL_MACRO_NAME "Macro name can not be instruction, directive, register " \
                          "or char different than letters, digits and underscore\n"
#define MACRO_NAME_AS_LABEL "MACRO name is the same as exising label name"
#define MACROEND_WITHOUT_START "mcroend without mcro start\n"
#define MACRO_WITHOUT_NAME "MCRO name is missing\n"
#define EXTRA_TEXT_AFTER_MCRO_START "Extra text after macro name\n"
#define EXTRA_TEXT_AFTER_MCROEND "Extra text after macro end\n"
#define FAIL_EXTRACT_MACROS "Fail to extract macros from as file\n"
#define FAIL_TO_SWITCH_MCRO_NAME "Fail to switch mcro name by it's content\n"


/*Internal errors*/
#define INTERNAL "internal"
#define FILE_NOT_OPEN_READING "Can not open file for reading\n"
#define FILE_NOT_OPEN_WRITING "Can not open file for writing\n"
#define MEMORY_FAIL "Fail to allocate memory\n"
#define ARG_NOT_AS_FILE "File does not .as file\n"
#define FAIL_CLEAN_FILE "Can not create clean file with no spaces, empty line or note line\n"
#define FAIL_TO_SET_POSITION_IN_FILE "Failed to set file position\n"

/* First Pass and Second Pass Errors */

#define IMM_LADDER "Immediate value must start with an #\n"
#define IMM_NUM_AFTER_LADDER "Immediate value must have num after #\n"
#define NOT_DIGIT "Char is not digit\n"
#define NOT_LETTER "Char is not letter\n"
#define NOT_DIGIT_LETTER "Char is not digit or letter\n"
#define MISSING_LABEL_NAME "Missing Label name\n"
#define LONG_LABEL "Label is too long \n"
#define LABEL_STARTS_WITH_LETTER "Label must starts with letter\n"
#define LABEL_LETTER_DIGIT "Label must contain only digits or letters \n"

#define MISSING_INSTRUCTION_ARG "Missing argument for instruction\n"
#define EMPTY_STRING_DIR "String directive can not be empty\n"
#define MANY_OP "Too many operands\n"
#define IMM_NOT_VALID_ARG_DEST_MOV "Immediate is not valid argument for dest in mov instruction\n"
#define IMM_NOT_VALID_ARG_DEST_ADD_SUB "Immediate is not valid argument for dest in add and sub instructions\n"
#define IMM_NOT_VALID_ARG_SRC_LEA "Immediate is not valid argument for source in lea instruction\n"
#define IMM_NOT_VALID_ARG_DEST_LEA "Immediate is not valid argument for dest in lea instruction\n"
#define IMM_NOT_VALID_ARG_DEST_REST_OP "Immediate is not valid argument for dest in clr,not"\
" inc, dec,jmp,bne,jsr and red instructions\n"

#define INVALID_SOURCE_ADDRESSING_MODE "Invalid source operand addressing mode for this instruction\n"
#define INVALID_DEST_ADDRESSING_MODE "Invalid destination operand addressing mode for this instruction\n"
#define INVALID_MATRIX_FORMAT_FIRST_BRACKET "Invalid matrix format - missing first bracket\n"
#define INVALID_MATRIX_FORMAT_FIRST_CLOSING "Invalid matrix format - missing first closing bracket\n"
#define INVALID_MATRIX_FORMAT_SECOND_BRACKET "Invalid matrix format - missing second bracket\n"
#define INVALID_MATRIX_FORMAT_SECOND_CLOSING "Invalid matrix format - missing second closing bracket\n"
#define MATRIX_INVALID_WHITESPACE "Invalid whitespace between matrix register brackets []  []\n"
#define INVALID_MATRIX_FIRST_REGISTER "Invalid register name in matrix first index\n"
#define INVALID_MATRIX_SECOND_REGISTER "Invalid register name in matrix second index\n"
#define INVALID_OPCODE "Invalid opcode name\n"
#define ILLEGAL_DIRECTIVE "Illegal directive name\n"
#define INVALID_LABEL_NAME "Invalid label name\n"
#define EMPTY_AFTER_LABEL "No continue after label name\n"
#define FAIL_CONVERT_STRING_TO_NUM "Can not convert string to num\n"
#define DUPLICATE_LABEL "Label is already defined\n"
#define FAILED_ADD_INSTRUCTION_LABEL "Failed to add instruction label\n"
#define FAILED_ADD_DATA_LABEL "Failed to add data label\n"
#define ILLEGAL_EXTERN_LABEL "Label cannot be defined for .extern\n"
#define INVALID_INSTRUCTION_OPERANDS "Invalid instruction operands\n"
#define EXTERN_SYNTAX_ERROR "Invalid .extern syntax\n"
#define DUPLICATE_EXTERN "Duplicate .extern definition\n"

#define INVALID_MATRIX_DIMENSIONS "Invalid matrix dimensions in .mat directive\n"
#define MATRIX_VALUE_COUNT_MISMATCH "Number of matrix values does not match dimensions\n"
#define DATA_IMAGE_OVERFLOW "Data image overflow – too much .data/.string/.mat content\n"
#define CODE_IMAGE_OVERFLOW "Code image overflow – exceeded MAX_CODE_SIZE\n"
#define EXTRANEOUS_TEXT_AFTER_COMMAND "Extraneous text after end of command\n"
#define INVALID_STRING ".string label values contain invalid char\n"

#define REGISTER_NAME_AS_LABEL "Register name cannot be used as label\n"
#define SAVED_WORD_AS_LABEL "Assemble saved word cannot be used as label name\n"
#define STRING_MISSING_QUOTES "String must be enclosed in quotation marks\n"
#define MISSING_OPERAND_EXTERN "Missing operand for .extern directive\n"
#define MISSING_OPERANDS_DATA "Missing operands for .data directive\n"
#define MATRIX_DIMENSION_FORMAT "[n][m] format is required for .mat dimensions\n"
#define MULTIPLE_COMMAS "It's illegal to have multiple consecutive commas\n"
#define COMMA_END_OF_LINE "Extra comma in the end of the line"
#define TOTAL_MEMORY_OVERFLOW "Total memory exceeded: IC + DC > 255\n"
#define IMMEDIATE_OUT_OF_RANGE "Immediate value out of range (-512 to +511)\n"

#define ENTRY_LABEL_NO_DEF "Entry label was not defined\n"
#define SAME_NAME_ENTRY_AND_EXTERNAL_LABEL "Same label is external and entry\n"




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
#endif /*PROJECT_ERRORS_HANDLER_H*/