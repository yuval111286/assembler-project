#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "utils.h"
#include "errors_handler.h"
#include "parser.h"
#include "code_image.h"


char *turn_line_to_base_4(unsigned int word) {
    static char word_is_coded_in_base4[6] = {0};
    char base4_options[4] = {'a', 'b', 'c', 'd'};
    int i,remainder;

    /*last char should be 0*/
    word_is_coded_in_base4[5] = '\0';

    /*loop is running from the end to the start*/
    for (i = 4; i >= 0; i--) {
        /*calculate the remainder by dividing by 4*/
        remainder = word % 4;
        /*build the word by placing the base4_options in remainder index in coded word*/
        word_is_coded_in_base4[i] = base4_options[remainder];
        /*divide address by 4 to move to the next digit*/
        word = word / 4;
    }

    /*return coded address word*/
    return word_is_coded_in_base4;
}

char *turn_address_to_base_4(unsigned int address) {
    static char address_is_coded_in_base4[5] = {0};         /* 5 digits + null terminator */
    char base4_options[4] = {'a', 'b', 'c', 'd'};
    int i, remainder;

    /*last char should be 0*/
    address_is_coded_in_base4[4] = '\0';

    /*loop is running from the end to the start*/
    for (i = 3; i >= 0; i--) {
        /*calculate the remainder by dividing by 4*/
        remainder = address % 4;
        /*build the word by placing the base4_options in remainder index in coded word*/
        address_is_coded_in_base4[i] = base4_options[remainder];
        /*divide address by 4 to move to the next digit*/
        address = address / 4;
    }

    /*return coded address word*/
    return address_is_coded_in_base4;
}



int is_label_operand(char *operand) {
    /*check if not num  */
    if (operand[0] == '#') return 0;

    /*check if not matrix */
    if (strchr(operand, '[')) return 0;

    /*check if not register */
    if (identify_register(operand) != -1) return 0;

    /* return 1 for label */
    return 1;
}


void add_extern_symbol(ExternList *extern_list, char *symbol_name, int address) {
    ExternSymbol *new_node;

    /* memory for new extern reference */
    new_node = malloc_allocation(sizeof(ExternSymbol));
    if (new_node == NULL) {
        return;
    }

    /* copy symbol name */
    strncpy(new_node->symbol_name, symbol_name, MAX_LABEL_LEN);
    new_node->symbol_name[MAX_LABEL_LEN] = '\0';

    /* set address and link to list */
    new_node->address = address;
    new_node->next = extern_list->head;
    extern_list->head = new_node;
}

/* Free extern symbol list using temp node*/
void free_extern_list(ExternList *extern_list) {
    ExternSymbol *current = extern_list->head;
    ExternSymbol *temp;

    /*go over the list and free all nodes*/
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }

    extern_list->head = NULL;
}


void write_code_image_to_ob_file(CodeImage *img, int ic_size, int dc_size, unsigned int *data_image, char *file_name) {
    FILE *fp;
    int i;
    char *obj_file_name;

    obj_file_name = change_ending_of_file(file_name, ".ob");

    fp = fopen(obj_file_name, "w");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_WRITING);
        free(obj_file_name);
        return;
    }

    /* go over code image section and code address and word*/
    for (i = 0; i < (*img).size; i++) {

        fprintf(fp, "%s %s\n", turn_address_to_base_4((*img).words[i].address), turn_line_to_base_4((*img).words[i].value));
    }

    /* go over data image section code address and word*/
    for (i = 0; i < dc_size; i++) {

        fprintf(fp, "%s %s\n", turn_address_to_base_4(IC_INIT_VALUE + ic_size + i), turn_line_to_base_4(data_image[i]));
    }

    fclose(fp);
    free(obj_file_name);
}



void write_ext_file(char *file_name, ExternList *extern_list) {
    FILE *fp;
    char *ext_file_name, *base4_address;
    ExternSymbol *current;

    /* see if there are extern labels */
    if (extern_list->head == NULL) {
        /* no extern labels so no need to create ext file */
        return;
    }

    /*create ext file name*/
    ext_file_name = change_ending_of_file(file_name, ".ext");
    if (ext_file_name == NULL) {
        return;
    }

    /*open ext file for writing*/
    fp = fopen(ext_file_name, "w");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_WRITING);
        free(ext_file_name);
        return;
    }

    /*writing all extern labels with matching addresses */
    current = extern_list->head;
    while (current != NULL) {

        /*calculate address in base4*/
        base4_address = turn_address_to_base_4(current->address);
        /*write to extern file extern label name and address */
        fprintf(fp, "%s %s\n", current->symbol_name, base4_address);
        current = current->next;
    }

    fclose(fp);
    free(ext_file_name);
}


void write_ent_file(char *file_name, SymbolTable *symbol_table) {
    FILE *fp;
    char *ent_file_name, *base4_address;
    Symbol *current;
    int has_entries_labels = 0;

    /* Check if there are any entry symbols */
    current = symbol_table->head;
    while (current != NULL) {
        if (current->is_entry == 1) {
            has_entries_labels = 1;
            break;
        }
        current = current->next;
    }

    if (!has_entries_labels) {
        /* no entry label so no need to create ent file */
        return;
    }

    /*create ent file name*/
    ent_file_name = change_ending_of_file(file_name, ".ent");
    if (ent_file_name == NULL) {
        return;
    }

    /*open ent file for writing*/
    fp = fopen(ent_file_name, "w");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_WRITING);
        free(ent_file_name);
        return;
    }

    /* go over all entry labels and write their name and address */
    current = symbol_table->head;
    while (current != NULL) {
        if (current->is_entry == 1) {

            /*calculate address in base 4*/
            base4_address = turn_address_to_base_4(current->address);
            /*write to file entry label name and address*/
            fprintf(fp, "%s %s\n", current->name, base4_address);

        }
        current = current->next;
    }

    fclose(fp);
    free(ent_file_name);
}


