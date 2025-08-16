#include <stdio.h>
#include <stdlib.h>
#include "second_pass.h"
#include "utils.h"
#include "errors_handler.h"
#include "parser.h"
#include "second_pass_helper.h"



/**
 * @brief Simplified operand parsing for second pass - no validation
 *
 * @param operands split operands by comma
 * @param out parsed line structure contains separate operand
 * @return 1 for success
 */
int shorter_parse_instruction_operands_for_second_pass(char *operands, ParsedLine *out) {
    char *token;
    int max_operands = expected_operands_for_each_opcode[out->opcode],i = 0;

    /* First token */
    token = strtok(operands, COMMA);

    /* Parse all operands */
    while (i < max_operands && token != NULL) {
        token = cut_spaces_before_and_after_string(token);

        strncpy(out->operands[i], token, MAX_LINE_LENGTH - 1);
        out->operands[i][MAX_LINE_LENGTH - 1] = '\0';
        i++;

        /* Next token */
        token = strtok(NULL, COMMA);
    }

    out->operand_count = i;
    return 1;
}

/**
 *@brief parsing line and separate operands, entering data to ParedLine
 *
 * @param line line to parse without discovering errors, it was done in the first pass
 * @param out parsed line structure
 * @return 1 for success
 */
int parse_line_second_pass(char *line, ParsedLine *out) {
    char *buffer, *cleaned_buffer,*token, *dynamic_operands_pointer;

    /* Initial output ParsedLine */
    out->label[0] = '\0';
    out->directive_name[0] = '\0';
    out->operand_count = 0;
    out->opcode = OPCODE_INVALID;
    out->line_type = LINE_INVALID;

    buffer = malloc_allocation(MAX_LINE_LENGTH);

    strncpy(buffer, line, MAX_LINE_LENGTH - 1);
    buffer[MAX_LINE_LENGTH - 1] = '\0';

    cleaned_buffer = cut_spaces_before_and_after_string(buffer);
    token = strtok(cleaned_buffer, " \t\n");

    /* extract labels */
    if (token != NULL && token[strlen(token) - 1] == DOUBLE_DOTS) {
        /* remove ':' from label*/
        token[strlen(token) - 1] = '\0';
        strncpy(out->label, token, MAX_LABEL_LEN);
        out->label[MAX_LABEL_LEN] = '\0';
        token = strtok(NULL, " \t\n");
    }

    /* process directive or instruction parsing without validations since it was performed in first pass */
    if (token != NULL && token[0] == '.') {
        /* Handle directive - just identify type */
        out->line_type = LINE_DIRECTIVE;
        copy_directive_name(token, out->directive_name);

        /* extract operands for directives that need them in second pass */
        dynamic_operands_pointer = strstr(line, token);
        if (dynamic_operands_pointer != NULL) {
            dynamic_operands_pointer += strlen(token) + 1; /* +1 for the '.' */
            dynamic_operands_pointer = cut_spaces_before_and_after_string(dynamic_operands_pointer);

            if (dynamic_operands_pointer[0] != '\0') {
                /* Simple operand extraction for .entry directive */
                if (strcmp(out->directive_name, DIR_ENTRY) == 0) {
                    strncpy(out->operands[0], dynamic_operands_pointer, MAX_LINE_LENGTH - 1);
                    out->operands[0][MAX_LINE_LENGTH - 1] = '\0';
                    out->operand_count = 1;
                }
            }
        }
    } else if (token != NULL) {
        /* handle instruction  just identify and extract operands */
        out->line_type = LINE_INSTRUCTION;
        out->opcode = identify_opcode(token);

        /* handle instructions with no operands */
        if (out->opcode == OPCODE_STOP || out->opcode == OPCODE_RTS) {
            free(buffer);
            return 1;
        }

        /* extract operands for instructions that have them */
        dynamic_operands_pointer = strstr(line, token);
        if (dynamic_operands_pointer != NULL) {
            dynamic_operands_pointer += strlen(token);
            dynamic_operands_pointer = cut_spaces_before_and_after_string(dynamic_operands_pointer);

            if (dynamic_operands_pointer[0] != '\0') {
                shorter_parse_instruction_operands_for_second_pass(dynamic_operands_pointer, out);
            }
        }
    }

    free(buffer);
    return 1;
}



