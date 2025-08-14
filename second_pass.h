#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "globals.h"
#include "symbol_table.h"
#include "code_image.h"

/**
 * @brief Convert unsigned int to 5 digit base 4 string.
 * @param word The unsigned integer to convert.
 * @return Pointer to static string with base-4 representation.
 */
char *turn_line_to_base_4(unsigned int word);

/**
 * @brief Convert address to 4 digit base 4 string.
 * @param address The address to convert.
 * @return Pointer to static string with base 4 representation.
 */
char *turn_address_to_base_4(unsigned int address);

/**
 * @brief Convert integer to base 4 string.
 * @param number The integer to convert.
 * @return Pointer to string with base 4 representation.
 */
char *turn_num_to_base_4(int number);

/**
 * @brief Run the second pass of the assembler.
 * Completes encoding by:
 *  - Resolving label addresses
 *  - Handling .entry and marking symbols
 *  - Saving external references
 *  - Creating output files .ob, .ent, .ext
 * @param file_name Base file name without extension.
 * @param symbol_table Pointer to symbol table from first pass.
 * @param code_image Pointer to code image to complete.
 * @param ic_final Final instruction counter from first pass.
 * @param dc_final Final data counter from first pass.
 * @param data_image Data array from first pass.
 * @return 0 if success, 1 if errors found.
 */
int second_pass(char *file_name, SymbolTable *symbol_table, CodeImage *code_image,
                int ic_final, int dc_final, unsigned int *data_image,int discover_errors);

/**
 * @brief Check if operand is a label.
 * Operand is a label if it’s not:
 *  - Immediate (#value)
 *  - Matrix reference ([row][col])
 *  - Register (r0–r7)
 * @param operand The operand string.
 * @return 1 if label, 0 otherwise.
 */
int is_label_operand(char *operand);

/**
 * @brief Add external symbol reference to list.
 * @param extern_list Pointer to extern reference list.
 * @param symbol_name Name of the external symbol.
 * @param address Address where symbol is used.
 */
void add_extern_symbol(ExternList *extern_list, char *symbol_name, int address);

/**
 * @brief Free all memory of extern reference list.
 * @param extern_list Pointer to extern reference list.
 */
void free_extern_list(ExternList *extern_list);

/**
 * @brief Write code image and data image to .ob file.
 * @param img Pointer to code image.
 * @param ic_size Final instruction counter.
 * @param dc_size Final data counter.
 * @param data_image Pointer to data image array.
 * @param file_name Base file name without extension.
 */
void write_code_image_to_ob_file(CodeImage *img, int ic_size, int dc_size,
                                 unsigned int *data_image, char *file_name);

/**
 * @brief Write .ext file with external symbol references.
 * @param base_filename Base file name without extension.
 * @param extern_list Pointer to extern reference list.
 */
void write_ext_file(char *base_filename, ExternList *extern_list);

/**
 * @brief Write .ent file with entry symbol definitions.
 * @param base_filename Base file name without extension.
 * @param symbol_table Pointer to symbol table.
 */
void write_ent_file(char *base_filename, SymbolTable *symbol_table);

/**
 * @brief Update a code word in code image.
 * @param code_image Pointer to code image.
 * @param address Address of the code word.
 * @param value New value to set.
 * @param are ARE field ('A', 'R', 'E').
 */
void update_code_word(CodeImage *code_image, int address, unsigned int value, char are);

#endif 
