#include "errors_handler.h"

void internal_error_log(char *error_msg) {
    /* Print the error message */
    printf("--ERROR--: %s\n", error_msg);
}

void external_error_log(char *error_msg, SourceFilelocation file) {
    /* Print the file name, line number and the error message */
    printf("--ERROR--: in the file %s -- at line:%d . The error is: %s\n",
           file.file_name, file.line, error_msg);
}