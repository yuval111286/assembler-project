#include <stdlib.h>
#include "code_image.h"
#include "errors_handler.h"


void init_code_image(CodeImage *img) {
    /* Start with an empty code image */
    (*img).size = 0;
}

void add_code_word(CodeImage *img, int address, unsigned int value, char are) {
    /* Not exceed maximum allowed code size */
    if ((*img).size >= MAX_CODE_SIZE) {
        error_log(INTERNAL, -1, CODE_IMAGE_OVERFLOW);
        exit(1);
    }

    /* Store the new code word in the next available slot */
    (*img).words[(*img).size].address = address; 
    (*img).words[(*img).size].value = value;     
    (*img).words[(*img).size].ARE = are;         
    
    /* Increment total number of stored words */
    (*img).size++;
}
