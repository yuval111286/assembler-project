#ifndef CODE_IMAGE_H
#define CODE_IMAGE_H

#include "globals.h"  /* For CodeImage, CodeWord, IC_INIT_VALUE, etc. */

/**
 * @brief Initializes the code image structure.
 * 
 * @param img Pointer to CodeImage to initialize.
 */
void init_code_image(CodeImage *img);

/**
 * @brief Adds a new code word to the code image.
 * 
 * @param img Pointer to CodeImage.
 * @param address The memory address for the new word.
 * @param value The encoded machine word (10-bit).
 * @param are The ARE field ('A', 'R', or 'E').
 */
void add_code_word(CodeImage *img, int address, unsigned int value, char are);

/**
 * @brief Writes the code and data images to the .ob output file.
 * 
 * @param img Pointer to CodeImage.
 * @param ic_size Final instruction counter (IC).
 * @param dc_size Final data counter (DC).
 * @param data_image Array of encoded .data/.string/.mat values.
 * @param file_name Base file name to write (without extension).
 */
void write_code_image_to_ob_file(CodeImage *img, int ic_size, int dc_size, unsigned int *data_image, char *file_name);

/**
 * @brief Converts a 10-bit unsigned integer into a 5-digit base-4 string.
 * 
 * @param value Integer value to convert (max 1023).
 * @return Pointer to a statically allocated base-4 string (e.g., "01231").
 */
char *turn_line_to_base_4(unsigned int value);

char *turn_address_to_base_4(unsigned int value);

#endif /* CODE_IMAGE_H */
