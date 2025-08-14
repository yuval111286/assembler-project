#ifndef SECOND_PASS_H
#define SECOND_PASS_H


#include "symbol_table.h"
#include "code_image.h"


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


#endif 
