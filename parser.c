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
static const int expected_operands[16] = {
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

/* Fixed instruction_word_count function */
int instruction_word_count(ParsedLine *parsed) {
    int count;
    int i;
    int mode;

    if (parsed == NULL) {
        return 0; /* Defensive check */
    }

    count = 1; /* Every instruction has at least one base word */

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
                count += 1; /* Needs one additional word */
                break;

            case ADDRESS_MATRIX:
                count += 2; /* Matrix operands require two words */
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
    char *string_without_quotes,*token, *rest, *original_rest;
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


    /* === LABEL === */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';

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

        strncpy(out->label, token, MAX_LABEL_LEN);
        out->label[MAX_LABEL_LEN] = '\0';

        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            error_log(file_name, line_number, SYNTAX_ERROR);
            return 0;
        }
    }

    /* === DIRECTIVE or OPCODE === */
    if (token[0] == '.') {
        int d = identify_directive(token);
        if (d == -1) {
            error_log(file_name, line_number, SYNTAX_ERROR);
            return 0;
        }
        out->line_type = LINE_DIRECTIVE;
        copy_directive_name(token, out->directive_name);
    } else {
        Opcode op = identify_opcode(token);
        if (op == OPCODE_INVALID) {
            error_log(file_name, line_number, SYNTAX_ERROR);
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

    /* Extract remaining line */
     rest = strstr(line, token);
    if (rest == NULL) return 1;
    rest += strlen(token);
    rest = cut_spaces_before_and_after_string(rest);
    original_rest = rest;

        /* === SPECIAL DIRECTIVE CHECKS === */
    if (out->line_type == LINE_DIRECTIVE && strcmp(out->directive_name, "string") == 0) {
        if (rest[0] != '"' || rest[strlen(rest) - 1] != '"') {
            error_log(file_name, line_number, STRING_MISSING_QUOTES);
            return 0;
        }
        strncpy(out->operands[0], rest, MAX_LINE_LENGTH - 1);
        string_without_quotes=strip_quotes(out->operands[0]);
        strncpy(out->operands[0], string_without_quotes, MAX_LINE_LENGTH - 1);

        out->operands[0][MAX_LINE_LENGTH - 1] = '\0';
        out->operand_count = 1;
        return 1;
    }

    if (out->line_type == LINE_DIRECTIVE &&
        strcmp(out->directive_name, "data") == 0 && rest[0] == '\0') {
        error_log(file_name, line_number, MISSING_OPERANDS_DATA);
        return 0;
    }

    if (out->line_type == LINE_DIRECTIVE &&
        strcmp(out->directive_name, "extern") == 0 && rest[0] == '\0') {
        error_log(file_name, line_number, MISSING_OPERAND_EXTERN);
        return 0;
    }

    if (out->line_type == LINE_DIRECTIVE && strcmp(out->directive_name, "mat") == 0) {
        int n, m;
        char *after_dims;
        if (sscanf(rest, " [%d][%d]%n", &n, &m, &i) != 2) {
            error_log(file_name, line_number, MATRIX_DIMENSION_FORMAT);
            return 0;
        }

        /* Add the matrix dimensions string as first operand */
        strncpy(out->operands[0], rest, i);
        out->operands[0][i] = '\0';
        out->operand_count = 1;

        after_dims = rest + i;
        after_dims = cut_spaces_before_and_after_string(after_dims);

        if (after_dims[0] == ',') {
            error_log(file_name, line_number, MULTIPLE_COMMAS);
            return 0;
        }

        /* Now parse the remaining values */
        token = strtok(after_dims, ",");
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

        if (original_rest[strlen(original_rest) - 1] == ',') {
            error_log(file_name, line_number, MULTIPLE_COMMAS);
            return 0;
        }

        return 1;
    }

    if (rest[0] == ',') {
        error_log(file_name, line_number, MULTIPLE_COMMAS);
        return 0;
    }

    /* === Default operand parsing === */
    i = 0;
    while ((token = strtok((i == 0) ? rest : NULL, ",")) != NULL) {
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

    if (original_rest[strlen(original_rest) - 1] == ',') {
        error_log(file_name, line_number, MULTIPLE_COMMAS);
        return 0;
    }

    out->operand_count = i;


        /* === Validate operand count for instructions === */
    if (out->line_type == LINE_INSTRUCTION) {
        int expected = expected_operands[out->opcode];

        if (out->operand_count != expected) {
            error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
            return 0;
        }
    
   }

     /* === Check immediate operands range === */
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