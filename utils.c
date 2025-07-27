#include "utils.h"

void *malloc_allocation(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        /*print error*/;
        return NULL;
    }
    return ptr;
}