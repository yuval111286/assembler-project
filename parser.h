
#ifndef PARSER_H
#define PARSER_H

#include "globals.h"  
#include "utils.h"    

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

/*
 * Checks if the given string is a valid instruction.
 * Returns 1 if valid, 0 otherwise.
 */
int is_instruction(char *str);

/*
 * Identifies the register number from name (e.g., r3).
 * Returns 0-7 if valid, -1 otherwise.
 */
int identify_register(char *str);

/*
 * Identifies the opcode enum from string.
 * Returns opcode enum value if found, or OPCODE_INVALID.
 */
int identify_opcode(char *str);

/*
 * Identifies the directive type from string (e.g., .data).
 * Returns directive enum value if found, or -1 if invalid.
 */
int is_directive(char *str);

/* Tables for opcode, directive, and register recognition */
extern const OpcodeEntry opcode_table[];
extern const Directive_Mode directive_table[];
extern const Register_Type register_table[];


#endif /* PARSER_H */
