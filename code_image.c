#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "code_image.h"
#include "errors_handler.h"
#include "utils.h"

/**
 * @brief Initializes the code image structure.
 *
 * Sets the number of words in the code image to 0.
 *
 * @param img Pointer to the CodeImage structure to initialize.
 */
void init_code_image(CodeImage *img) {
    (*img).size = 0;
}

/**
 * @brief Adds a new code word to the code image.
 *
 * Verifies that the code image has space before adding the word.
 * If full, logs an error and aborts.
 *
 * @param img Pointer to the CodeImage.
 * @param address The memory address of the word.
 * @param value The 10-bit machine code value.
 * @param are The ARE character ('A', 'R', or 'E').
 */
void add_code_word(CodeImage *img, int address, unsigned int value, char are) {
    if ((*img).size >= MAX_CODE_SIZE) {
        error_log("internal", -1, CODE_IMAGE_OVERFLOW); /* log and abort */
        exit(1);
    }

    (*img).words[(*img).size].address = address;
    (*img).words[(*img).size].value = value;
    (*img).words[(*img).size].ARE = are;
    (*img).size++;
}

