
#ifndef PARSER_H
#define PARSER_H
#include "globals.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"
#include "utils.h"
#include <stdarg.h>

/*
 * Parses a single line of assembly code and fills a ParsedLine structure.
 *
 * Parameters:
 *   line – the raw string from the source file
 *   out  – pointer to a ParsedLine structure to be filled
 *
 * Returns:
 *   1 if parsing succeeded,
 *   0 if syntax error detected
 */
int parse_line(const char *line, ParsedLine *out);


int is_instruction(char *str);

int identify_register(char *str);

int identify_opcode(char *str);

#endif /* PARSER_H */
