#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "first_pass.h"
#include "parser.h"
#include "utils.h"
#include "errors_handler.h"
#include "code_image.h"

int parse_matrix_dimensions(const char *token, int *rows, int *cols) {
    char cleaned[MAX_LINE_LENGTH];
    int len, read_len;

    if (token == NULL || rows == NULL || cols == NULL) {
        return 0;
    }

    strncpy(cleaned, token, MAX_LINE_LENGTH - 1);
    cleaned[MAX_LINE_LENGTH - 1] = '\0';

    len = strlen(cleaned);
    while (len > 0 && (cleaned[len - 1] == ',' || isspace((unsigned char)cleaned[len - 1]))) {
        cleaned[len - 1] = '\0';
        len--;
    }

    if (sscanf(cleaned, "[%d][%d]%n", rows, cols, &read_len) == 2) {
        if (cleaned[read_len] != '\0') {
            return 0; /* extra characters after matrix dimension */
        }

        if (*rows > 0 && *cols > 0) {
            return 1;
        }
    }

    return 0;
}

/* encode signed integer in 10-bit two's complement */
unsigned int encode_signed_num(int num) {
    /*for positive num return the num
     *for negative num return two's complement representation*/
    return (unsigned int)num & 0x3FF;
}

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


unsigned short parse_number_from_string(const char *str, int *error_flag) {
    char *endptr;
    long value;

    if (str == NULL) {
        *error_flag = 1;
        return 0;
    }

    errno = 0;
    value = strtol(str, &endptr, 10);

    if (endptr == str || *endptr != '\0') {
        *error_flag = 1;
        return 0;
    }

    if ((value > MAX_NUM) || (value < MIN_NUM)) {
        *error_flag = 1;
        return 0;
    }

    *error_flag = 0;

    /* Convert to 2's complement encoding in 10 bits*/
    if (value < 0) {
        value = (1 << BITS_IN_WORD) + value;
    }

    return (unsigned short)value;
}