int second_pass(char *am_file, SymbolTable *symbol_table, CodeImage *code_image,
                int ic_final, int dc_final, unsigned int *data_image,int discover_errors) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0, current_address = IC_INIT_VALUE;
    ParsedLine parsed;
    ExternList extern_list;

    /* initialize extern linked list */
    extern_list.head = NULL;

    /*opening am file for reading*/
    fp = fopen(am_file, "r");
    if (fp == NULL) {
        error_log(am_file, 0, FILE_NOT_OPEN_READING);
        return 1;
    }

    /* go over the line and process*/
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        /* Parse the line for second pass */
        if (!parse_line_second_pass(line, &parsed)) {
            continue;
        }


        /* Instruction lines */
        if (parsed.line_type == LINE_INSTRUCTION) {
            /* Complete encoding of operand words that need symbol resolution */
            int i, word_offset = 1; /* Skip the first word which is already encoded */

            for (i = 0; i < parsed.operand_count; i++) {
                char *operand = parsed.operands[i];
                int addressing_mode = get_addressing_mode(operand);

                switch (addressing_mode) {
                    case ADDRESS_DIRECT:
                        if (is_operand_label(operand)) {
                            Symbol *sym = get_symbol(symbol_table, operand);
                            unsigned int encoded_value = 0;
                            char are_field = ARE_ABSOLUTE;

                            if (sym != NULL) {
                                if (sym->type == SYMBOL_EXTERN) {
                                    encoded_value = 0;
                                    are_field = ARE_EXTERNAL;
                                    add_extern_symbol(&extern_list, operand, current_address + word_offset);
                                } else {
                                    encoded_value = sym->address;
                                    are_field = ARE_RELOCATABLE;
                                }
                            }
                            else{
                                error_log(am_file, line_number, UNDEFINED_LABEL);
                                discover_errors = 1;
                            }

                            update_code_word(am_file,line_number,code_image, current_address + word_offset, encoded_value, are_field);
                        }
                        word_offset++;
                        break;

                    case ADDRESS_MATRIX: {
                        /* First word: matrix address */
                        unsigned int encoded_value = 0;
                        char are_field = ARE_ABSOLUTE;
                        char matrix_name[MAX_LABEL_LEN + 1];
                        Symbol *sym;

                        /* Extract matrix name before the first '[' */
                        sscanf(operand, "%[^[]", matrix_name);

                        sym = get_symbol(symbol_table, matrix_name);

                        if (sym != NULL) {
                            if (sym->type == SYMBOL_EXTERN) {
                                encoded_value = 0;
                                are_field = ARE_EXTERNAL;
                                add_extern_symbol(&extern_list, matrix_name, current_address + word_offset);
                            } else {
                                encoded_value = sym->address;
                                are_field = ARE_RELOCATABLE;
                            }
                        }
                        else{
                            error_log(am_file, line_number, UNDEFINED_LABEL);
                            discover_errors = 1;
                        }

                        update_code_word(am_file,line_number,code_image, current_address + word_offset, encoded_value, are_field);
                        word_offset += 2; /* Matrix takes 2 additional words */
                        break;
                    }

                    case ADDRESS_REGISTER:
                        /* Check if both operands are registers */
                        if (parsed.operand_count == 2 &&
                            get_addressing_mode(parsed.operands[0]) == ADDRESS_REGISTER &&
                            get_addressing_mode(parsed.operands[1]) == ADDRESS_REGISTER) {
                            /* Skip the second register since they share a word */
                            if (i == 0) word_offset++;
                        } else {
                            word_offset++;
                        }
                        break;

                    case ADDRESS_IMMEDIATE:
                        word_offset++;
                        break;
                }
            }

            /* Update current address */
            current_address += instruction_word_count(&parsed);
        }

        if (parsed.line_type == LINE_DIRECTIVE){
            int directive_rep;
            directive_rep = identify_directive_without_dots(parsed.directive_name);
            if(directive_rep == 0 || directive_rep == 1 || directive_rep == 2 || directive_rep == 4){
                continue;
            }
        }

        /*.entry directive */
        if (parsed.line_type == LINE_DIRECTIVE &&
                identify_directive_without_dots(parsed.directive_name) == 3) {

            /* Validate that the symbol exists */
            Symbol *sym = get_symbol(symbol_table, parsed.operands[0]);
            if (sym == NULL) {
                error_log(am_file, line_number, ENTRY_LABEL_NO_DEF);
                discover_errors = 1;
            } else if (sym->type == SYMBOL_EXTERN) {
                error_log(am_file, line_number, SAME_NAME_ENTRY_AND_EXTERNAL_LABEL);
                discover_errors = 1;
            } else {
                /* Mark symbol as entry */
                sym->is_entry = 1;
            }
        }
    }

    fclose(fp);

    /* Generate output files if no errors */
    if (!discover_errors) {
        write_code_image_to_ob_file(code_image, ic_final - IC_INIT_VALUE, dc_final, data_image, am_file);

        write_ent_file(am_file, symbol_table);

        write_ext_file(am_file, &extern_list);
    }

    free_extern_list(&extern_list);
    return discover_errors;
}