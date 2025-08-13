#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

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

/*  Identification functions  */

/**
 * @brief Identify register number from name (r0–r7).
 * @param str String to check.
 * @return Register number (0–7) or -1 if invalid.
 */
int identify_register(char *str);

/**
 * @brief Identify opcode from string.
 * @param str Opcode name.
 * @return Opcode enum or OPCODE_INVALID if not found.
 */
int identify_opcode(char *str);

/**
 * @brief Identify directive from string (with dot).
 * @param str Directive name.
 * @return Directive enum or -1 if invalid.
 */
int identify_directive(char *str);

/**
 * @brief Identify directive from string (without dot).
 * @param directive Directive name without dot.
 * @return Directive enum or -1 if invalid.
 */
int identify_directive_without_dots(char *directive);

/* Validation functions */

/**
 * @brief Check if label name is valid.
 * @param label Label string.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_label(char *label, char *file_name, int line_number);

/**
 * @brief Check if operand is a valid immediate (#number).
 * @param operand Operand string.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_immediate(char *operand, char *file_name, int line_number);

/**
 * @brief Validate matrix format and registers inside it.
 * @param operand Matrix operand string.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int verify_matrix_registers_are_valid(char *operand, char *file_name, int line_number);

/**
 * @brief Check that addressing modes are allowed for instruction.
 * @param parsed Pointer to ParsedLine struct.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int verify_addressing_modes_are_valid(ParsedLine *parsed, char *file_name, int line_number);

/**
 * @brief Check if string contains only valid characters for .string.
 * @param tested_word String to check.
 * @return 1 if invalid chars found, 0 if valid.
 */
int verify_string_is_valid(char *tested_word);

/**
 * @brief Validate matrix dimension format ([n][m]).
 * @param operands Dimension string.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_matrix_dim(char *operands, char *file_name, int line_number);

/**
 * @brief Validate that string is only digits, only letters, or both.
 * @param tested_word String to check.
 * @param digit_or_letter_or_both Mode: 0=digits only, 1=letters only, 2=both.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int is_digit_or_char(char *tested_word, int digit_or_letter_or_both, char *file_name, int line_number);

/* Utility functions  */

/**
 * @brief Get addressing mode from operand string.
 * @param operand Operand string.
 * @return Addressing mode enum or -1 if invalid.
 */
int get_addressing_mode(char *operand);

/**
 * @brief Count memory words for an instruction.
 * @param parsed Pointer to ParsedLine struct.
 * @return Number of words used by instruction.
 */
int instruction_word_count(ParsedLine *parsed);

/**
 * @brief Count memory words for .data, .string, or .mat directive.
 * @param parsed Pointer to ParsedLine struct.
 * @return Number of words used by directive.
 */
int count_data_items(ParsedLine *parsed);

/**
 * @brief Remove quotes from string.
 * @param str String with quotes.
 * @return Pointer to string without quotes.
 */
char *strip_quotes(char *str);

/**
 * @brief Shift string left by one character.
 * @param str String to shift.
 */
void safe_shift_left(char *str);

/**
 * @brief Copy directive name without dot to buffer.
 * @param token Directive token (e.g., ".data").
 * @param dest Destination buffer.
 */
void copy_directive_name(char *token, char *dest);

/**
 * @brief Check if string is empty or contains only spaces.
 * @param str String to check.
 * @return 1 if empty or whitespace only, 0 otherwise.
 */
int is_empty_or_whitespace(char *str);

#endif 