/**
 * Performs the first pass over a .am file and builds the symbol table,
 * encodes instructions, and processes data directives into the image arrays.
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final, CodeImage *code_image, node **head,unsigned int *data_image)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0,IC = IC_INIT_VALUE, DC = 0,value,error,i, words, discover_errors = 0;
    ParsedLine parsed;
    unsigned int encoded_word;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_READING);
        return 1;
    }

    init_code_image(code_image);

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        if (!parse_line(line, &parsed, file_name, line_number)) {
            discover_errors = 1;
            continue;
        }

        /* Skip empty lines and comments */
        if (parsed.line_type == LINE_EMPTY || parsed.line_type == LINE_COMMENT) {
            continue;
        }

        /* label handling */
        if (parsed.label[0] != '\0') {

            if (identify_opcode(parsed.label) != OPCODE_INVALID ||
                identify_directive(parsed.label) != -1 ||
                identify_register(parsed.label) != -1) {
                error_log(file_name, line_number, RESERVED_WORD_AS_LABEL);
                discover_errors = 1;
                continue;
            }
            if (symbol_exists(symbol_table, parsed.label)) {
                error_log(file_name, line_number, DUPLICATE_LABEL);
                discover_errors = 1;
                continue;
            }
            if (parsed.line_type == LINE_INSTRUCTION) {
                if (!add_symbol(symbol_table, parsed.label, IC, SYMBOL_CODE)) {
                    error_log(file_name, line_number, FAILED_ADD_INSTRUCTION_LABEL);
                    discover_errors = 1;
                    continue;
                }
            } else if (identify_directive_without_dots(parsed.directive_name) == 0 ||
                       identify_directive_without_dots(parsed.directive_name) == 1 ||
                       identify_directive_without_dots(parsed.directive_name) == 2) {
                if (!add_symbol(symbol_table, parsed.label, DC, SYMBOL_DATA)) {
                    error_log(file_name, line_number, FAILED_ADD_DATA_LABEL);
                    discover_errors = 1;
                    continue;
                }
            } else if (identify_directive_without_dots(parsed.directive_name) == 4) {
                error_log(file_name, line_number, ILLEGAL_EXTERN_LABEL);
                discover_errors = 1;
                continue;
            }
        }

        /* instruction line handling */
        if (parsed.line_type == LINE_INSTRUCTION) {
            /* Encode base word of instruction */
            int opcode,src_mode = 0, dest_mode = 0, ARE = 0,current_word;/* Absolute */

            words = instruction_word_count(&parsed);
            if (words <= 0) {
                error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
                discover_errors = 1;
                continue;
            }

            opcode = parsed.opcode;
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

            add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
            IC++;

            /* Now encode additional words that can be encoded in first pass */
            current_word = 1;

            /* Handle operands */
            for (i = 0; i < parsed.operand_count; i++) {
                int addressing_mode = get_addressing_mode(parsed.operands[i]);

                switch (addressing_mode) {
                    case ADDRESS_IMMEDIATE: {
                        /* Parse and encode immediate value */
                        int local_error_flag = 0;
                        int immediate_value = parse_number_from_string(parsed.operands[i] + 1, &local_error_flag); /* Skip '#' */

                        if (!local_error_flag) {
                            encoded_word = encode_signed_num(immediate_value);
                            encoded_word = (encoded_word << 2) | 0; /* ARE = 00 (Absolute) */
                            add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
                        } else {
                            add_code_word(code_image, IC, 0, ARE_ABSOLUTE); /* Error case */
                        }
                        IC++;
                        current_word++;
                        break;
                    }

                    case ADDRESS_DIRECT: {
                        /* Direct addressing - will be filled in second pass */
                        add_code_word(code_image, IC, 0, ARE_ABSOLUTE); /* Placeholder */
                        IC++;
                        current_word++;
                        break;
                    }

                    case ADDRESS_MATRIX: {
                        /* Second word - encode register indices */
                        char matrix_operand[MAX_LINE_LENGTH],*bracket1, *bracket2, *bracket3, *bracket4;
                        int first_reg = -1, second_reg = -1;

                        /* Matrix addressing - first word (address) will be filled in second pass */
                        add_code_word(code_image, IC, 0, ARE_ABSOLUTE); /* Placeholder for matrix address */
                        IC++;
                        current_word++;


                        strcpy(matrix_operand, parsed.operands[i]);

                        /* Find register names between brackets */
                        bracket1 = strchr(matrix_operand, '[');
                        if (bracket1) {
                            bracket2 = strchr(bracket1 + 1, ']');
                            if (bracket2) {
                                *bracket2 = '\0';
                                first_reg = identify_register(bracket1 + 1);

                                bracket3 = strchr(bracket2 + 1, '[');
                                if (bracket3) {
                                    bracket4 = strchr(bracket3 + 1, ']');
                                    if (bracket4) {
                                        *bracket4 = '\0';
                                        second_reg = identify_register(bracket3 + 1);
                                    }
                                }
                            }
                        }

                        if (first_reg != -1 && second_reg != -1) {
                            encoded_word = 0;
                            encoded_word |= (first_reg & 0xF) << 6;  /* Bits 9-6: first register */
                            encoded_word |= (second_reg & 0xF) << 2;  /* Bits 5-2: second register */
                            encoded_word |= 0;                   /* Bits 1-0: ARE = 00 (Absolute) */
                            add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
                        } else {
                            add_code_word(code_image, IC, 0, ARE_ABSOLUTE); /* Error case */
                        }
                        IC++;
                        current_word++;
                        break;
                    }

                    case ADDRESS_REGISTER: {
                        /* Check if both operands are registers - they share one word */
                        if (parsed.operand_count == 2 &&
                            get_addressing_mode(parsed.operands[0]) == ADDRESS_REGISTER &&
                            get_addressing_mode(parsed.operands[1]) == ADDRESS_REGISTER) {

                            if (i == 0) { /* First register operand */
                                int src_reg = identify_register(parsed.operands[0]);
                                int dest_reg = identify_register(parsed.operands[1]);

                                if (src_reg != -1 && dest_reg != -1) {
                                    encoded_word = 0;
                                    encoded_word |= (src_reg & 0xF) << 6;   /* Bits 9-6: source register */
                                    encoded_word |= (dest_reg & 0xF) << 2; /* Bits 5-2: destination register */
                                    encoded_word |= 0;                     /* Bits 1-0: ARE = 00 (Absolute) */
                                    add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
                                } else {
                                    add_code_word(code_image, IC, 0, ARE_ABSOLUTE); /* Error case */
                                }
                                IC++;
                                current_word++;
                            }
                            /* Skip second register as it's already encoded */
                        } else {
                            /* Single register operand */
                            int reg_num = identify_register(parsed.operands[i]);

                            if (reg_num != -1) {
                                encoded_word = 0;
                                if (i == 0) { /* Source operand */
                                    encoded_word |= (reg_num & 0xF) << 6; /* Bits 9-6 */
                                } else { /* Destination operand */
                                    encoded_word |= (reg_num & 0xF) << 2; /* Bits 5-2 */
                                }
                                encoded_word |= 0; /* ARE = 00 (Absolute) */
                                add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
                            } else {
                                add_code_word(code_image, IC, 0, ARE_ABSOLUTE); /* Error case */
                            }
                            IC++;
                            current_word++;
                        }
                        break;
                    }

                    default:
                        /* Unknown addressing mode */
                        add_code_word(code_image, IC, 0, ARE_ABSOLUTE);
                        IC++;
                        current_word++;
                        break;
                }
            }
        }

        /* directive line handling */
        if (parsed.line_type == LINE_DIRECTIVE) {
            if (strcmp(parsed.directive_name, "data") == 0) {
                for (i = 0; i < parsed.operand_count; i++) {
                    if (DC >= MAX_DATA_SIZE) {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        discover_errors = 1;
                        continue;
                    }

                    value = parse_number_from_string(parsed.operands[i], &error);
                    if (!error) {
                        data_image[DC++] = (short)value;
                    } else {
                        error_log(file_name,line_number,FAIL_CONVERT_STRING_TO_NUM);
                    }
                }
            } else if (strcmp(parsed.directive_name, "string") == 0) {
                char *s;
                s = parsed.operands[0];
                for (i = 0; s[i] != '\0'; i++) {
                    if (DC >= MAX_DATA_SIZE) {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        discover_errors = 1;
                        break;
                    }
                    data_image[DC++] = (unsigned int)s[i];
                }
                if (DC < MAX_DATA_SIZE) {
                    data_image[DC++] = 0;
                } else {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    discover_errors = 1;
                }
            } else if (strcmp(parsed.directive_name, "mat") == 0) {
                int mat_rows, mat_cols, expected_values;

                if (!parse_matrix_dimensions(parsed.operands[0], &mat_rows, &mat_cols)) {
                    error_log(file_name, line_number, INVALID_MATRIX_DIMENSIONS);
                    discover_errors = 1;
                    continue;
                }

                expected_values = mat_rows * mat_cols;

                if (parsed.operand_count != 1 + expected_values) {
                    error_log(file_name, line_number, MATRIX_VALUE_COUNT_MISMATCH);
                    discover_errors = 1;
                    continue;
                }

                if (DC + 2 + expected_values > MAX_DATA_SIZE) {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    discover_errors = 1;
                    continue;
                }

                for (i = 1; i < parsed.operand_count; i++) {
                    value = parse_number_from_string(parsed.operands[i], &error);
                    if (!error) {
                        data_image[DC++] = (short)value;
                    } else {
                        error_log(file_name,line_number,FAIL_CONVERT_STRING_TO_NUM);
                    }
                }
            } else if (strcmp(parsed.directive_name, "extern") == 0) {
                if (parsed.operand_count != 1) {
                    error_log(file_name, line_number, EXTERN_SYNTAX_ERROR);
                    discover_errors = 1;
                } else if (!add_symbol(symbol_table, parsed.operands[0], 0, SYMBOL_EXTERN)) {
                    error_log(file_name, line_number, DUPLICATE_EXTERN);
                    discover_errors = 1;
                }
            }
        }
    }

    fclose(fp);

    *IC_final = IC;
    *DC_final = DC;

    update_data_symbols_base_address(symbol_table, IC);

    if (check_mcro_name_not_label(symbol_table, head, file_name)) {
        discover_errors = 1;
    }

    /* === DEBUG: CODE IMAGE OUTPUT === */
    printf("\n--- CODE IMAGE DUMP (FIRST Pass) ---\n");

    for (i = 0; i < code_image->size; i++) {
        printf("DEBUG: IC=%d, value=%u (0x%03X), ARE=%c\n",
               code_image->words[i].address,
               code_image->words[i].value,
               code_image->words[i].value,
               code_image->words[i].ARE);
    }

    /* === DEBUG: DATA IMAGE OUTPUT === */
    printf("\n--- DATA IMAGE DUMP (FIRST Pass) ---\n");

    for (i = 0; i < DC; i++) {
        printf("DEBUG: data_image[%d] = %u\n", i, data_image[i]);
    }

    /* make sure IC + DC doesn't exceed allowed memory*/
    if ((IC + DC - IC_INIT_VALUE) > MAX_CODE_SIZE) {
        error_log(file_name, -1, TOTAL_MEMORY_OVERFLOW);
        discover_errors = 1;
    }

    return discover_errors;
}