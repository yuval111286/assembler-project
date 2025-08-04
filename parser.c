#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "parser.h"
#include "analyze_text.h"
#include <ctype.h>




OpcodeEntry opcode_table[] = {
        {"mov", OPCODE_MOV},
        {"cmp", OPCODE_CMP},
        {"add", OPCODE_ADD},
        {"sub", OPCODE_SUB},
        {"lea", OPCODE_LEA},
        {"clr", OPCODE_CLR},
        {"not", OPCODE_NOT},
        {"inc", OPCODE_INC},
        {"dec", OPCODE_DEC},
        {"jmp", OPCODE_JMP},
        {"bne", OPCODE_BNE},
        {"red", OPCODE_RED},
        {"prn", OPCODE_PRN},
        {"jsr", OPCODE_JSR},
        {"rts", OPCODE_RTS},
        {"stop", OPCODE_STOP},
        {NULL, OPCODE_INVALID}
};


Directive_Mode directive_table[] = {
        {".data", DATA},
        {".string", STRING},
        {".mat", MAT},
        {".entry", ENTRY},
        {".extern", EXTERN}
};


Register_Type register_table[] = {
        {"r0", R0},
        {"r1", R1},
        {"r2", R2},
        {"r3", R3},
        {"r4", R4},
        {"r5", R5},
        {"r6", R6},
        {"r7", R7}
};


int identify_opcode(char* op_code) {

    int i;
    if (op_code == NULL) {
        return OPCODE_INVALID;
    }

    for (i = 0; opcode_table[i].name != NULL; i++) {
        if (strcasecmp(op_code, opcode_table[i].name) == 0) {
            return opcode_table[i].opcode;
        }
    }

    return OPCODE_INVALID;
}


int identify_directive(char *directive){

    int i;
    for (i = 0; i<NUM_DIRECTIVE; i++) {
        if (strcasecmp(directive, directive_table[i].name) == 0) {
            return directive_table[i].directive;
        }
    }

    return -1;

}

int identify_register(char *reg){
    int i;
    for (i = 0; i<NUM_REGISTERS; i++) {
        if (strcmp(reg, register_table[i].name) == 0) {
            return register_table[i].reg;
        }
    }

    return -1;
}



/*
 * Removes the leading '.' from a directive token and stores the name.
 */
static void copy_directive_name(char *token, char *dest) {
    if (token[0] == '.') {
        strncpy(dest, token + 1, 7); /* Copy up to 7 characters after '.' */
        dest[7] = '\0';  /* Ensure null-termination */
    } else {
        dest[0] = '\0';   /* Invalid directive format */
    }
}


/* Checks whether the given label is valid (starts with letter, alphanumeric, not too long) */
int is_valid_label(char *label) {
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


/* Determines the addressing mode of a given operand string */
int get_addressing_mode( char *operand) {
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
    if (identify_register(operand)!=-1) return ADDRESS_REGISTER;

    /* Otherwise: assume direct addressing (a label) */
    return ADDRESS_DIRECT;
}

/* Calculates the number of memory words needed to encode an instruction line */
int instruction_word_count(ParsedLine *parsed) {
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
int count_data_items(ParsedLine *parsed) {
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


/*
 * Parses a single line of assembly code and fills the ParsedLine structure.
 *
 * Parameters:
 *   line - the original text line from the source file
 *   out  - pointer to a ParsedLine struct to be populated
 *
 * Returns:
 *   1 if parsing was successful (valid line, even if empty or comment)
 *   0 if syntax error occurred (invalid label, directive, etc.)
 */
int parse_line(char *line, ParsedLine *out) {
    char buffer[MAX_LINE_LENGTH];
    char *token, *rest;
    int len;

    (*out).label[0] = '\0';
    (*out).directive_name[0] = '\0';
    (*out).operand_count = 0;
    (*out).opcode = OPCODE_INVALID;
    (*out).line_type = LINE_INVALID;

    strncpy(buffer, line, MAX_LINE_LENGTH);
    buffer[MAX_LINE_LENGTH - 1] = '\0';

    token = strtok(buffer, " \t");
    if (token == NULL) {
        (*out).line_type = LINE_EMPTY;
        return 1;
    }

    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token)) return 0;
        strncpy((*out).label, token, MAX_LABEL_LEN);
        (*out).label[MAX_LABEL_LEN] = '\0';
        token = strtok(NULL, " \t");
        if (token == NULL) return 0;
    }

    if (token[0] == '.') {
        if (identify_directive(token) == -1) return 0;
        (*out).line_type = LINE_DIRECTIVE;
        copy_directive_name(token, (*out).directive_name);
    } else {
        (*out).line_type = LINE_INSTRUCTION;
        (*out).opcode = identify_opcode(token);
        if ((*out).opcode == OPCODE_INVALID) return 0;
    }

    if ((*out).line_type == LINE_DIRECTIVE &&
        strcmp((*out).directive_name, "mat") == 0) {

        /* Get rest of line from original line after .mat */
        rest = strstr(line, ".mat");
        if (rest == NULL) return 0;

        rest = strchr(rest, ' ');
        if (rest == NULL) return 0;

        rest = trim_spaces(rest);

        /* Now split the entire operand string */
        token = strtok(rest, ",");
        while (token != NULL && (*out).operand_count < MAX_OPERANDS) {
            token = trim_spaces(token);

            /* Remove trailing comma if exists */
             len = strlen(token);
            if (len > 0 && token[len - 1] == ',') {
                token[len - 1] = '\0';
            }

            strncpy((*out).operands[(*out).operand_count], token, MAX_LINE_LENGTH - 1);
            (*out).operands[(*out).operand_count][MAX_LINE_LENGTH - 1] = '\0';
            (*out).operand_count++;
            token = strtok(NULL, ",");
        }

    } else {
        token = strtok(NULL, ",");
        while (token != NULL && (*out).operand_count < MAX_OPERANDS) {
            token = trim_spaces(token);
            strncpy((*out).operands[(*out).operand_count], token, MAX_LINE_LENGTH - 1);
            (*out).operands[(*out).operand_count][MAX_LINE_LENGTH - 1] = '\0';
            (*out).operand_count++;
            token = strtok(NULL, ",");
        }
    }

    return 1;
}
