#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser_helper.h"
#include "errors_handler.h"
#include "utils.h"

/* 
 * Opcode table
 * Maps instruction mnemonic strings to their corresponding Opcode enum values.
 */
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

/*
 * Directive table 
 * Matches directive strings to their Directive enum values.
 */
Directive_Mode directive_table[] = {
        {".data", DATA},
        {".string", STRING},
        {".mat", MAT},
        {".entry", ENTRY},
        {".extern", EXTERN}
};

/*
 * Directive table without leading dots.
 */

Directive_Mode directive_table_without_dots[] = {
        {"data", DATA},
        {"string", STRING},
        {"mat", MAT},
        {"entry", ENTRY},
        {"extern", EXTERN}
};

/*
 * Register table
 * Maps register names r0–r7 to their Register enum values.
 */

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

/* Expected number of operands for each opcode */
const int expected_operands_for_each_opcode[16] = {
        2, /* mov */
        2, /* cmp */
        2, /* add */
        2, /* sub */
        2, /* lea */
        1, /* clr */
        1, /* not */
        1, /* inc */
        1, /* dec */
        1, /* jmp */
        1, /* bne */
        1, /* red */
        1, /* prn */
        1, /* jsr */
        0, /* rts */
        0  /* stop */
};

/* Table of allowed addressing modes for each opcode
0 = immediate 1 = direct 2 = matrix 3 = register */

const int allowed_source_modes[16][4] = {
        /* mov */ {1, 1, 1, 1},
        /* cmp */ {1, 1, 1, 1},
        /* add */ {1, 1, 1, 1},
        /* sub */ {1, 1, 1, 1},
        /* lea */ {0, 1, 1, 0},
        /* clr */ {0, 0, 0, 0},
        /* not */ {0, 0, 0, 0},
        /* inc */ {0, 0, 0, 0},
        /* dec */ {0, 0, 0, 0},
        /* jmp */ {0, 0, 0, 0},
        /* bne */ {0, 0, 0, 0},
        /* red */ {0, 0, 0, 0},
        /* prn */ {0, 0, 0, 0},
        /* jsr */ {0, 0, 0, 0},
        /* rts */ {0, 0, 0, 0},
        /* stop */ {0, 0, 0, 0}
};

/* Allowed destination addressing modes per opcode:
   immediate direct matrix register */
const int allowed_dest_modes[16][4] = {
        /* mov */ {0, 1, 1, 1},
        /* cmp */ {1, 1, 1, 1},
        /* add */ {0, 1, 1, 1},
        /* sub */ {0, 1, 1, 1},
        /* lea */ {0, 1, 1, 1},
        /* clr */ {0, 1, 1, 1},
        /* not */ {0, 1, 1, 1},
        /* inc */ {0, 1, 1, 1},
        /* dec */ {0, 1, 1, 1},
        /* jmp */ {0, 1, 1, 1},
        /* bne */ {0, 1, 1, 1},
        /* jsr */ {0, 1, 1, 1},
        /* red */ {0, 1, 1, 1},
        /* prn */ {1, 1, 1, 1},
        /* rts */ {0, 0, 0, 0},
        /* stop */ {0, 0, 0, 0}
};

int identify_opcode(char* op_code) {
    int i;

    /* Check for NULL input */
    if (op_code == NULL) {
        return OPCODE_INVALID;
    }

    /* Loop through the opcode table to find a matching opcode name */
    for (i = 0; opcode_table[i].name != NULL; i++) {
        if (strcmp(op_code, opcode_table[i].name) == 0) {
            return opcode_table[i].opcode; /* Return the matching opcode value */
        }
    }

    /* If not found return invalid opcode */
    return OPCODE_INVALID;
}


int identify_directive(char *directive){
    int i;

    /* Loop through the directive table and check for a match */
    for (i = 0; i < NUM_DIRECTIVE; i++) {
        if (strcmp(directive, directive_table[i].name) == 0) {
            return directive_table[i].directive; /* Return matching directive ID */
        }
    }

    
    return -1;
}

int identify_directive_without_dots(char *directive){
    int i;

    /* Similar to identify_directive but uses the table without leading dots */
    for (i = 0; i < NUM_DIRECTIVE; i++) {
        if (strcmp(directive, directive_table_without_dots[i].name) == 0) {
            return directive_table_without_dots[i].directive; /* Return matching directive ID */
        }
    }

        return -1;
}

