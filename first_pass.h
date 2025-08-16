#ifndef FIRST_PASS_H
#define FIRST_PASS_H


#include "symbol_table.h"
#include "linked_list.h"




/**
 * @brief Perform the first pass of the assembler.
 *
 * Goes through the source file once to:
 *   - Collect labels into the symbol table.
 *   - Encode .data/.string/.mat into the data image.
 *   - Encode instructions into the code image.
 *   - Update IC and DC.
 * Produces the symbol table and section sizes required for the second pass.
 * 
 * @param file_name Name of the source file (without extension).
 * @param symbol_table Pointer to symbol table to populate.
 * @param IC_final Pointer to store final instruction counter.
 * @param DC_final Pointer to store final data counter.
 * @param code_image Pointer to code image for storing encoded instructions.
 * @param head Pointer to macro list head.
 * @param data_image Array to store encoded data (.data/.string/.mat).
 * @return 1 if completed, 0 if critical failure occurred.
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final,
               CodeImage *code_image, node **head, unsigned int *data_image);

#endif 
