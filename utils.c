

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "utils.h"
#include "globals.h"
#include "errors_handler.h"




/* Checks whether the given label is valid (starts with letter, alphanumeric, not too long) */
int is_valid_label(const char *label) {
    int i;
    int len;

    if (label == NULL || label[0] == '\0') {
        return 0; /* Null or empty label */
    }

    len = strlen(label);

    if (len > MAX_LABEL_LEN) {
        return 0; /* Label exceeds maximum allowed length */
    }

    if (!isalpha((unsigned char)label[0])) {
        return 0; /* Label must start with an alphabetic character */
    }

    for (i = 1; i < len; i++) {
        if (!isalnum((unsigned char)label[i])) {
            return 0; /* Label must contain only alphanumeric characters */
        }
    }

    return 1; /* Valid label */
}

void print_error(const char *filename, int lineno, const char *message) {
    SourceFileLocation location;

    /* Fill SourceFilelocation struct */
    location.file_name = (char *)filename;
    location.line = lineno;

    /* Use standardized error logger */
    external_error_log((char *)message, location);
}

/* Determines the addressing mode of a given operand string */
int get_addressing_mode(const char *operand) {
    int len;

    if (operand == NULL) {
        return -1; /* Invalid operand */
    }

    len = strlen(operand);

    /* Immediate addressing: begins with '#' followed by number */
    if (operand[0] == '#') {
        if (len > 1 && (isdigit((unsigned char)operand[1]) ||
                        operand[1] == '+' || operand[1] == '-')) {
            return ADDRESS_IMMEDIATE;
        }
        return -1; /* Invalid immediate operand */
    }

    /* Matrix addressing: contains both '[' and ']' characters */
    if (strchr(operand, '[') && strchr(operand, ']')) {
        return ADDRESS_MATRIX;
    }

    /* Register addressing: matches exactly r0 to r7 */
    if (strcmp(operand, "r0") == 0 || strcmp(operand, "r1") == 0 ||
        strcmp(operand, "r2") == 0 || strcmp(operand, "r3") == 0 ||
        strcmp(operand, "r4") == 0 || strcmp(operand, "r5") == 0 ||
        strcmp(operand, "r6") == 0 || strcmp(operand, "r7") == 0) {
        return ADDRESS_REGISTER;
    }

    /* Otherwise: assume direct addressing (a label) */
    return ADDRESS_DIRECT;
}

/* Calculates the number of memory words needed to encode an instruction line */
int instruction_word_count(const ParsedLine *parsed) {
    int count;
    int i;
    int mode;

    if (parsed == NULL) {
        return 0; /* Defensive check */
    }

    count = 1; /* Every instruction has at least one base word */

    for (i = 0; i < (*parsed).operand_count; i++) {
        mode = get_addressing_mode((*parsed).operands[i]);

        switch (mode) {
            case ADDRESS_IMMEDIATE:
            case ADDRESS_DIRECT:
                count += 1; /* Needs one additional word */
                break;

            case ADDRESS_MATRIX:
                count += 2; /* Matrix operands require two words */
                break;

            case ADDRESS_REGISTER:
                /* If both operands are registers, they share one word */
                if ((*parsed).operand_count == 2 &&
                    get_addressing_mode((*parsed).operands[0]) == ADDRESS_REGISTER &&
                    get_addressing_mode((*parsed).operands[1]) == ADDRESS_REGISTER) {
                    return 2; /* Base + 1 shared word */
                }
                count += 1;
                break;

            default:
                return 0; /* Invalid addressing mode */
        }
    }

    return count;
}

/* Returns the number of memory words required for .data, .string or .mat directives */
int count_data_items(const ParsedLine *parsed) {
    /*int count;*/
    /*int i;*/

    if (parsed == NULL) {
        return 0;
    }

    /* For .data, each operand is a number stored in one word */
    if (strcmp((*parsed).directive_name, "data") == 0) {
        return (*parsed).operand_count;
    }

    /* For .string, each character is one word including null terminator */
    if (strcmp((*parsed).directive_name, "string") == 0) {
        if ((*parsed).operand_count != 1) {
            return 0; /* Error: .string must have exactly one operand */
        }

        if ((*parsed).operands[0][0] == '"' &&
            (*parsed).operands[0][strlen((*parsed).operands[0]) - 1] == '"') {
            return strlen((*parsed).operands[0]) - 1; /* Includes null terminator */
        }

        return 0; /* Invalid format (missing quotes) */
    }

    /* For .mat, return number of operands (including size and values) */
    if (strcmp((*parsed).directive_name, "mat") == 0) {
        return (*parsed).operand_count;
    }

    return 0; /* Unknown or unsupported directive */
}

void *malloc_allocation(size_t size) {
    void *ptr = malloc(size); /* Allocate memory */

    if (ptr == NULL) {
        internal_error_log("Memory allocation failed");  /* Report error */
        return NULL;
    }

    return ptr; /* Return pointer */
}

char *change_ending_of_file(char *file_name, char *new_ending) {
    char *c, *new_file_name;

    new_file_name = malloc_allocation(MAX_LINE_LENGTH * sizeof(char));
    strcpy(new_file_name, file_name);

    /* cuts the exising ending of a file  */
    if ((c = strchr(new_file_name, '.')) != NULL) {
        *c = '\0';
    }
    /* adds the new ending of the new file name */
    strcat(new_file_name, new_ending);

    return new_file_name;
}