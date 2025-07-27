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
void external_error_log(char *error_msg, SourceFilelocation file);

/**
 * @brief Prints an internal error message.
 *
 * Prints to stdout only errors discovered during internal process
 * @param error_msg error message
 */
void internal_error_log(char *error_msg);

/*list of errors*/
#define Error() "errorExample"
#define MACRO_MULTI_DEF()"Multi definitions for the same MACRO"



#endif /*PROJECT_ERRORS_HANDLER_H*/
