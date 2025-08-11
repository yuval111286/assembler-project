#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "parser.h"
#include "analyze_text.h"
#include "errors_handler.h"
#include "globals.h"

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

int verify_string_is_valid(char *tested_word) {
    int i;

    for (i = 0; i < strlen(tested_word); i++) {
        if (!isprint(tested_word[i])) {
            return 1;
        }
    }
    return 0;
}




char *strip_quotes(char *str) {
    size_t len;

    if (str == NULL) return str;

    len = strlen(str);

    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        str[len - 1] = '\0';
        safe_shift_left(str);
        return str;
    }
    return str;
}

void safe_shift_left(char *str) {
    int i = 0;
    if (str == NULL) return;

    while (str[i] != '\0') {
        str[i] = str[i + 1];
        i++;
    }
}

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

/**
 * @brief Validates matrix operand register names
 *
 * @param operand Matrix operand string (e.g., "M1[r2][r7]")
 * @param file_name Filename for error reporting
 * @param line_number Line number for error reporting
 * @return 1 if valid, 0 if invalid register names
 */
int verify_matrix_registers_are_valid(char *operand, char *file_name, int line_number) {
    char *open_bracket_first_reg, *closed_bracket_first_reg, *open_bracket_second_reg, *closed_bracket_second_reg;
    char temp_operand[MAX_LINE_LENGTH],*point_after_first_reg;

    /* Make a copy to avoid modifying original */
    strncpy(temp_operand, operand, MAX_LINE_LENGTH - 1);
    temp_operand[MAX_LINE_LENGTH - 1] = '\0';

    /* Find register names between brackets */
    open_bracket_first_reg = strchr(temp_operand, '[');
    if (!open_bracket_first_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_FIRST_BRACKET);
        return 0;
    }

    closed_bracket_first_reg = strchr(open_bracket_first_reg + 1, ']');
    if (!closed_bracket_first_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_FIRST_CLOSING);
        return 0;
    }

    *closed_bracket_first_reg = '\0';
    if (identify_register(open_bracket_first_reg + 1) == -1) {
        error_log(file_name, line_number, INVALID_MATRIX_FIRST_REGISTER);
        return 0;
    }

    /* Check for illegal whitespace between ']' and '[' */
    point_after_first_reg = closed_bracket_first_reg + 1;
    while (*point_after_first_reg != '\0' && *point_after_first_reg != '[') {
        if (isspace((unsigned char)*point_after_first_reg)) {
            error_log(file_name, line_number, MATRIX_INVALID_WHITESPACE);
            return 0;
        }
        point_after_first_reg++;
    }

    open_bracket_second_reg = strchr(closed_bracket_first_reg + 1, '[');
    if (!open_bracket_second_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_SECOND_BRACKET);
        return 0;
    }

    closed_bracket_second_reg = strchr(open_bracket_second_reg + 1, ']');
    if (!closed_bracket_second_reg) {
        error_log(file_name, line_number, INVALID_MATRIX_FORMAT_SECOND_CLOSING);
        return 0;
    }

    *closed_bracket_second_reg = '\0';
    if (identify_register(open_bracket_second_reg + 1) == -1) {
        error_log(file_name, line_number, INVALID_MATRIX_SECOND_REGISTER);
        return 0;
    }

    return 1;
}

