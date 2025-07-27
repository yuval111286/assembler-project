
#ifndef PARSER_H
#define PARSER_H
#include "globals.h" 

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

#endif /* PARSER_H */