int identify_register(char *reg){
    int i;

    /* Loop through the register table to find a matching register name */
    for (i = 0; i < NUM_REGISTERS; i++) {
        if (strcmp(reg, register_table[i].name) == 0) {
            return register_table[i].reg; /* Return the register number */
        }
    }

    
    return -1;
}

int is_valid_immediate(char *immediate, char* file_name, int line_number){
    int i = 0;

    /* Immediate values must start with '#' */
    if (immediate[i] != LADDER) {
        error_log(file_name, line_number, IMM_LADDER);
        return 1;
    }

    i++;

    /* '#' must be followed by a number  */
    if (immediate[i] == '\0') {
        error_log(file_name, line_number, IMM_NUM_AFTER_LADDER);
        return 1;
    } else if (immediate[i] == '-' || immediate[i] == '+') {
        i++;
    }

    /* Validate that the rest of the string contains only digits */
    if (is_digit_or_char(immediate + i, 0, file_name, line_number)) {
        return 1; /* Invalid characters found */
    }

    
    return 0;
}


int is_valid_matrix_dim(char *mat_dim, char *file_name, int line_number)
{
    char matrix_dim_editable[MAX_LINE_LENGTH], *first_bracket, *second_bracket, *third_bracket, *forth_bracket;
    int n, m;

    /* Make an editable copy of the matrix dimension string */
    strcpy(matrix_dim_editable, mat_dim);

    /* Locate the first opening bracket '[' */
    first_bracket = strchr(matrix_dim_editable, OPENING_BRACKET);
    if (first_bracket) {
        /* Locate the first closing bracket ']' after the first '[' */
        second_bracket = strchr(first_bracket + 1, CLOSED_BRACKET);
        if (second_bracket) {
            /* Temporarily terminate the string at the closing bracket */
            *second_bracket = '\0';

            /* Validate that the first dimension contains only digits */
            n = is_digit_or_char(first_bracket + 1, 0, file_name, line_number);
            if (!n) {
                return 1; 
            }

            /* Locate the second dimensions opening bracket */
            third_bracket = strchr(second_bracket + 1, OPENING_BRACKET);
            if (third_bracket) {
                /* Locate the second dimensions closing bracket */
                forth_bracket = strchr(third_bracket + 1, CLOSED_BRACKET);
                if (forth_bracket) {
                    *forth_bracket = '\0';

                    /* Validate that the second dimension contains only digits */
                    m = is_digit_or_char(third_bracket + 1, 0, file_name, line_number);
                    if (!m) {
                        return 1; 
                    }
                }
            }
        }
    }

    return 0; 
}


int is_digit_or_char(char *tested_word, int digit_or_letter_or_both, char *file_name, int line_number) {
    int i = 0;

    if (!digit_or_letter_or_both) {
        /* Check if the tested word contains only digits */

        if (tested_word[i] == '-' || tested_word[i] == '+') {
            i++; 
        }

        /* Iterate through the rest of the string */
        for (i = i; i < strlen(tested_word); i++) {
            if (!isdigit(tested_word[i])) {
                error_log(file_name, line_number, NOT_DIGIT);
                return 1; 
            }
        }

    } else if (digit_or_letter_or_both == 1) {
        /* Check if the tested word contains only letters */
        for (i = 0; i < strlen(tested_word); i++) {
            if (!isalpha(tested_word[i])) {
                error_log(file_name, line_number, NOT_LETTER);
                return 1; 
            }
        }

    } else if (digit_or_letter_or_both == 2) {
        /* Check if the tested word contains only alphanumeric characters */
        for (i = 0; i < strlen(tested_word); i++) {
            if (!isalnum((unsigned char)tested_word[i])) {
                error_log(file_name, line_number, NOT_DIGIT_LETTER);
                return 1; 
            }
        }
    }

    return 0; 
}

int verify_string_has_invalid_chars(char *tested_word) {
    int i;

    /* Check each character to ensure it is printable */
    for (i = 0; i < strlen(tested_word); i++) {
        if (!isprint(tested_word[i])) {
            return 1; 
        }
    }
    return 0; 
}

void string_shifting_forward(char *str) {
    int i = 0;
    if (str == NULL) return;

    /* Shift all characters one position to the left */
    while (str[i] != '\0') {
        str[i] = str[i + 1];
        i++;
    }
}


char *strip_quotes(char *str) {
    size_t len;

    if (str == NULL) return str; /* Return as is if null pointer */

    len = strlen(str);

    /* Check if the string has at least two characters and is wrapped in double quotes */
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        str[len - 1] = '\0'; /* Remove trailing quote */
        string_shifting_forward(str); /* Remove leading quote by shifting characters */
        return str;
    }
    return str; 
}

