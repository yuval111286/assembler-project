#include "second_pass_helper.h"
#include "errors_handler.h"
#include "utils.h"
#include "parser_helper.h"


unsigned int shift_and_set_are(unsigned int final_value, int are_type) {
    /* Shift the given value left by 2 bits and add the ARE bits  */
    return (final_value << 2) | are_type;
}


char *turn_line_to_base_4(unsigned int word) {
    static char word_is_coded_in_base4[6] = {0};
    char base4_options[4] = {'a', 'b', 'c', 'd'};
    int i, remainder;

    /* Mask to 10 bits to ensure we only work with valid range */
    word &= 0x3FF;

    /* Last char should be 0 */
    word_is_coded_in_base4[5] = '\0';

    /* Loop is running from the end to the start */
    for (i = 4; i >= 0; i--) {
        /* Calculate the remainder by dividing by 4 */
        remainder = word % 4;
        /* Build the word by placing the base4_options in remainder index in coded word */
        word_is_coded_in_base4[i] = base4_options[remainder];
        /* Divide address by 4 to move to the next digit */
        word = word / 4;
    }

    return word_is_coded_in_base4;
}


char *turn_address_to_base_4(unsigned int address) {
    static char address_is_coded_in_base4[5] = {0};
    char base4_options[4] = {'a', 'b', 'c', 'd'};
    int i, remainder;

    /* Last char should be 0 */
    address_is_coded_in_base4[4] = '\0';

    /* Loop is running from the end to the start */
    for (i = 3; i >= 0; i--) {
        /* Calculate the remainder by dividing by 4 */
        remainder = address % 4;
        /* Build the word by placing the base4_options in remainder index in coded word */
        address_is_coded_in_base4[i] = base4_options[remainder];
        /* Divide address by 4 to move to the next digit */
        address = address / 4;
    }

    /*return coded address word*/
    return address_is_coded_in_base4;
}


char *turn_num_to_base_4(int number) {
    static char num_in_base4[10]={0};
    char temp[10]={0}, base4_chars[4] = {'a', 'b', 'c', 'd'};
    int i, digit_count, remainder;

    /* for zero return 'a'*/
    if (number == 0) {
        num_in_base4[0] = 'a';
        num_in_base4[1] = '\0';
        return num_in_base4;
    }

    /* Build digits in reverse order */
    digit_count = 0;
    while (number > 0) {
        /* Calculate the remainder by dividing by 4 */
        remainder = number % 4;
        /* Build the word by placing the base4_options in remainder index in coded word */
        temp[digit_count] = base4_chars[remainder];
        /* Divide num by 4 to move to the next digit */
        number = number / 4;
        digit_count++;
    }

    num_in_base4[digit_count] = '\0';
    /* Reverse the digits to get correct order */
    for (i = 0; i < digit_count; i++) {
        num_in_base4[i] = temp[digit_count - 1 - i];
    }

    return num_in_base4;
}



int is_operand_label(char *operand) {
    /*check if not num  */
    if (operand[0] == LADDER) return 0;

    /*check if not matrix */
    if (strchr(operand, OPENING_BRACKET)) return 0;

    /*check if not register */
    if (identify_register(operand) != -1) return 0;

    return 1;
}


void add_extern_symbol(ExternList *extern_list, char *symbol_name, int address) {
    ExternSymbol *new_node;

    /* Memory for new extern reference */
    new_node = malloc_allocation(sizeof(ExternSymbol));
    if (new_node == NULL) {
        return;
    }

    /* Copy symbol name */
    strncpy(new_node->symbol_name, symbol_name, MAX_LABEL_LEN);
    new_node->symbol_name[MAX_LABEL_LEN] = '\0';

    /* Set address and link to list */
    new_node->address = address;
    new_node->next = extern_list->head;
    extern_list->head = new_node;
}


