#include "utils.h"
#include "errors_handler.h"
#include <ctype.h>


void *malloc_allocation(size_t size) {
    void *ptr = malloc(size); /* allocate memory */

    if (ptr == NULL) {
        /*error fail to allocate memory*/
        printf(MEMORY_FAIL);
        return NULL;
    }

    /* return pointer */
    return ptr;
}


char *change_ending_of_file(char *file_name, char *new_ending) {
    char *new_file_name, *ch;

    new_file_name = malloc_allocation(MAX_LINE_LENGTH * sizeof(char));
    /*puts the current file name into new file name*/
    strcpy(new_file_name, file_name);

    /* cuts the exising ending of a file  */
    if ((ch = strchr(new_file_name, '.')) != NULL) {
        *ch = '\0';
    }
    /* link the new file name and the new ending */
    strcat(new_file_name, new_ending);

    return new_file_name;
}

char *cut_spaces_before_and_after_string(char *str) {
    char *end_pointer_of_string;

    /* Skip leading spaces */
    while (isspace(*str)) str++;

    /* If string is empty after trimming spaces at start */
    if (*str == '\0') return str;

    /* Move to the end and trim trailing spaces */
    end_pointer_of_string = str + strlen(str) - 1;
    while (end_pointer_of_string > str && isspace(*end_pointer_of_string)) {
        *end_pointer_of_string = '\0'; /* Cut off trailing spaces */
        end_pointer_of_string--;
    }

    return str;
}