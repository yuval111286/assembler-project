
#ifndef ASSEMBLER_PROJECT_PARSER_HELPER_H
#define ASSEMBLER_PROJECT_PARSER_HELPER_H

#include "globals.h"        /* ל-MAX_LINE_LENGTH, MAX_LABEL_LEN, ADDRESS_* constants */

int identify_opcode(char* op_code);

int identify_directive(char *directive);

int identify_directive_without_dots(char *directive);

int identify_register(char *reg);

int is_valid_immediate(char *immediate, char* file_name, int line_number);

int is_valid_matrix_dim(char *mat_dim, char *file_name, int line_number);

int is_digit_or_char(char *tested_word, int digit_or_letter_or_both, char *file_name, int line_number);

int verify_string_is_valid(char *tested_word);

void string_shifting_forward(char *str);

char *strip_quotes(char *str);

void copy_directive_name(char *token, char *dest);

int is_valid_label(char *label,char *file_name, int line_number);

int get_addressing_mode( char *operand);

int verify_addressing_modes_are_valid(ParsedLine *parsed, char *file_name, int line_number);

int verify_matrix_registers_are_valid(char *operand, char *file_name, int line_number);

int instruction_word_count(ParsedLine *parsed);

int count_data_items(ParsedLine *parsed);










#endif