void copy_directive_name(char *token, char *dest) {
    /* Check if token starts with '.' indicating a directive */
    if (token[0] == '.') {
        strncpy(dest, token + 1, 7);
        dest[7] = '\0';  
    } else {
        dest[0] = '\0';   
    }
}

int is_valid_label(char *label, char *file_name, int line_number) {
    int i;
    int len;

    /* Check if label is null or empty */
    if (label == NULL || label[0] == '\0') {
        error_log(file_name, line_number, MISSING_LABEL_NAME);
        return 0; 
    }

    len = strlen(label);

    /* Check if label length exceeds maximum allowed length */
    if (len > MAX_LABEL_LEN) {
        error_log(file_name, line_number, LONG_LABEL);
        return 0;
    }

    /* First character must be a letter */
    if (!isalpha((unsigned char)label[0])) {
        error_log(file_name, line_number, LABEL_STARTS_WITH_LETTER);
        return 0;
    }

    /* All subsequent characters must be alphanumeric */
    for (i = 1; i < len; i++) {
        if (!isalnum((unsigned char)label[i])) {
            error_log(file_name, line_number, LABEL_LETTER_DIGIT);
            return 0;
        }
    }

    return 1; /* Label passed all checks */
}



/* Determines the addressing mode of a given operand string */
int get_addressing_mode( char *operand) {
    int len;

    if (operand == NULL) {
        return -1; 
    }

    len = strlen(operand);

    /* Immediate addressing begins with '#' followed by number */
    if (operand[0] == LADDER) {
        if (len > 1 && (isdigit((unsigned char)operand[1]) ||
                        operand[1] == '+' || operand[1] == '-')) {
            return ADDRESS_IMMEDIATE;
        }
        return -1; 
    }

    /* Matrix addressing: contains both '[' and ']' characters */
    if (strchr(operand, OPENING_BRACKET) && strchr(operand, CLOSED_BRACKET)) {
        return ADDRESS_MATRIX;
    }


    /* Register addressing: matches exactly r0 to r7 */
    if (identify_register(operand)!=-1) return ADDRESS_REGISTER;

   
    return ADDRESS_DIRECT;
}


int verify_addressing_modes_are_valid(ParsedLine *parsed, char *file_name, int line_number) {
    int opcode = parsed->opcode;

    /* validate source operand only if exists */
    if (parsed->operand_count >= 1) {
        int src_mode = get_addressing_mode(parsed->operands[0]);

        /* For two-operand instructions the first operand is source address */
        if (parsed->operand_count == 2) {
            if (src_mode >= 0 && src_mode <= 3) {
                if (!allowed_source_modes[opcode][src_mode]) {
                    error_log(file_name, line_number, INVALID_SOURCE_ADDRESSING_MODE);
                    return 0;
                }
            }
        }

            /* For single operand instructions operand is destination */
        else if (parsed->operand_count == 1) {
            if (src_mode >= 0 && src_mode <= 3) {
                if (!allowed_dest_modes[opcode][src_mode]) {
                    error_log(file_name, line_number, INVALID_DEST_ADDRESSING_MODE);
                    return 0;
                }
            }
        }
    }

    /* Validate destination operand for two operand instructions */
    if (parsed->operand_count == 2) {
        int dest_mode = get_addressing_mode(parsed->operands[1]);

        if (dest_mode >= 0 && dest_mode <= 3) {
            if (!allowed_dest_modes[opcode][dest_mode]) {
                error_log(file_name, line_number, INVALID_DEST_ADDRESSING_MODE);
                return 0;
            }
        }
    }

    return 1;
}


