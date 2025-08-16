#include "first_pass_helper.h"
#include "globals.h"
#include "ctype.h"
#include "errors_handler.h"
#include "parser_helper.h"


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

    /* Copy the token and ensure it is null terminated */
    strncpy(cleaned, token, MAX_LINE_LENGTH - 1);
    cleaned[MAX_LINE_LENGTH - 1] = '\0';

    /* Remove any trailing commas or spaces */
    len = strlen(cleaned);
    while (len > 0 && (cleaned[len - 1] == ',' || isspace((unsigned char)cleaned[len - 1]))) {
        cleaned[len - 1] = '\0';
        len--;
    }

    /* Try to parse the format [rows][cols] and also capture where parsing stopped */
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
    /*for positive num return the num for negative num return two complement representation*/
    return (unsigned int)num & 0x3FF;
}


int check_mcro_name_not_label(SymbolTable *symbol_table, node **macro_head, char *file_name) {
    node *current_macro;
    Symbol *current_symbol;
    int len,same_name = 0;
    char macro_name_without_last_char[MAX_LINE_LENGTH];

    /* Check if macro list is empty no need to check*/
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

        /* For each macro check against all symbols in symbol table */
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

