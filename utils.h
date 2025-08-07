#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

/**
 * @brief Perform memory allocation according to given size using malloc
 *
 * @param size - requested memory type
 * @return pointer to requested data
 */
void *malloc_allocation(size_t size);

int parse_matrix_dimensions(const char *token, int *rows, int *cols);

/**
 *
 * @param file_name file name for the new file
 * @param new_ending name of the ending of the new file
 * @return new string pointer containing the file name with the new ending
 */
char *change_ending_of_file(char *file_name, char *new_ending);

/**
 * Helper function to encode signed integer in 10-bit two's complement
 */
unsigned int encode_signed_value(int value);

/**
 * Converts an unsigned integer to a 5-digit base-4 string
 */
char *turn_line_to_base_4(unsigned int word);

/**
 * Converts an address to a 4-digit base-4 string
 */
char *turn_address_to_base_4(unsigned int address);

#endif