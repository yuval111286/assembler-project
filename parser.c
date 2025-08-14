#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "errors_handler.h"
#include "globals.h"


int parse_line(char *line, ParsedLine *out, char *file_name, int line_number) {
    char *buffer,*cleaned_buffer;
    char *token, *dynamic_operands_pointer;
    int expected;

    /* initial output ParsedLine */
    out->label[0] = '\0';
    out->directive_name[0] = '\0';
    out->operand_count = 0;
    out->opcode = OPCODE_INVALID;
    out->line_type = LINE_INVALID;

    buffer = malloc_allocation(MAX_LINE_LENGTH);

    /* Copy line to buffer for tokenization */
    strncpy(buffer, line, MAX_LINE_LENGTH - 1);
    buffer[MAX_LINE_LENGTH - 1] = '\0';

    cleaned_buffer = cut_spaces_before_and_after_string(buffer);
    /*check for invalid comma*/
    if(comma_validation(cleaned_buffer,file_name,line_number)){
        return 0;
    }
    token = strtok(cleaned_buffer, " \t\n");

    /* Handle Label */
    if (token[strlen(token) - 1] == DOUBLE_DOTS) {
        token[strlen(token) - 1] = '\0';  /* Remove ':' */

        /*checking label validity*/
        if (!is_valid_label(token, file_name, line_number)) {
            return 0;
        }
        if (identify_opcode(token) != OPCODE_INVALID || identify_directive(token) != -1) {
            error_log(file_name, line_number, SAVED_WORD_AS_LABEL);
            return 0;
        } else if (identify_register(token) != -1) {
            error_log(file_name, line_number, REGISTER_NAME_AS_LABEL);
            return 0;
        }

        strncpy(out->label, token, MAX_LABEL_LEN);
        out->label[MAX_LABEL_LEN] = '\0';

        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            error_log(file_name, line_number, EMPTY_AFTER_LABEL);
            return 0;
        }
    }

    /* Process directive or instruction */
    if (token[0] == '.') {
        /* Handle directive */
        int directive_identifier = identify_directive(token);
        if (directive_identifier == -1) {
            error_log(file_name, line_number, ILLEGAL_DIRECTIVE);
            return 0;
        }
        out->line_type = LINE_DIRECTIVE;
        copy_directive_name(token, out->directive_name);

        return parse_directive_line(line, out, file_name, line_number);
    } else {
        /* Handle instruction */
        Opcode opcode_identifier = identify_opcode(token);
        if (opcode_identifier == OPCODE_INVALID) {
            error_log(file_name, line_number, INVALID_OPCODE);
            return 0;
        }

        out->line_type = LINE_INSTRUCTION;
        out->opcode = opcode_identifier;

        /* Handle instructions with no operands */
        if (opcode_identifier == OPCODE_STOP || opcode_identifier == OPCODE_RTS) {
            token = strtok(NULL, "\n");
            if (token != NULL) {
                token = cut_spaces_before_and_after_string(token);
                if (token[0] != '\0') {
                    error_log(file_name, line_number, EXTRANEOUS_TEXT_AFTER_COMMAND);
                    return 0;
                }
            }
            return 1;
        }

        /* Process remaining line for operands */
        dynamic_operands_pointer = strstr(line, token);
        if (dynamic_operands_pointer == NULL) return 1;
        dynamic_operands_pointer += strlen(token);
        dynamic_operands_pointer = cut_spaces_before_and_after_string(dynamic_operands_pointer);

        /* Parse operands */
        if (dynamic_operands_pointer[0] == '\0') {
            /* No operands found */
            error_log(file_name,line_number,MISSING_INSTRUCTION_ARG);
            out->operand_count = 0;
            return 0;
        } else {
            if (!parse_instruction_operands(dynamic_operands_pointer, out, file_name, line_number)) {
                return 0;
            }
        }

        /* validate operand count match operand name*/
        expected = expected_operands_for_each_opcode[out->opcode];
        if (out->operand_count != expected) {
            error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
            return 0;
        }

        /* Verify addressing modes */
        if (!verify_addressing_modes_are_valid(out, file_name, line_number)) {
            return 0;
        }
    }

    return 1;
}

