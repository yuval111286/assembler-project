#ifndef ASSEMBLER_PROJECT_SECOND_PASS_H
#define ASSEMBLER_PROJECT_SECOND_PASS_H

#include "first_pass.h"

void add_entry_label_to_symbol_table();

void add_extern_label_to_symbol_table();

int binary_line_to_base_4(char *code);

char *two_binary_digits_one_base4_char (int binary_num);

int create_obj_file(char *obj_file_name);

int create_ent_file(char *ent_file_name);

int create_ext_file(char *ext_file_name);

int create_files(char *org_file_name);

int second_pass(char *file_name,SymbolTable *symbol_table, CodeImage *code_image);







#endif /*ASSEMBLER_PROJECT_SECOND_PASS_H*/
