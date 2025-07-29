
#ifndef UTILS_H
#define UTILS_H

#include "globals.h"
#include <stdio.h>
#include <ctype.h>




/*
 * Validates a label name according to the language rules.
 * Returns 1 if the label is valid; 0 otherwise.
 */
int is_valid_label(const char *label);

/*
 * Prints a formatted error message with file name and line number.
 * Used for consistent error reporting.
 */
void print_error(const char *filename, int lineno, const char *message);

/*
 * Returns the addressing mode of a given operand string.
 * Returns ADDRESS_IMMEDIATE, ADDRESS_DIRECT, etc.
 * If invalid, returns -1.
 */
int get_addressing_mode(const char *operand);

/*
 * Computes the number of memory words required for a parsed instruction.
 * Used to update the IC during first pass.
 */
int instruction_word_count(const ParsedLine *parsed);

/*
 * Counts how many memory words are needed for a .data, .string or .mat directive.
 * Used to update the DC during first pass.
 */
int count_data_items(const ParsedLine *parsed);


void *malloc_allocation(size_t size);

char *change_ending_of_file(char *file_name, char *new_ending);

#endif 
