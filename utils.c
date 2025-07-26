#include "utils.h"



char* trim_spaces(char* str) {
    char* end;
    /* Skip leading whitespace */
    while (isspace(*str)) str++;

    if (*str == '\0') return str;

    /* Find end of string and trim trailing whitespace */
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        *end = '\0';
        end--;
    }

    return str;
}

void *malloc_allocation(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        /*print error*/;
        return NULL;
    }
    return ptr;
}