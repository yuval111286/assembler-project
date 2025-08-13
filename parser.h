#ifndef PARSER_H
#define PARSER_H

#include "utils.h"
#include "parser_helper.h"


/* External tables from parser_helper.c */
extern OpcodeEntry opcode_table[];
extern Directive_Mode directive_table[];
extern Directive_Mode directive_table_without_dots[];
extern Register_Type register_table[];
extern const int expected_operands_for_each_opcode[16];
extern const int allowed_source_modes[16][4];
extern const int allowed_dest_modes[16][4];

/* Main parsing functions */

/**
 * @brief Parse one line of assembly and fill ParsedLine struct.
 * @param line Raw line from source file.
 * @param out Pointer to ParsedLine struct to fill.
 * @param file_name Source file name for error reporting.
 * @param line_number Line number in source file.
 * @return 1 if parsing succeeded, 0 if syntax error.
 */
int parse_line(char *line, ParsedLine *out, char *file_name, int line_number);

/**
 * @brief Parse a directive line and fill ParsedLine struct.
 * @param line Full line containing the directive.
 * @param out Pointer to ParsedLine struct to fill.
 * @param file_name Source file name.
 * @param line_number Line number in file.
 * @return 1 if parsing succeeded, 0 if syntax error.
 */
int parse_directive_line(char *line, ParsedLine *out, char *file_name, int line_number);

/**
 * @brief Parse operands for .string directive.
 * @param operands Operands string from line.
 * @param out Pointer to ParsedLine struct.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if parsing succeeded, 0 if syntax error.
 */
int parse_string_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/**
 * @brief Parse operands for .data directive.
 */
int parse_data_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/**
 * @brief Parse operands for .extern directive.
 */
int parse_extern_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/**
 * @brief Parse operands for .entry directive.
 */
int parse_entry_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/**
 * @brief Parse operands for .mat directive.
 */
int parse_mat_directive(char *operands, ParsedLine *out, char *file_name, int line_number);

/**
 * @brief Parse instruction operands and validate them.
 * @param operands Operand string.
 * @param out Pointer to ParsedLine struct.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if parsing succeeded, 0 if syntax error.
 */
int parse_instruction_operands(char *operands, ParsedLine *out, char *file_name, int line_number);

/**
 * @brief Validate single operand for given instruction and position.
 * @param operand Operand string.
 * @param opcode Opcode enum.
 * @param position Operand position 0=first, 1=second.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int validate_operand_for_instruction(char *operand, Opcode opcode, int position, char *file_name, int line_number);


#endif 
