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


/*Macro Errors*/
#define LONG_LINE "Line too long more than 80 characters \n"
#define MACRO_MULTI_DEF "Multi definitions for the same MACRO \n"
#define ILLEGAL_MACRO_NAME "Macro name can not be instruction, directive, register " \
                          "or char different than letters, digits and underscore \n"
#define MACRO_NAME_AS_LABEL "MACRO name is the same as exising label name"
#define MACROEND_WITHOUT_START "mcroend without mcro start \n"
#define MACRO_WITHOUT_NAME "MCRO name is missing \n"
#define EXTRA_TEXT_AFTER_MCRO_START "Extra text after macro name \n"
#define EXTRA_TEXT_AFTER_MCROEND "Extra text after macro end \n"
#define FAIL_EXTRACT_MACROS "Fail to extract macros from as file\n"
#define FAIL_TO_SWITCH_MCRO_NAME "Fail to switch mcro name by it's content\n"




/*Internal errors*/

#define FILE_NOT_OPEN_READING "Can not open file for reading \n"
#define FILE_NOT_OPEN_WRITING "Can not open file for writing \n"
#define MEMORY_FAIL "Fail to allocate memory \n"
#define ARG_NOT_AS_FILE "File does not .as file \n"
#define FAIL_CLEAN_FILE "Failed to create clean file with no spaces, empty line or note line \n"
#define FAIL_TO_SET_POSITION_IN_FILE "Failed to set file position\n"

/* First Pass Parsing Errors */
#define SYNTAX_ERROR "Syntax error"
#define INVALID_LABEL_NAME "Invalid label name \n"
#define DUPLICATE_LABEL "Label already defined \n"
#define FAILED_ADD_INSTRUCTION_LABEL "Failed to add instruction label \n"
#define FAILED_ADD_DATA_LABEL "Failed to add data label \n"
#define ILLEGAL_EXTERN_LABEL "Label cannot be defined for .extern \n"
#define INVALID_INSTRUCTION_OPERANDS "Invalid instruction operands \n"
#define EXTERN_SYNTAX_ERROR "Invalid .extern syntax \n"
#define DUPLICATE_EXTERN "Duplicate .extern definition \n"

#define INVALID_MATRIX_DIMENSIONS "Invalid matrix dimensions in .mat directive"
#define MATRIX_VALUE_COUNT_MISMATCH "Number of matrix values does not match dimensions"
#define DATA_IMAGE_OVERFLOW "Data image overflow – too much .data/.string/.mat content"
#define CODE_IMAGE_OVERFLOW "Code image overflow – exceeded MAX_CODE_SIZE\n"
#define EXTRANEOUS_TEXT_AFTER_COMMAND "Extraneous text after end of command"


#define REGISTER_NAME_AS_LABEL "Register name cannot be used as label"
#define RESERVED_WORD_AS_LABEL "Reserved word cannot be used as label"
#define STRING_MISSING_QUOTES "String must be enclosed in quotation marks"
#define MISSING_OPERAND_EXTERN "Missing operand for .extern directive"
#define MISSING_OPERANDS_DATA "Missing operands for .data directive"
#define MATRIX_DIMENSION_FORMAT "[n][m] format is required for .mat dimensions"
#define MULTIPLE_COMMAS "Multiple consecutive commas"
#define TOTAL_MEMORY_OVERFLOW "Total memory exceeded: IC + DC > 255\n"





/* Second Pass Parsing Errors */

#define ENTRY_LABEL_NO_DEF "Entry label was not defined"
#define SAME_NAME_ENTRY_AND_EXTERNAL_LABEL "Same label is external and entry"


#endif /*PROJECT_ERRORS_HANDLER_H*/
