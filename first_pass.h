#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "globals.h"        
#include "symbol_table.h"   
#include <stdio.h>          

/*
 * Runs the first pass on the given source file.
 *
 * Parameters:
 *   fp   - pointer to the opened .am file for reading.
 *   table  - pointer to the symbol table to populate.
 *   ic - pointer to the instruction counter to update.
 *   dc   - pointer to the data counter to update.
 *   filename - name of the file (used for error reporting).
 
 * Returns:
 *   1 if the first pass completed successfully (even if errors were found),
 *   0 if a critical error occurred (e.g., file problem).
 */
int first_pass(FILE *fp, SymbolTable *table, int *ic, int *dc, char *filename);

#endif 