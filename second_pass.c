#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "second_pass.h"
#include "utils.h"
#include "errors_handler.h"
#include "parser.h"
#include "analyze_text.h"
#include "code_image.h"



/* Helper function to check if operand is a label */
int is_label_operand(char *operand) {
    /* Not immediate (#5) */
    if (operand[0] == '#') return 0;

    /* Not matrix (label[2][3]) */
    if (strchr(operand, '[')) return 0;

    /* Not register (r0-r7) */
    if (identify_register(operand) != -1) return 0;

    /* If we got here - it's a label */
    return 1;
}

/* Add extern reference to list */
void add_extern_reference(ExternList *extern_list, char *symbol_name, int address) {
    ExternRef *new_ref;

    /* Allocate memory for new extern reference */
    new_ref = malloc_allocation(sizeof(ExternRef));
    if (new_ref == NULL) {
        return;
    }

    /* Copy symbol name */
    strncpy(new_ref->symbol_name, symbol_name, MAX_LABEL_LEN);
    new_ref->symbol_name[MAX_LABEL_LEN] = '\0';

    /* Set address and link to list */
    new_ref->address = address;
    new_ref->next = extern_list->head;
    extern_list->head = new_ref;
}

/* Free extern references list */
void free_extern_list(ExternList *extern_list) {
    ExternRef *current = extern_list->head;
    ExternRef *temp;

    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }

    extern_list->head = NULL;
}

/* Write .ext file if there are extern references */
void write_ext_file(char *base_filename, ExternList *extern_list) {
    FILE *fp;
    char *ext_filename, *base4_address;
    ExternRef *current;

    /* Check if there are any extern references */
    if (extern_list->head == NULL) {
        return; /* No extern references, don't create file */
    }

    ext_filename = change_ending_of_file(base_filename, ".ext");
    if (ext_filename == NULL) {
        return;
    }

    fp = fopen(ext_filename, "w");
    if (fp == NULL) {
        error_log(base_filename, 0, FILE_NOT_OPEN_WRITING);
        free(ext_filename);
        return;
    }

    /* Write all extern references */
    current = extern_list->head;
    while (current != NULL) {

        base4_address = turn_address_to_base_4(current->address);
        fprintf(fp, "%s %s\n", current->symbol_name, base4_address);
        current = current->next;
    }

    fclose(fp);
    free(ext_filename);
}

/* Write .ent file for entry symbols */
void write_ent_file(char *base_filename, SymbolTable *symbol_table) {
    FILE *fp;
    char *ent_filename, *base4_address;
    Symbol *current;
    int has_entries = 0;

    /* Check if there are any entry symbols */
    current = symbol_table->head;
    while (current != NULL) {
        if (current->is_entry == 1) {
            has_entries = 1;
            break;
        }
        current = current->next;
    }

    if (!has_entries) {
        return; /* No entry symbols, don't create file */
    }

    ent_filename = change_ending_of_file(base_filename, ".ent");
    if (ent_filename == NULL) {
        return;
    }

    fp = fopen(ent_filename, "w");
    if (fp == NULL) {
        error_log(base_filename, 0, FILE_NOT_OPEN_WRITING);
        free(ent_filename);
        return;
    }

    /* Write all entry symbols */
    current = symbol_table->head;
    while (current != NULL) {
        if (current->is_entry == 1) {

            base4_address = turn_address_to_base_4(current->address);
            fprintf(fp, "%s %s\n", current->name, base4_address);

        }
        current = current->next;
    }

    fclose(fp);
    free(ent_filename);
}

