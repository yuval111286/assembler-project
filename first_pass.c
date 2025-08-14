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


unsigned int shift_and_set_are(unsigned int final_value, int are_type) {
    /* Shift the given value left by 2 bits and add the ARE bits  */
    return (final_value << 2) | are_type;
}

unsigned int coding_word(int encoded_word, unsigned int value, unsigned int bit_mask, int shift) {
    /* Mask the value to keep only allowed and insert into the encoded word */
    encoded_word |= (value & bit_mask) << shift;
    return encoded_word;
}

int parse_matrix_dimensions(const char *token, int *rows, int *cols) {
    char cleaned[MAX_LINE_LENGTH];
    int len, read_len;

    /* Validate that input pointers are not NULL */
    if (token == NULL || rows == NULL || cols == NULL) {
        return 0;
    }

    /* Copy the token and ensure it is null-terminated */
    strncpy(cleaned, token, MAX_LINE_LENGTH - 1);
    cleaned[MAX_LINE_LENGTH - 1] = '\0';

    /* Remove any trailing commas or spaces */
    len = strlen(cleaned);
    while (len > 0 && (cleaned[len - 1] == ',' || isspace((unsigned char)cleaned[len - 1]))) {
        cleaned[len - 1] = '\0';
        len--;
    }

    /* Try to parse the format "[rows][cols]" and also capture where parsing stopped */
    if (sscanf(cleaned, MAT_DIM, rows, cols, &read_len) == 2) {
        if (cleaned[read_len] != '\0') {
            return 0;
        }

        /* Valid positive numbers */
        if (*rows > 0 && *cols > 0) {
            return 1;
        }
    }

    
    return 0;
}


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

    /* Go over macro linked list */
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

    /* Convert the string to a long using base 10 and save position in endptr */
    value = strtol(str, &endptr, 10);

    /* If no digits were found or there are extra characters after the number */
    if (endptr == str || *endptr != '\0') {
        *error_flag = 1;
        return 0;
    }

    /* Check if the value is outside the range */
    if ((value > MAX_NUM) || (value < MIN_NUM)) {
        *error_flag = 1;
        return 0;
    }

    *error_flag = 0;

    /* Convert it to 2 complement in 10bit range */
    if (value < 0) {
        value = (1 << BITS_IN_WORD) + value;
    }

    
    return (unsigned short)value;
}