int parse_directive_line(char *line, ParsedLine *out, char *file_name, int line_number) {
    char *dynamic_operands_pointer;

    /* Find operands part */
    dynamic_operands_pointer = strstr(line, out->directive_name);
    if (dynamic_operands_pointer == NULL) return 0;
    dynamic_operands_pointer += strlen(out->directive_name) + 1; /* +1 for the '.' */
    dynamic_operands_pointer = cut_spaces_before_and_after_string(dynamic_operands_pointer);

    if (strcmp(out->directive_name, DIR_STRING) == 0) {
        return parse_string_directive(dynamic_operands_pointer, out, file_name, line_number);
    }
    else if (strcmp(out->directive_name, DIR_DATA) == 0) {
        return parse_data_directive(dynamic_operands_pointer, out, file_name, line_number);
    }
    else if (strcmp(out->directive_name, DIR_EXTERN) == 0) {
        return parse_extern_directive(dynamic_operands_pointer, out, file_name, line_number);
    }
    else if (strcmp(out->directive_name, DIR_ENTRY) == 0) {
        return parse_entry_directive(dynamic_operands_pointer, out, file_name, line_number);
    }
    else if (strcmp(out->directive_name, DIR_MAT) == 0) {
        return parse_mat_directive(dynamic_operands_pointer, out, file_name, line_number);
    }

    return 0;
}


int parse_string_directive(char *operands, ParsedLine *out, char *file_name, int line_number) {
    char *string_without_quotes;

    if(operands[0]=='\n'||operands[0]=='\0'){
        error_log(file_name, line_number, EMPTY_STRING_DIR);
        return 0;
    }

    if (operands[0] != '"' || operands[strlen(operands) - 1] != '"') {
        error_log(file_name, line_number, STRING_MISSING_QUOTES);
        return 0;
    }

    strncpy(out->operands[0], operands, MAX_LINE_LENGTH - 1);
    string_without_quotes = strip_quotes(out->operands[0]);

    if (verify_string_has_invalid_chars(string_without_quotes)) {
        error_log(file_name, line_number, INVALID_STRING);
        return 0;
    }

    strncpy(out->operands[0], string_without_quotes, MAX_LINE_LENGTH - 1);
    out->operands[0][MAX_LINE_LENGTH - 1] = '\0';
    out->operand_count = 1;

    return 1;
}

int parse_data_directive(char *operands, ParsedLine *out, char *file_name, int line_number) {
    char *token;
    int i = 0;

    if (operands[0] == '\0') {
        error_log(file_name, line_number, MISSING_OPERANDS_DATA);
        return 0;
    }

    token = strtok(operands, COMMA);
    while (token != NULL) {
        token = cut_spaces_before_and_after_string(token);

        if (token[0] == '\0') {
            error_log(file_name, line_number, MULTIPLE_COMMAS);
            return 0;
        }

        if (is_digit_or_char(token, 0, file_name, line_number)) {
            return 0;
        }

        strncpy(out->operands[i], token, MAX_LINE_LENGTH - 1);
        out->operands[i][MAX_LINE_LENGTH - 1] = '\0';
        i++;

        token = strtok(NULL, COMMA);
    }

    out->operand_count = i;
    return 1;
}

int parse_extern_directive(char *operands, ParsedLine *out, char *file_name, int line_number) {
    if (operands[0] == '\0') {
        error_log(file_name, line_number, MISSING_OPERAND_EXTERN);
        return 0;
    }

    operands = cut_spaces_before_and_after_string(operands);

    if (!is_valid_label(operands, file_name, line_number)) {
        return 0;
    }
    if (identify_opcode(operands) != OPCODE_INVALID || identify_directive(operands) != -1) {
        error_log(file_name, line_number, SAVED_WORD_AS_LABEL);
        return 0;
    } else if (identify_register(operands) != -1) {
        error_log(file_name, line_number, REGISTER_NAME_AS_LABEL);
        return 0;
    }

    strncpy(out->operands[0], operands, MAX_LINE_LENGTH - 1);
    out->operands[0][MAX_LINE_LENGTH - 1] = '\0';
    out->operand_count = 1;

    return 1;
}