/* Encode instruction operand */
void encode_operand(CodeImage *code_image, int *code_index, char *operand,
                           SymbolTable *symbol_table, ExternList *extern_list, int current_address) {
    int addressing_mode = get_addressing_mode(operand);
    unsigned int encoded_value = 0;
    char are_field = 'A';

    switch (addressing_mode) {
        case ADDRESS_IMMEDIATE:
            /* Remove # and convert to integer */
            encoded_value = atoi(operand + 1);
            are_field = 'A'; /* Absolute */
            add_code_word(code_image, current_address + *code_index, encoded_value, are_field);
            (*code_index)++;
            break;

        case ADDRESS_DIRECT:
            /* Direct label reference */
            if (is_label_operand(operand)) {
                Symbol *sym = get_symbol(symbol_table, operand);
                if (sym != NULL) {
                    if (sym->type == SYMBOL_EXTERN) {
                        /* External label */
                        encoded_value = 0; /* Will be resolved by linker */
                        are_field = 'E'; /* External */
                        add_extern_reference(extern_list, operand, current_address + *code_index);
                    } else {
                        /* Local label */
                        encoded_value = sym->address;
                        are_field = 'R'; /* Relocatable */
                    }
                }
            }
            add_code_word(code_image, current_address + *code_index, encoded_value, are_field);
            (*code_index)++;
            break;

        case ADDRESS_MATRIX:
            /* Matrix addressing: label[row][col] */
            /* First word: base address of matrix */
            /* Second word: index calculation */
            /* This is a simplified implementation - you may need to parse the indices */
            add_code_word(code_image, current_address + *code_index, 0, 'A');
            (*code_index)++;
            add_code_word(code_image, current_address + *code_index, 0, 'A');
            (*code_index)++;
            break;

        case ADDRESS_REGISTER:
            /* Register addressing */
            encoded_value = identify_register(operand);
            are_field = 'A'; /* Absolute */
            add_code_word(code_image, current_address + *code_index, encoded_value, are_field);
            (*code_index)++;
            break;

        default:
            /* Invalid addressing mode */
            add_code_word(code_image, current_address + *code_index, 0, 'A');
            (*code_index)++;
            break;
    }
}

/* Encode complete instruction */
void encode_instruction(CodeImage *code_image, ParsedLine *parsed, int current_address,
                               SymbolTable *symbol_table, ExternList *extern_list) {
    unsigned int encoded_word = 0;
    int i,code_index = 0;
    int src_mode = -1, dst_mode = -1;

    /* Get addressing modes for operands */
    if (parsed->operand_count >= 1) {
        src_mode = get_addressing_mode(parsed->operands[0]);
    }
    if (parsed->operand_count >= 2) {
        dst_mode = get_addressing_mode(parsed->operands[1]);
    }

    /* Encode first word: opcode and addressing modes */
    encoded_word |= (parsed->opcode << 6); /* Opcode in bits 6-9 */
    if (src_mode != -1) {
        encoded_word |= (src_mode << 4); /* Source mode in bits 4-5 */
    }
    if (dst_mode != -1) {
        encoded_word |= (dst_mode << 2); /* Destination mode in bits 2-3 */
    }
    /* ARE bits 0-1 are always 'A' for first word */

    /* Add first word to code image */
    add_code_word(code_image, current_address, encoded_word, 'A');
    code_index = 1;

    /* Encode operands */
    for (i = 0; i < parsed->operand_count; i++) {
        encode_operand(code_image, &code_index, parsed->operands[i],
                       symbol_table, extern_list, current_address);
    }
}

/**
 * @brief Performs the second pass of the assembler
 *
 * @param am_file Base filename (without extension)
 * @param symbol_table Pointer to the symbol table from first pass
 * @param code_image Pointer to the code image to complete
 * @param ic_final Final instruction counter from first pass
 * @param dc_final Final data counter from first pass
 * @param data_image Array containing data image from first pass
 * @return 0 if successful, 1 if errors occurred
 */
int second_pass(char *am_file, SymbolTable *symbol_table, CodeImage *code_image,
                int ic_final, int dc_final, unsigned int *data_image) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0,discover_errors = 0,current_address = IC_INIT_VALUE;
    ParsedLine parsed;
    ExternList extern_list;

    /* Initialize extern list */
    extern_list.head = NULL;

    fp = fopen(am_file, "r");
    if (fp == NULL) {
        error_log(am_file, 0, FILE_NOT_OPEN_READING);
        return 1;
    }

    /* Process each line */
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        /* Parse the line */
        if (!parse_line(line, &parsed, am_file, line_number)) {
            continue; /* Skip invalid lines */
        }

        /* Handle instruction lines */
        if (parsed.line_type == LINE_INSTRUCTION) {
            /* Encode the complete instruction */
            encode_instruction(code_image, &parsed, current_address, symbol_table, &extern_list);

            /* Update current address */
            current_address += instruction_word_count(&parsed);
        }

        /* Handle .entry directive */
        if (parsed.line_type == LINE_DIRECTIVE &&
            strcmp(parsed.directive_name, "entry") == 0) {

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
        /* Write .ob file */
        write_code_image_to_ob_file(code_image, ic_final- IC_INIT_VALUE, dc_final, data_image, am_file);

        /* Write .ent file (only if there are entry symbols) */
        write_ent_file(am_file, symbol_table);

        /* Write .ext file (only if there are extern references) */
        write_ext_file(am_file, &extern_list);
    }

    /* Clean up */
    free_extern_list(&extern_list);

    return discover_errors;
}