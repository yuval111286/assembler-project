#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "parser.h"
#include "analyze_text.h"



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
        if (strcasecmp(reg, register_table[i].name) == 0) {
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
    char buffer[MAX_LINE_LENGTH];   /* Buffer to hold a mutable copy of the line */
    char *token;   /* Pointer to each token in the line */
    /*int i;*/

    /* Initialize the ParsedLine structure fields */
    (*out).label[0] = '\0';     /* Clear label */
    (*out).directive_name[0] = '\0';  /* Clear directive name */
    (*out).operand_count = 0;  /* Start with zero operands */
    (*out).opcode = OPCODE_INVALID;  /* Default to invalid opcode */
    (*out).line_type = LINE_INVALID;   /* Default to invalid line */

    strncpy(buffer, line, MAX_LINE_LENGTH);  /* Copy input line to buffer */
    buffer[MAX_LINE_LENGTH - 1] = '\0';    /* Ensure null-termination */

    token = strtok(buffer, " \t");
    if (token == NULL) {
        (*out).line_type = LINE_EMPTY;    /* Line is empty or whitespace only */
        return 1;
    }

    /* Check if the first token is a label (ends with ':') */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';   /* Remove ':' */
        if (!is_valid_label(token)) {
            return 0;
        }
        strncpy((*out).label, token, MAX_LABEL_LEN); /* Save label */
        (*out).label[MAX_LABEL_LEN] = '\0';  /* Ensure null-termination */

        token = strtok(NULL, " \t\n");   /* Get next token after label */
        if (token == NULL) {
            return 0;    /* Label without directive/instruction */
        }
    }

    /* Check if it's a directive */
    if (token[0] == '.') {
        if (identify_directive(token) == -1) {
            return 0; /* Invalid directive name */
        }
        (*out).line_type = LINE_DIRECTIVE;     /* Mark line type as directive */
        copy_directive_name(token, (*out).directive_name);  /* Save directive name (without '.') */
    } else {
        /* Must be an instruction */
        (*out).line_type = LINE_INSTRUCTION;   /* Mark line type as instruction */
        (*out).opcode = identify_opcode(token);  /* Translate to enum */
        if ((*out).opcode == OPCODE_INVALID) {
            return 0;     /* Unknown instruction */
        }
    }

    /* Process operands, if any (separated by commas) */
    token = strtok(NULL, ",");
    while (token != NULL && (*out).operand_count < MAX_OPERANDS) {
        token = trim_spaces(token); /* Removes leading and trailing whitespace */
        strncpy((*out).operands[(*out).operand_count], token, MAX_LINE_LENGTH - 1); /* Store operand */
        (*out).operands[(*out).operand_count][MAX_LINE_LENGTH - 1] = '\0'; /* Null-terminate */
        (*out).operand_count++;
        token = strtok(NULL, ",");
    }


    return 1;  /* Successfully parsed */
}

