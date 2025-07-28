
#ifndef PROJECT_PREPROCESSOR_H
#define PROJECT_PREPROCESSOR_H

#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include "analyze_text.h"
#include "linked_list.h"


int preprocessor_full_flow(char file_name[]);

void prepro_first_pass();
void preproc_second_pass();


#endif /*PROJECT_PREPROCESSOR_H*/
