
#include <stdio.h>              
#include <string.h>             
#include <ctype.h>              
#include <stdlib.h>             
#include "utils.h"              
#include "globals.h"            
#include "errors_handler.h"     



/*
 * Validates whether a label is legal.
 * Must start with letter, contain only alphanum, and be short enough.
 */
int is_valid_label(const char *label) {
    int i, len;

    if (label == NULL || label[0] == '\0') return 0;    /* Null or empty */
    
    len = strlen(label);      /* Calculate length */
    if (len > MAX_LABEL_LEN) return 0;     /* Too long */

    if (!isalpha((unsigned char)label[0])) return 0;   /* First char must be letter */

    for (i = 1; i < len; i++) {
        if (!isalnum((unsigned char)label[i])) return 0; /* All chars must be alphanumeric */
    }

    return 1;  /* Label is valid */
}

/*
 * Wrapper function to report an error with file and line info.
 */
void print_error(const char *filename, int lineno, const char *message) {
    SourceFilelocation loc;     /* Struct holding file + line number */
    loc.file_name = filename;   /* Set file name */
    loc.line = lineno;  /* Set line number */
    external_error_log((char *)message, loc);  /* Delegate to centralized logger */
}

/*
 * Determines the addressing mode of a given operand string.
 * Returns an AddressingMode enum value or -1 on error.
 */
int get_addressing_mode(const char *operand) {
    int len;

    if (operand == NULL) return -1;     /* Null operand */

    len = strlen(operand);    /* Compute string length */

    if (operand[0] == '#') {     /* Immediate addressing */
        if (len > 1 && (isdigit((unsigned char)operand[1]) ||
            operand[1] == '+' || operand[1] == '-')) {
            return ADDRESS_IMMEDIATE;   /* Valid immediate */
        }
        return -1;   /* Invalid immediate */
    }

    if (strchr(operand, '[') && strchr(operand, ']')) { /* Matrix addressing */
        return ADDRESS_MATRIX;
    }

    if (strcmp(operand, "r0") == 0 || strcmp(operand, "r1") == 0 ||
        strcmp(operand, "r2") == 0 || strcmp(operand, "r3") == 0 ||
        strcmp(operand, "r4") == 0 || strcmp(operand, "r5") == 0 ||
        strcmp(operand, "r6") == 0 || strcmp(operand, "r7") == 0) {
        return ADDRESS_REGISTER;   /* Register addressing */
    }

    return ADDRESS_DIRECT;    /* Default: direct addressing */
}

/*
 * Computes the number of memory words required for an instruction.
 */
int instruction_word_count(const ParsedLine *parsed) {
    int count = 1;    /* Base instruction word */
    int i, mode;

    if (parsed == NULL) return 0;    /* Null input safety */

    for (i = 0; i < (*parsed).operand_count; i++) {
        mode = get_addressing_mode((*parsed).operands[i]); /* Detect mode */

        switch (mode) {
            case ADDRESS_IMMEDIATE:
            case ADDRESS_DIRECT:
                count += 1;      /* One word extra */
                break;
            case ADDRESS_MATRIX:
                count += 2;     /* Two words extra */
                break;
            case ADDRESS_REGISTER:
                if ((*parsed).operand_count == 2 &&
                    get_addressing_mode((*parsed).operands[0]) == ADDRESS_REGISTER &&
                    get_addressing_mode((*parsed).operands[1]) == ADDRESS_REGISTER) {
                    return 2;     /* Shared register word */
                }
                count += 1;
                break;
            default:
                return 0;      /* Invalid operand */
        }
    }

    return count;    /* Return total word count */
}

/*
 * Counts how many memory words are required for data-type directives.
 */
int count_data_items(const ParsedLine *parsed) {
    if (parsed == NULL) return 0;  /* Null check */

    if (strcmp((*parsed).directive_name, "data") == 0) {   /* .data directive */
        return (*parsed).operand_count;  /* Each number = 1 word */
    }

    if (strcmp((*parsed).directive_name, "string") == 0) {   /* .string directive */
        if ((*parsed).operand_count != 1) return 0;   /* Should have 1 operand */
        if ((*parsed).operands[0][0] == '"' &&
            (*parsed).operands[0][strlen((*parsed).operands[0]) - 1] == '"') {
            return strlen((*parsed).operands[0]) - 1;   /* Includes null terminator */
        }
        return 0;
    }

    if (strcmp((*parsed).directive_name, "mat") == 0) {  /* .mat directive */
        return (*parsed).operand_count;  /* Rows + cols + values */
    }

    return 0;     /* Unknown directive */
}


/*
 * Wrapper for malloc with error checking.
 * Returns allocated pointer or NULL if failed.
 */
void *malloc_allocation(size_t size) {
    void *ptr = malloc(size);                            /* Allocate memory */

    if (ptr == NULL) {
        internal_error_log("Memory allocation failed");  /* Report error */
        return NULL;
    }

    return ptr;                                          /* Return pointer */
}
