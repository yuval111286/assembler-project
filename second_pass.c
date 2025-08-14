#include <stdio.h>
#include <stdlib.h>
#include "second_pass.h"
#include "utils.h"
#include "errors_handler.h"
#include "parser.h"
#include "second_pass_helper.h"



int second_pass(char *am_file, SymbolTable *symbol_table, CodeImage *code_image,
                int ic_final, int dc_final, unsigned int *data_image,int discover_errors) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0, current_address = IC_INIT_VALUE;
    ParsedLine parsed;
    ExternList extern_list;

    /* Initialize extern linked list */
    extern_list.head = NULL;

    /*Open am file for reading*/
    fp = fopen(am_file, "r");
    if (fp == NULL) {
        error_log(am_file, 0, FILE_NOT_OPEN_READING);
        return 1;
    }

    /* Go over all line and process*/
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        /* Parse the line */
        if (!parse_line(line, &parsed, am_file, line_number)) {
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

                            update_code_word(code_image, current_address + word_offset, encoded_value, are_field);
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

                        update_code_word(code_image, current_address + word_offset, encoded_value, are_field);
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

    /* === DEBUG: CODE IMAGE OUTPUT === */
    /*printf("\n--- CODE IMAGE DUMP (SECOND Pass) ---\n");

    for (i = 0; i < code_image->size; i++) {
        printf("DEBUG: IC=%d, value=%u (0x%03X), ARE=%c\n",
               code_image->words[i].address,
               code_image->words[i].value,
               code_image->words[i].value,
               code_image->words[i].ARE);
    }*/

    /* === DEBUG: DATA IMAGE OUTPUT === */
    /*printf("\n--- DATA IMAGE DUMP (SECOND Pass) ---\n");

    for (i = 0; i < dc_final; i++) {
        printf("DEBUG: data_image[%d] = %u\n", i, data_image[i]);
    }*/

    /* Generate output files if no errors */
    if (!discover_errors) {
        write_code_image_to_ob_file(code_image, ic_final - IC_INIT_VALUE, dc_final, data_image, am_file);

        write_ent_file(am_file, symbol_table);

        write_ext_file(am_file, &extern_list);
    }

    free_extern_list(&extern_list);
    return discover_errors;
}