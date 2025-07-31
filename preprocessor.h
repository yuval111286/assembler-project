
#ifndef PROJECT_PREPROCESSOR_H
#define PROJECT_PREPROCESSOR_H

#include <stdio.h>
#include "linked_list.h"

#define MCRO "mcro "
#define MCROEND "mcroend\n"

int preprocessor_full_flow(char file_name[]);

int check_as_file_ending(char *file_name);

int prepro_first_pass(char *org_file_name,char *file_name, int *line_counter , node **head);

int preproc_second_pass(char *org_file_name,node **head,char *as_file_name,int *line_counter, char *am_file_name);

char *identify_macro_name(char *org_file_name, char *line, char *file_name, int *line_counter);

int mcro_name_validation(char *mcro_name);


int mcro_name_only_letters_num_underscore(char *mcro_name);

int is_save_word(char *mcro_name);

char *extract_mcro_text(FILE *fp, fpos_t *pos, int *line_counter);


#endif /*PROJECT_PREPROCESSOR_H*/
