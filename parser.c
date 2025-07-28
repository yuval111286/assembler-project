#include "parser.h"




int identify_opcode(char* op_code_str) {

    int i;
    if (op_code_str == NULL) {
        return OPCODE_INVALID;
    }

    for (i = 0; opcode_table[i].name != NULL; i++) {
        if (strcasecmp(op_code_str, opcode_table[i].name) == 0) {
            return opcode_table[i].opcode;
        }
    }

    return OPCODE_INVALID;
}


int is_directive(char *directive){

    int i;
    for (i = 0; directive_table[i].name != NULL; i++) {
        if (strcasecmp(directive, directive_table[i].name) == 0) {
            return directive_table[i].directive;
        }
    }

    return -1;

}

int identify_register(char *reg){
    int i;
    for (i = 0; register_table[i].name != NULL; i++) {
        if (strcasecmp(reg, register_table[i].name) == 0) {
            return register_table[i].reg;
        }
    }

    return -1;
}
