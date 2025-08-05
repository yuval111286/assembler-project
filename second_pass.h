#ifndef ASSEMBLER_PROJECT_SECOND_PASS_H
#define ASSEMBLER_PROJECT_SECOND_PASS_H

#include "globals.h"
#include "symbol_table.h"
#include "code_image.h"




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
void add_extern_reference(ExternList *extern_list, char *symbol_name, int address);

/**
 * @brief Frees memory allocated for extern references list
 *
 * @param extern_list Pointer to the extern references list to free
 */
void free_extern_list(ExternList *extern_list);

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
 * @brief Encodes a single operand and adds it to the code image
 *
 * @param code_image Pointer to the code image
 * @param code_index Pointer to current position in code image (will be updated)
 * @param operand The operand string to encode
 * @param symbol_table Pointer to the symbol table
 * @param extern_list Pointer to the extern references list
 * @param current_address Current memory address
 */
void encode_operand(CodeImage *code_image, int *code_index, char *operand,
                    SymbolTable *symbol_table, ExternList *extern_list, int current_address);

/**
 * @brief Encodes a complete instruction and adds it to the code image
 *
 * @param code_image Pointer to the code image
 * @param parsed Pointer to the parsed instruction line
 * @param current_address Current memory address
 * @param symbol_table Pointer to the symbol table
 * @param extern_list Pointer to the extern references list
 */
void encode_instruction(CodeImage *code_image, ParsedLine *parsed, int current_address,
                        SymbolTable *symbol_table, ExternList *extern_list);

#endif /*ASSEMBLER_PROJECT_SECOND_PASS_H*/