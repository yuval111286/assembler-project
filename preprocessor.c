#include "preprocessor.h"


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

int preprocessor_full_flow(char *file_name){

    FILE *fp_read, *fp_write, *first_copy;
    node *head = NULL;
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


}