int parse_entry_directive(char *operands, ParsedLine *out, char *file_name, int line_number) {
    if (operands[0] == '\0') {
        error_log(file_name, line_number, MISSING_OPERANDS_DATA);
        return 0;
    }

    operands = cut_spaces_before_and_after_string(operands);

    if (!is_valid_label(operands, file_name, line_number)) {
        return 0;
    }
    if (identify_opcode(operands) != OPCODE_INVALID || identify_directive(operands) != -1) {
        error_log(file_name, line_number, SAVED_WORD_AS_LABEL);
        return 0;
    } else if (identify_register(operands) != -1) {
        error_log(file_name, line_number, REGISTER_NAME_AS_LABEL);
        return 0;
    }

    strncpy(out->operands[0], operands, MAX_LINE_LENGTH - 1);
    out->operands[0][MAX_LINE_LENGTH - 1] = '\0';
    out->operand_count = 1;

    return 1;
}

int parse_mat_directive(char *operands, ParsedLine *out, char *file_name, int line_number) {
    int n, m, mat_size,i;
    char *mat_values, *token,*number;


    if (!is_valid_matrix_dim(operands, file_name, line_number)) {
        return 0;
    }

    if (sscanf(operands, MAT_DIM_SPACE, &n, &m, &mat_size) != 2) {
        error_log(file_name, line_number, MATRIX_DIMENSION_FORMAT);
        return 0;
    }

    /* Add matrix dimensions as first operand */
    strncpy(out->operands[0], operands, mat_size);
    out->operands[0][mat_size] = '\0';
    out->operand_count = 1;

    /* Parse matrix values */
    mat_values = operands + mat_size;
    mat_values = cut_spaces_before_and_after_string(mat_values);

    if (mat_values[0] != '\0') {
        token = strtok(mat_values, COMMA);

        while (token != NULL) {
            token = cut_spaces_before_and_after_string(token);

            if (token[0] == '\0') {
                error_log(file_name, line_number, MULTIPLE_COMMAS);
                return 0;
            }
            number = token;

            if (is_digit_or_char(number, 0, file_name, line_number)) {
                return 0;
            }

            strncpy(out->operands[out->operand_count], token, MAX_LINE_LENGTH - 1);
            out->operands[out->operand_count][MAX_LINE_LENGTH - 1] = '\0';
            out->operand_count++;

            token = strtok(NULL, COMMA);
        }
    } else{
        for (i = 0; i < mat_size; i++) {
            strncpy(out->operands[out->operand_count], "0", MAX_LINE_LENGTH - 1);
            out->operands[out->operand_count][MAX_LINE_LENGTH - 1] = '\0';
            out->operand_count++;
        }
    }

    return 1;
}

int parse_instruction_operands(char *operands, ParsedLine *out, char *file_name, int line_number) {
    char *token;
    int i = 0;
    int max_operands = expected_operands_for_each_opcode[out->opcode];

    while (i < max_operands && (token = strtok((i == 0) ? operands : NULL, COMMA)) != NULL) {
        token = cut_spaces_before_and_after_string(token);

        if (token[0] == '\0') {
            error_log(file_name, line_number, MULTIPLE_COMMAS);
            return 0;
        }

        /* Validate operand based on instruction and position */
        if (!validate_operand_for_instruction(token, out->opcode, i, file_name, line_number)) {
            return 0;
        }

        strncpy(out->operands[i], token, MAX_LINE_LENGTH - 1);
        out->operands[i][MAX_LINE_LENGTH - 1] = '\0';
        i++;
    }

    /* Check for extra operands */
    if (i == max_operands) {
        token = strtok(NULL, "");
        if (token != NULL) {
            token = cut_spaces_before_and_after_string(token);
            if (token[0] != '\0') {
                error_log(file_name, line_number, MANY_OP);
                return 0;
            }
        }
    }

    out->operand_count = i;
    return 1;
}

