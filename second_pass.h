
#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "globals.h"
#include "symbol_table.h"
#include "code_image.h"

/**
 * Converts an unsigned integer to a 5-digit base-4 string
 */
char *turn_line_to_base_4(unsigned int word);

/**
 * Converts an address to a 4-digit base-4 string
 */
char *turn_address_to_base_4(unsigned int address);

char *int_to_base_4(int number);


/**
 * @brief Performs the second pass of the assembler
 *
 * Completes the encoding of instructions by:
 * - Resolving all label addresses and encoding operands
 * - Processing .entry directives and marking symbols as entry
 * - Collecting external symbol references for .ext file
 * - Generating output files: .ob (object), .ent (entry), .ext (external)
 *
 * @param file_name Base filename (without extension) - used to create .am input and output files
 * @param symbol_table Pointer to the symbol table populated during first pass
 * @param code_image Pointer to the code image structure to complete with encoded instructions
 * @param ic_final Final instruction counter value from first pass
 * @param dc_final Final data counter value from first pass
 * @param data_image Array containing encoded .data/.string/.mat values from first pass
 * @return 0 if second pass completed successfully, 1 if errors occurred
 */
int second_pass(char *file_name, SymbolTable *symbol_table, CodeImage *code_image,
                int ic_final, int dc_final, unsigned int *data_image);

/**
 * @brief Checks if an operand string represents a label reference
 *
 * @param operand The operand string to check
 * @return 1 if operand is a label, 0 otherwise
 */
int is_label_operand(char *operand);

/**
 * @brief Adds an external symbol reference to the extern list
 *
 * @param extern_list Pointer to the extern references list
 * @param symbol_name Name of the external symbol
 * @param address Address where the external symbol is referenced
 */
void add_extern_symbol(ExternList *extern_list, char *symbol_name, int address);

/**
 * @brief Frees memory allocated for extern references list
 *
 * @param extern_list Pointer to the extern references list to free
 */
void free_extern_list(ExternList *extern_list);

/**
 * @brief Writes the contents of the code image and data image to a .ob file.
 *
 * Creates the output file with ".ob" extension and writes the IC and DC sizes,
 * followed by the code and data memory words in base-4 encoding.
 *
 * @param img Pointer to the CodeImage.
 * @param ic_size Final instruction counter.
 * @param dc_size Final data counter.
 * @param data_image Pointer to the data image array.
 * @param file_name Base name of the file (without extension).
 */
void write_code_image_to_ob_file(CodeImage *img, int ic_size, int dc_size, unsigned int *data_image, char *file_name);

/**
 * @brief Writes .ext file containing external symbol references
 *
 * @param base_filename Base filename (without extension)
 * @param extern_list Pointer to the extern references list
 */
void write_ext_file(char *base_filename, ExternList *extern_list);

/**
 * @brief Writes .ent file containing entry symbol definitions
 *
 * @param base_filename Base filename (without extension)
 * @param symbol_table Pointer to the symbol table
 */
void write_ent_file(char *base_filename, SymbolTable *symbol_table);

/**
 * @brief Updates a code word with a new value and ARE field
 *
 * @param code_image Pointer to the code image
 * @param address Address of the word to update
 * @param value New value for the word
 * @param are ARE field ('A', 'R', or 'E')
 */
void update_code_word(CodeImage *code_image, int address, unsigned int value, char are);

#endif