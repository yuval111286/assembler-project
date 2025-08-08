#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "globals.h"
#include "symbol_table.h"
#include "linked_list.h"
#include <stdio.h>

/**
 *
 * @param symbol_table
 * @param macro_head
 * @param file_name
 * @return 0 if mcro name is not like label name , 1 if does
 */
int check_mcro_name_not_label(SymbolTable *symbol_table, node **macro_head, char *file_name);

/*
 * Runs the first pass on the given source file.
 *
 * Parameters:
 *   fp       - pointer to the opened .am file for reading.
 *   table    - pointer to the symbol table to populate.
 *   ic       - pointer to the instruction counter to update.
 *   dc       - pointer to the data counter to update.
 *   filename - name of the file (used for error reporting).
 *
 * Returns:
 *   1 if the first pass completed successfully (even if errors were found),
 *   0 if a critical error occurred (e.g., file problem).
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final, CodeImage *code_image,node **head,unsigned int *data_image);
#endif /* FIRST_PASS_H */