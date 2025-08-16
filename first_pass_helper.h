
#ifndef FIRST_PASS_HELPER_H
#define FIRST_PASS_HELPER_H

#include "symbol_table.h"
#include "linked_list.h"
/**
 * @brief Encode bits into a machine code word.
 * @param encoded_word Current encoded word value.
 * @param value Value to insert.
 * @param bit_mask Bit mask for insertion.
 * @param shift Number of bits to shift left.
 * @return Updated encoded word.
 */
unsigned int coding_word(int encoded_word, unsigned int value, unsigned int bit_mask, int shift);

/**
 * @brief Parse matrix dimensions from a token.
 * @param token String containing matrix dimensions [n][m].
 * @param rows Pointer to store number of rows.
 * @param cols Pointer to store number of columns.
 * @return 1 if valid dimensions, 0 otherwise.
 */
int parse_matrix_dimensions(const char *token, int *rows, int *cols);

/**
 * @brief Encode signed number into machine code.
 * @param num Number to encode.
 * @return For positive numbers: same value. For negative numbers: twos complement representation.
 */
unsigned int encode_signed_num(int num);

/**
 * @brief Check that macro name is not the same as a label name.
 * @param symbol_table Pointer to the symbol table.
 * @param macro_head Pointer to macro list head.
 * @param file_name Source file name.
 * @return 0 if macro name is different from labels, 1 if same name found.
 */
int check_mcro_name_not_label(SymbolTable *symbol_table, node **macro_head, char *file_name);

/**
 * @brief Parse a signed 10-bit number and convert to unsigned representation.
 * Values from -512 to +511 and converts negative values to
 * twos complement representation.
 * @param str String containing the number.
 * @param error_flag Pointer to int set to 1 if parsing failed.
 * @return Twos complement representation as unsigned short.
 */
unsigned short parse_number_from_string(const char *str, int *error_flag);


#endif
