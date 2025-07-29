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
 */
void external_error_log(char *error_msg, SourceFileLocation file);

/**
 * @brief Prints an internal error message.
 *
 * Prints to stdout only errors discovered during internal process
 * @param error_msg error message
 */
void internal_error_log(char *error_msg);

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




#endif /*PROJECT_ERRORS_HANDLER_H*/
