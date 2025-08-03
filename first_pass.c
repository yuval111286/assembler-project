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

/* Global definition of the data image array */
unsigned int data_image[MAX_DATA_SIZE];

/**
 * @brief Performs the first pass over a preprocessed .am assembly file.
 *
 * This function parses each line of the source file, builds the symbol table,
 * encodes data directives into the data image, and encodes instruction metadata
 * into the code image. IC and DC are updated accordingly.
 *
 * @param file_name     The name of the .am file to read.
 * @param symbol_table  Pointer to the symbol table to update.
 * @param IC_final      Pointer to store the final instruction counter.
 * @param DC_final      Pointer to store the final data counter.
 * @param code_image    Pointer to the code image structure.
 * @return 1 on success, 0 on error.
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final, CodeImage *code_image)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int IC = IC_INIT_VALUE;
    int DC = 0;
    ParsedLine parsed;
    int words;
    unsigned int encoded_word;

    /* Open source file for reading */
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_READING);
        return 0;
    }

    /* Initialize code image */
    init_code_image(code_image);

    /* Read file line by line */
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        /* Skip comment or empty lines */
        if (check_line_comment_or_empty(line)) {
            continue;
        }

        /* Parse current line */
        if (!parse_line(line, &parsed)) {
            error_log(file_name, line_number, SYNTAX_ERROR);
            continue;
        }

        /* === Handle label definitions === */
        if (parsed.label[0] != '\0') {
            if (!is_valid_label(parsed.label)) {
                error_log(file_name, line_number, INVALID_LABEL_NAME);
                continue;
            }

            if (identify_opcode(parsed.label) != OPCODE_INVALID ||
                identify_directive(parsed.label) != -1 ||
                identify_register(parsed.label) != -1) {
                error_log(file_name, line_number, LABEL_RESERVED_WORD);
                continue;
            }

            if (symbol_exists(symbol_table, parsed.label)) {
                error_log(file_name, line_number, DUPLICATE_LABEL);
                continue;
            }

            /* Add label to symbol table */
            if (parsed.line_type == LINE_INSTRUCTION) {
                if (!add_symbol(symbol_table, parsed.label, IC, SYMBOL_CODE)) {
                    error_log(file_name, line_number, FAILED_ADD_INSTRUCTION_LABEL);
                    continue;
                }
            } else if (parsed.line_type == LINE_DIRECTIVE &&
                       (strcmp(parsed.directive_name, "data") == 0 ||
                        strcmp(parsed.directive_name, "string") == 0 ||
                        strcmp(parsed.directive_name, "mat") == 0)) {
                if (!add_symbol(symbol_table, parsed.label, DC, SYMBOL_DATA)) {
                    error_log(file_name, line_number, FAILED_ADD_DATA_LABEL);
                    continue;
                }
            } else if (strcmp(parsed.directive_name, "extern") == 0) {
                error_log(file_name, line_number, ILLEGAL_EXTERN_LABEL);
                continue;
            }
        }

        /* === Handle instructions === */
        if (parsed.line_type == LINE_INSTRUCTION) {
            words = instruction_word_count(&parsed);
            if (words <= 0) {
                error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
                continue;
            }

            /* Placeholder: insert first word (encoding done in second pass) */
            encoded_word = 0;
            add_code_word(code_image, IC, encoded_word, 'A');
            IC++;

            /* Add empty words for operands */
            while (words > 1) {
                add_code_word(code_image, IC, 0, 'A'); /* Placeholder */
                IC++;
                words--;
            }
        }

        /* === Handle .data, .string, .mat === */
        if (parsed.line_type == LINE_DIRECTIVE) {
            if (strcmp(parsed.directive_name, "data") == 0) {
                int i;
                for (i = 0; i < parsed.operand_count; i++) {
                    if (DC >= MAX_DATA_SIZE) {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        continue;
                    }
                    data_image[DC++] = atoi(parsed.operands[i]);
                }
            } else if (strcmp(parsed.directive_name, "string") == 0) {
                char *s = parsed.operands[0];
                int i;
                for (i = 0; s[i] != '\0'; i++) {
                    if (DC >= MAX_DATA_SIZE) {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        continue;
                    }
                    data_image[DC++] = (unsigned int)s[i];
                }
                if (DC >= MAX_DATA_SIZE) {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    continue;
                }
                data_image[DC++] = 0; /* null terminator */
            } else if (strcmp(parsed.directive_name, "mat") == 0) {
                int mat_rows, mat_cols, i, expected_values;

                mat_rows = atoi(parsed.operands[0]);
                mat_cols = atoi(parsed.operands[1]);

                if (mat_rows <= 0 || mat_cols <= 0) {
                    error_log(file_name, line_number, INVALID_MATRIX_DIMENSIONS);
                    continue;
                }

                expected_values = mat_rows * mat_cols;

                if (parsed.operand_count != 2 + expected_values) {
                    error_log(file_name, line_number, MATRIX_VALUE_COUNT_MISMATCH);
                    continue;
                }

                if (DC + 2 + expected_values >= MAX_DATA_SIZE) {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    continue;
                }

                data_image[DC++] = mat_rows;
                data_image[DC++] = mat_cols;

                for (i = 2; i < parsed.operand_count; i++) {
                    data_image[DC++] = atoi(parsed.operands[i]);
                }
            } else if (strcmp(parsed.directive_name, "extern") == 0) {
                if (parsed.operand_count != 1) {
                    error_log(file_name, line_number, EXTERN_SYNTAX_ERROR);
                } else if (!add_symbol(symbol_table, parsed.operands[0], 0, SYMBOL_EXTERN)) {
                    error_log(file_name, line_number, DUPLICATE_EXTERN);
                }
            }
        }
    }

    fclose(fp);

    /* Finalize counters */
    *IC_final = IC;
    *DC_final = DC;

    /* Update base addresses of data symbols */
    update_data_symbols_base_address(symbol_table, IC);

    return 1;
}