int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final, CodeImage *code_image, node **head,unsigned int *data_image)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0,IC = IC_INIT_VALUE, DC = 0,value,error,i, words, discover_errors = 0;
    ParsedLine parsed;
    unsigned int encoded_word;

    /* Try to open the source file .am for reading */
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_READING);
        return 1;
    }

    /* Initialize code image structure before starting */
    init_code_image(code_image);

    /* Read the file */
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        /* Parse current line into structured format */
        if (!parse_line(line, &parsed, file_name, line_number)) {
            discover_errors = 1; /* Mark that an error was found */
            continue; 
        }

        /* LABEL HANDLING */
        if (parsed.label[0] != '\0') { /* If line has a label */
            if (symbol_exists(symbol_table, parsed.label)) { /* Duplicate check */
                error_log(file_name, line_number, DUPLICATE_LABEL);
                discover_errors = 1;
                continue;
            }
            /* Label before an instruction */
            if (parsed.line_type == LINE_INSTRUCTION) {
                if (!add_symbol(symbol_table, parsed.label, IC, SYMBOL_CODE)) {
                    error_log(file_name, line_number, FAILED_ADD_INSTRUCTION_LABEL);
                    discover_errors = 1;
                    continue;
                }
            }
            /* Label before .data / .string / .mat */
            else if (identify_directive_without_dots(parsed.directive_name) == 0 ||
                     identify_directive_without_dots(parsed.directive_name) == 1 ||
                     identify_directive_without_dots(parsed.directive_name) == 2) {
                if (!add_symbol(symbol_table, parsed.label, DC, SYMBOL_DATA)) {
                    error_log(file_name, line_number, FAILED_ADD_DATA_LABEL);
                    discover_errors = 1;
                    continue;
                }
            }
            /* Label before .extern is illegal */
            else if (identify_directive_without_dots(parsed.directive_name) == 4) {
                error_log(file_name, line_number, ILLEGAL_EXTERN_LABEL);
                discover_errors = 1;
                continue;
            }
        }

        /*  INSTRUCTION HANDLING  */
        if (parsed.line_type == LINE_INSTRUCTION) {
            int opcode,src_mode = 0, dest_mode = 0, ARE = 0,current_word; /* ARE = Absolute */

            /* Validate operand count for this opcode */
            words = instruction_word_count(&parsed);
            if (words <= 0) {
                error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
                discover_errors = 1;
                continue;
            }

            /* Determine opcode and addressing modes */
            opcode = parsed.opcode;
            if (parsed.operand_count == 2) {
                src_mode = get_addressing_mode(parsed.operands[0]);
                dest_mode = get_addressing_mode(parsed.operands[1]);
            } else if (parsed.operand_count == 1) {
                dest_mode = get_addressing_mode(parsed.operands[0]);
            }

            /* Encode first word */
            encoded_word = 0;
            encoded_word = coding_word(encoded_word,opcode,FOUR_BITS_MASK,OPCODE_BITS);
            encoded_word = coding_word(encoded_word,src_mode,TWO_BITS_MASK,SRC_BITS);
            encoded_word = coding_word(encoded_word,dest_mode,TWO_BITS_MASK,DEST_BITS);
            encoded_word = coding_word(encoded_word,ARE,TWO_BITS_MASK,ARE_BITS);
            add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
            IC++;

            /* Prepare to encode additional words if needed */
            current_word = 1;

            /*  OPERANDS ENCODING  */
            for (i = 0; i < parsed.operand_count; i++) {
                int addressing_mode = get_addressing_mode(parsed.operands[i]);

                switch (addressing_mode) {
                    case ADDRESS_IMMEDIATE: {
                        /* Immediate value #num encode directly */
                        int local_error_flag = 0;
                        int immediate_value = parse_number_from_string(parsed.operands[i] + 1, &local_error_flag); /* Skip '#' */

                        if (!local_error_flag) {
                            encoded_word = encode_signed_num(immediate_value);
                            encoded_word = (encoded_word << 2) | 0; /* ARE = 00 Absolute */
                            add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
                        } else {
                            add_code_word(code_image, IC, 0, ARE_ABSOLUTE); 
                        }
                        IC++;
                        current_word++;
                        break;
                    }

                    case ADDRESS_DIRECT: {
                        /* Label address — will be resolved in second pass */
                        add_code_word(code_image, IC, 0, ARE_ABSOLUTE);
                        IC++;
                        current_word++;
                        break;
                    }

                    case ADDRESS_MATRIX: {
                        char matrix_operand[MAX_LINE_LENGTH],*first_bracket, *second_bracket, *third_bracket, *forth_bracket;
                        int first_reg = -1, second_reg = -1;

                        /* First placeholder for matrix base address */
                        add_code_word(code_image, IC, 0, ARE_ABSOLUTE);
                        IC++;
                        current_word++;

                        /* Extract register numbers from inside the brackets */
                        strcpy(matrix_operand, parsed.operands[i]);
                        first_bracket = strchr(matrix_operand, OPENING_BRACKET);
                        if (first_bracket) {
                            second_bracket = strchr(first_bracket + 1, CLOSED_BRACKET);
                            if (second_bracket) {
                                *second_bracket = '\0';
                                first_reg = identify_register(first_bracket + 1);

                                third_bracket = strchr(second_bracket + 1, OPENING_BRACKET);
                                if (third_bracket) {
                                    forth_bracket = strchr(third_bracket + 1, CLOSED_BRACKET);
                                    if (forth_bracket) {
                                        *forth_bracket = '\0';
                                        second_reg = identify_register(third_bracket + 1);
                                    }
                                }
                            }
                        }

                        /* If both registers are valid encode them into one word */
                        if (first_reg != -1 && second_reg != -1) {
                            encoded_word = 0;
                            encoded_word = coding_word(encoded_word, first_reg, FOUR_BITS_MASK, OPCODE_BITS);
                            encoded_word = coding_word(encoded_word, second_reg, FOUR_BITS_MASK, DEST_BITS);
                            add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
                        } else {
                            add_code_word(code_image, IC, 0, ARE_ABSOLUTE); /* Error placeholder */
                        }
                        IC++;
                        current_word++;
                        break;
                    }

                    case ADDRESS_REGISTER: {
                        /* Two registers can share one word */
                        if (parsed.operand_count == 2 &&
                            get_addressing_mode(parsed.operands[0]) == ADDRESS_REGISTER &&
                            get_addressing_mode(parsed.operands[1]) == ADDRESS_REGISTER) {

                            if (i == 0) { /* Only on first operand */
                                int src_reg = identify_register(parsed.operands[0]);
                                int dest_reg = identify_register(parsed.operands[1]);

                                if (src_reg != -1 && dest_reg != -1) {
                                    encoded_word = 0;
                                    encoded_word = coding_word(encoded_word, src_reg, FOUR_BITS_MASK, OPCODE_BITS);
                                    encoded_word = coding_word(encoded_word, dest_reg, FOUR_BITS_MASK, DEST_BITS);
                                    add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
                                } else {
                                    add_code_word(code_image, IC, 0, ARE_ABSOLUTE);
                                }
                                IC++;
                                current_word++;
                            }
                        } else {
                            /* Single register operand encode normally */
                            int reg_num = identify_register(parsed.operands[i]);

                            if (reg_num != -1) {
                                encoded_word = 0;
                                if (i == 0) { /* Source reg */
                                    encoded_word = coding_word(encoded_word, reg_num, FOUR_BITS_MASK, OPCODE_BITS);
                                } else { /* Destination reg */
                                    encoded_word = coding_word(encoded_word, reg_num, FOUR_BITS_MASK, DEST_BITS);
                                }
                                add_code_word(code_image, IC, encoded_word, ARE_ABSOLUTE);
                            } else {
                                add_code_word(code_image, IC, 0, ARE_ABSOLUTE);
                            }
                            IC++;
                            current_word++;
                        }
                        break;
                    }

                    default:
                        /* Unknown addressing mode placeholder */
                        add_code_word(code_image, IC, 0, ARE_ABSOLUTE);
                        IC++;
                        current_word++;
                        break;
                }
            }
        }

        /*  DIRECTIVE HANDLING  */
        if (parsed.line_type == LINE_DIRECTIVE) {
            if (strcmp(parsed.directive_name, DIR_DATA) == 0) {
                /* Store integers in data image */
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
            } 
            else if (strcmp(parsed.directive_name, DIR_STRING) == 0) {
                /* Store string characters + null terminator in data image */
                char *s = parsed.operands[0];
                for (i = 0; s[i] != '\0'; i++) {
                    if (DC >= MAX_DATA_SIZE) {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        discover_errors = 1;
                        break;
                    }
                    data_image[DC++] = (unsigned int)s[i];
                }
                if (DC < MAX_DATA_SIZE) {
                    data_image[DC++] = 0; /* Null terminator */
                } else {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    discover_errors = 1;
                }
            } 
            else if (strcmp(parsed.directive_name, DIR_MAT) == 0) {
                /* Store matrix dimensions and values */
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
            } 
            else if (strcmp(parsed.directive_name, DIR_EXTERN) == 0) {
                /* Add external label to symbol table */
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

    /* Close the source file */
    fclose(fp);

    /* Save final IC and DC values for second pass */
    *IC_final = IC;
    *DC_final = DC;

    /* Update data symbol addresses to account for final IC */
    update_data_symbols_base_address(symbol_table, IC);

    /* Check for macro names that conflict with labels */
    if (check_mcro_name_not_label(symbol_table, head, file_name)) {
        discover_errors = 1;
    }

    /* === DEBUG: CODE IMAGE OUTPUT === 
   printf("\n--- CODE IMAGE DUMP (FIRST Pass) ---\n");

    for (i = 0; i < code_image->size; i++) {
        printf("DEBUG: IC=%d, value=%u (0x%03X), ARE=%c\n",
               code_image->words[i].address,
               code_image->words[i].value,
               code_image->words[i].value,
               code_image->words[i].ARE);
    }

     === DEBUG: DATA IMAGE OUTPUT === 
    printf("\n--- DATA IMAGE DUMP (FIRST Pass) ---\n");

    for (i = 0; i < DC; i++) {
        printf("DEBUG: data_image[%d] = %u\n", i, data_image[i]);
    }*/

     /*make sure IC + DC doesn't exceed allowed memory*/
    if ((IC + DC - IC_INIT_VALUE) > MAX_CODE_SIZE) {
        error_log(file_name, -1, TOTAL_MEMORY_OVERFLOW);
        discover_errors = 1;
    }


    /* Return 1 if any errors were found */
    return discover_errors;
}
