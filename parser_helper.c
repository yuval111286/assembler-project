#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser_helper.h"
#include "errors_handler.h"
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

Directive_Mode directive_table_without_dots[] = {
        {"data", DATA},
        {"string", STRING},
        {"mat", MAT},
        {"entry", ENTRY},
        {"extern", EXTERN}
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

/* Table of allowed addressing modes for each opcode*/
/* 0=immediate, 1=direct, 2=matrix, 3=register */
const int allowed_source_modes[16][4] = {
        /* mov  */ {1, 1, 1, 1},
        /* cmp  */ {1, 1, 1, 1},
        /* add  */ {0, 1, 1, 1},
        /* sub  */ {0, 1, 1, 1},
        /* lea  */ {0, 1, 1, 0},
        /* clr  */ {0, 0, 0, 0},
        /* not  */ {0, 0, 0, 0},
        /* inc  */ {0, 0, 0, 0},
        /* dec  */ {0, 0, 0, 0},
        /* jmp  */ {0, 0, 0, 0},
        /* bne  */ {0, 0, 0, 0},
        /* red  */ {0, 0, 0, 0},
        /* prn  */ {0, 0, 0, 0},
        /* jsr  */ {0, 0, 0, 0},
        /* rts  */ {0, 0, 0, 0},
        /* stop */ {0, 0, 0, 0}
};

const int allowed_dest_modes[16][4] = {
        /* mov  */ {0, 1, 1, 1},
        /* cmp  */ {1, 1, 1, 1},
        /* add  */ {1, 1, 1, 1},
        /* sub  */ {1, 1, 1, 1},
        /* lea  */ {0, 1, 1, 0},
        /* clr  */ {0, 1, 1, 1},
        /* not  */ {0, 1, 1, 1},
        /* inc  */ {0, 1, 1, 1},
        /* dec  */ {0, 1, 1, 1},
        /* jmp  */ {0, 1, 1, 1},
        /* bne  */ {0, 1, 1, 1},
        /* jsr  */ {0, 1, 1, 1},
        /* red  */ {0, 1, 1, 1},
        /* prn  */ {1, 1, 1, 1},
        /* rts  */ {0, 0, 0, 0},
        /* stop */ {0, 0, 0, 0}
};

int identify_opcode(char* op_code) {

    int i;
    if (op_code == NULL) {
        return OPCODE_INVALID;
    }

    for (i = 0; opcode_table[i].name != NULL; i++) {
        if (strcmp(op_code, opcode_table[i].name) == 0) {
            return opcode_table[i].opcode;
        }
    }

    return OPCODE_INVALID;
}


int identify_directive(char *directive){

    int i;
    for (i = 0; i<NUM_DIRECTIVE; i++) {
        if (strcmp(directive, directive_table[i].name) == 0) {
            return directive_table[i].directive;
        }
    }

    return -1;
}

int identify_directive_without_dots(char *directive){

    int i;
    for (i = 0; i<NUM_DIRECTIVE; i++) {
        if (strcmp(directive, directive_table_without_dots[i].name) == 0) {
            return directive_table_without_dots[i].directive;
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

int is_valid_immediate(char *immediate, char* file_name, int line_number){

    int i=0;

    if (immediate[i] != '#') {
       error_log(file_name, line_number, "Immediate value must start with an #\n");
        return 1;
    }
    i++;
    if (immediate[i] == '\0') {
        error_log(file_name, line_number, "Immediate value must have num after #\n");
        return 1;
    } else if(immediate[i] =='-'){
        i++;
    }

    if(is_digit_or_char(immediate+i,0,file_name,line_number)){
        return 1;
    }

    return 0;
}

int is_valid_matrix_dim(char *mat_dim, char *file_name, int line_number)
{
    char matrix_dim_editable[MAX_LINE_LENGTH],*first_bracket, *second_bracket, *third_bracket, *forth_bracket;
    int n,m;

    strcpy(matrix_dim_editable, mat_dim);

    /* Find register names between brackets */
    first_bracket = strchr(matrix_dim_editable, '[');
    if (first_bracket) {
        second_bracket = strchr(first_bracket + 1, ']');
        if (second_bracket) {
            *second_bracket = '\0';
            n = is_digit_or_char(first_bracket + 1,0,file_name,line_number);
            if(!n){
                return 1;
            }

            third_bracket = strchr(second_bracket + 1, '[');
            if (third_bracket) {
                forth_bracket = strchr(third_bracket + 1, ']');
                if (forth_bracket) {
                    *forth_bracket = '\0';
                    m = is_digit_or_char(third_bracket + 1,0,file_name,line_number);
                    if(!m){
                        return 1;
                    }
                }
            }
        }
    }


    return 0;
}




int is_digit_or_char(char *tested_word, int digit_or_letter_or_both, char *file_name, int line_number) {
    int i;
    if (!digit_or_letter_or_both){
        /*check tested word contain only digits*/

        for (i = 0; i < strlen(tested_word); i++) {
            if (!isdigit(tested_word[i])) {
                error_log(file_name, line_number, "Char is not digit\n");
                return 1;
            }
        }


    } else if (digit_or_letter_or_both == 1){
        /*check tested word contain only letter*/
        for (i = 0; i < strlen(tested_word); i++) {
            if (!isalpha(tested_word[i])) {
                error_log(file_name, line_number, "Char is not letter\n");
                return 1;
            }
        }

    } else if (digit_or_letter_or_both == 2)
    {
        /*check tested word contain digits and letters*/

        for (i = 0; i < strlen(tested_word); i++) {
            if (!isalnum((unsigned char)tested_word[i])) {
                error_log(file_name, line_number, "Char is not digit or letter\n");
                return 1;
            }
        }

    }

    return 0;
}

int verify_string_is_valid(char *tested_word) {
    int i;

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

    while (str[i] != '\0') {
        str[i] = str[i + 1];
        i++;
    }
}

char *strip_quotes(char *str) {
    size_t len;

    if (str == NULL) return str;

    len = strlen(str);

    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        str[len - 1] = '\0';
        string_shifting_forward(str);
        return str;
    }
    return str;
}

/*
 * Removes the leading '.' from a directive token and stores the name.
 */
void copy_directive_name(char *token, char *dest) {
    if (token[0] == '.') {
        strncpy(dest, token + 1, 7); /* Copy up to 7 characters after '.' */
        dest[7] = '\0';  /* Ensure null-termination */
    } else {
        dest[0] = '\0';   /* Invalid directive format */
    }
}


/* Checks whether the given label is valid (starts with letter, alphanumeric, not too long) */
int is_valid_label(char *label,char *file_name, int line_number) {
    int i;
    int len;

    if (label == NULL || label[0] == '\0') {
        error_log(file_name, line_number, "Missing Label name\n");
        return 0; /* Null or empty label */
    }

    len = strlen(label);

    if (len > MAX_LABEL_LEN) {
        error_log(file_name, line_number, "Label is too long \n");
        return 0; /* Label exceeds maximum allowed length */
    }

    if (!isalpha((unsigned char)label[0])) {
        error_log(file_name, line_number, "Label must starts with letter\n");
        return 0; /* Label must start with an alphabetic character */
    }

    for (i = 1; i < len; i++) {
        if (!isalnum((unsigned char)label[i])) {
            /*error_log(file_name, line_number, "Label must contain only digits or letters \n");*/
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

/**
 * @brief Validates that operands use allowed addressing modes for the specific instruction
 *
 * @param parsed Pointer to parsed line structure
 * @param file_name Filename for error reporting
 * @param line_number Line number for error reporting
 * @return 1 if valid, 0 if invalid addressing mode detected
 */
int verify_addressing_modes_are_valid(ParsedLine *parsed, char *file_name, int line_number) {
    int opcode = parsed->opcode;

    /* Validate source operand (if exists) */
    if (parsed->operand_count >= 1) {
        int src_mode = get_addressing_mode(parsed->operands[0]);

        /* For two-operand instructions, first operand is source */
        if (parsed->operand_count == 2) {
            if (src_mode >= 0 && src_mode <= 3) {
                if (!allowed_source_modes[opcode][src_mode]) {
                    error_log(file_name, line_number, INVALID_SOURCE_ADDRESSING_MODE);
                    return 0;
                }
            }
        }

            /* For single-operand instructions, operand is destination */
        else if (parsed->operand_count == 1) {
            if (src_mode >= 0 && src_mode <= 3) {
                if (!allowed_dest_modes[opcode][src_mode]) {
                    error_log(file_name, line_number, INVALID_DEST_ADDRESSING_MODE);
                    return 0;
                }
            }
        }
    }

    /* Validate destination operand for two-operand instructions */
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
    char *reg;

    strncpy(temp_operand, operand, MAX_LINE_LENGTH - 1);
    temp_operand[MAX_LINE_LENGTH - 1] = '\0';

    open_bracket_first_reg = strchr(temp_operand, '[');
    if (!open_bracket_first_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_FIRST_BRACKET);
        return 1;
    }

    closed_bracket_first_reg = strchr(open_bracket_first_reg + 1, ']');
    if (!closed_bracket_first_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_FIRST_CLOSING);
        return 1;
    }


    strncpy(first_register, open_bracket_first_reg + 1,
            closed_bracket_first_reg - open_bracket_first_reg - 1);
    first_register[closed_bracket_first_reg - open_bracket_first_reg - 1] = '\0';

    reg = cut_spaces_before_and_after_string(first_register);
    if (identify_register(reg) == -1) {
        error_log(file_name, line_number, INVALID_MATRIX_FIRST_REGISTER);
        return 1;
    }


    point_after_first_reg = closed_bracket_first_reg + 1;
    while (*point_after_first_reg != '\0' && *point_after_first_reg != '[') {
        if (isspace((unsigned char)*point_after_first_reg)) {
            error_log(file_name, line_number, MATRIX_INVALID_WHITESPACE);
            return 1;
        }
        point_after_first_reg++;
    }


    open_bracket_second_reg = strchr(closed_bracket_first_reg + 1, '[');
    if (!open_bracket_second_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_SECOND_BRACKET);
        return 1;
    }

    closed_bracket_second_reg = strchr(open_bracket_second_reg + 1, ']');
    if (!closed_bracket_second_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_SECOND_CLOSING);
        return 1;
    }

    strncpy(second_register, open_bracket_second_reg + 1,
            closed_bracket_second_reg - open_bracket_second_reg - 1);
    second_register[closed_bracket_second_reg - open_bracket_second_reg - 1] = '\0';

    reg = cut_spaces_before_and_after_string(second_register);
    if (identify_register(reg) == -1) {
        error_log(file_name, line_number, INVALID_MATRIX_SECOND_REGISTER);
        return 1;
    }

    return 0;
}


int instruction_word_count(ParsedLine *parsed) {
    int word_counter, i, mode;

    if (parsed == NULL) {
        return 0; /* Defensive check */
    }

    word_counter = 1; /* Every instruction has at least one base word */

    /* Handle case where both operands are registers - they share one word */
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

/* Returns the number of memory words required for .data, .string or .mat directives */
int count_data_items(ParsedLine *parsed) {

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
