#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "globals.h"
#include "symbol_table.h"
#include "linked_list.h"
#include <stdio.h>




/**
 * @brief Shift value left and set ARE bits.
 * @param final_value Current value.
 * @param are_type ARE type (A=0, R=1, E=2).
 * @return Value with ARE bits set.
 */
unsigned int shift_and_set_are(unsigned int final_value, int are_type);

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
 * @param token String containing matrix dimensions (e.g., "[2][3]").
 * @param rows Pointer to store number of rows.
 * @param cols Pointer to store number of columns.
 * @return 1 if valid dimensions, 0 otherwise.
 */
int parse_matrix_dimensions(const char *token, int *rows, int *cols);

/**
 * @brief Encode signed number into machine code.
 * @param num Number to encode.
 * @return For positive numbers: same value. For negative numbers: two's complement representation.
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
 * @brief Parse a number from string safely.
 * @param str String containing the number.
 * @param error_flag Pointer to int set to 1 if parsing failed.
 * @return Parsed number as unsigned short.
 */
unsigned short parse_number_from_string(const char *str, int *error_flag);

/**
 * @brief Perform the first pass of the assembler.
 *
 * Reads the source file, builds the symbol table, encodes instructions/data into memory,
 * and calculates the final IC (Instruction Counter) and DC (Data Counter) values.
 *
 * @param file_name Name of the source file (without extension).
 * @param symbol_table Pointer to symbol table to populate.
 * @param IC_final Pointer to store final instruction counter.
 * @param DC_final Pointer to store final data counter.
 * @param code_image Pointer to code image for storing encoded instructions.
 * @param head Pointer to macro list head.
 * @param data_image Array to store encoded data (.data/.string/.mat).
 * @return 1 if completed (even with non-critical errors), 0 if critical failure occurred.
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final,
               CodeImage *code_image, node **head, unsigned int *data_image);

#endif 
