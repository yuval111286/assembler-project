#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

/* Extern declarations for tables defined in parser_helper.c */
extern OpcodeEntry opcode_table[];
extern Directive_Mode directive_table[];
extern Directive_Mode directive_table_without_dots[];
extern Register_Type register_table[];
extern const int expected_operands_for_each_opcode[16];
extern const int allowed_source_modes[16][4];
extern const int allowed_dest_modes[16][4];


/*
 * Parses a single line of assembly code and fills a ParsedLine structure.
 *
 * Parameters:
 *   line – the raw string from the source file
 *   out – pointer to a ParsedLine structure to be filled
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if parsing succeeded,
 *   0 if syntax error detected
 */
int parse_line(char *line, ParsedLine *out, char *file_name, int line_number);

/* Helper functions for parse_line */

/*
 * Parses directive lines and fills the ParsedLine structure accordingly.
 *
 * Parameters:
 *   line – the complete line containing the directive
 *   out – pointer to a ParsedLine structure to be filled
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if parsing succeeded, 0 if syntax error detected
 */
int parse_directive_line(char *line, ParsedLine *out, char *file_name, int line_number);

/*
 * Parses .string directive operands.
 */
int parse_string_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/*
 * Parses .data directive operands.
 */
int parse_data_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/*
 * Parses .extern directive operands.
 */
int parse_extern_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/*
 * Parses .entry directive operands.
 */
int parse_entry_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/*
 * Parses .mat directive operands.
 */
int parse_mat_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/*
 * Parses instruction operands and validates them.
 *
 * Parameters:
 *   operands – string containing the operands part of the instruction
 *   out – pointer to a ParsedLine structure to be filled
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if parsing succeeded, 0 if syntax error detected
 */
int parse_instruction_operands(char *operands, ParsedLine *out, char *file_name, int line_number);

/*
 * Validates a single operand for a specific instruction at a specific position.
 *
 * Parameters:
 *   operand – the operand string to validate
 *   opcode – the instruction opcode
 *   position – position of the operand (0=first, 1=second)
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if operand is valid for this instruction/position, 0 otherwise
 */
int validate_operand_for_instruction(char *operand, Opcode opcode, int position, char *file_name, int line_number);

/* Identification functions */

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
int identify_directive(char *str);

/*
 * Identifies the directive type from string when directive name are without the initial dots.
 * Returns directive enum value if found, or -1 if invalid.
 */
int identify_directive_without_dots(char *directive);

/* Validation functions */

/*
 * Validates a label name according to the language rules.
 *
 * Parameters:
 *   label – the label string to validate
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if the label is valid; 0 otherwise
 */
int is_valid_label(char *label, char *file_name, int line_number);

/*
 * Validates if a string represents a valid immediate operand (#number).
 *
 * Parameters:
 *   operand – the operand string to validate
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if valid immediate operand, 0 otherwise
 */
int is_valid_immediate(char *operand, char *file_name, int line_number);

/*
 * Validates matrix operand format and register names.
 *
 * Parameters:
 *   operand – the matrix operand string to validate
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if valid matrix format, 0 otherwise
 */
int verify_matrix_registers_are_valid(char *operand, char *file_name, int line_number);

/*
 * Validates that addressing modes are correct for the specific instruction.
 *
 * Parameters:
 *   parsed – pointer to ParsedLine structure containing the parsed instruction
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if addressing modes are valid, 0 otherwise
 */
int verify_addressing_modes_are_valid(ParsedLine *parsed, char *file_name, int line_number);

/*
 * Validates string content for .string directive.
 *
 * Parameters:
 *   tested_word – the string content to validate
 *
 * Returns:
 *   1 if string contains invalid characters, 0 if valid
 */
int verify_string_is_valid(char *tested_word);

/*
 * Validates matrix dimensions format.
 *
 * Parameters:
 *   operands – string containing matrix definition
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if valid matrix dimensions, 0 otherwise
 */
int is_valid_matrix_dim(char *operands, char *file_name, int line_number);

/*
 * Validates numeric strings and handles different validation modes.
 *
 * Parameters:
 *   tested_word – the string to validate
 *   digit_or_letter_or_both – validation mode (0=digits only, 1=letters only, 2=both)
 *   file_name – name of the source file (for error reporting)
 *   line_number – line number in the source file (for error reporting)
 *
 * Returns:
 *   1 if valid according to mode, 0 otherwise
 */
int is_digit_or_char(char *tested_word, int digit_or_letter_or_both, char *file_name, int line_number);

/* Utility functions */

/*
 * Returns the addressing mode of a given operand string.
 * Returns ADDRESS_IMMEDIATE, ADDRESS_DIRECT, etc.
 * If invalid, returns -1.
 */
int get_addressing_mode(char *operand);

/*
 * Computes the number of memory words required for a parsed instruction.
 * Used to update the IC during first pass.
 */
int instruction_word_count(ParsedLine *parsed);

/*
 * Counts how many memory words are needed for a .data, .string or .mat directive.
 * Used to update the DC during first pass.
 */
int count_data_items(ParsedLine *parsed);

/*
 * Removes quotes from a string and returns the string without quotes.
 */
char *strip_quotes(char *str);

/*
 * Shifts all characters in a string one position to the left.
 */
void safe_shift_left(char *str);

/*
 * Copies directive name without the leading dot.
 *
 * Parameters:
 *   token – the directive token (e.g., ".data")
 *   dest – destination buffer to store the directive name
 */
void copy_directive_name(char *token, char *dest);

/*
 * Checks if a string is empty or contains only whitespace characters.
 *
 * Parameters:
 *   str – the string to check
 *
 * Returns:
 *   1 if empty or whitespace only, 0 otherwise
 */
int is_empty_or_whitespace(char *str);

#endif /* PARSER_H */