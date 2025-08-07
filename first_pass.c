#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first_pass.h"
#include "parser.h"
#include "utils.h"
#include "errors_handler.h"
#include "analyze_text.h"
#include "code_image.h"


/* Global array to store data values from .data, .string, .mat */
unsigned int data_image[MAX_DATA_SIZE];

int check_mcro_name_not_label(SymbolTable *symbol_table, node **macro_head, char *file_name) {
    node *current_macro;
    Symbol *current_symbol;
    int len,same_name = 0;
    char macro_name_without_last_char[MAX_LINE_LENGTH];

    /* check if macro list is empty no need to check*/
    if (macro_head == NULL || *macro_head == NULL) {
        return 0;
    }

    /* Check if symbol table is empty no need to check*/
    if (symbol_table == NULL || symbol_table->head == NULL) {
        return 0;
    }

    /* go over macro linked list */
    current_macro = *macro_head;

    len = strlen(current_macro->name);
    if (len > 0) {
        strncpy(macro_name_without_last_char, current_macro->name, len - 1);
        macro_name_without_last_char[len - 1] = '\0';
    } else {
        strcpy(macro_name_without_last_char, current_macro->name);
    }

    while (current_macro != NULL) {

        /* For each macro, check against all symbols in symbol table */
        current_symbol = symbol_table->head;
        while (current_symbol != NULL) {

            /* Compare macro name with symbol name */
            if (strcmp(macro_name_without_last_char, current_symbol->name) == 0) {
                /* report error */
                error_log(file_name, current_macro->line,
                          MACRO_NAME_AS_LABEL);
                same_name = 1;
                break;
            }

            current_symbol = current_symbol->next;
        }

        current_macro = current_macro->next;
    }

    return same_name;
}


/**
 * Performs the first pass over a .am file and builds the symbol table,
 * encodes instructions, and processes data directives into the image arrays.
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final, CodeImage *code_image, node **head)
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

        if (!parse_line(line, &parsed, file_name, line_number)) {
            has_errors = 1;
            continue;
        }

/* Skip empty lines and comments */
        if (parsed.line_type == LINE_EMPTY || parsed.line_type == LINE_COMMENT) {
            continue;
        }



        /* === LABEL HANDLING === */
        if (parsed.label[0] != '\0') {


            if (identify_opcode(parsed.label) != OPCODE_INVALID ||
                identify_directive(parsed.label) != -1 ||
                identify_register(parsed.label) != -1) {
                error_log(file_name, line_number, RESERVED_WORD_AS_LABEL);
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

            /* Encode base word of instruction */
            {
                int opcode = parsed.opcode;
                int src_mode = 0, dest_mode = 0;
                int ARE = 0; /* Absolute */

                if (parsed.operand_count == 2) {
                    src_mode = get_addressing_mode(parsed.operands[0]);
                    dest_mode = get_addressing_mode(parsed.operands[1]);
                } else if (parsed.operand_count == 1) {
                    dest_mode = get_addressing_mode(parsed.operands[0]);
                }

                encoded_word = 0;
                encoded_word |= (opcode & 0xF) << 6;
                encoded_word |= (src_mode & 0x3) << 4;
                encoded_word |= (dest_mode & 0x3) << 2;
                encoded_word |= (ARE & 0x3);

                add_code_word(code_image, IC, encoded_word, 'A');
                IC++;

                while (words > 1) {
                    add_code_word(code_image, IC, 0, 'A');
                    IC++;
                    words--;
                }
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

    if (check_mcro_name_not_label(symbol_table, head, file_name)) {
        has_errors = 1;
    }

    /* === DEBUG: CODE IMAGE OUTPUT === */
    printf("\n--- CODE IMAGE DUMP (First Pass) ---\n");

    for (i = 0; i < code_image->size; i++) {
        printf("DEBUG: IC=%d, value=%u (0x%03X), ARE=%c\n",
               code_image->words[i].address,
               code_image->words[i].value,
               code_image->words[i].value,
               code_image->words[i].ARE);
    }

/* === DEBUG: DATA IMAGE OUTPUT === */
    printf("\n--- DATA IMAGE DUMP (First Pass) ---\n");

    for (i = 0; i < DC; i++) {
        printf("DEBUG: data_image[%d] = %u\n", i, data_image[i]);
    }

/* === FINAL CHECK: Ensure IC + DC doesn't exceed allowed memory === */
    if ((IC + DC - IC_INIT_VALUE) > MAX_CODE_SIZE) {
        error_log(file_name, -1, TOTAL_MEMORY_OVERFLOW);
        has_errors = 1;
    }

    return has_errors;
}