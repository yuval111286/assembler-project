#include "utils.h"
#include "errors_handler.h"
#include <ctype.h>


void *malloc_allocation(size_t size) {
    void *ptr = malloc(size); /* allocate memory */

    if (ptr == NULL) {
        /*error fail to allocate memory*/
        printf(MEMORY_FAIL);
        return NULL;
    }

    /* return pointer */
    return ptr;
}

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

char *change_ending_of_file(char *file_name, char *new_ending) {
    char *new_file_name, *ch;

    new_file_name = malloc_allocation(MAX_LINE_LENGTH * sizeof(char));
    /*puts the current file name into new file name*/
    strcpy(new_file_name, file_name);

    /* cuts the exising ending of a file  */
    if ((ch = strchr(new_file_name, '.')) != NULL) {
        *ch = '\0';
    }
    /* link the new file name and the new ending */
    strcat(new_file_name, new_ending);

    return new_file_name;
}

/* Helper function to encode signed integer in 10-bit two's complement */
unsigned int encode_signed_value(int value) {
    if (value < 0) {
        /* For negative numbers, use two's complement representation */
        return (unsigned int)(value) & 0x3FF;
    } else {
        return (unsigned int)value & 0x3FF;
    }
}

char *turn_line_to_base_4(unsigned int word) {
    static char word_is_coded_in_base4[6] = {0};
    char base4_options[4] = {'a', 'b', 'c', 'd'};
    int i, remainder;

    /* Mask to 10 bits to ensure we only work with valid range */
    word &= 0x3FF;

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
    static char address_is_coded_in_base4[5] = {0};
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