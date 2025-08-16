#ifndef CODE_IMAGE_H
#define CODE_IMAGE_H

#include "globals.h"  

/**
 * @brief Initializes the code image structure.
 * @param img Pointer to CodeImage to initialize.
 */
void init_code_image(CodeImage *img);

/**
 * @brief Adds a new code word to the code image.
 * @param img Pointer to CodeImage.
 * @param address The memory address for the new word.
 * @param value The encoded machine word 10bit.
 * @param are The ARE field.
 */
void add_code_word(CodeImage *img, int address, unsigned int value, char are);



#endif 
