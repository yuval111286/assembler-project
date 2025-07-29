#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "preprocessor.h"
#include "errors_handler.h"
#include "linked_list.h"
#include "analyze_text.h"
#include "utils.h"
#include "globals.h"



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


int preprocessor_full_flow(char *file_name) {
    FILE *clean_file;   
    char clean_file_name[256];                 
    
    if (check_as_file_ending(file_name) != 0) { /* Check that the input file ends with .as */
        printf("Error: Did not receive .as file\n");
        return -1; 
    }

    /* Generate new cleaned file name, e.g., prog.as_clean.as */
    snprintf(clean_file_name, sizeof(clean_file_name), "%s_clean.as", file_name);

    /* Create cleaned version of the file  */
    clean_file = create_clean_file(file_name, clean_file_name);
    if (clean_file == NULL) {
        printf("Error: Failed to create clean file\n");
        return -1; 
    }

    
    prepro_first_pass();     /* First pass: store macro definitions */
    preproc_second_pass();   /* Second pass: expand macros */

    return 0; 
}





   

