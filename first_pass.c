#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "symbol_table.h"
#include "parser.h"
#include "utils.h"
#include "errors_handler.h"
#include "analyze_text.h"
#include "code_image.h"

/* Global array to store data values from .data, .string, .mat */
unsigned int data_image[MAX_DATA_SIZE];

/**
 * Performs the first pass over a .am file and builds the symbol table,
 * encodes instructions, and processes data directives into the image arrays.
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final, CodeImage *code_image)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int IC = IC_INIT_VALUE;
    int DC = 0;
    ParsedLine parsed;
    int i, words;
    unsigned int encoded_word;
    int has_errors = 0;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_READING);
        return 1;
    }

    init_code_image(code_image);

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        if (!parse_line(line, &parsed)) {
            error_log(file_name, line_number, SYNTAX_ERROR);
            has_errors = 1;
            continue;
        }

        /* === LABEL HANDLING === */
        if (parsed.label[0] != '\0') {
            if (!is_valid_label(parsed.label)) {
                error_log(file_name, line_number, INVALID_LABEL_NAME);
                has_errors = 1;
                continue;
            }

            if (identify_opcode(parsed.label) != OPCODE_INVALID ||
                identify_directive(parsed.label) != -1 ||
                identify_register(parsed.label) != -1) {
                error_log(file_name, line_number, LABEL_RESERVED_WORD);
                has_errors = 1;
                continue;
            }

            if (symbol_exists(symbol_table, parsed.label)) {
                error_log(file_name, line_number, DUPLICATE_LABEL);
                has_errors = 1;
                continue;
            }

            if (parsed.line_type == LINE_INSTRUCTION) {
                if (!add_symbol(symbol_table, parsed.label, IC, SYMBOL_CODE)) {
                    error_log(file_name, line_number, FAILED_ADD_INSTRUCTION_LABEL);
                    has_errors = 1;
                    continue;
                }
            } else if (strcmp(parsed.directive_name, "data") == 0 ||
                       strcmp(parsed.directive_name, "string") == 0 ||
                       strcmp(parsed.directive_name, "mat") == 0) {
                if (!add_symbol(symbol_table, parsed.label, DC, SYMBOL_DATA)) {
                    error_log(file_name, line_number, FAILED_ADD_DATA_LABEL);
                    has_errors = 1;
                    continue;
                }
            } else if (strcmp(parsed.directive_name, "extern") == 0) {
                error_log(file_name, line_number, ILLEGAL_EXTERN_LABEL);
                has_errors = 1;
                continue;
            }
        }

        /* === INSTRUCTION HANDLING === */
        if (parsed.line_type == LINE_INSTRUCTION) {
            words = instruction_word_count(&parsed);
            if (words <= 0) {
                error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
                has_errors = 1;
                continue;
            }

            encoded_word = 0;
            add_code_word(code_image, IC, encoded_word, 'A');
            IC++;

            while (words > 1) {
                add_code_word(code_image, IC, 0, 'A');
                IC++;
                words--;
            }
        }

        /* === DIRECTIVE HANDLING === */
        if (parsed.line_type == LINE_DIRECTIVE) {
            if (strcmp(parsed.directive_name, "data") == 0) {
                for (i = 0; i < parsed.operand_count; i++) {
                    if (DC >= MAX_DATA_SIZE) {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        has_errors = 1;
                        continue;
                    }
                    data_image[DC++] = atoi(parsed.operands[i]);
                }
            } else if (strcmp(parsed.directive_name, "string") == 0) {
                char *s = parsed.operands[0];
                for (i = 0; s[i] != '\0'; i++) {
                    if (DC >= MAX_DATA_SIZE) {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        has_errors = 1;
                        break;
                    }
                    data_image[DC++] = (unsigned int)s[i];
                }
                if (DC < MAX_DATA_SIZE) {
                    data_image[DC++] = 0;
                } else {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    has_errors = 1;
                }
            } else if (strcmp(parsed.directive_name, "mat") == 0) {
                int mat_rows, mat_cols, expected_values;

                if (!parse_matrix_dimensions(parsed.operands[0], &mat_rows, &mat_cols)) {
                    error_log(file_name, line_number, INVALID_MATRIX_DIMENSIONS);
                    has_errors = 1;
                    continue;
                }

                expected_values = mat_rows * mat_cols;
               

                if (parsed.operand_count != 1 + expected_values) {
                    error_log(file_name, line_number, MATRIX_VALUE_COUNT_MISMATCH);
                    has_errors = 1;
                    continue;
                }

                if (DC + 2 + expected_values > MAX_DATA_SIZE) {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    has_errors = 1;
                    continue;
                }

                data_image[DC++] = mat_rows;
                data_image[DC++] = mat_cols;

                for (i = 1; i < parsed.operand_count; i++) {
                    data_image[DC++] = atoi(parsed.operands[i]);
                }
            } else if (strcmp(parsed.directive_name, "extern") == 0) {
                if (parsed.operand_count != 1) {
                    error_log(file_name, line_number, EXTERN_SYNTAX_ERROR);
                    has_errors = 1;
                } else if (!add_symbol(symbol_table, parsed.operands[0], 0, SYMBOL_EXTERN)) {
                    error_log(file_name, line_number, DUPLICATE_EXTERN);
                    has_errors = 1;
                }
            }
        }
    }

    fclose(fp);

    *IC_final = IC;
    *DC_final = DC;

    update_data_symbols_base_address(symbol_table, IC);

    return has_errors;
}