void free_extern_list(ExternList *extern_list) {
    ExternSymbol *current = extern_list->head;
    ExternSymbol *temp;

    /* Go over the list and free all nodes */
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
    char *obj_file_name,ic_size_in_base_4[10] = {0},dc_size_in_base_4[10] = {0},temp[10]={0};
    obj_file_name = change_ending_of_file(file_name, ".ob");

    fp = fopen(obj_file_name, "w");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_WRITING);
        free(obj_file_name);
        return;
    }

    strcpy(temp, turn_num_to_base_4(ic_size));
    strcpy(ic_size_in_base_4, temp);
    strcpy(temp, turn_num_to_base_4(dc_size));
    strcpy(dc_size_in_base_4, temp);

    fprintf(fp, "%s\t%s\n", ic_size_in_base_4,dc_size_in_base_4);

    /* Go over code image section and code address and word */
    for (i = 0; i < (*img).size; i++) {
        if (i == (*img).size - 1 && dc_size == 0) {
            fprintf(fp, "%s\t%s", turn_address_to_base_4((*img).words[i].address), turn_line_to_base_4((*img).words[i].value));
        } else {
            fprintf(fp, "%s\t%s\n", turn_address_to_base_4((*img).words[i].address), turn_line_to_base_4((*img).words[i].value));
        }
    }

    /* Go over data image section code address and word */
    for (i = 0; i < dc_size; i++) {
        if (i == dc_size - 1) {
            fprintf(fp, "%s\t%s", turn_address_to_base_4(IC_INIT_VALUE + ic_size + i), turn_line_to_base_4(data_image[i]));
        } else {
            fprintf(fp, "%s\t%s\n", turn_address_to_base_4(IC_INIT_VALUE + ic_size + i), turn_line_to_base_4(data_image[i]));
        }
    }

    fclose(fp);
    free(obj_file_name);
}



void write_ext_file(char *file_name, ExternList *extern_list) {
    FILE *fp;
    char *ext_file_name, *base4_address;
    ExternSymbol *current;

    /* See if there are extern labels */
    if (extern_list->head == NULL) {
        /* No extern labels so no need to create ext file */
        return;
    }

    /* Create ext file name */
    ext_file_name = change_ending_of_file(file_name, ".ext");
    if (ext_file_name == NULL) {
        return;
    }

    /* Open ext file for writing */
    fp = fopen(ext_file_name, "w");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_WRITING);
        free(ext_file_name);
        return;
    }

    /* Writing all extern labels with matching addresses */
    current = extern_list->head;
    while (current != NULL) {
        /*calculate address in base4*/
        base4_address = turn_address_to_base_4(current->address);
        /*write to extern file extern label name and address */
        fprintf(fp, "%s\t%s\n", current->symbol_name, base4_address);
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
            has_entries_labels++;
            break;
        }
        current = current->next;
    }

    if (!has_entries_labels) {
        /* No entry label so no need to create ent file */
        return;
    }

    /* Create ent file name */
    ent_file_name = change_ending_of_file(file_name, ".ent");
    if (ent_file_name == NULL) {
        return;
    }

    /* Open ent file for writing */
    fp = fopen(ent_file_name, "w");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_WRITING);
        free(ent_file_name);
        return;
    }

    /* Go over all entry labels and write their name and address */
    current = symbol_table->head;
    while (current != NULL) {
        if (current->is_entry == 1) {
            /* Calculate address in base 4 */
            base4_address = turn_address_to_base_4(current->address);
            /* Write to file entry label name and address */
            fprintf(fp, "%s\t%s\n", current->name, base4_address);
        }
        current = current->next;
    }

    fclose(fp);
    free(ent_file_name);
}


void update_code_word(char *file_name,int line_number,CodeImage *code_image, int address, unsigned int value, char are) {
    int i;
    unsigned int final_value = value;

    /* Add ARE field to the value */
    if (are == ARE_ABSOLUTE) {
        final_value = shift_and_set_are(final_value, ABSOLUTE);  /* 00 */
    }
    else if (are == ARE_EXTERNAL) {
        final_value = shift_and_set_are(final_value, EXTERNAL);  /* 01 */
    }
    else if (are == ARE_RELOCATABLE) {
        final_value = shift_and_set_are(final_value, RELOCATABLE);  /* 10 */
    }

    /* Ensure we stay within 10-bit limit */
    final_value &= 0x3FF;

    for (i = 0; i < code_image->size; i++) {
        if (code_image->words[i].address == address) {
            code_image->words[i].value = final_value;
            code_image->words[i].ARE = are;
            return;
        }
    }

    error_log(file_name,line_number,WORD_NOT_IN_ADDRESS);
}