int verify_matrix_registers_are_valid(char *operand, char *file_name, int line_number) {
    char *open_bracket_first_reg, *closed_bracket_first_reg, *open_bracket_second_reg, *closed_bracket_second_reg;
    char temp_operand[MAX_LINE_LENGTH], *point_after_first_reg;
    char first_register[MAX_LINE_LENGTH], second_register[MAX_LINE_LENGTH];
    char *reg, *point_after_second_reg;

    /* Copy operand into a modifiable buffer */
    strncpy(temp_operand, operand, MAX_LINE_LENGTH - 1);
    temp_operand[MAX_LINE_LENGTH - 1] = '\0';

    /* Locate first opening bracket for first register */
    open_bracket_first_reg = strchr(temp_operand, OPENING_BRACKET);
    if (!open_bracket_first_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_FIRST_BRACKET);
        return 1;
    }

    /* Locate first closing bracket for first register */
    closed_bracket_first_reg = strchr(open_bracket_first_reg + 1, CLOSED_BRACKET);
    if (!closed_bracket_first_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_FIRST_CLOSING);
        return 1;
    }

    /* Extract the first register string */
    strncpy(first_register, open_bracket_first_reg + 1,
            closed_bracket_first_reg - open_bracket_first_reg - 1);
    first_register[closed_bracket_first_reg - open_bracket_first_reg - 1] = '\0';

    /* Trim spaces and validate first register */
    reg = cut_spaces_before_and_after_string(first_register);
    if (identify_register(reg) == -1) {
        error_log(file_name, line_number, INVALID_MATRIX_FIRST_REGISTER);
        return 1;
    }

    /* Ensure no spaces or invalid characters between first closing bracket and second opening bracket */
    point_after_first_reg = closed_bracket_first_reg + 1;
    while (*point_after_first_reg != '\0' && *point_after_first_reg != '[') {
        if (isspace((unsigned char)*point_after_first_reg)) {
            error_log(file_name, line_number, MATRIX_INVALID_WHITESPACE);
            return 1;
        }
        point_after_first_reg++;
    }

    /* Locate second opening bracket for second register */
    open_bracket_second_reg = strchr(closed_bracket_first_reg + 1, OPENING_BRACKET);
    if (!open_bracket_second_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_SECOND_BRACKET);
        return 1;
    }

    /* Locate second closing bracket for second register */
    closed_bracket_second_reg = strchr(open_bracket_second_reg + 1, CLOSED_BRACKET);
    if (!closed_bracket_second_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_SECOND_CLOSING);
        return 1;
    }

    /* Extract the second register string */
    strncpy(second_register, open_bracket_second_reg + 1,
            closed_bracket_second_reg - open_bracket_second_reg - 1);
    second_register[closed_bracket_second_reg - open_bracket_second_reg - 1] = '\0';

    /* Trim spaces and validate second register */
    reg = cut_spaces_before_and_after_string(second_register);
    if (identify_register(reg) == -1) {
        error_log(file_name, line_number, INVALID_MATRIX_SECOND_REGISTER);
        return 1;
    }

    /* After second register closing bracket, only spaces are allowed */
    point_after_second_reg = closed_bracket_second_reg + 1;
    while (*point_after_second_reg != '\0') {
        if (!isspace((unsigned char)*point_after_second_reg)) {
            error_log(file_name, line_number, MATRIX_DIMENSION_FORMAT);
            return 1;
        }
        point_after_second_reg++;
    }

    
    return 0;
}



int instruction_word_count(ParsedLine *parsed) {
    int word_counter, i, mode;

    if (parsed == NULL) {
        return 0; 
    }

    word_counter = 1; /* Every instruction has at least one base word */

    /* Handle case where both operands are registers they share one word */
    if (parsed->operand_count == 2 &&
        get_addressing_mode(parsed->operands[0]) == ADDRESS_REGISTER &&
        get_addressing_mode(parsed->operands[1]) == ADDRESS_REGISTER) {
        return 2; /* Base word + 1 shared register word */
    }

    for (i = 0; i < parsed->operand_count; i++) {
        mode = get_addressing_mode(parsed->operands[i]);

        switch (mode) {
            case ADDRESS_IMMEDIATE:
            case ADDRESS_DIRECT:
            case ADDRESS_REGISTER:
                word_counter += 1; /* Needs one additional word */
                break;

            case ADDRESS_MATRIX:
                word_counter += 2; /* Matrix operands require two words */
                break;

            default:
                return 0; /* Invalid addressing mode */
        }
    }

    return word_counter;
}


int comma_validation(char *line, char *file_name, int line_number) {
    int i = 0;

    /* Iterate through each character in the line */
    while (line[i] != '\0') {

        /* Check for a comma */
        if (line[i] == ',') {
            i++;

            /* Skip over any spaces or tabs after the comma */
            while (line[i] == ' ' || line[i] == '\t') {
                i++;
            }

            /* If the next non-space character is another comma, it's an error */
            if (line[i] == ',') {
                error_log(file_name, line_number, MULTIPLE_COMMAS);
                return 1;
            }

            /* extra comma is displayed in end of line */
            if (line[i] == '\0' || line[i] == '\n') {
                error_log(file_name, line_number, COMMA_END_OF_LINE);
                return 1;
            }
        } else {
            i++;
        }
    }

    return 0;
}
