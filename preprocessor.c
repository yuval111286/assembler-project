#include "preprocessor.h"
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include "analyze_text.h"
#include "linked_list.h"
#include "parser.h"

int check_as_file_ending(char *file_name)
{
    char *c;

    /* cuts the exising ending of a file  */
    c = strrchr(file_name, '.');
    if (strcmp(c, ".as") == 0){
        return 0;
    }
    return -1;
}

int prepro_first_pass(char *file_name){

    FILE *fp;
    char line[MAX_LINE_LENGTH];
    
    fp = fopen(file_name,"r");
    if (fp == NULL) {
        printf("Error opening the file for reading\n");
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, MCRO, 5) == 0){
            identify_macro_name(line);
        }
    }

    return 0;
}

int identify_macro_name(char *line){

    char *mcro_name, *extra_text_after_mcro_name;

    mcro_name = skip_spaces(line);
    if(mcro_name == NULL)
    {
        printf("macro name is missing\n");
        return -1;
    }
    extra_text_after_mcro_name= skip_word(mcro_name);
    if(extra_text_after_mcro_name !=NULL)
    {
        printf("extra text after mcro name\n");
        return -1;
    }


return 0;

}

int mcro_name_validation(char *mcro_name){

    return (identify_opcode(mcro_name)&& identify_register(mcro_name)&& is_directive(mcro_name));

}

int add_macro_to_linked_list(){
 return 0;
}

int preprocessor_full_flow(char *file_name){

    FILE /**fp_read, *fp_write,*/ *first_copy;
    /*node *head = NULL;*/
    char line[MAX_LINE_LENGTH], *new_file_name,clean_file_name[256];
    SourceFileLocation as_file;

    if(check_as_file_ending(file_name)!=0){
        printf("Error: Did not receive .as file\n");
        return -1;
    }

    snprintf(clean_file_name, sizeof(clean_file_name), "%s_clean.as", file_name);

    first_copy = create_clean_file(file_name, clean_file_name);
    if (first_copy == NULL) {
        printf("Error: Failed to create clean file\n");
        return -1;
    }

 return 0;
}