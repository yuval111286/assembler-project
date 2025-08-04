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

int parse_matrix_dimensions(const char *token, int *rows, int *cols) {
    char cleaned[MAX_LINE_LENGTH];
    int len;

    if (token == NULL || rows == NULL || cols == NULL) {
        return 0;
    }

    /* Copy and sanitize token */
    strncpy(cleaned, token, MAX_LINE_LENGTH - 1);
    cleaned[MAX_LINE_LENGTH - 1] = '\0';

    /* Remove trailing commas or spaces */
    len = strlen(cleaned);
    while (len > 0 && (cleaned[len - 1] == ',' || isspace((unsigned char)cleaned[len - 1]))) {
        cleaned[len - 1] = '\0';
        len--;
    }

    /* Now extract dimensions */
    if (sscanf(cleaned, "[%d][%d]", rows, cols) != 2) {
        return 0;
    }

    if (*rows <= 0 || *cols <= 0) {
        return 0;
    }

    return 1;
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