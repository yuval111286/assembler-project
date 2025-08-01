#include "utils.h"
#include "errors_handler.h"

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
    char *c, *new_file_name;

    new_file_name = malloc_allocation(MAX_LINE_LENGTH * sizeof(char));
    strcpy(new_file_name, file_name);

    /* cuts the exising ending of a file  */
    if ((c = strchr(new_file_name, '.')) != NULL) {
        *c = '\0';
    }
    /* adds the new ending of the new file name */
    strcat(new_file_name, new_ending);

    return new_file_name;
}