int validate_operand_for_instruction(char *operand, Opcode opcode, int position, char *file_name, int line_number) {

    switch (opcode) {
        case OPCODE_MOV:
            if (position == 0) {
                if (strchr(operand, LADDER)) {
                    if (is_valid_immediate(operand, file_name, line_number) == 1) {
                        return 0;
                    }
                } else if (strchr(operand, OPENING_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (strchr(operand, CLOSED_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (identify_register(operand) == -1) {
                    if (!is_valid_label(operand, file_name, line_number)) {
                        return 0;
                    }
                }
            } else {
                if (strchr(operand, LADDER)){
                    error_log(file_name,line_number,IMM_NOT_VALID_ARG_DEST_MOV);
                    return 0;
                }
                else if (strchr(operand, OPENING_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (strchr(operand, CLOSED_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (identify_register(operand) == -1) {
                    if (!is_valid_label(operand, file_name, line_number)) {
                        return 0;
                    }
                }
            }
            break;

        case OPCODE_CMP:
            /* Both operands: 0,1,2,3 */
            if (strchr(operand, LADDER)) {
                if (is_valid_immediate(operand, file_name, line_number) == 1) {
                    return 0;
                }
            } else if (strchr(operand, OPENING_BRACKET)) {
                if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                    return 0;
                }
            } else if (strchr(operand, CLOSED_BRACKET)) {
                if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                    return 0;
                }
            } else if (identify_register(operand) == -1) {
                if (!is_valid_label(operand, file_name, line_number)) {
                    return 0;
                }
            }
            break;

        case OPCODE_ADD:
        case OPCODE_SUB:
            if (position == 0) {
                if (strchr(operand, LADDER)) {
                    if (is_valid_immediate(operand, file_name, line_number) == 1) {
                        return 0;
                    }
                } else if (strchr(operand, OPENING_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (strchr(operand, CLOSED_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (identify_register(operand) == -1) {
                    if (!is_valid_label(operand, file_name, line_number)) {
                        return 0;
                    }
                }
            } else {
                if (strchr(operand, LADDER)){
                    error_log(file_name,line_number,IMM_NOT_VALID_ARG_DEST_ADD_SUB);
                    return 0;
                }
                else if (strchr(operand, OPENING_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (strchr(operand, CLOSED_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (identify_register(operand) == -1) {
                    if (!is_valid_label(operand, file_name, line_number)) {
                        return 0;
                    }
                }
            }
            break;

        case OPCODE_LEA:
            if (position == 0) {
                if (strchr(operand, LADDER)){
                    error_log(file_name,line_number,IMM_NOT_VALID_ARG_SRC_LEA);
                    return 0;
                }
                else if (strchr(operand, OPENING_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (strchr(operand, CLOSED_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (!is_valid_label(operand, file_name, line_number)) {
                    return 0;
                }
            } else {
                if (strchr(operand, LADDER)){
                    error_log(file_name,line_number,IMM_NOT_VALID_ARG_DEST_LEA);
                    return 0;
                }
                else if (strchr(operand, OPENING_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (strchr(operand, CLOSED_BRACKET)) {
                    if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                        return 0;
                    }
                } else if (identify_register(operand) == -1) {
                    if (!is_valid_label(operand, file_name, line_number)) {
                        return 0;
                    }
                }
            }
            break;

        case OPCODE_CLR:
        case OPCODE_NOT:
        case OPCODE_INC:
        case OPCODE_DEC:
        case OPCODE_JMP:
        case OPCODE_BNE:
        case OPCODE_JSR:
        case OPCODE_RED:
            if (strchr(operand, LADDER)){
                error_log(file_name,line_number,IMM_NOT_VALID_ARG_DEST_REST_OP);
                return 0;
            }
            else if (strchr(operand, OPENING_BRACKET)) {
                if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                    return 0;
                }
            } else if (strchr(operand, CLOSED_BRACKET)) {
                if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                    return 0;
                }
            } else if (identify_register(operand) == -1) {
                if (!is_valid_label(operand, file_name, line_number)) {
                    return 0;
                }
            }
            break;

        case OPCODE_PRN:
            if (strchr(operand, LADDER)) {
                if (is_valid_immediate(operand, file_name, line_number) == 1) {
                    return 0;
                }
            } else if (strchr(operand, OPENING_BRACKET)) {
                if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                    return 0;
                }
            } else if (strchr(operand, CLOSED_BRACKET)) {
                if (verify_matrix_registers_are_valid(operand, file_name, line_number)) {
                    return 0;
                }
            } else if (identify_register(operand) == -1) {
                if (!is_valid_label(operand, file_name, line_number)) {
                    return 0;
                }
            }
            break;

        default:
            return 0;
    }

    return 1;
}