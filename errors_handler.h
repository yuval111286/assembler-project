#ifndef PROJECT_ERRORS_HANDLER_H
#define PROJECT_ERRORS_HANDLER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "symbol_table.h"
#include "utils.h"
#include <stdarg.h>

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
#define MACRO_WITHOUT_NAME "MCRO without name"
#define EXTRA_TEXT_AFTER_MCRO_START "Extra text after macro name"
#define EXTRA_TEXT_AFTER_MCROEND "Extra text after macro end"


/*internal errors*/

#define FILE_NOT_OPEN_READING "Can not open file for reading"
#define FILE_NOT_OPEN_WRITING "Can not open file for writing"
#define MEMORY_FAIL "Fail to allocate memory"



#endif /*PROJECT_ERRORS_HANDLER_H*/
