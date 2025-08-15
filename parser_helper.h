#ifndef ASSEMBLER_PROJECT_PARSER_HELPER_H
#define ASSEMBLER_PROJECT_PARSER_HELPER_H

#include "globals.h" 


/*used strings*/
#define DIR_EXTERN "extern"
#define DIR_MAT "mat"
#define DIR_STRING "string"
#define DIR_ENTRY "entry"
#define DIR_DATA "data"
#define OPENING_BRACKET '['
#define CLOSED_BRACKET ']'
#define LADDER '#'
#define MAT_DIM "[%d][%d]%n"
#define MAT_DIM_SPACE " [%d][%d]%n"
#define COMMA ","
#define DOUBLE_DOTS ':'


/**
 * @brief Identify opcode number from its name.
 * @param op_code String containing opcode name.
 * @return Opcode enum value, or OPCODE_INVALID if not found.
 */
int identify_opcode(char* op_code);

/**
 * @brief Identify directive type from string with dot .
 * @param directive Directive name with dot.
 * @return Directive enum value, or -1 if not found.
 */
int identify_directive(char *directive);

/**
 * @brief Identify directive type from string without dot.
 * @param directive Directive name without dot.
 * @return Directive enum value, or -1 if not found.
 */
int identify_directive_without_dots(char *directive);

/**
 * @brief Identify register number from name.
 * @param reg String containing register name r0–r7.
 * @return Register number 0–7 or -1 if invalid.
 */
int identify_register(char *reg);

/**
 * @brief Validate if operand is a correct immediate value.
 * @param immediate Immediate string to check.
 * @param file_name Source file name.
 * @param line_number Line number in file.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_immediate(char *immediate, char* file_name, int line_number);

/**
 * @brief Validate matrix dimension format [n][m].
 * @param mat_dim Dimension string.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_matrix_dim(char *mat_dim, char *file_name, int line_number);

/**
 * @brief Check if string contains only digits, only letters, or both.
 * @param tested_word String to check.
 * @param digit_or_letter_or_both Mode: 0=digits only 1=letters only 2=both.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 0 if valid, 1 otherwise.
 */
int is_digit_or_char(char *tested_word, int digit_or_letter_or_both, char *file_name, int line_number);

/**
 * @brief Validate characters for .string directive.
 * @param tested_word String to check.
 * @return 1 if invalid characters found, 0 if valid.
 */
int verify_string_has_invalid_chars(char *tested_word);

/**
 * @brief Shift string one character forward.
 * @param str String to shift.
 */
void string_shifting_forward(char *str);

/**
 * @brief Remove surrounding quotes from string.
 * @param str String with quotes.
 * @return Pointer to string without quotes.
 */
char *strip_quotes(char *str);

/**
 * @brief Copy directive name without the dot to destination.
 * @param token Directive token.
 * @param dest Destination buffer.
 */
void copy_directive_name(char *token, char *dest);

/**
 * @brief Check if label name is valid.
 * @param label Label string.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_label(char *label, char *file_name, int line_number);

/**
 * @brief Get addressing mode for operand string.
 * @param operand Operand string.
 * @return Addressing mode enum or -1 if invalid.
 */
int get_addressing_mode(char *operand);

/**
 * @brief Validate that addressing modes match instruction requirements.
 * @param parsed Pointer to ParsedLine struct.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int verify_addressing_modes_are_valid(ParsedLine *parsed, char *file_name, int line_number);

/**
 * @brief Validate matrix operand format and registers inside.
 * @param operand Matrix operand string.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 1 if valid, 0 otherwise.
 */
int verify_matrix_registers_are_valid(char *operand, char *file_name, int line_number);

/**
 * @brief Count how many memory words are needed for instruction.
 * @param parsed Pointer to ParsedLine struct.
 * @return Number of words.
 */
int instruction_word_count(ParsedLine *parsed);


/**
 * @brief Validate commas in a line.
 * @param line Line to check.
 * @param file_name Source file name.
 * @param line_number Line number.
 * @return 0 if commas are valid, 1 otherwise.
 */
int comma_validation(char *line, char *file_name, int line_number);

#endif 
