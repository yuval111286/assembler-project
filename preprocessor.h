
#ifndef PROJECT_PREPROCESSOR_H
#define PROJECT_PREPROCESSOR_H

#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include "analyze_text.h"
#include "linked_list.h"
#include "parser.h"


#define MCRO "mcro "
#define MCROEND "mcroend "

int preprocessor_full_flow(char file_name[]);

int check_as_file_ending(char *file_name);

int prepro_first_pass(char *file_name);

void preproc_second_pass();

int identify_macro_name(char *line);


#endif /*PROJECT_PREPROCESSOR_H*/
