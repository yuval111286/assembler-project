#ifndef ASSEMBLER_PROJECT_SECOND_PASS_H
#define ASSEMBLER_PROJECT_SECOND_PASS_H



void add_entry_label_to_symbol_table();

void add_extern_label_to_symbol_table();

int binary_line_to_base_4(int binary_line, char *code);

char *two_binary_digits_one_base4_char (int binary_num);

void create_obj_file();

void create_ent_file();

void create_ext_file();





#endif /*ASSEMBLER_PROJECT_SECOND_PASS_H*/