/* Fixed instruction_word_count function */
int instruction_word_count(ParsedLine *parsed) {
    int word_counter,i,mode;

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


/**
 * @brief Parses a single line from the source file.
 *
 * Identifies label (if present), type of line (instruction/directive),
 * opcode or directive name, and splits operands.
 *
 * @param line       The input line to parse
 * @param out        Pointer to ParsedLine struct to populate
 * @param file_name  Name of the source file (for error logging)
 * @param line_number Line number (for error logging)
 * @return 1 if the line is valid and parsed successfully, 0 otherwise
 */

int parse_line(char *line, ParsedLine *out, char *file_name, int line_number) {
    char buffer[MAX_LINE_LENGTH];
    char *string_without_quotes,*token, *starts_operands_pointer, *dynamic_operands_pointer;
    int i;

    /* Reset output */
    (*out).label[0] = '\0';
    (*out).directive_name[0] = '\0';
    (*out).operand_count = 0;
    (*out).opcode = OPCODE_INVALID;
    (*out).line_type = LINE_INVALID;


    /* Copy line to buffer for tokenization */
    strncpy(buffer, line, MAX_LINE_LENGTH - 1);
    buffer[MAX_LINE_LENGTH - 1] = '\0';

    /* Check for double commas */
    if (strstr(buffer, ",,") != NULL) {
        error_log(file_name, line_number, MULTIPLE_COMMAS);
        return 0;
    }

    token = strtok(buffer, " \t\n");
    if (token == NULL) {
        (*out).line_type = LINE_EMPTY;
        return 1;
    }

    if (token[0] == ';') {
        (*out).line_type = LINE_COMMENT;
        return 1;
    }


    /* Handle Label */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';

        /*check if label name is valid, containing only allowed char and the name is not saved word*/
        if (!is_valid_label(token)) {
            if (identify_opcode(token) != OPCODE_INVALID || identify_directive(token) != -1) {
                error_log(file_name, line_number, RESERVED_WORD_AS_LABEL);
            } else if (identify_register(token) != -1) {
                error_log(file_name, line_number, REGISTER_NAME_AS_LABEL);
            } else {
                error_log(file_name, line_number, INVALID_LABEL_NAME);
            }
            return 0;
        }

        /*placing label name in parser structure */
        strncpy(out->label, token, MAX_LABEL_LEN);
        out->label[MAX_LABEL_LEN] = '\0';

        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            error_log(file_name, line_number, EMPTY_AFTER_LABEL);
            return 0;
        }
    }

    /* DIRECTIVE or OPCODE */
    if (token[0] == '.') {
        int d = identify_directive(token);
        if (d == -1) {
            error_log(file_name, line_number, ILLEGAL_DIRECTIVE);
            return 0;
        }
        out->line_type = LINE_DIRECTIVE;
        copy_directive_name(token, out->directive_name);
    } else {
        Opcode op = identify_opcode(token);
        if (op == OPCODE_INVALID) {
            error_log(file_name, line_number, INVALID_OPCODE);
            return 0;
        }
        out->line_type = LINE_INSTRUCTION;
        out->opcode = op;

        if (op == OPCODE_STOP || op == OPCODE_RTS) {
            token = strtok(NULL, "\n");
            if (token != NULL) {
                token = cut_spaces_before_and_after_string(token);
                if (token[0] != '\0' && token[0] != ';') {
                    error_log(file_name, line_number, EXTRANEOUS_TEXT_AFTER_COMMAND);
                    return 0;
                }
            }
            return 1;
        }
    }

    /* process remaining line */
    dynamic_operands_pointer = strstr(line, token);
    if (dynamic_operands_pointer == NULL) return 1;
    dynamic_operands_pointer += strlen(token);
    dynamic_operands_pointer = cut_spaces_before_and_after_string(dynamic_operands_pointer);
    starts_operands_pointer = dynamic_operands_pointer;


    if (out->line_type == LINE_DIRECTIVE && strcmp(out->directive_name, "string") == 0) {
        if (dynamic_operands_pointer[0] != '"' || dynamic_operands_pointer[strlen(dynamic_operands_pointer) - 1] != '"') {
            error_log(file_name, line_number, STRING_MISSING_QUOTES);
            return 0;
        }
        strncpy(out->operands[0], dynamic_operands_pointer, MAX_LINE_LENGTH - 1);
        string_without_quotes=strip_quotes(out->operands[0]);
        if(verify_string_is_valid(string_without_quotes)){
            error_log(file_name, line_number, INVALID_STRING);
            return 0;
        }
        strncpy(out->operands[0], string_without_quotes, MAX_LINE_LENGTH - 1);

        out->operands[0][MAX_LINE_LENGTH - 1] = '\0';
        out->operand_count = 1;
        return 1;
    }

    if (out->line_type == LINE_DIRECTIVE &&
        strcmp(out->directive_name, "data") == 0 && dynamic_operands_pointer[0] == '\0') {
        error_log(file_name, line_number, MISSING_OPERANDS_DATA);
        return 0;
    }

    if (out->line_type == LINE_DIRECTIVE &&
        strcmp(out->directive_name, "extern") == 0 && dynamic_operands_pointer[0] == '\0') {
        error_log(file_name, line_number, MISSING_OPERAND_EXTERN);
        return 0;
    }

    /*parse matrix part*/
    if (out->line_type == LINE_DIRECTIVE && strcmp(out->directive_name, "mat") == 0) {
        int n, m;
        char *mat_values;
        if (sscanf(dynamic_operands_pointer, " [%d][%d]%n", &n, &m, &i) != 2) {
            error_log(file_name, line_number, MATRIX_DIMENSION_FORMAT);
            return 0;
        }

        /* Add the matrix dimensions string as first operand */
        strncpy(out->operands[0], dynamic_operands_pointer, i);
        out->operands[0][i] = '\0';
        out->operand_count = 1;

        mat_values = dynamic_operands_pointer + i;
        mat_values = cut_spaces_before_and_after_string(mat_values);

        if (mat_values[0] == ',') {
            error_log(file_name, line_number, MULTIPLE_COMMAS);
            return 0;
        }

        /* parse the remaining values of matrix */
        token = strtok(mat_values, ",");
        while (token != NULL) {
            token = cut_spaces_before_and_after_string(token);
            if (token[0] == '\0') {
                error_log(file_name, line_number, MULTIPLE_COMMAS);
                return 0;
            }
            if (out->operand_count >= MAX_OPERANDS) {
                error_log(file_name, line_number, EXTRANEOUS_TEXT_AFTER_COMMAND);
                return 0;
            }

            strncpy(out->operands[out->operand_count], token, MAX_LINE_LENGTH - 1);
            out->operands[out->operand_count][MAX_LINE_LENGTH - 1] = '\0';
            out->operand_count++;

            token = strtok(NULL, ",");
        }

        if (starts_operands_pointer[strlen(starts_operands_pointer) - 1] == ',') {
            error_log(file_name, line_number, MULTIPLE_COMMAS);
            return 0;
        }

        return 1;
    }

    if (dynamic_operands_pointer[0] == ',') {
        error_log(file_name, line_number, MULTIPLE_COMMAS);
        return 0;
    }

    /* decompose operands by commas and process each operand*/
    i = 0;
    while ((token = strtok((i == 0) ? dynamic_operands_pointer : NULL, ",")) != NULL) {
        token = cut_spaces_before_and_after_string(token);
        if (token[0] == '\0') {
            error_log(file_name, line_number, MULTIPLE_COMMAS);
            return 0;
        }

        if (i >= MAX_OPERANDS) {
            error_log(file_name, line_number, EXTRANEOUS_TEXT_AFTER_COMMAND);
            return 0;
        }

        strncpy(out->operands[i], token, MAX_LINE_LENGTH - 1);
        out->operands[i][MAX_LINE_LENGTH - 1] = '\0';
        i++;
    }

    if (starts_operands_pointer[strlen(starts_operands_pointer) - 1] == ',') {
        error_log(file_name, line_number, MULTIPLE_COMMAS);
        return 0;
    }

    out->operand_count = i;


    /* verify operand count match the instructions */
    if (out->line_type == LINE_INSTRUCTION) {
        int expected = expected_operands_for_each_opcode[out->opcode];

        if (out->operand_count != expected) {
            error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
            return 0;
        }

        /* verify addressing modes are valid for instructions === */
        if (!verify_addressing_modes_are_valid(out, file_name, line_number)) {
            return 0;
        }

        /* verify matrix operands are valid according to matrix roles */
        for (i = 0; i < out->operand_count; i++) {
            if (get_addressing_mode(out->operands[i]) == ADDRESS_MATRIX) {
                if (!verify_matrix_registers_are_valid(out->operands[i], file_name, line_number)) {
                    return 0;
                }
            }
        }
    }

    /*check immediate operands range is between -512 to +511*/
    for (i = 0; i < out->operand_count; i++) {
        if (out->operands[i][0] == '#') {
            long val = strtol(out->operands[i] + 1, NULL, 10);
            if (val < MIN_NUM || val > MAX_NUM) {
                error_log(file_name, line_number, IMMEDIATE_OUT_OF_RANGE);
                return 0;
            }
        }
    }

    return 1;
}