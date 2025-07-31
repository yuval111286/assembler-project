#ifndef PROJECT_ERRORS_HANDLER_H
#define PROJECT_ERRORS_HANDLER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "symbol_table.h"



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


/**
 * @brief Prints an internal error message.
 *
 * Prints to stdout only errors discovered during internal process
 * @param error_msg error message
 */
/* void internal_error_log(char *error_msg);*/

/*list of errors*/
#define Error "errorExample"

/*macro errors*/
#define MACRO_MULTI_DEF "Multi definitions for the same MACRO"
#define ILLEGAL_MACRO_NAME "Macro name can not be instruction, directive or register"
#define MACRO_REPEAT_DEFINITION "MACRO is define more than once"
#define MACROEND_WITHOUT_START "mcroend without mcro start"
#define MACRO_WITHOUT_NAME "MCRO name is missing"
#define EXTRA_TEXT_AFTER_MCRO_START "Extra text after macro name"
#define EXTRA_TEXT_AFTER_MCROEND "Extra text after macro end"
#define FAIL_EXTRACT_MACROS "Fail to extract macros from as file\n"
#define FAIL_TO_SWITCH_MCRO_NAME "Fail to switch mcro name by it's content\n"


/*internal errors*/

#define FILE_NOT_OPEN_READING "Can not open file for reading"
#define FILE_NOT_OPEN_WRITING "Can not open file for writing"
#define MEMORY_FAIL "Fail to allocate memory"
#define ARG_NOT_AS_FILE "File does not as file \n"
#define FAIL_CLEAN_FILE "Failed to create clean file with no spaces, empty line or note line \n"
#define FAIL_TO_SET_POSITION_IN_FILE "Failed to set file position\n"

/* first Pass Parsing Errors */
#define SYNTAX_ERROR "Syntax error"
#define INVALID_LABEL_NAME "Invalid label name"
#define LABEL_RESERVED_WORD "Label name cannot be opcode, directive or register"
#define DUPLICATE_LABEL "Label already defined"
#define FAILED_ADD_INSTRUCTION_LABEL "Failed to add instruction label"
#define FAILED_ADD_DATA_LABEL "Failed to add data label"
#define ILLEGAL_EXTERN_LABEL "Label cannot be defined for .extern"
#define INVALID_INSTRUCTION_OPERANDS "Invalid instruction operands"
#define EXTERN_SYNTAX_ERROR "Invalid .extern syntax"
#define DUPLICATE_EXTERN "Duplicate .extern definition"





#endif /*PROJECT_ERRORS_HANDLER_H*/
