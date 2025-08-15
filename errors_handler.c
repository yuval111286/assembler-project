#include "errors_handler.h"


void error_log(char *file_name, int line, char *error_msg) {
    /* Print the file name line number and the error message */
    printf(ERROR_STRUCT,file_name, line, error_msg);
}