void encode_operand(CodeImage *code_image, int *code_index, char *operand,
                    SymbolTable *symbol_table, ExternList *extern_list, int current_address) {
    int addressing_mode = get_addressing_mode(operand);
    unsigned int encoded_value = 0;
    char are_field = 'A';

    switch (addressing_mode) {
        case ADDRESS_IMMEDIATE:
            /* remove # and convert to integer */
            encoded_value = atoi(operand + 1);
            are_field = 'A'; /* absolute */
            add_code_word(code_image, current_address + *code_index, encoded_value, are_field);
            (*code_index)++;
            break;

        case ADDRESS_DIRECT:
            /* direct label reference */
            if (is_label_operand(operand)) {
                Symbol *sym = get_symbol(symbol_table, operand);
                if (sym != NULL) {
                    if (sym->type == SYMBOL_EXTERN) {
                        /* external label */
                        encoded_value = 0;
                        /* for external */
                        are_field = 'E';
                        /*adding extern label to extern linked list*/
                        add_extern_symbol(extern_list, operand, current_address + *code_index);
                    } else {
                        /* local label */
                        encoded_value = sym->address;
                        /* for relocatable */
                        are_field = 'R';
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

void encode_instruction(CodeImage *code_image, ParsedLine *parsed, int current_address,
                               SymbolTable *symbol_table, ExternList *extern_list) {
    unsigned int encoded_word = 0;
    int i,code_index = 0;
    int src_mode = -1, dst_mode = -1;

    /* get addressing modes for operands */
    if (parsed->operand_count >= 1) {
        src_mode = get_addressing_mode(parsed->operands[0]);
    }
    if (parsed->operand_count >= 2) {
        dst_mode = get_addressing_mode(parsed->operands[1]);
    }

    /* encode first word: opcode and addressing modes */
    encoded_word |= (parsed->opcode << 6); /* opcode in bits 6-9 */
    if (src_mode != -1) {
        encoded_word |= (src_mode << 4); /* source mode in bits 4-5 */
    }
    if (dst_mode != -1) {
        encoded_word |= (dst_mode << 2); /* destination mode in bits 2-3 */
    }
    /* ARE bits 0-1 are always 'A' for first word */

    /* add first word to code image */
    add_code_word(code_image, current_address, encoded_word, 'A');
    code_index = 1;

    /* encode operands */
    for (i = 0; i < parsed->operand_count; i++) {
        encode_operand(code_image, &code_index, parsed->operands[i],
                       symbol_table, extern_list, current_address);
    }
}


int second_pass(char *am_file, SymbolTable *symbol_table, CodeImage *code_image,
                int ic_final, int dc_final, unsigned int *data_image) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0,discover_errors = 0,current_address = IC_INIT_VALUE;
    ParsedLine parsed;
    ExternList extern_list;

    /* initialize extern linked list */
    extern_list.head = NULL;

    /*open am file for reading*/
    fp = fopen(am_file, "r");
    if (fp == NULL) {
        error_log(am_file, 0, FILE_NOT_OPEN_READING);
        return 1;
    }

    /* go over all line and process*/
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        /* parse the line */
        if (!parse_line(line, &parsed, am_file, line_number)) {
            /*no label in the line, move to the next line*/
            continue;
        }

        /* instruction lines */
        if (parsed.line_type == LINE_INSTRUCTION) {
            /* encode the complete instruction */
            encode_instruction(code_image, &parsed, current_address, symbol_table, &extern_list);

            /* update current address */
            current_address += instruction_word_count(&parsed);
        }

        /*.entry directive */
        if (parsed.line_type == LINE_DIRECTIVE &&
            strcmp(parsed.directive_name, "entry") == 0) {

            /* validate that the symbol exists */
            Symbol *sym = get_symbol(symbol_table, parsed.operands[0]);
            if (sym == NULL) {
                error_log(am_file, line_number, ENTRY_LABEL_NO_DEF);
                discover_errors = 1;
            } else if (sym->type == SYMBOL_EXTERN) {
                error_log(am_file, line_number, SAME_NAME_ENTRY_AND_EXTERNAL_LABEL);
                discover_errors = 1;
            } else {
                /* mark symbol as entry */
                sym->is_entry = 1;
            }
        }
    }

    fclose(fp);

    /* Generate output files if no errors */
    if (!discover_errors) {

        /* creating .ent file containing label name and their address only if exists*/
        write_ent_file(am_file, symbol_table);

        /* creating .ext file containing label name and their address only if exists*/
        write_ext_file(am_file, &extern_list);
        write_code_image_to_ob_file(code_image, ic_final - IC_INIT_VALUE, dc_final, data_image, am_file);
    }

    free_extern_list(&extern_list);

    return discover_errors;
}